// MP3Decode

// header
#include <pspsdk.h>
#include <pspkernel.h>
#include <pspaudiocodec.h>
#include <string.h>
#include "memory.h"
#include "mp3.h"

// prototype
int Get_ID3v1Size(SceUID handle);
int Get_ID3v2Size(SceUID handle);
int Get_Framesize(u8 *buf);

// global
static int id3v1pos;
static int id3v2pos;

// function
int KernelAudiocodecInit(unsigned long *buff, int type)
{
	int k1 = pspSdkSetK1(0);
	int res = sceAudiocodecInit(buff, type);
	pspSdkSetK1(k1);
	return res;
}

int KernelAudiocodecCheckNeedMem(unsigned long *buff, int type)
{
	int k1 = pspSdkSetK1(0);
	int res = sceAudiocodecCheckNeedMem(buff, type);
	pspSdkSetK1(k1);
	return res;
}

int KernelAudiocodecGetEDRAM(unsigned long *buff, int type)
{
	int k1 = pspSdkSetK1(0);
	int res = sceAudiocodecGetEDRAM(buff, type);
	pspSdkSetK1(k1);
	return res;
}

int KernelAudiocodecDecode(unsigned long *buff, int type)
{
	int k1 = pspSdkSetK1(0);
	int res = sceAudiocodecDecode(buff, type);
	pspSdkSetK1(k1);
	return res;
}

int KernelAudiocodecReleaseEDRAM(unsigned long *buff)
{
	int k1 = pspSdkSetK1(0);
	int res = sceAudiocodecReleaseEDRAM(buff);
	pspSdkSetK1(k1);
	return res;
}

int Get_ID3v1Size(SceUID handle)
{
	// ID3v1Tag
	char buf[3];
	int pos, res = 0;

	if(handle < 0)
		return 0;

	pos = sceIoLseek32(handle, 0, PSP_SEEK_CUR);
	sceIoLseek32(handle, 128, PSP_SEEK_END);

	// ReadTag
	if(sceIoRead(handle, (void *)buf, 3) != 3) return 0;
	if(strncmp(buf, "TAG", 3) == 0) res = 128;

	sceIoLseek32(handle, pos, PSP_SEEK_SET);
	return res;
}

int Get_ID3v2Size(SceUID handle)
{
	// ID3v2Tag
	char header[10];
	int pos, res = 0;
	u32 tagsize;

	if(handle < 0)
		return 0;

	pos = sceIoLseek32(handle, 0, PSP_SEEK_CUR);
	sceIoLseek32(handle, 0, PSP_SEEK_SET);

	// ReadTag
	if(sceIoRead(handle, header, 10) != 10) return 0;
	if(strncmp(header, "ID3", 3) == 0 || strncmp(header, "ea3", 3) == 0 || strncmp(header, "EA3", 3) == 0)
	{
		tagsize = (u8)header[6];
		tagsize = (tagsize << 7) | (u8)header[7];
		tagsize = (tagsize << 7) | (u8)header[8];
		tagsize = (tagsize << 7) | (u8)header[9];

		// CheckFlag
		if(header[5] & 0x10)
			tagsize += 10;

		res = tagsize + 10;
	}

	sceIoLseek32(handle, pos, PSP_SEEK_SET);
	return res;
}

int Get_Framesize(u8 *buf)
{
	u32 header, version;
	int samplerate, bitrate, padding;

	const int samplerates[4] = { 44100, 48000, 32000, 2 };
	const int bitrates[2][15] =
	{
		{ 0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320 }, 	// MPEG1
		{ 0, 8,	16,	24,	32,	40,	48,	56,	64, 80, 96, 112, 128, 144, 160 }, 		// MPEG2
	};

	header = buf[0];
	header = (header << 8) | buf[1];
	header = (header << 8) | buf[2];
	header = (header << 8) | buf[3];

	version = (header & 0x180000) >> 19;
	switch (version)
	{
		case 3:
			version = 0;
			break;
		case 2:
			version = 1;
			break;
		default:
			return 0;
	}

	samplerate = samplerates[(header & 0xc00) >> 10];
	bitrate = bitrates[version][(header & 0xf000) >> 12];
	padding = (header & 0x200) >> 9;

	return 144000 * bitrate / (samplerate / (version + 1)) + padding;
}

