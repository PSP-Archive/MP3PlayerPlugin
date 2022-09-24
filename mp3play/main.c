// MP3PlayerPlugin v2.1

// header
#include <pspkernel.h>
#include <systemctrl.h>
#include <pspsysevent.h>
#include <pspdisplay.h>
#include <pspaudio.h>
#include <psputility.h>
#include <pspinit.h>
#include <pspctrl.h>
#include <stdlib.h>
#include <string.h>
#include "mp3pp.h"
#include "memory.h"
#include "mp3.h"
#include "file.h"
#include "blit.h"
#include "charConv.h"
#include "conf.h"
#include "sysclib.h"

// info
PSP_MODULE_INFO("mp3play", 0x1000, 0, 21);

// prototype
void SetSpeed(void);
int Suspend_Handler(int ev_id, char *ev_name, void *param, int *result);
int LoadExecForUser_D1FB50DC(void);
int sceDmacplusLcdcSetBaseAddr(void *baseaddr);

// function_ptr
int (*_loadexec_func)(int api, char *file, struct SceKernelLoadExecVSHParam *param, int unk);
int (*_setclock_func)(int cpuspd1, int cpuspd2, int busspd);
int (*_srcoutput_func)(int vol, void *buf);
int (*_sceAudioOutput)(int ch, int vol, void *buf);
int (*_sceAudioOutputBlocking)(int ch, int vol, void *buf);
int (*_sceAudioOutputPanned)(int ch, int leftvol, int rightvol, void *buf);
int (*_sceAudioOutputPannedBlocking)(int ch, int leftvol, int rightvol, void *buf);
int (*_sceVaudioOutputBlocking)(int vol, void *buf);
int (*_sceIoRead)(SceUID fd, void *buffer, int size);
int (*_sceIoWrite)(SceUID fd, void *buffer, int size);

// global
const int cpulist[] = { 0, 20, 75, 100, 133, 222, 266, 300, 333 };
const int buslist[] = { 0, 10, 37, 50, 66, 111, 133, 150, 166 };
const char *hmsg[] = { "0 CHANNEL","1 CHANNEL","2 CHANNEL","3 CHANNEL","4 CHANNEL",
						"5 CHANNEL","6 CHANNEL","7 CHANNEL","ALL CHANNEL","OFF" };
const char *smsg[] = { "THREAD MODE", "HOOK MODE" };
const char *fmsg[] = { "PAUSE", "PLAYING", };
const char *mmsg[] = { "NORMAL", "LOOP", "RANDOM", };
u32 fg = WHITE;
u32 bg = BLACK;
u32 rg = RED;

vram_buffers vb;			// DrawBuffer
SceUID main_thid;			// MainThread
SceUID music_thid;			// MP3PlayThread
SceUID draw_thid;			// DrawThread
FileList *path_list;		// MP3FileList
char list_name[256];		// MP3FileName
int count;					// ListNumber
int list_num;				// FileCount
int list_id;				// MemoryID
int channel;				// Channel
int flag;					// PlayFlag
int mode;					// PlayMode
int mp3_run;				// RunFlag
float volume;				// Volume
int hold;					// HoldFlag
int draw_flag;				// DrawFlag
int skip;					// SkipFlag
int pos;					// MenuPosition
int drawing;				// DrawingFlag
clock_t time;				// DrawTimeFlag
int hook_run;				// HOOKFlag
int stop_flag;				// I/OFlag
int clock_pos;				// ClockPosition
int last_clock;				// LastPosition
SceUID load_sema;			// SemaID
int load_flag;				// LoadEndFlag
Conf_Key key;				// ConfigData
int eboot_flag;				// PSPHomebrewFlag

// EventCallback_struct
PspSysEventHandler events =
{
	0x40,
	"Suspend_Event",
	0x0000FF00,
	Suspend_Handler,
};

// function
void ClearCaches(void)
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}

u32 sceAudioFindProc(u32 nid)
{
	return FindProc("sceAudio_Driver", "sceAudio", nid);
}

u32 scePowerFindProc(u32 nid)
{
	return FindProc("scePower_Service", "scePower", nid);
}

void SyscallHelper(char *szMod, char *szLib, u32 nid, void *newone)
{
	sctrlHENPatchSyscall(FindProc(szMod, szLib, nid), newone);
}

