#include <features.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <string.h>
#include <paths.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <ctype.h> 

#include <sys/select.h>

#include <stddef.h>   
#include <errno.h>
#include <sys/procfs.h>   
#include <stropts.h>   
#include <dirent.h>   
#include <sys/signal.h> 
#include <sys/syscall.h> 
#include <sys/param.h> 
#include <linux/rtc.h>

#include "em335x_drivers.h"

#include "watchdog.h"
#include "tinz_common_helper.h"
#include "tinz_base_def.h"
#include "config_file.h"
#include "tinz_base_data.h"
#include "tinz_pub_shm.h" 

#define	STH   	0
#define	STL 	1	
//#define	RUN_DEV	"/dev/tc0/RUN"
int rebootflag=0;
unsigned char	reb_count=0;  //僵尸进程连续计数
unsigned char	delayxx=0;   //程序启动后前3s 运行灯不闪烁

static ProgInfo progs[MAX_WATCH_PROGS];
static pid_t the_pid=0;
static int wait_for_daemon_status = 0;
static ProgInfo defaultprogs[]={{0,"dataproc",0},{0,"interface",0},{0,"up_main",0},{0,"ValveControl",0}};

int gPrintLevel = 5;
static int watchdog_fd = 0;
pstData pgData;

/*static void pabort(const char *s)
{
    perror(s);
    abort();
}*/

int main(int argc, char *argv[], char *env[]){
	pid_t parent_pid;
	int count=0;
    int lte_count = 0;
    int lte_flag = 0;
	sleep(1);
	parent_pid = getpid();

    pgData = (pstData)getDataShm();
	wait_for_daemon_status = 1;
	daemonize();

    // SIGNALS treatment
	signal(SIGHUP, signal_treatment);
	signal(SIGPIPE, signal_treatment);
	signal(SIGKILL, signal_treatment);
	signal(SIGINT, signal_treatment);
    signal(SIGUSR1, signal_treatment);
	signal(SIGTERM, signal_treatment);

	if (wait_for_daemon_status) {
		kill(parent_pid, SIGHUP);
	}
	
	ReadWatchProgs();
    watchdog_fd = open("/dev/watchdog", O_RDONLY);
	signal(SIGALRM, sigalrm_fn);
    alarm(1);
	
	KillProgs();

    /*int event_fd;
    event_fd = open("/dev/event0", O_RDWR);
    if (event_fd < 0)
        pabort("can't open /dev/event0");
	fd_set rd;
    struct timeval tv;
    int err;
    FD_ZERO(&rd);
    FD_SET(event_fd,&rd);
    tv.tv_sec = 10;
    tv.tv_usec = 0;*/

	while(1){     
        //DEBUG_PRINT_INFO(gPrintLevel,"Watchdog proc cnt[%d]\r\n",count);
        /*err = select(event_fd+1,&rd,NULL,NULL,&tv);
        DEBUG_PRINT_INFO(gPrintLevel,"event0 err[%d][%d]\r\n",err,event_fd);*/
	    count++;
	    if(delayxx<20)
	   	    delayxx++;
		if(rebootflag){
			sleep(1);
			continue;
		}
		CheckProgs();				
		
		if((count%10)==0)
			CheckZombieProc();
        /*4G网络已连接 启动ping定时任务*/
        lte_count = lte_count < 120 ? lte_count+1 : 121;
        if(0 == pgData->state.LTE && lte_count > 120 && 0 == lte_flag){
            lte_flag = 1;
            crontab_start();
        }
        
		sleep(1);
        
	}	
}

void crontab_start(){
    DEBUG_PRINT_ERR(gPrintLevel,"[Watchdog] crond start\n");
    system("crond");
}
//创建主机
int daemonize()
{
   	signal(SIGHUP, parent_signal_treatment);
	the_pid=fork();  //成功调用一次则返回两个值，子进程返回0，父进程返回子进程ID
   	if( the_pid< 0) {
        wait_for_daemon_status = 0;
		return(-1);
	}
   	else if(the_pid!=0) {
    	if (wait_for_daemon_status) {
        	wait(NULL);
	    	fprintf(stderr, "Watch dog startup failed.\n");
            exit(1);
        } 
        exit(0);//parent goes bye-bye
    }

	//child continues
	setsid();   //become session leader
	if(wait_for_daemon_status == 0 && (the_pid = fork()) != 0)
		exit(0);
	chdir("/"); //change working directory
	umask(0);// clear our file mode creation mask

    // Close out the standard file descriptors
//    close(STDIN_FILENO);
//    close(STDOUT_FILENO);
//    close(STDERR_FILENO);

	return 0;
}


//等待子进程杀死父进程
void parent_signal_treatment(int param){
  fprintf(stderr, "Watch dog started\n");
  exit(0);
}

