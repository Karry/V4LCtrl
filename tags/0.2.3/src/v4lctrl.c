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

#include <sys/time.h>
#include <sys/ioctl.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "videodev.h"
#include "videodev2.h"

#include "struct-dump.h"
#include "struct-v4l.h"
#include "struct-v4l2.h"

#include "v4lctrl.h"

/* --------------------------------------------------------------------- */
/* libv4l control via shm */

int try_ged_shm_id(char *device){
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
    
    shm_semaphores_id = id;

    if (( id = shmget(data_key, sizeof( struct v4lcontrol_shared ), 0666)  ) < 0) {
		shm_semaphores_id = -1;
		shm_data_id = -1;
		return id;
    }
	shm_data_id = id;
	
	if ((data = (struct v4lcontrol_shared *)shmat(shm_data_id, NULL, 0)) == (char *) -1) {
		perror("shmat");
		shm_semaphores_id = -1;
		shm_data_id = -1;
		return -1;
	}
	
	//(char *)&buf1 = ;
	//strcpy((char *)&buf1, "/proc/");
	sprintf(buf1, "/proc/%d/cmdline", data->lib_pid);
	fr = fopen(buf1, "rb");
	if (!fr) {
		shm_semaphores_id = -1;
		shm_data_id = -1;
		return -1;
	}
	fread(buf1, 1, sizeof(buf1), fr);
	buf1[ sizeof(buf1) -1 ] = 0;
	fclose(fr);
	//readlink (__const char *__restrict __path,char *__restrict __buf, size_t __len);
	
	printf("%s is controled by application with pid %d (\"%s\")\n",device, data->lib_pid,buf1);
	if ( shmdt( data ) < 0 ) {
		perror("shmdt");
		shm_semaphores_id = -1;
		shm_data_id = -1;
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

static int doioctl(int fd,unsigned long int ctl,void *param, int size){
    struct v4lcontrol_shared *data; 
    int result = -1;
    
 	//printf("ioctl 0x%x param length %d\n",ctl, size);

    if (shm_data_id == -1 || shm_semaphores_id == -1){
        //printf("direct ioctl\n");
        return ioctl(fd,ctl,param);
    }
    
    //printf("\nget client lock...\n");
    binary_semaphore_down(shm_semaphores_id, SEMAPHORE_CLIENT_SEM_ID);
    // PUSH DATA
    if ((data = (struct v4lcontrol_shared *)shmat(shm_data_id, NULL, 0)) == (char *) -1) {
        perror("shmat");
    }
    data->ioctl_request = ctl;
    memcpy(&data->arg, param, size);
    
    //printf("\nsend data...\n");
    
    binary_semaphore_up(shm_semaphores_id, SEMAPHORE_LIB_DATA_SEM_ID);
    binary_semaphore_down(shm_semaphores_id, SEMAPHORE_CLIENT_DATA_SEM_ID);
    // POP DATA
    
    result = data->result;
    memcpy(param, &data->arg, size);
    binary_semaphore_up(shm_semaphores_id, SEMAPHORE_CLIENT_SEM_ID);
    
    if ( shmdt( data ) < 0 ) {
        perror("shmdt");
        return -1;
    }
    
    return result;

}

/* --------------------------------------------------------------------- */
/* v4l(1)                                                                */

static int dump_v4l(int fd, int tab)
{
	struct video_capability  capability;
	struct video_channel     channel;
	struct video_tuner       tuner;
	struct video_audio       audio;
	struct video_picture     picture;
	struct video_buffer      buffer;
	struct video_window      window;
	unsigned int i;

	printf("general info\n");
	memset(&capability,0,sizeof(capability));
	if (-1 == doioctl(fd,VIDIOCGCAP,&capability, sizeof(capability)))
		return -1;
	printf("    VIDIOCGCAP\n");
	print_struct(stdout,desc_video_capability,&capability,"",tab);
	printf("\n");

	printf("channels\n");
	for (i = 0; i < capability.channels; i++) {
		memset(&channel,0,sizeof(channel));
		channel.channel = i;
		if (-1 == doioctl(fd,VIDIOCGCHAN,&channel,sizeof(channel))) {
			perror("ioctl VIDIOCGCHAN");
			continue;
		}
		printf("    VIDIOCGCHAN(%d)\n",i);
		print_struct(stdout,desc_video_channel,&channel,"",tab);
	}
	printf("\n");

	printf("tuner\n");
	memset(&tuner,0,sizeof(tuner));
	if (-1 == doioctl(fd,VIDIOCGTUNER,&tuner,sizeof(tuner))) {
		perror("ioctl VIDIOCGTUNER");
	} else {
		printf("    VIDIOCGTUNER\n");
		print_struct(stdout,desc_video_tuner,&tuner,"",tab);
	}
	printf("\n");

	printf("audio\n");
	memset(&audio,0,sizeof(audio));
	if (-1 == doioctl(fd,VIDIOCGAUDIO,&audio,sizeof(audio))) {
		perror("ioctl VIDIOCGAUDIO");
	} else {
		printf("    VIDIOCGAUDIO\n");
		print_struct(stdout,desc_video_audio,&audio,"",tab);
	}
	printf("\n");

	printf("picture\n");
	memset(&picture,0,sizeof(picture));
	if (-1 == doioctl(fd,VIDIOCGPICT,&picture,sizeof(picture))) {
		perror("ioctl VIDIOCGPICT");
	} else {
		printf("    VIDIOCGPICT\n");
		print_struct(stdout,desc_video_picture,&picture,"",tab);
	}
	printf("\n");

	printf("buffer\n");
	memset(&buffer,0,sizeof(buffer));
	if (-1 == doioctl(fd,VIDIOCGFBUF,&buffer,sizeof(buffer))) {
		perror("ioctl VIDIOCGFBUF");
	} else {
		printf("    VIDIOCGFBUF\n");
		print_struct(stdout,desc_video_buffer,&buffer,"",tab);
	}
	printf("\n");

	printf("window\n");
	memset(&window,0,sizeof(window));
	if (-1 == doioctl(fd,VIDIOCGWIN,&window,sizeof(window))) {
		perror("ioctl VIDIOCGWIN");
	} else {
		printf("    VIDIOCGWIN\n");
		print_struct(stdout,desc_video_window,&window,"",tab);
	}
	printf("\n");

	return 0;
}

/* --------------------------------------------------------------------- */
/* v4l2                                                                  */

static int dump_v4l2(int fd, int tab)
{
	struct v4l2_capability  capability;
	struct v4l2_standard    standard;
	struct v4l2_input       input;
	struct v4l2_tuner       tuner;
	struct v4l2_fmtdesc     fmtdesc;
	struct v4l2_format      format;
	struct v4l2_framebuffer fbuf;
	struct v4l2_queryctrl   qctrl;
	int i;

	printf("general info\n");
	memset(&capability,0,sizeof(capability));
	if (-1 == doioctl(fd,VIDIOC_QUERYCAP,&capability,sizeof(capability)))
		return -1;
	printf("    VIDIOC_QUERYCAP\n");
	print_struct(stdout,desc_v4l2_capability,&capability,"",tab);
	printf("\n");

	printf("standards\n");
	for (i = 0;; i++) {
		memset(&standard,0,sizeof(standard));
		standard.index = i;
		if (-1 == doioctl(fd,VIDIOC_ENUMSTD,&standard,sizeof(standard)))
			break;
		printf("    VIDIOC_ENUMSTD(%d)\n",i);
		print_struct(stdout,desc_v4l2_standard,&standard,"",tab);
	}
	printf("\n");

	printf("inputs\n");
	for (i = 0;; i++) {
		memset(&input,0,sizeof(input));
		input.index = i;
		if (-1 == doioctl(fd,VIDIOC_ENUMINPUT,&input,sizeof(input)))
			break;
		printf("    VIDIOC_ENUMINPUT(%d)\n",i);
		print_struct(stdout,desc_v4l2_input,&input,"",tab);
	}
	printf("\n");

	if (capability.capabilities & V4L2_CAP_TUNER) {
		printf("tuners\n");
		for (i = 0;; i++) {
			memset(&tuner,0,sizeof(tuner));
			tuner.index = i;
			if (-1 == doioctl(fd,VIDIOC_G_TUNER,&tuner,sizeof(tuner)))
				break;
			printf("    VIDIOC_G_TUNER(%d)\n",i);
			print_struct(stdout,desc_v4l2_tuner,&tuner,"",tab);
		}
		printf("\n");
	}

	if (capability.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
		printf("video capture\n");
		for (i = 0;; i++) {
			memset(&fmtdesc,0,sizeof(fmtdesc));
			fmtdesc.index = i;
			fmtdesc.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (-1 == doioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc,sizeof(fmtdesc)))
				break;
			printf("    VIDIOC_ENUM_FMT(%d,VIDEO_CAPTURE)\n",i);
			print_struct(stdout,desc_v4l2_fmtdesc,&fmtdesc,"",tab);
		}
		memset(&format,0,sizeof(format));
		format.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (-1 == doioctl(fd,VIDIOC_G_FMT,&format,sizeof(format))) {
			perror("VIDIOC_G_FMT(VIDEO_CAPTURE)");
		} else {
			printf("    VIDIOC_G_FMT(VIDEO_CAPTURE)\n");
			print_struct(stdout,desc_v4l2_format,&format,"",tab);
		}
		printf("\n");
	}

	if (capability.capabilities & V4L2_CAP_VIDEO_OVERLAY) {
		printf("video overlay\n");
		for (i = 0;; i++) {
			memset(&fmtdesc,0,sizeof(fmtdesc));
			fmtdesc.index = i;
			fmtdesc.type  = V4L2_BUF_TYPE_VIDEO_OVERLAY;
			if (-1 == doioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc,sizeof(fmtdesc)))
				break;
			printf("    VIDIOC_ENUM_FMT(%d,VIDEO_OVERLAY)\n",i);
			print_struct(stdout,desc_v4l2_fmtdesc,&fmtdesc,"",tab);
		}
		memset(&format,0,sizeof(format));
		format.type  = V4L2_BUF_TYPE_VIDEO_OVERLAY;
		if (-1 == doioctl(fd,VIDIOC_G_FMT,&format,sizeof(format))) {
			perror("VIDIOC_G_FMT(VIDEO_OVERLAY)");
		} else {
			printf("    VIDIOC_G_FMT(VIDEO_OVERLAY)\n");
			print_struct(stdout,desc_v4l2_format,&format,"",tab);
		}
		memset(&fbuf,0,sizeof(fbuf));
		if (-1 == doioctl(fd,VIDIOC_G_FBUF,&fbuf,sizeof(fbuf))) {
			perror("VIDIOC_G_FBUF");
		} else {
			printf("    VIDIOC_G_FBUF\n");
			print_struct(stdout,desc_v4l2_framebuffer,&fbuf,"",tab);
		}
		printf("\n");
	}

	if (capability.capabilities & V4L2_CAP_VBI_CAPTURE) {
		printf("vbi capture\n");
		for (i = 0;; i++) {
			memset(&fmtdesc,0,sizeof(fmtdesc));
			fmtdesc.index = i;
			fmtdesc.type  = V4L2_BUF_TYPE_VBI_CAPTURE;
			if (-1 == doioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc,sizeof(fmtdesc)))
				break;
			printf("    VIDIOC_ENUM_FMT(%d,VBI_CAPTURE)\n",i);
			print_struct(stdout,desc_v4l2_fmtdesc,&fmtdesc,"",tab);
		}
		memset(&format,0,sizeof(format));
		format.type  = V4L2_BUF_TYPE_VBI_CAPTURE;
		if (-1 == doioctl(fd,VIDIOC_G_FMT,&format,sizeof(format))) {
			perror("VIDIOC_G_FMT(VBI_CAPTURE)");
		} else {
			printf("    VIDIOC_G_FMT(VBI_CAPTURE)\n");
			print_struct(stdout,desc_v4l2_format,&format,"",tab);
		}
		printf("\n");
	}

	printf("controls\n");
	memset(&qctrl,0,sizeof(qctrl));
	
	/* find first CID */
	i = V4L2_CID_BASE;
	for (qctrl.id = V4L2_CID_BASE; qctrl.id < V4L2_CID_LASTP1;qctrl.id++){
		if (!doioctl(fd,VIDIOC_QUERYCTRL,&qctrl,sizeof(qctrl))){
			i = qctrl.id;
			break;
		}
	}
	qctrl.id = i;
	
	for (i = 0; i < V4L2_CTRL_ID_MASK; i++) {
		if (i>0){
			printf("\n    VIDIOC_QUERYCTRL (%d | V4L2_CTRL_FLAG_NEXT_CTRL)\n",qctrl.id);
			qctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
		}else
			printf("\n    VIDIOC_QUERYCTRL (%d)\n",qctrl.id);
			
		if (doioctl(fd,VIDIOC_QUERYCTRL,&qctrl,sizeof(qctrl)) < 0){
			printf("        break\n");
			break;
		}
		if (qctrl.flags & V4L2_CTRL_FLAG_DISABLED)
			continue;
		
		
		print_struct(stdout,desc_v4l2_queryctrl,&qctrl,"",tab);
	}
	return 0;
}