int sceAudioOutputPatched(int ch, int vol, void *buf)
{
	if(channel != ch && hook_run == ch)
	{
		return _sceAudioOutput(ch, 0, buf);
	}
	else if(hook_run == 8)
	{
		return _sceAudioOutput(ch, 0, buf);
	}
	return _sceAudioOutput(ch, vol, buf);
}

int sceAudioOutputBlockingPatched(int ch, int vol, void *buf)
{
	if(channel != ch && hook_run == ch)
	{
		return _sceAudioOutputBlocking(ch, 0, buf);
	}
	else if(hook_run == 8)
	{
		return _sceAudioOutputBlocking(ch, 0, buf);
	}
	return _sceAudioOutputBlocking(ch, vol, buf);
}

int sceAudioOutputPannedPatched(int ch, int leftvol, int rightvol, void *buf)
{
	if(channel != ch && hook_run == ch)
	{
		return _sceAudioOutputPanned(ch, 0, 0, buf);
	}
	else if(hook_run == 8)
	{
		return _sceAudioOutputPanned(ch, 0, 0, buf);
	}
	return _sceAudioOutputPanned(ch, leftvol, rightvol, buf);
}

int sceAudioOutputPannedBlockingPatched(int ch, int leftvol, int rightvol, void *buf)
{
	if(channel != ch && hook_run == ch)
	{
		return _sceAudioOutputPannedBlocking(ch, 0, 0, buf);
	}
	else if(hook_run == 8)
	{
		return _sceAudioOutputPannedBlocking(ch, 0, 0, buf);
	}
	return _sceAudioOutputPannedBlocking(ch, leftvol, rightvol, buf);
}

int sceVaudioOutputBlockingPatched(int vol, void *buf)
{
	if(hook_run == 8)
	{
		return _sceVaudioOutputBlocking(0, buf);
	}
	return _sceVaudioOutputBlocking(vol, buf);
}

int sceAudioOutput2OutputBlockingPatched(int vol, void *buf)
{
	if(hook_run == 8)
	{
		return sceAudioSRCOutputBlocking(0, buf);
	}

	return sceAudioSRCOutputBlocking(vol, buf);
}

int sceAudioSRCOutputBlockingPatched(int vol, void *buf)
{
	if(hook_run == 8)
	{
		_srcoutput_func(0, buf);
	}

	return _srcoutput_func(vol, buf);
}

void HookAudioFunc(void)
{
	u32 orgaddr = 0;
	u32 func, orgend;

	// sceAudioOutput
	orgaddr = sceAudioFindProc(0x8C1009B2);
	_sceAudioOutput = (void *)orgaddr;
	sctrlHENPatchSyscall(orgaddr, sceAudioOutputPatched);

	// sceAudioOutputBlocking
	orgaddr = sceAudioFindProc(0x136CAF51);
	_sceAudioOutputBlocking = (void *)orgaddr;
	sctrlHENPatchSyscall(orgaddr, sceAudioOutputBlockingPatched);

	// sceAudioOutputPanned
	orgaddr = sceAudioFindProc(0xE2D56B2D);
	_sceAudioOutputPanned = (void *)orgaddr;
	sctrlHENPatchSyscall(orgaddr, sceAudioOutputPannedPatched);

	// sceAudioOutputPannedBlocking
	orgaddr = sceAudioFindProc(0x13F592BC);
	_sceAudioOutputPannedBlocking = (void *)orgaddr;
	sctrlHENPatchSyscall(orgaddr, sceAudioOutputPannedBlockingPatched);

	// sceAudioOutput2OutputBlocking
	orgaddr = sceAudioFindProc(0x2D53F36E);
	if(orgaddr != 0) MAKE_CALL(orgaddr + 8, sceAudioOutput2OutputBlockingPatched);

	// sceAudioSRCOutputBlocking
	orgaddr = sceAudioFindProc(0xE0727056);
	if(orgaddr != 0)
	{
		// sceKernelCpuSuspendIntr
		func = MAKE_CALL_FUNC(FindProc("sceInterruptManager", "InterruptManagerForKernel", 0x092968F4));
		orgend = orgaddr + 0x200;

		for(; orgaddr < orgend; orgaddr += 4)
		{
			if(_lw(orgaddr) == func)
			{
				_srcoutput_func = (void *)orgaddr + 0x10;
				MAKE_CALL(orgaddr + 0x10, sceAudioSRCOutputBlockingPatched);
				break;
			}
		}
	}

	// clear
	ClearCaches();
	return;
}

