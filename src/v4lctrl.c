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

#include "videodev.h"
#include "videodev2.h"

#include "struct-dump.h"
#include "struct-v4l.h"
#include "struct-v4l2.h"

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
	if (-1 == ioctl(fd,VIDIOCGCAP,&capability))
		return -1;
	printf("    VIDIOCGCAP\n");
	print_struct(stdout,desc_video_capability,&capability,"",tab);
	printf("\n");

	printf("channels\n");
	for (i = 0; i < capability.channels; i++) {
		memset(&channel,0,sizeof(channel));
		channel.channel = i;
		if (-1 == ioctl(fd,VIDIOCGCHAN,&channel)) {
			perror("ioctl VIDIOCGCHAN");
			continue;
		}
		printf("    VIDIOCGCHAN(%d)\n",i);
		print_struct(stdout,desc_video_channel,&channel,"",tab);
	}
	printf("\n");

	printf("tuner\n");
	memset(&tuner,0,sizeof(tuner));
	if (-1 == ioctl(fd,VIDIOCGTUNER,&tuner)) {
		perror("ioctl VIDIOCGTUNER");
	} else {
		printf("    VIDIOCGTUNER\n");
		print_struct(stdout,desc_video_tuner,&tuner,"",tab);
	}
	printf("\n");

	printf("audio\n");
	memset(&audio,0,sizeof(audio));
	if (-1 == ioctl(fd,VIDIOCGAUDIO,&audio)) {
		perror("ioctl VIDIOCGAUDIO");
	} else {
		printf("    VIDIOCGAUDIO\n");
		print_struct(stdout,desc_video_audio,&audio,"",tab);
	}
	printf("\n");

	printf("picture\n");
	memset(&picture,0,sizeof(picture));
	if (-1 == ioctl(fd,VIDIOCGPICT,&picture)) {
		perror("ioctl VIDIOCGPICT");
	} else {
		printf("    VIDIOCGPICT\n");
		print_struct(stdout,desc_video_picture,&picture,"",tab);
	}
	printf("\n");

	printf("buffer\n");
	memset(&buffer,0,sizeof(buffer));
	if (-1 == ioctl(fd,VIDIOCGFBUF,&buffer)) {
		perror("ioctl VIDIOCGFBUF");
	} else {
		printf("    VIDIOCGFBUF\n");
		print_struct(stdout,desc_video_buffer,&buffer,"",tab);
	}
	printf("\n");

	printf("window\n");
	memset(&window,0,sizeof(window));
	if (-1 == ioctl(fd,VIDIOCGWIN,&window)) {
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
	if (-1 == ioctl(fd,VIDIOC_QUERYCAP,&capability))
		return -1;
	printf("    VIDIOC_QUERYCAP\n");
	print_struct(stdout,desc_v4l2_capability,&capability,"",tab);
	printf("\n");

	printf("standards\n");
	for (i = 0;; i++) {
		memset(&standard,0,sizeof(standard));
		standard.index = i;
		if (-1 == ioctl(fd,VIDIOC_ENUMSTD,&standard))
			break;
		printf("    VIDIOC_ENUMSTD(%d)\n",i);
		print_struct(stdout,desc_v4l2_standard,&standard,"",tab);
	}
	printf("\n");

	printf("inputs\n");
	for (i = 0;; i++) {
		memset(&input,0,sizeof(input));
		input.index = i;
		if (-1 == ioctl(fd,VIDIOC_ENUMINPUT,&input))
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
			if (-1 == ioctl(fd,VIDIOC_G_TUNER,&tuner))
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
			if (-1 == ioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc))
				break;
			printf("    VIDIOC_ENUM_FMT(%d,VIDEO_CAPTURE)\n",i);
			print_struct(stdout,desc_v4l2_fmtdesc,&fmtdesc,"",tab);
		}
		memset(&format,0,sizeof(format));
		format.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (-1 == ioctl(fd,VIDIOC_G_FMT,&format)) {
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
			if (-1 == ioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc))
				break;
			printf("    VIDIOC_ENUM_FMT(%d,VIDEO_OVERLAY)\n",i);
			print_struct(stdout,desc_v4l2_fmtdesc,&fmtdesc,"",tab);
		}
		memset(&format,0,sizeof(format));
		format.type  = V4L2_BUF_TYPE_VIDEO_OVERLAY;
		if (-1 == ioctl(fd,VIDIOC_G_FMT,&format)) {
			perror("VIDIOC_G_FMT(VIDEO_OVERLAY)");
		} else {
			printf("    VIDIOC_G_FMT(VIDEO_OVERLAY)\n");
			print_struct(stdout,desc_v4l2_format,&format,"",tab);
		}
		memset(&fbuf,0,sizeof(fbuf));
		if (-1 == ioctl(fd,VIDIOC_G_FBUF,&fbuf)) {
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
			if (-1 == ioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc))
				break;
			printf("    VIDIOC_ENUM_FMT(%d,VBI_CAPTURE)\n",i);
			print_struct(stdout,desc_v4l2_fmtdesc,&fmtdesc,"",tab);
		}
		memset(&format,0,sizeof(format));
		format.type  = V4L2_BUF_TYPE_VBI_CAPTURE;
		if (-1 == ioctl(fd,VIDIOC_G_FMT,&format)) {
			perror("VIDIOC_G_FMT(VBI_CAPTURE)");
		} else {
			printf("    VIDIOC_G_FMT(VBI_CAPTURE)\n");
			print_struct(stdout,desc_v4l2_format,&format,"",tab);
		}
		printf("\n");
	}

	printf("controls\n");
	memset(&qctrl,0,sizeof(qctrl));
	qctrl.id = V4L2_CID_BASE;
	for (i = 0; i < V4L2_CTRL_ID_MASK; i++) {
		if (i>0){
			printf("\n    VIDIOC_QUERYCTRL (%d | V4L2_CTRL_FLAG_NEXT_CTRL)\n",qctrl.id);
			qctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
		}else
			printf("\n    VIDIOC_QUERYCTRL (%d)\n",qctrl.id);
			
		if (ioctl(fd,VIDIOC_QUERYCTRL,&qctrl) < 0){
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
	result = ioctl(fd,VIDIOC_G_CTRL,&ctrl);
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
	result = ioctl(fd,VIDIOC_S_CTRL,&ctrl);
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
		c = getopt(argc, argv, "hc:dg:s:v:");
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
			case 'h':
			default:
				print_help();
				exit(1);
		}
	}
	
	fd = open(device,O_RDONLY);
	if (-1 == fd) {
		fprintf(stderr,"open %s: %s\n",device,strerror(errno));
		exit(1);
	};

	if (-1 != ioctl(fd,VIDIOC_QUERYCAP,dummy)) {
		printf("\n### v4l2 device info [%s] ###\n",device);
		v4l2_device = 1;
	}

	if (-1 != ioctl(fd,VIDIOCGCAP,dummy)) {
		printf("\n### video4linux device info [%s] ###\n",device);
		v4l_device = 1;
	}

	if (!(v4l_device | v4l2_device)) {
		fprintf(stderr,"%s: not an video4linux device\n",device);
		exit(1);
	}
	
	
	if (dump && v4l_device)
		dump_v4l(fd,tab);
	if (dump && v4l2_device)
		dump_v4l2(fd,tab);
	
	if (g_cid)
		getCotrol(fd, g_cid);
	if (s_cid)
		setCotrol(fd, s_cid, value);
	
	return 0;
}