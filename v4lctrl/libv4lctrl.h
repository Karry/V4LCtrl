
#ifndef __V4LCTL_H
#define __V4LCTL_H

#include "linux/videodev2.h"
#include "libv4l1-videodev.h"
#include "libv4l2.h"

#define SHM_SEMAPTHORES_ID      0
#define SHM_DATA_ID             1

#define SEMAPHORE_CLIENT_SEM_ID     0
#define SEMAPHORE_LIB_DATA_SEM_ID       1
#define SEMAPHORE_CLIENT_DATA_SEM_ID    2

#define SEMAPHORES_COUNT    3

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef V4LCTRL_EXPORT
#if __GNUC__ >= 4
#define V4LCTRL_PUBLIC __attribute__ ((visibility("default")))
#else
#define V4LCTRL_PUBLIC
#endif
#else
#define V4LCTRL_PUBLIC
#endif

struct v4lcontrol_shared{
	uint64_t lib_pid;
	uint64_t ioctl_request;
	uint64_t result;
	uint8_t arg[256];	
};

enum access_method {SHM, DIRECT};

V4LCTRL_PUBLIC struct v4ldevice{
	enum access_method access;
	int fd; // file descriptor
	int shm_data_id;
	int shm_semaphores_id;
};

int try_ged_shm_id(struct v4ldevice *vd, char *device);
int binary_semaphore_down (int semid, int counterId);
int binary_semaphore_up(int semid, int counterId);

V4LCTRL_PUBLIC int doioctl(struct v4ldevice *vd,unsigned long int ctl,void *param, int size);
V4LCTRL_PUBLIC int v4ldevice_close(struct v4ldevice *vd);
V4LCTRL_PUBLIC int v4ldevice_open(struct v4ldevice *vd, char *devf, int force_direct);
V4LCTRL_PUBLIC int setCotrol(struct v4ldevice *vd, unsigned long int cid, unsigned long int value);
V4LCTRL_PUBLIC int getCotrol(struct v4ldevice *vd, unsigned long int cid, unsigned long int *value);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __V4LCTL_H */