int LoadExecFuncPatched(int api, char *file, struct SceKernelLoadExecVSHParam *param)
{
	// Stop
	flag = PAUSE_MUSIC;
	time = 0;

	// ClearMemory
	ClearFilelist(path_list, list_id, list_num);

	// Delete Threads
	sceKernelTerminateDeleteThread(main_thid);
	sceKernelTerminateDeleteThread(music_thid);
	sceKernelTerminateDeleteThread(draw_thid);

	// Delay
	sceKernelDelayThread(800000);

	return _loadexec_func(api, file, param, 0x10000);
}

void HookLoadExecFunc(u32 text_addr, u32 text_end)
{
	u32 func = 0;
	u32 addr = 0;
	int cnt = 0;

	for(addr = text_addr; addr < text_end; addr += 4)
	{
		if(_lw(addr) == 0x34640064 && _lw(addr - 8) == 0x3C038002)
		{
			if(cnt == 0)
			{
				cnt = 1;
				continue;
			}

			_loadexec_func = (void *)addr - 0x44;
			func = MAKE_CALL_FUNC(addr - 0x44);
			break;
		}
	}

	for(addr = text_addr; addr < text_end; addr += 4)
	{
		if(_lw(addr) == func)
		{
			MAKE_CALL(addr, LoadExecFuncPatched);
		}
	}

	ClearCaches();
	return;
}

void LoadExecForUser_D1FB50DCPatched(void)
{
	// Stop
	flag = PAUSE_MUSIC;
	time = 0;

	// ClearMemory
	ClearFilelist(path_list, list_id, list_num);

	// Delete Threads
	sceKernelTerminateDeleteThread(main_thid);
	sceKernelTerminateDeleteThread(music_thid);
	sceKernelTerminateDeleteThread(draw_thid);

	// Delay
	sceKernelDelayThread(800000);

	LoadExecForUser_D1FB50DC();
	return;
}

void RestorePowerFunc(void)
{
	u32 orgaddr = 0;

	// scePowerSetCpuClockFrequency
	orgaddr = scePowerFindProc(0x843FBF43);
	_sw(0x27BDFFE0, orgaddr + 0);
	_sw(0xAFB20008, orgaddr + 4);

	// scePowerSetBusClockFrequency
	orgaddr = scePowerFindProc(0xB8D7B3FB);
	_sw(0x27BDFFE0, orgaddr + 0);
	_sw(0xAFB20008, orgaddr + 4);

	ClearCaches();
	return;
}

void HookPowerFunc(u32 text_addr, u32 text_end)
{
	u32 func = 0;
	u32 addr = 0;
	u32 orgaddr = 0;

	// scePowerSetCpuClockFrequency
	orgaddr = scePowerFindProc(0x843FBF43);
	_sw(0x27BDFFE0, orgaddr + 0);
	_sw(0xAFB20008, orgaddr + 4);

	// scePowerSetBusClockFrequency
	orgaddr = scePowerFindProc(0xB8D7B3FB);
	_sw(0x27BDFFE0, orgaddr + 0);
	_sw(0xAFB20008, orgaddr + 4);

	// scePowerSetClockFrequency
	for(addr = text_addr; addr < text_end; addr += 4)
	{
		if(_lw(addr) == 0x2A270001 && _lw(addr + 4) == 0x38650001)
		{
			_setclock_func = (void *)addr - 0x1C;
			func = MAKE_CALL_FUNC(addr - 0x1C);
			break;
		}
	}

	for(addr = text_addr; addr < text_end; addr += 4)
	{
		if(_lw(addr) == func)
		{
			_sw(0x00000000, addr);
		}
	}

	// clear
	ClearCaches();
	return;
}

int sceIoReadPatched(SceUID fd, void *buffer, int size)
{
	int k1 = pspSdkSetK1(0);
	int res = 0;

	stop_flag = 1;
	res = _sceIoRead(fd, buffer, size);
	stop_flag = 0;

	pspSdkSetK1(k1);
	return res;
}

int sceIoWritePatched(SceUID fd, void *buffer, int size)
{
	int k1 = pspSdkSetK1(0);
	int res = 0;

	stop_flag = 1;
	res = _sceIoWrite(fd, buffer, size);
	stop_flag = 0;

	pspSdkSetK1(k1);
	return res;
}

