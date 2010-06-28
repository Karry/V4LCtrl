
#define SHM_SEMAPTHORES_ID      0
#define SHM_DATA_ID             1

#define SEMAPHORE_CLIENT_SEM_ID     0
#define SEMAPHORE_LIB_DATA_SEM_ID       1
#define SEMAPHORE_CLIENT_DATA_SEM_ID    2

#define SEMAPHORES_COUNT    3

struct v4lcontrol_shared{
	__u64 lib_pid;
	__u64 ioctl_request;
	__u64 result;
	__u8 arg[256];	
};

static int shm_data_id = -1;
static int shm_semaphores_id = -1;
static struct v4lcontrol_shared *shared;