/* --------------------------------------------------------------------- */
/* getCotrol                                                             */

void getCotrol(int fd, unsigned long int cid){
	int result;
	struct v4l2_control ctrl = {
		.id = cid
	};
	result = doioctl(fd,VIDIOC_G_CTRL,&ctrl,sizeof(ctrl));
	printf("\nget control id %d : %d ",ctrl.id,ctrl.value);
	if (result)
		printf("; returned code: %d\n", result);
	else
		printf("\n");
}

/* --------------------------------------------------------------------- */
/* setCotrol                                                             */

void setCotrol(int fd, unsigned long int cid, unsigned long int value){
	int result;
	struct v4l2_control ctrl = {
		.id = cid,
		.value = value
	};
	result = doioctl(fd,VIDIOC_S_CTRL,&ctrl,sizeof(ctrl));
	printf("\nset control id %d to %d ",ctrl.id,ctrl.value);
	if (result)
		printf("; returned code: %d\n", result);
	else
		printf("\n");
}

/* --------------------------------------------------------------------- */
/* help                                                                  */

void print_help(){
	printf("usage: v4lctrl [options]\n");
	printf("options:\n");
	printf("    -h 	          print this text\n");
	printf("    -r 	          try direct access\n");
	printf("    -c file       use <file> as video4linux device\n");
	printf("    -d 	          dump device informations\n");
	printf("    -g cid        get value of control with id <cid>\n");
	printf("    -s cid        set control with id <cid> to value specified in parameter -v\n");
	printf("    -v value      value for set\n");
	
}

