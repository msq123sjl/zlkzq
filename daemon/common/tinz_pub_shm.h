#ifndef __TINZ_PUB_SHM__
#define __TINZ_PUB_SHM__

#define FS_NAME_PARA		"/mnt/nandflash/para/fs_para.dat"
#define FS_PATH_PARA		"/mnt/nandflash/para/"
#define SHM_NAME_PARA		"/mnt/nandflash/shm/shm_para"
#define SHM_PATH_PARA		"/mnt/nandflash/shm/"
#define SHM_PARA_ID	0x01

#define FS_NAME_VALVE_PARA		"/mnt/nandflash/para/fs_valve_para.dat"
#define FS_PATH_VALVE_PARA		"/mnt/nandflash/para/"
#define SHM_NAME_VALVE_PARA		"/mnt/nandflash/shm/shm_valve_para"
#define SHM_PATH_VALVE_PARA		"/mnt/nandflash/shm/"
#define SHM_PARA_VALVE_ID	0x01

#define SHM_NAME_POLLUTANT_DATA		"/mnt/nandflash/shm/shm_pollutant_data"
#define SHM_PATH_POLLUTANT_DATA		"/mnt/nandflash/shm/"
#define SHM_DATA_POLLUTANT_ID	0x01

#define FS_NAME_POLLUTANT_PARA		"/mnt/nandflash/para/fs_pollutant_para.dat"
#define FS_PATH_POLLUTANT_PARA		"/mnt/nandflash/para/"
#define SHM_NAME_POLLUTANT_PARA		"/mnt/nandflash/shm/shm_pollutant_para"
#define SHM_PATH_POLLUTANT_PARA		"/mnt/nandflash/shm/"
#define SHM_PARA_POLLUTANT_ID	0x01

#define SHM_NAME_DATA		"/mnt/nandflash/shm/shm_data"
#define SHM_PATH_DATA		"/mnt/nandflash/shm/"
#define SHM_DATA_ID	0x01

struct  SHM_DESC{
	int shm_id;
	char *shm_mem;
};

char * getParaShm();
void rmParaShm();
void syncParaShm();
void initParaShm();

char * getValveParaShm();
void rmValveParaShm();
void syncValveParaShm();
void initValveParaShm();
char * getPollutantDataShm();
void rmPollutantDataShm();
char * getPollutantParaShm();
void rmPollutantParaShm();
void syncPollutantParaShm();
void initPollutantParaShm();
char * getDataShm();
void rmDataShm();
#endif