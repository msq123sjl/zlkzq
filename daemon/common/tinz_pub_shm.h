#ifndef __TINZ_PUB_SHM__
#define __TINZ_PUB_SHM__

#define FS_NAME_PARA_CONF	"/mnt/nandflash/para/fs_para.conf"
#define FS_NAME_PARA		"/mnt/nandflash/para/fs_para.dat"
#define FS_PATH_PARA		"/mnt/nandflash/para/"
#define SHM_NAME_PARA		"/mnt/nandflash/shm/shm_para"
#define SHM_PATH_PARA		"/mnt/nandflash/shm/"
#define SHM_PARA_ID	0x01

#define FS_NAME_NET_PARA_CONF	"/mnt/nandflash/para/fs_net_para.conf"
#define FS_NAME_NET_PARA		"/mnt/nandflash/para/fs_net_para.dat"
#define FS_PATH_NET_PARA		"/mnt/nandflash/para/"
#define SHM_NAME_NET_PARA		"/mnt/nandflash/shm/shm_net_para"
#define SHM_PATH_NET_PARA		"/mnt/nandflash/shm/"
#define SHM_PARA_NET_ID	0x01

#define FS_NAME_VALVE_PARA		"/mnt/nandflash/para/fs_valve_para.dat"
#define FS_PATH_VALVE_PARA		"/mnt/nandflash/para/"
#define SHM_NAME_VALVE_PARA		"/mnt/nandflash/shm/shm_valve_para"
#define SHM_PATH_VALVE_PARA		"/mnt/nandflash/shm/"
#define SHM_PARA_VALVE_ID	0x01

#define FS_NAME_CALIBRATION_PARA		"/mnt/nandflash/para/fs_calbration_para.dat"
#define FS_PATH_CALIBRATION_PARA		"/mnt/nandflash/para/"
#define SHM_NAME_CALIBRATION_PARA		"/mnt/nandflash/shm/shm_calbration_para"
#define SHM_PATH_CALIBRATION_PARA		"/mnt/nandflash/shm/"
#define SHM_PARA_CALIBRATION_ID	0x01

#define SHM_NAME_POLLUTANT_DATA		"/mnt/nandflash/shm/shm_pollutant_data"
#define SHM_PATH_POLLUTANT_DATA		"/mnt/nandflash/shm/"
#define SHM_DATA_POLLUTANT_ID	0x01

#define FS_NAME_POLLUTANT_PARA		"/mnt/nandflash/para/fs_pollutant_para.dat"
#define FS_PATH_POLLUTANT_PARA		"/mnt/nandflash/para/"
#define SHM_NAME_POLLUTANT_PARA		"/mnt/nandflash/shm/shm_pollutant_para"
#define SHM_PATH_POLLUTANT_PARA		"/mnt/nandflash/shm/"
#define SHM_PARA_POLLUTANT_ID	0x01

#define FS_NAME_DATA		"/mnt/nandflash/para/fs_data.dat"
#define FS_PATH_DATA		"/mnt/nandflash/para/"
#define SHM_NAME_DATA		"/mnt/nandflash/shm/shm_data"
#define SHM_PATH_DATA		"/mnt/nandflash/shm/"
#define SHM_DATA_ID	0x01

#define SHM_NAME_HISTORY_DATA		"/mnt/nandflash/shm/shm_history_data"
#define SHM_PATH_HISTORY_DATA		"/mnt/nandflash/shm/"
#define SHM_DATA_HISTORY_ID	0x01

struct  SHM_DESC{
	int shm_id;
	char *shm_mem;
};

char * getParaShm();
void rmParaShm();
void syncParaShm();
void initParaShm();

char * getNetParaShm();
void rmNetParaShm();
void syncNetParaShm();
void initNetParaShm();

char * getValveParaShm();
void rmValveParaShm();
void syncValveParaShm();
void initValveParaShm();
char * getCalibrationParaShm();
void rmCalibrationParaShm();
void syncCalibrationParaShm();
void initCalibrationParaShm();
char * getPollutantDataShm();
void rmPollutantDataShm();
char * getPollutantParaShm();
void rmPollutantParaShm();
void syncPollutantParaShm();
void initPollutantParaShm();
char * getDataShm();
void syncDataShm();
void initDataShm();
void rmDataShm();
char * getHistoryDataShm();
void rmHistoryDataShm();
void initHistoryDataShm();
#endif