/* --------------------------------------------------------------------- */
/* main                                                                  */

int main(int argc, char *argv[])
{
	char dummy[256];
	char *device = "/dev/video0";
	int tab = 1;
	int fd;
	int direct_access = 0;
	unsigned char v4l2_device = 0;
	unsigned char v4l_device = 0;
	unsigned char dump = 0;
	int c;
	unsigned long int g_cid = 0;
	unsigned long int s_cid = 0;
	unsigned long int value = 0;
	
	
	if (argc < 2){
		print_help();
		exit(0);
	}
	
	for (;;) {
		c = getopt(argc, argv, "hrc:dg:s:v:");
		if (c == -1)
			break;
		
		switch (c) {
			case 'd':
				dump = 1;
				break;
			case 'c':
				device = optarg;
				break;
			case 'g':
				g_cid = atoi(optarg);
				break;
			case 's':
				s_cid = atoi(optarg);
				break;
			case 'v':
				value = atoi(optarg);
				break;
			case 'r':
				direct_access = 1;
				break;				
			case 'h':
			default:
				print_help();
				exit(1);
		}
	}
    
	if (direct_access || try_ged_shm_id(device) < 0 ){
        printf("try direct access to %s\n",device);    
        fd = open(device,O_RDONLY);
        printf("device %s open for read",device);
    }else{
        printf("try communicate via shm\n");
    }
	
	if (-1 == fd) {
		fprintf(stderr,"open %s: %s\n",device,strerror(errno));
		exit(1);
	};

	if (-1 != doioctl(fd,VIDIOC_QUERYCAP,dummy,sizeof(dummy))) {
		v4l2_device = 1;
	}

	if (-1 != doioctl(fd,VIDIOCGCAP,dummy,sizeof(dummy))) {
		v4l_device = 1;
	}

	if (!(v4l_device | v4l2_device)) {
		fprintf(stderr,"%s: not an video4linux device\n",device);
		exit(1);
	}
	
	
	if (dump && v4l_device){
        printf("\n### video4linux device info [%s] ###\n",device);
		dump_v4l(fd,tab);
    }
	if (dump && v4l2_device){
        printf("\n### v4l2 device info [%s] ###\n",device);
		dump_v4l2(fd,tab);
    }
	
	if (g_cid)
		getCotrol(fd, g_cid);
	if (s_cid)
		setCotrol(fd, s_cid, value);
	
	return 0;
}