int sceDmacplusLcdcSetBaseAddrPatched(void *baseaddr)
{
	// GetInfo
	Set_Buffers(1);

	// Get_VRAM
	vb.vram = baseaddr;

	// SetPosition
	int unk = clock_pos;

	// Draw
	if(drawing && time > sceKernelLibcClock())
	{
		blit_printf(0, 200,					  fg, bg, "[フラグ]   = %s", flag > 0 ? fmsg[1] : fmsg[0]);
		blit_printf(0, 208,					  fg, bg, "[ホールド]  = %s", hold == 0 ? "OFF" : "ON");
		blit_printf(0, 216, (pos == 6) ? rg : fg, bg, "[クロック]  = %d/%d", cpulist[unk], buslist[unk]);
		blit_printf(0, 224, (pos == 5) ? rg : fg, bg, "[描画]    = %s", smsg[draw_flag]);
		blit_printf(0, 232, (pos == 4) ? rg : fg, bg, "[スキップ]  = %s", skip == 0 ? "FILE" : "FOLDER");
		blit_printf(0, 240, (pos == 3) ? rg : fg, bg, "[ミュート]  = %s", hmsg[hook_run]);
		blit_printf(0, 248, (pos == 2) ? rg : fg, bg, "[モード]   = %s", mmsg[mode]);
		blit_printf(0, 256, (pos == 1) ? rg : fg, bg, "[音量]    = %d", (int)(volume / MINI_VOLUME));
		blit_printf(0, 264, (pos == 0) ? rg : fg, bg, "[ファイル名] = %s", list_name);
	}

	else if(drawing && time <= sceKernelLibcClock())
	{
		blit_printf(0, 200,	bg, bg, "[フラグ]   = %s", flag > 0 ? fmsg[1] : fmsg[0]);
		blit_printf(0, 208,	bg, bg, "[ホールド]  = %s", hold == 0 ? "OFF" : "ON");
		blit_printf(0, 216, bg, bg, "[クロック]  = %d/%d", cpulist[unk], buslist[unk]);
		blit_printf(0, 224, bg, bg, "[描画]    = %s", smsg[draw_flag]);
		blit_printf(0, 232, bg, bg, "[スキップ]  = %s", skip == 0 ? "FILE" : "FOLDER");
		blit_printf(0, 240, bg, bg, "[ミュート]  = %s", hmsg[hook_run]);
		blit_printf(0, 248, bg, bg, "[モード]   = %s", mmsg[mode]);
		blit_printf(0, 256, bg, bg, "[音量]    = %d", (int)(volume / MINI_VOLUME));
		blit_printf(0, 264, bg, bg, "[ファイル名] = %s", list_name);

		SetSpeed();

		pos = 0;
		drawing = 0;
	}

	return sceDmacplusLcdcSetBaseAddr(baseaddr);
}

int DrawThread(SceSize args, void *argp)
{
	int unk = 0;

	while(!drawing && time > sceKernelLibcClock())
	{
		// GetInfo
		Set_Buffers(1);

		// SetPosition
		unk = clock_pos;

		// Draw
		blit_printf(0, 200,					  fg, bg, "[フラグ]   = %s", flag > 0 ? fmsg[1] : fmsg[0]);
		blit_printf(0, 208,					  fg, bg, "[ホールド]  = %s", hold == 0 ? "OFF" : "ON");
		blit_printf(0, 216, (pos == 6) ? rg : fg, bg, "[クロック]  = %d/%d", cpulist[unk], buslist[unk]);
		blit_printf(0, 224, (pos == 5) ? rg : fg, bg, "[描画]    = %s", smsg[draw_flag]);
		blit_printf(0, 232, (pos == 4) ? rg : fg, bg, "[スキップ]  = %s", skip == 0 ? "FILE" : "FOLDER");
		blit_printf(0, 240, (pos == 3) ? rg : fg, bg, "[ミュート]  = %s", hmsg[hook_run]);
		blit_printf(0, 248, (pos == 2) ? rg : fg, bg, "[モード]   = %s", mmsg[mode]);
		blit_printf(0, 256, (pos == 1) ? rg : fg, bg, "[音量]    = %d", (int)(volume / MINI_VOLUME));
		blit_printf(0, 264, (pos == 0) ? rg : fg, bg, "[ファイル名] = %s", list_name);

		// Wait
		sceDisplayWaitVblankStart();

		// Delay
		sceKernelDelayThread(1000);
	}

	blit_printf(0, 200,	bg, bg, "[フラグ]   = %s", flag > 0 ? fmsg[1] : fmsg[0]);
	blit_printf(0, 208,	bg, bg, "[ホールド]  = %s", hold == 0 ? "OFF" : "ON");
	blit_printf(0, 216, bg, bg, "[クロック]  = %d/%d", cpulist[unk], buslist[unk]);
	blit_printf(0, 224, bg, bg, "[描画]    = %s", smsg[draw_flag]);
	blit_printf(0, 232, bg, bg, "[スキップ]  = %s", skip == 0 ? "FILE" : "FOLDER");
	blit_printf(0, 240, bg, bg, "[ミュート]  = %s", hmsg[hook_run]);
	blit_printf(0, 248, bg, bg, "[モード]   = %s", mmsg[mode]);
	blit_printf(0, 256, bg, bg, "[音量]    = %d", (int)(volume / MINI_VOLUME));
	blit_printf(0, 264, bg, bg, "[ファイル名] = %s", list_name);

	SetSpeed();

	pos = 0;
	draw_thid = 0;
	return 0;
}

