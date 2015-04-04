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


#include "../v4lctrl/libv4lctrl.h"
#include "v4lctrlcli.h"

#include "struct-dump.h"
//#include "struct-v4l.h"
#include "struct-v4l2.h"



/* --------------------------------------------------------------------- */
/* v4l(1)                                                                */
/*
static int dump_v4l(struct v4ldevice *vd, int tab)
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
	if (-1 == doioctl(vd,VIDIOCGCAP,&capability, sizeof(capability)))
		return -1;
	printf("    VIDIOCGCAP\n");
	print_struct(stdout,desc_video_capability,&capability,"",tab);
	printf("\n");

	printf("channels\n");
	for (i = 0; i < capability.channels; i++) {
		memset(&channel,0,sizeof(channel));
		channel.channel = i;
		if (-1 == doioctl(vd,VIDIOCGCHAN,&channel,sizeof(channel))) {
			perror("ioctl VIDIOCGCHAN");
			continue;
		}
		printf("    VIDIOCGCHAN(%d)\n",i);
		print_struct(stdout,desc_video_channel,&channel,"",tab);
	}
	printf("\n");

	printf("tuner\n");
	memset(&tuner,0,sizeof(tuner));
	if (-1 == doioctl(vd,VIDIOCGTUNER,&tuner,sizeof(tuner))) {
		perror("ioctl VIDIOCGTUNER");
	} else {
		printf("    VIDIOCGTUNER\n");
		print_struct(stdout,desc_video_tuner,&tuner,"",tab);
	}
	printf("\n");

	printf("audio\n");
	memset(&audio,0,sizeof(audio));
	if (-1 == doioctl(vd,VIDIOCGAUDIO,&audio,sizeof(audio))) {
		perror("ioctl VIDIOCGAUDIO");
	} else {
		printf("    VIDIOCGAUDIO\n");
		print_struct(stdout,desc_video_audio,&audio,"",tab);
	}
	printf("\n");

	printf("picture\n");
	memset(&picture,0,sizeof(picture));
	if (-1 == doioctl(vd,VIDIOCGPICT,&picture,sizeof(picture))) {
		perror("ioctl VIDIOCGPICT");
	} else {
		printf("    VIDIOCGPICT\n");
		print_struct(stdout,desc_video_picture,&picture,"",tab);
	}
	printf("\n");

	printf("buffer\n");
	memset(&buffer,0,sizeof(buffer));
	if (-1 == doioctl(vd,VIDIOCGFBUF,&buffer,sizeof(buffer))) {
		perror("ioctl VIDIOCGFBUF");
	} else {
		printf("    VIDIOCGFBUF\n");
		print_struct(stdout,desc_video_buffer,&buffer,"",tab);
	}
	printf("\n");

	printf("window\n");
	memset(&window,0,sizeof(window));
	if (-1 == doioctl(vd,VIDIOCGWIN,&window,sizeof(window))) {
		perror("ioctl VIDIOCGWIN");
	} else {
		printf("    VIDIOCGWIN\n");
		print_struct(stdout,desc_video_window,&window,"",tab);
	}
	printf("\n");

	return 0;
}
 * */

/* --------------------------------------------------------------------- */
/* v4l2                                                                  */