// 信号响应
void signal_treatment(int param){
	switch(param)
	{
		case SIGPIPE:
			exit(0);
			break;
		case SIGHUP:
			//reread the configuration files
			break;
		case SIGINT:
			//exit(0);
			break;
		case SIGKILL:
			//exit(0);
			break;
		case SIGUSR1:
			break;
		case SIGTERM:
			break;
		default:
			exit(0);
			break;
	}
}

void CheckProgs(){
	int i=0,status=0;
	pid_t pid;
	char cmd[MAX_FILENAME_SIZE];
	for(i=0;i<sizeof(progs)/sizeof(ProgInfo);i++){		
		if(strlen(progs[i].Name)==0)continue;
		if(progs[i].pid>0)continue;
        sprintf(cmd,"%s/%s",FS_NAME_PROGDIR,progs[i].Name);
		progs[i].runTimes++;
		DEBUG_PRINT_ERR(gPrintLevel,"[Watchdog] now %ldth start %s.\r\n",progs[i].runTimes,progs[i].Name);
		if(!(progs[i].pid=fork())){
            if(0 != strncmp(progs[i].Name,"interface",strlen("interface"))){
                execvp(cmd,0);
            }else{
                char *argv[] = {"interface","-qws",0};
                execvp(cmd,argv);
            }
			exit(1);
		}
		setpgid(progs[i].pid,the_pid);

		sleep(1);
	}

	while((pid=waitpid(-1,&status,WNOHANG))>0){
		for(i=0;i<sizeof(progs)/sizeof(ProgInfo);i++){
			if(pid==progs[i].pid){
				progs[i].pid=0;
			}
		}
	}
}

unsigned int KillProc(char* name){
	DIR *dir;
    struct dirent *next;
   //int i=0;
	pid_t pid=-1; 
	FILE *status;
	char filename[MAX_FILENAME_SIZE];
	char buffer[MAX_FILENAME_SIZE];
	char procname[MAX_FILENAME_SIZE];
	int count=0;
    dir = opendir("/proc");
    if (!dir){
        fprintf(stderr, "Cannot open /proc\n");
		return count;
    }
    while ((next = readdir(dir)) != NULL){
        /* Must skip ".." since that is outside /proc */
        if (strcmp(next->d_name, "..") == 0)continue;

        /* If it isn't a number, we don't want it */
        if (!isdigit(*next->d_name))continue;

        sprintf(filename, "/proc/%s/status", next->d_name);
        if (! (status = fopen(filename, "r")) ) continue;

		/* Read first line in /proc/?pid?/status */
        if (fgets(buffer, MAX_FILENAME_SIZE-1, status) == NULL){
            fclose(status);
            continue;
        }
        fclose(status);

        /* Buffer should contain a string like "Name:   binary_name" */
		sscanf(buffer, "%*s %s", procname);
        if (strcmp(procname, name) == 0) {
			pid=(pid_t)atoi(next->d_name);    
            kill(pid,SIGKILL);//SIGSTOP  
			count++;
		}
    }
	closedir(dir);
    return count;
}

void KillProgs(){
	int i=0;

	for(i=0;i<sizeof(progs)/sizeof(ProgInfo);i++){
		if(progs[i].pid>0)kill(progs[i].pid,SIGSTOP);  
		progs[i].pid=0;
	}
	
	for(i=0;i<sizeof(progs)/sizeof(ProgInfo);i++){
		progs[i].pid=0;
		if(strlen(progs[i].Name)>0)
			KillProc(progs[i].Name);
	}
}

void ReliableSleep(unsigned long howlong){
	unsigned long count=0,equescount=0;
	unsigned char timereliable=1;
	time_t oldtime=0,newtime=0;
	if(time(&oldtime)==(time_t)-1)
		timereliable=0;
	
	while(1){
		if(timereliable){
			if(time(&newtime)==(time_t)-1)
				timereliable=0;
			if(newtime<oldtime)
				timereliable=0;
			if(newtime==oldtime)
				equescount++;
			if(equescount>=3)
				timereliable=0;				
		}

		if(timereliable){
			if((count>=howlong)&&((newtime-oldtime)>=howlong)){
				DEBUG_PRINT_INFO(gPrintLevel,"[Watchdog] Reliable Sleep back from 1: count=%ld,howlong=%ld,delt=%ld\r\n",count,howlong,(unsigned long)(newtime-oldtime));
				break;
			}
		}
		else{
			if(count>=howlong){
				DEBUG_PRINT_INFO(gPrintLevel,"[Watchdog] Reliable Sleep back from 2: count=%ld,howlong=%ld\r\n",count,howlong);		
				break;
			}
		}
		
		count++;
		sleep(1);
	}
}