void HookDisplayFunc(u32 text_addr, u32 text_end)
{
	for(; text_addr < text_end; text_addr += 4)
	{
		if(_lw(text_addr) == 0x00000000 && _lw(text_addr + 4) == 0x00112880)
		{
			MAKE_CALL(text_addr - 0xC, sceDmacplusLcdcSetBaseAddrPatched);
			break;
		}
	}

	ClearCaches();
	return;
}

int Suspend_Handler(int ev_id, char *ev_name, void *param, int *result)
{
	// SleepMode
	if(ev_id == 0x100)
	{
		flag = PAUSE_MUSIC;
		time = 0;

		sceKernelDelayThread(800000);
	}

	return 0;
}

void SetSpeed(void)
{
	if(clock_pos != last_clock)
	{
		last_clock = clock_pos;

		// Restore
		RestorePowerFunc();
		sceKernelDelayThread(50000);

		if(clock_pos == 0)
		{
			_setclock_func(222, 222, 111);
		}
		else
		{
			_setclock_func(cpulist[clock_pos], cpulist[clock_pos], buslist[clock_pos]);
		}
	}
}

void SkipFolder(int flag)
{
	int i;
	char buf[512];
	char *ptr;

	strcpy(buf, path_list[count].name);
	ptr = strrchr(buf, '/');
	if(ptr != NULL)
	{
		*ptr = '\0';

		if(flag == 1)
		{
			for(i = count; i < list_num; i++)
			{
				if(strstr(path_list[i].name, buf) == NULL)
				{
					count = i;
					return;
				}
			}

			for(i = 0; i < count; i++)
			{
				if(strstr(path_list[i].name, buf) == NULL)
				{
					count = i;
					return;
				}
			}

			count++;
			if(count > (list_num - 1)) count = 0;
		}
		else
		{
			for(i = count; i > 0; i--)
			{
				if(strstr(path_list[i].name, buf) == NULL)
				{
					count = i;
					return;
				}
			}

			for(i = (list_num - 1); i > count; i--)
			{
				if(strstr(path_list[i].name, buf) == NULL)
				{
					count = i;
					return;
				}
			}

			count--;
			if(count < 0) count = (list_num - 1);
		}
	}

	return;
}

void DrawMenu()
{
	// GetFileName
	char *str = strrchr(path_list[count].name, '/');
	if(!str)	strcpy(list_name, "[ERROR MP3PLAYERPLUGIN]");
	else
	{
		if(eboot_flag == 0) psp2chUTF82Sjis(list_name, (str + 1));
		else				strcpy(list_name, (str + 1));
	}

	// 5 seconds
	time = sceKernelLibcClock() + 1000000 * 5;

	if(draw_flag == 0)
	{
		if(draw_thid == 0)
		{
			draw_thid = sceKernelCreateThread("draw_thread", DrawThread, 8, 0x1000, 0, NULL);
			if(draw_thid >= 0) sceKernelStartThread(draw_thid, 0, 0);
		}
	}
	else
	{
		if(drawing == 0)
		{
			drawing = 1;
		}
	}

	return;
}

void Wait_Button(u32 buttons)
{
	SceCtrlData pad;

	buttons &= ~PSP_CTRL_LTRIGGER;
	buttons &= ~PSP_CTRL_RTRIGGER;

	pad.Buttons = buttons;

	// Wait
	while((pad.Buttons & buttons) != 0)
	{
		sceKernelDelayThread(1000);
		sceCtrlPeekBufferPositive(&pad, 1);
	}

	return;
}

