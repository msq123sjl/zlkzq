#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

#include "tinz_base_def.h"

#define MAX_WATCH_PROGS	8

typedef struct _ProgInfo{
	pid_t 	pid;
	char	Name[MAX_FILENAME_SIZE];
	unsigned long runTimes;
}ProgInfo;

int daemonize();
void parent_signal_treatment(int param);
void signal_treatment(int param);

void CheckProgs();
void KillProgs();
unsigned int KillProc(char* name);
void CheckZombieProc();
void STHL(void);
void sigalrm_fn(int sig);
void ReadWatchProgs();
void crontab_start();
void crontab_stop();

#endif
