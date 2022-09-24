#ifndef __MP3_DECODE_H__
#define	__MP3_DECODE_H__

// struct
typedef struct
{
	int handle;
	int file_size;
	int frame_size;
	int data_start;
	int output_num;

	u8 *read_buffer;
	u8 *decode_buffer;
	u8 *output_buffer[2];
	unsigned long *codec_buffer;
} DecodeData;

// function
int MP3_Init(const char *file_name, DecodeData *mp3);
int MP3_End(DecodeData *mp3);
int MP3_Decode(DecodeData *mp3);

#endif