int mp3_thread(SceSize args, void *argp)
{
	DecodeData mp3;

	// set
	mp3_run = 1;

	// Init
	if(MP3_Init(path_list[count].name, &mp3) < 0)
	{
		MP3_End(&mp3);
		flag = DONE_MUSIC;
		return sceKernelExitDeleteThread(0);
	}

	// GetChannel
	if(channel > 8)
		channel = sceAudioChReserve(PSP_AUDIO_NEXT_CHANNEL, 1152, PSP_AUDIO_FORMAT_STEREO);

	// Loop
	while(mp3_run)
	{
		sceKernelDelayThread(1000);

		if(flag == PLAYING_MUSIC && stop_flag == 0)
		{
			// Decode
			if(MP3_Decode(&mp3) < 0)
			{
				MP3_End(&mp3);
				flag = DONE_MUSIC;
				sceAudioChRelease(channel);
				channel = 9;

				return sceKernelExitDeleteThread(0);
			}

			// Play
			_sceAudioOutputBlocking(channel, (int)volume, mp3.output_buffer[mp3.output_num]);
		}
	}

	// Done
	MP3_End(&mp3);
	sceAudioChRelease(channel);
	channel = 9;

	if(flag == PLAYING_MUSIC) flag = DONE_MUSIC;
	return sceKernelExitDeleteThread(0);
}

void Make_MP3Thread()
{
	music_thid = sceKernelCreateThread("music_thread", mp3_thread, 1, 0x1000, 0, NULL);
	if(music_thid >= 0) sceKernelStartThread(music_thid, 0, 0);
	return;
}

int load_thread(SceSize args, void *argp)
{
	int count_n = 0;
	int pid = 1;
	int init = 0;

	// GetMode
	init = sceKernelInitKeyConfig();

	// VshMode
	if(init == PSP_INIT_KEYCONFIG_VSH)
	{
		// UserMemory
		pid = 2;

		// Not EBOOT
		eboot_flag = 0;
	}

	// Delay
	sceKernelDelayThread(4000000);

	// GameMode
	if(init == PSP_INIT_KEYCONFIG_GAME && sceKernelBootFrom() != PSP_BOOT_DISC)
	{
		// Check Driver
		eboot_flag = 1;
		int i;
		SceModule2 *mod = NULL;

		for(i = 0; i < 5; i++)
		{
			mod = sceKernelFindModuleByName(drvname[i]);
			if(mod != NULL) eboot_flag = 0;
		}
	}

	// GetKey
	Read_Conf((char *)argp, &key);

	// MP3FileCount
	list_num = FileCount(key.dirpath);
	if(list_num == 0)
	{
		sceKernelTerminateDeleteThread(main_thid);
		return -1;
	}

	// MemoryAlloc
	path_list = (FileList *)malloc_p(sizeof(FileList) * list_num, &list_id, pid);
	if(!path_list)
	{
		sceKernelTerminateDeleteThread(main_thid);
		return -1;
	}

	// MP3FileList
	GetFileList(key.dirpath, path_list, &count_n, pid);
	if(count_n == 0)
	{
		sceKernelTerminateDeleteThread(main_thid);
		return -1;
	}

	// file_count
	list_num = count_n;

	// GameMode
	if(init == PSP_INIT_KEYCONFIG_GAME)
	{
		// Delay
		sceKernelDelayThread(9000000);

		// LoadModule
		sceUtilityLoadAvModule(0);
	}

	// UnLockSema
	load_flag = 1;
	sceKernelSignalSema(load_sema, 1);

	return sceKernelExitDeleteThread(0);
}