int MP3_Init(const char *file_name, DecodeData *mp3)
{
	// Open
	mp3->handle = sceIoOpen(file_name, PSP_O_RDONLY, 0777);
	if(mp3->handle < 0) return -1;

	// GetFileSize
	mp3->file_size = sceIoLseek32(mp3->handle, 0, PSP_SEEK_END);
	sceIoLseek32(mp3->handle, 0, PSP_SEEK_SET);

	// ID3Tag
	id3v1pos = mp3->file_size - Get_ID3v1Size(mp3->handle);
	id3v2pos = Get_ID3v2Size(mp3->handle);

	// ID3v2Tag
	mp3->data_start = id3v2pos;
	sceIoLseek32(mp3->handle, mp3->data_start, PSP_SEEK_SET);

	// MemoryAlloc
	mp3->codec_buffer			= memory_alloc(sizeof(unsigned long) * 65);
	mp3->decode_buffer			= memory_alloc(1152 * 4);
	mp3->output_buffer[0]		= memory_alloc(1152 * 4);
	mp3->output_buffer[1]		= memory_alloc(1152 * 4);

	if(mp3->codec_buffer == NULL || mp3->decode_buffer == NULL ||
		mp3->output_buffer[0] == NULL || mp3->output_buffer[1] == NULL)
	{
		return -1;
	}

	// set
	memset(mp3->codec_buffer, 0, (sizeof(unsigned long) * 65));
	memset(mp3->decode_buffer, 0, 1152 * 4);
	memset(mp3->output_buffer[0], 0, 1152 * 4);
	memset(mp3->output_buffer[1], 0, 1152 * 4);

	// CheckNeedMemory
	if(KernelAudiocodecCheckNeedMem(mp3->codec_buffer, PSP_CODEC_MP3) < 0) return -1;

	// MemoryAlloc
	if(KernelAudiocodecGetEDRAM(mp3->codec_buffer, PSP_CODEC_MP3) < 0) return -1;

	// set
	if(KernelAudiocodecInit(mp3->codec_buffer, PSP_CODEC_MP3) < 0) return -1;

	// set
	mp3->frame_size = 0;
	mp3->output_num = 1;
	mp3->read_buffer = NULL;

	return 0;
}

int MP3_End(DecodeData *mp3)
{
	// FreeMemory
	KernelAudiocodecReleaseEDRAM(mp3->codec_buffer);

	free_alloc(mp3->read_buffer);
	free_alloc(mp3->codec_buffer);
	free_alloc(mp3->decode_buffer);
	free_alloc(mp3->output_buffer[0]);
	free_alloc(mp3->output_buffer[1]);

	sceIoClose(mp3->handle);
	return 0;
}

int MP3_Decode(DecodeData *mp3)
{
	u8 buf[4];

	// ReadFrame
	if(sceIoRead(mp3->handle, buf, 4) != 4) return -1;

	// GetFrameSize
	mp3->frame_size = Get_Framesize(buf);

	// MemoryAlloc
	if(mp3->read_buffer != NULL)
		free_alloc(mp3->read_buffer);

	mp3->read_buffer = memory_alloc(mp3->frame_size);
	if(mp3->read_buffer == NULL) return -1;

	// ReadPosition
	sceIoLseek32(mp3->handle, mp3->data_start, PSP_SEEK_SET);

	// ReadData
	if(sceIoRead(mp3->handle, mp3->read_buffer, mp3->frame_size) != mp3->frame_size) return -1;

	// set
	mp3->codec_buffer[6] = (unsigned long)mp3->read_buffer;
	mp3->codec_buffer[8] = (unsigned long)mp3->decode_buffer;
	mp3->codec_buffer[7] = mp3->codec_buffer[10] = mp3->frame_size;
	mp3->codec_buffer[9] = (1152 * 4);

	// Decode
	if(KernelAudiocodecDecode(mp3->codec_buffer, PSP_CODEC_MP3) < 0) return -1;

	// Copy
	mp3->output_num ^= 1;
	memcpy((void *)mp3->output_buffer[mp3->output_num], (void *)mp3->decode_buffer, (1152 * 4));

	mp3->data_start += mp3->frame_size;
	if(mp3->file_size > id3v1pos && mp3->data_start >= id3v1pos) return -1;
	return 0;
}

