/*
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 2.1 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


This program is based on xawtv code. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <ctype.h>
#include <syscall.h>

#include <sys/time.h>
#include <sys/ioctl.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "libv4lctrl.h"

/* --------------------------------------------------------------------- */
/* libv4l control via shm */

int try_ged_shm_id(struct v4ldevice *vd, char *device){
	struct v4lcontrol_shared *data; 
	key_t data_key = ftok( device, SHM_DATA_ID);
	key_t semaphores_key = ftok( device, SHM_SEMAPTHORES_ID);
	int id;
	char buf1[1024];
	FILE *fr;
	//char buf2[256];

	if ((id = semget(semaphores_key, SEMAPHORES_COUNT, 0666 )) < 0) {
		return id;
	}
    
	vd->shm_semaphores_id = id;

	if (( id = shmget(data_key, sizeof( struct v4lcontrol_shared ), 0666)  ) < 0) {
		vd->shm_semaphores_id = -1;
		vd->shm_data_id = -1;
		return id;
	}
	vd->shm_data_id = id;
	
	if ((data = (struct v4lcontrol_shared *)shmat(vd->shm_data_id, NULL, 0)) == (char *) -1) {
		perror("shmat");
		vd->shm_semaphores_id = -1;
		vd->shm_data_id = -1;
		return -1;
	}
	
	sprintf(buf1, "/proc/%d/cmdline", data->lib_pid);
	fr = fopen(buf1, "rb");
	if (!fr) {
		vd->shm_semaphores_id = -1;
		vd->shm_data_id = -1;
		return -1;
	}
	fread(buf1, 1, sizeof(buf1), fr);
	buf1[ sizeof(buf1) -1 ] = 0;
	fclose(fr);
	
	printf("%s is controled by application with pid %d (\"%s\")\n",device, data->lib_pid,buf1);
	if ( shmdt( data ) < 0 ) {
		perror("shmdt");
		vd->shm_semaphores_id = -1;
		vd->shm_data_id = -1;
		return -1;
	}
	
	return 0;
}

int binary_semaphore_down (int semid, int counterId){
    //int av = binary_semaphore_get_value(semid, counterId);
    //printf("binary_semaphore_down(%d) / %d \n",counterId,av);
	struct sembuf operation;
	operation.sem_num = counterId;           /* Use the X counter. */
	operation.sem_op = -1;           /* Decrement by 1. */
	operation.sem_flg = 0;    /* Permit undo'ing. */
	return semop (semid, &operation, 1);
}

int binary_semaphore_up(int semid, int counterId){
    //int av = binary_semaphore_get_value(semid, counterId);
    //printf("binary_semaphore_up(%d) / %d \n",counterId,av);
	struct sembuf operation;
	operation.sem_num = counterId;           /* Use the X counter. */
	operation.sem_op = 1;           /* increment by 1. */
	operation.sem_flg = 0;    /* Permit undo'ing. */
	return semop (semid, &operation, 1);
}

int doioctl(struct v4ldevice *vd,unsigned long int ctl,void *param, int size){
	//printf("ioctl 0x%x param length %d\n",ctl, size);
	struct v4lcontrol_shared *data; 
	int result = -1;
	char buf1[1024];
	FILE *fr;
    

	if (vd->access == DIRECT){
		int result = ioctl(vd->fd,ctl,param);
		//printf("%d\n", result);
		return result;
	}
	
	if (vd->shm_semaphores_id == -1 || vd->shm_data_id == -1)
		return -1;
  
  //printf("\nget client lock...\n");
	if (binary_semaphore_down(vd->shm_semaphores_id, SEMAPHORE_CLIENT_SEM_ID) < 0)
		return -1;
	
  // PUSH DATA
	if ((data = (struct v4lcontrol_shared *)shmat(vd->shm_data_id, NULL, 0)) == (char *) -1) {
		perror("shmat error");
		return -1;
	}
	
	// try if host application still running
	sprintf(buf1, "/proc/%d/cmdline", data->lib_pid);
	fr = fopen(buf1, "rb");
	if (!fr) {
		vd->shm_semaphores_id = -1;
		vd->shm_data_id = -1;
		fprintf(stderr,"Application with PID %d isn't running\n",data->lib_pid);
		return -1;
	}
	fread(buf1, 1, sizeof(buf1), fr);
	buf1[ sizeof(buf1) -1 ] = 0;
	fclose(fr);
	
	data->ioctl_request = ctl;
	memcpy(&data->arg, param, size);
    
  //printf("\nsend data...\n");
    
	if (binary_semaphore_up(vd->shm_semaphores_id, SEMAPHORE_LIB_DATA_SEM_ID) < 0)
		return -1;
	if (binary_semaphore_down(vd->shm_semaphores_id, SEMAPHORE_CLIENT_DATA_SEM_ID) < 0)
		return -1;
  // POP DATA
    
	result = data->result;
	memcpy(param, &data->arg, size);
	if (binary_semaphore_up(vd->shm_semaphores_id, SEMAPHORE_CLIENT_SEM_ID) < 0 )
		return -1;
    
	if ( shmdt( data ) < 0 ) {
		perror("shmdt error");
		return -1;
	}
    
	return result;
}

/* --------------------------------------------------------------------- */
/* getCotrol                                                             */

int getCotrol(struct v4ldevice *vd, unsigned long int cid, unsigned long int *value){
	int result;
	struct v4l2_control ctrl = {
		.id = cid
	};
	result = doioctl(vd,VIDIOC_G_CTRL,&ctrl,sizeof(ctrl));
	*value = ctrl.value;
	
	return result;
}

/* --------------------------------------------------------------------- */
/* setCotrol                                                             */

int setCotrol(struct v4ldevice *vd, unsigned long int cid, unsigned long int value){
	int result;
	struct v4l2_control ctrl = {
		.id = cid,
	.value = value
	};
	return doioctl(vd,VIDIOC_S_CTRL,&ctrl,sizeof(ctrl));
}

int v4ldevice_open(struct v4ldevice *vd, char *devf, int force_direct){
	
	if (force_direct || try_ged_shm_id(vd,devf) < 0 ){
		printf("try direct access to %s\n",devf);    
		//vd->fd = open(devf,O_RDONLY);
		vd->fd = syscall(SYS_open, devf, O_RDONLY);
		vd->access = DIRECT;
		printf("device %s open for read\n",devf);
		return 0;
	}else{
		printf("try communicate via shm\n");
	}
	
	if (-1 == vd->fd) {
		fprintf(stderr,"open %s: %s\n",devf,strerror(errno));
		return -1;
	}
	return 0;
}	

int v4ldevice_close(struct v4ldevice *vd){
	//printf("v4ldevice_close (0x%x)\n",vd->fd);
	if (vd->access == DIRECT){
		return close(vd->fd);
		//return syscall(SYS_close, vd->fd);
	}
	
	return 0;
}