int main_thread(SceSize args, void *argp)
{
	SceCtrlData pad;
	load_flag = 0;

	// LoadWait
	while(load_flag == 0)
	{
		// LockSema
		sceKernelWaitSema(load_sema, 1, 0);

		// Delay
		sceKernelDelayThread(50000);
	}

	// Delete
	sceKernelDeleteSema(load_sema);

	// Init
	clock_pos = 5;
	last_clock = 5;
	hook_run = 9;
	channel = 9;
	count = 0;
	flag = PAUSE_MUSIC;
	mode = NORMAL_MODE;
	volume = MAX_VOLUME;
	hold = 0;
	pos = 0;
	skip = 0;
	draw_flag = 0;
	music_thid = 0;
	draw_thid = 0;
	stop_flag = 0;

	// Loop
	while(1)
	{
		sceCtrlPeekBufferPositive(&pad, 1);

		// Done
		if(flag == DONE_MUSIC)
		{
			flag = PLAYING_MUSIC;

			// Next
			if(mode == NORMAL_MODE)
			{
				count++;
				if(count > (list_num - 1)) count = 0;
			}

			// Random
			else if(mode == RANDOM_MODE)
			{
				srand(sceKernelLibcClock());
				count = rand() % list_num;
			}

			Make_MP3Thread();

			DrawMenu();
		}

		if((pad.Buttons & key.Next_Key) == key.Next_Key && !hold)
		{
			switch(pos)
			{
				// MP3NAME
				case 0:
				{
					flag = NEXT_MUSIC;

					// Shutdown Thread
					mp3_run = 0;
					sceKernelDelayThread(50000 * 2);

					flag = PLAYING_MUSIC;

					if(skip)
					{
						SkipFolder(1);
					}
					else
					{
						count++;
						if(count > (list_num - 1)) count = 0;
					}

					Make_MP3Thread();

					DrawMenu();
					break;
				}

				// VOLUME
				case 1:
				{
					volume += MINI_VOLUME;
					if(volume > MAX_VOLUME) volume = MAX_VOLUME;

					DrawMenu();
					break;
				}

				// MODE
				case 2:
				{
					mode++;
					if(mode > 2) mode = 0;

					DrawMenu();
					break;
				}

				// MUTE
				case 3:
				{
					hook_run++;
					if(hook_run > 9) hook_run = 0;

					DrawMenu();
					break;
				}

				// SKIP
				case 4:
				{
					skip ^= 1;

					DrawMenu();
					break;
				}

				// DRAW
				case 5:
				{
					draw_flag ^= 1;

					DrawMenu();
					break;
				}

				// CPU
				case 6:
				{
					clock_pos++;
					if(clock_pos > 8) clock_pos = 0;

					DrawMenu();
					break;
				}
			}

			if(pos != 1)	Wait_Button(key.Next_Key);
			else			sceKernelDelayThread(60000);
		}

		else if((pad.Buttons & key.Prev_Key) == key.Prev_Key && !hold)
		{
			switch(pos)
			{
				// MP3NAME
				case 0:
				{
					flag = PREV_MUSIC;

					// Shutdown Thread
					mp3_run = 0;
					sceKernelDelayThread(50000 * 2);

					flag = PLAYING_MUSIC;

					if(skip)
					{
						SkipFolder(0);
					}
					else
					{
						count--;
						if(count < 0) count = (list_num - 1);
					}

					Make_MP3Thread();

					DrawMenu();
					break;
				}

				// VOLUME
				case 1:
				{
					volume -= MINI_VOLUME;
					if(volume < 0) volume = 0;

					DrawMenu();
					break;
				}

				// MODE
				case 2:
				{
					mode--;
					if(mode < 0) mode = 2;

					DrawMenu();
					break;
				}

				// MUTE
				case 3:
				{
					hook_run--;
					if(hook_run < 0) hook_run = 9;

					DrawMenu();
					break;
				}

				// SKIP
				case 4:
				{
					skip ^= 1;

					DrawMenu();
					break;
				}

				// DRAW
				case 5:
				{
					draw_flag ^= 1;

					DrawMenu();
					break;
				}

				// CPU
				case 6:
				{
					clock_pos--;
					if(clock_pos < 0) clock_pos = 8;

					DrawMenu();
					break;
				}
			}

			if(pos != 1)	Wait_Button(key.Prev_Key);
			else			sceKernelDelayThread(60000);
		}

		else if((pad.Buttons & key.Up_Key) == key.Up_Key && !hold)
		{
			pos++;
			if(pos > 6) pos = 0;
			DrawMenu();
			Wait_Button(key.Up_Key);
		}

		else if((pad.Buttons & key.Down_Key) == key.Down_Key && !hold)
		{
			pos--;
			if(pos < 0) pos = 6;
			DrawMenu();
			Wait_Button(key.Down_Key);
		}

		// Play & Stop
		else if((pad.Buttons & key.Play_Pause_Key) == key.Play_Pause_Key && !hold)
		{
			flag = (flag > 0) ? PAUSE_MUSIC : PLAYING_MUSIC;

			// Not Run Thread
			if(music_thid == 0)
			{
				Make_MP3Thread();
			}

			DrawMenu();
			Wait_Button(key.Play_Pause_Key);
		}

		// Hold
		else if((pad.Buttons & key.Hold_Key) == key.Hold_Key)
		{
			hold ^= 1;

			DrawMenu();
			Wait_Button(key.Hold_Key);
		}

		// Delay
		sceKernelDelayThread(50000);
	}

	return sceKernelExitDeleteThread(0);
}