static int dump_v4l2(struct v4ldevice *vd, int tab)
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
	if (-1 == doioctl(vd,VIDIOC_QUERYCAP,&capability,sizeof(capability)))
		return -1;
	printf("    VIDIOC_QUERYCAP\n");
	print_struct(stdout,desc_v4l2_capability,&capability,"",tab);
	printf("\n");

	printf("standards\n");
	for (i = 0;; i++) {
		memset(&standard,0,sizeof(standard));
		standard.index = i;
		if (-1 == doioctl(vd,VIDIOC_ENUMSTD,&standard,sizeof(standard)))
			break;
		printf("    VIDIOC_ENUMSTD(%d)\n",i);
		print_struct(stdout,desc_v4l2_standard,&standard,"",tab);
	}
	printf("\n");

	printf("inputs\n");
	for (i = 0;; i++) {
		memset(&input,0,sizeof(input));
		input.index = i;
		if (-1 == doioctl(vd,VIDIOC_ENUMINPUT,&input,sizeof(input)))
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
			if (-1 == doioctl(vd,VIDIOC_G_TUNER,&tuner,sizeof(tuner)))
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
			if (-1 == doioctl(vd,VIDIOC_ENUM_FMT,&fmtdesc,sizeof(fmtdesc)))
				break;
			printf("    VIDIOC_ENUM_FMT(%d,VIDEO_CAPTURE)\n",i);
			print_struct(stdout,desc_v4l2_fmtdesc,&fmtdesc,"",tab);
		}
		memset(&format,0,sizeof(format));
		format.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (-1 == doioctl(vd,VIDIOC_G_FMT,&format,sizeof(format))) {
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
			if (-1 == doioctl(vd,VIDIOC_ENUM_FMT,&fmtdesc,sizeof(fmtdesc)))
				break;
			printf("    VIDIOC_ENUM_FMT(%d,VIDEO_OVERLAY)\n",i);
			print_struct(stdout,desc_v4l2_fmtdesc,&fmtdesc,"",tab);
		}
		memset(&format,0,sizeof(format));
		format.type  = V4L2_BUF_TYPE_VIDEO_OVERLAY;
		if (-1 == doioctl(vd,VIDIOC_G_FMT,&format,sizeof(format))) {
			perror("VIDIOC_G_FMT(VIDEO_OVERLAY)");
		} else {
			printf("    VIDIOC_G_FMT(VIDEO_OVERLAY)\n");
			print_struct(stdout,desc_v4l2_format,&format,"",tab);
		}
		memset(&fbuf,0,sizeof(fbuf));
		if (-1 == doioctl(vd,VIDIOC_G_FBUF,&fbuf,sizeof(fbuf))) {
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
			if (-1 == doioctl(vd,VIDIOC_ENUM_FMT,&fmtdesc,sizeof(fmtdesc)))
				break;
			printf("    VIDIOC_ENUM_FMT(%d,VBI_CAPTURE)\n",i);
			print_struct(stdout,desc_v4l2_fmtdesc,&fmtdesc,"",tab);
		}
		memset(&format,0,sizeof(format));
		format.type  = V4L2_BUF_TYPE_VBI_CAPTURE;
		if (-1 == doioctl(vd,VIDIOC_G_FMT,&format,sizeof(format))) {
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
		if (!doioctl(vd,VIDIOC_QUERYCTRL,&qctrl,sizeof(qctrl))){
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
			
		if (doioctl(vd,VIDIOC_QUERYCTRL,&qctrl,sizeof(qctrl)) < 0){
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
	struct v4ldevice *vd = malloc(sizeof(struct v4ldevice));
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

	if (v4ldevice_open(vd, device, direct_access) < 0)
		exit(-1);
	

	if (-1 != doioctl(vd,VIDIOC_QUERYCAP,dummy,sizeof(dummy))) {
		v4l2_device = 1;
	}

	if (-1 != doioctl(vd,VIDIOCGCAP,dummy,sizeof(dummy))) {
		v4l_device = 1;
	}

	if (!(v4l_device | v4l2_device)) {
		fprintf(stderr,"%s: not an video4linux device\n",device);
		exit(1);
	}
	
	
	if (dump && v4l_device){
		printf("\n### video4linux device info [%s] ###\n",device);
		printf("\n### v4l1 API is not supported anymore!\n");
		//dump_v4l(vd,tab);
	}
	if (dump && v4l2_device){
		printf("\n### v4l2 device info [%s] ###\n",device);
		dump_v4l2(vd,tab);
	}
	
	if (g_cid){
		int result = getCotrol(vd, g_cid,&value);
		printf("\nget control id %d : %d ",g_cid,value);
		if (result)
			printf("; returned code: %d\n", result);
		else
			printf("\n");		
	}
	if (s_cid){
		int result = setCotrol(vd, s_cid, value);
		printf("\nset control id %d to %d ",s_cid,value);
		if (result)
			printf("; returned code: %d\n", result);
		else
			printf("\n");
	}
	
	v4ldevice_close(vd);
	
	return 0;
}