void sigalrm_fn(int sig){
   	int i=0;
	unsigned long runTimes=0;
   	for(i=0;i<sizeof(progs)/sizeof(ProgInfo);i++){
		runTimes+=progs[i].runTimes;
   	}
    if(runTimes<100){
		STHL();
        //DEBUG_PRINT_INFO(gPrintLevel,"Watchdog\r\n");
        ioctl(watchdog_fd, WDIOC_KEEPALIVE, 0);
    }
	alarm(1);
    return;
}

void STHL(void){
  int fd;
  static char Run_Led=STH;
  //FtpControl* ftpServer=(FtpControl*)Ftp_GetFtpCtrlShm();
  //if(ftpServer&&ftpServer->StopFeedDog)return;
  if(rebootflag)return;
  if(delayxx<3)return;
  //fd=open(RUN_DEV,O_RDWR);
  fd = 0;
  if(fd>=0)
  {
     ioctl(fd,Run_Led,NULL);
     close(fd);
     if(Run_Led==STH)
        Run_Led=STL;
     else
        Run_Led=STH;
   }
}

void CheckZombieProc(){
	DIR   *pdir=NULL;   
  	struct dirent *pde=NULL;   
  	FILE   *pf=NULL;   
  	char   buff[MAX_FILENAME_SIZE];//,buff2[MAX_FILENAME_SIZE];   
  	char   *pstr =NULL;   
  	int   pid=0,zombieCount=0,n;  
    static int pppdid=0,pppdcount=0;
  	pdir=opendir("/proc");   
  	if(!pdir){   
  		perror("open /proc fail.\n");   
  		return ;   
  	}   
    
  	while((pde=readdir(pdir))){  
        //printf("d_name:%s\n",pde->d_name);
		if((pde->d_name[0]<'0')||(pde->d_name[0]>'9')){   
  			continue;   
  		}   
	  	sprintf(buff,"/proc/%s/status",pde->d_name);   
	  	pf=fopen(buff,"r");   
	  	if(pf){   
	  		n=fread(buff,1,MAX_FILENAME_SIZE-1,pf);   
	  		fclose(pf);   
	  		buff[n]=0;  
	  		pstr=strstr(buff,"(zombie)");
	  		if(pstr==NULL)pstr=strstr(buff,"(stopped)");			
	  		if(pstr){   
	  			pid=atoi(pde->d_name);   
	  			//kill(pid,SIGKILL);  
	  			//printf("pid=%d is zombie progress,kill it.\r\n",pid);
	  			//printf(" %s\n ",buff);
	  			zombieCount++;
				if(strstr(buff,"pppd")){
					if(pppdid==pid)
						pppdcount++;
					else{
						pppdcount=1;
						pppdid=pid;
					}
				}
	  		}
  		}
        pstr=NULL;
        pf=NULL;
  	}   
  	closedir(pdir);  
	//printf("\nzombiecount=%d, pppdcount=%d, gsmttycount=%d\n",zombieCount,pppdcount,gsmttycount);
	//if(zombieCount>=2)	
		//system("ps \n");
		
	if(zombieCount>=2||pppdcount>=9){
		DEBUG_PRINT_ERR(gPrintLevel,"[Watchdog] zombiecount=%d, pppdcount=%d\n",zombieCount,pppdcount);
		reb_count++;
		if(reb_count>20){
			DEBUG_PRINT_ERR(gPrintLevel,"[Watchdog] Found too more zombie or stopped,Now system reboot!!!.\r\n");
			rebootflag=1;
			system("reboot -d 10 -n\n");
			}
	}
	else{
		reb_count=0;
    }
}

void ReadWatchProgs(){
	int i=0,nCount=0;
	char filename[128],nodename[16],strValue[64];
	sprintf(filename,"%s",WATCHDOG_CONFIG_FILE);
	
	//各守护程序信息
	memset(progs,0,sizeof(ProgInfo)*MAX_WATCH_PROGS);
	memset(strValue,0,64);		
	for(i=0;i<MAX_WATCH_PROGS;i++){		
		progs[i].pid=0;
		progs[i].Name[0]=0;
		sprintf(nodename,"prog%d",i);
		if(getconfigstring("WatchProgs",nodename,strValue,32,filename)==CFG_FILE_NOERROR&&strlen(strValue)>0){
			memcpy(progs[nCount].Name,strValue,32);
			progs[nCount].Name[32]=0;
			DEBUG_PRINT_INFO(gPrintLevel,"[Watchdog] will watch program%d %s.\r\n",nCount,progs[nCount].Name);
			nCount++;
		}
	}
	if(nCount<=0){
		for(i=0;i<sizeof(defaultprogs)/sizeof(ProgInfo);i++){
			memcpy(&progs[i],&defaultprogs[i],sizeof(ProgInfo));
		}
	}
}