int module_start(SceSize args, void *argp)
{
	u32 orgaddr = 0;
	SceModule2 *mod = NULL;

	// HookAudio
	mod = sceKernelFindModuleByName("sceAudio_Driver");
	if(mod != NULL)
	{
		HookAudioFunc();
	}

	// HookVaudio
	mod = sceKernelFindModuleByName("sceVaudio_driver");
	if(mod != NULL)
	{
		_sceVaudioOutputBlocking = (void *)FindProc("sceVaudio_driver", "sceVaudio", 0x8986295E);
		SyscallHelper("sceVaudio_driver", "sceVaudio", 0x8986295E, sceVaudioOutputBlockingPatched);
	}

	// HookDisplay
	mod = sceKernelFindModuleByName("sceDisplay_Service");
	if(mod != NULL)
	{
		HookDisplayFunc(mod->text_addr, mod->text_addr + mod->text_size);
	}

	// HookLoadExec
	mod = sceKernelFindModuleByName("sceLoadExec");
	if(mod != NULL)
	{
		HookLoadExecFunc(mod->text_addr, mod->text_addr + mod->text_size);
	}

	// HookSystemControl
	mod = sceKernelFindModuleByName("SystemControl");
	if(mod != NULL)
	{
		// sceKernelLoadExecVSHWithApitype
		REDIRECT_FUNCTION(FindProc("SystemControl", "SystemCtrlForKernel", 0x2D10FB28), LoadExecFuncPatched);
	}

	// GameMode
	if(sceKernelInitKeyConfig() == PSP_INIT_KEYCONFIG_GAME)
	{
		// HookPower
		mod = sceKernelFindModuleByName("scePower_Service");
		if(mod != NULL)
		{
			HookPowerFunc(mod->text_addr, mod->text_addr + mod->text_size);
		}

		// sceKernelExitGame
		mod = sceKernelFindModuleByName("sceLoadExec");
		if(mod != NULL)
		{
			orgaddr = FindProc("sceLoadExec", "LoadExecForUser", 0x05572A5F);
			if(orgaddr != 0) MAKE_CALL(orgaddr + 8, LoadExecForUser_D1FB50DCPatched);
		}

		// HookIoFunc
		mod = sceKernelFindModuleByName("sceIOFileManager");
		if(mod != NULL)
		{
			// GetsceIoFunc
			_sceIoRead = (void *)FindProc("sceIOFileManager", "IoFileMgrForUser", 0x6A638D83);
			_sceIoWrite = (void *)FindProc("sceIOFileManager", "IoFileMgrForUser", 0x42EC03AC);

			// sceIoPatch
			SyscallHelper("sceIOFileManager", "IoFileMgrForUser", 0x6A638D83, sceIoReadPatched);
			SyscallHelper("sceIOFileManager", "IoFileMgrForUser", 0x42EC03AC, sceIoWritePatched);
		}
	}
	else
	{
		// GetPowerFunc
		mod = sceKernelFindModuleByName("scePower_Service");
		if(mod != NULL)
		{
			_setclock_func = (void *)scePowerFindProc(0x545A7F3C);
		}
	}

	// LoadControl
	load_sema = sceKernelCreateSema("LoadControl", 0, 1, 1, 0);

	// EventCallback
	sceKernelRegisterSysEventHandler(&events);

	// MainThread
	main_thid = sceKernelCreateThread("mp3player_main", main_thread, 16, 0x1500, 0, NULL);
	if(main_thid >= 0) sceKernelStartThread(main_thid, args, argp);

	// LoadThread
	SceUID load = sceKernelCreateThread("load_thread", load_thread, 16, 0x1000, 0, NULL);
	if(load >= 0) sceKernelStartThread(load, args, argp);

	// clear
	ClearCaches();
	return 0;
}

int module_stop(SceSize args, void *argp)
{
	return 0;
}

