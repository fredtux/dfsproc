#include <linux/kernel.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <linux/types.h>

typedef unsigned long u64;
struct ProcStruct {
	pid_t procID;
	int32_t level;
	int32_t state;
	unsigned int flags;
	unsigned int ptrace;
	int on_rq;
	int prio;
	int static_prio;
	int normal_prio;
	unsigned int rt_priority;
	unsigned int policy;
	int nr_cpus_allowed;
	unsigned short migration_flags;
	int exit_state;
	int exit_code;
	int exit_signal;
	int pdeath_signal;
	unsigned long jobctl;
	unsigned int personality;
	unsigned sched_reset_on_fork;
	unsigned sched_contributes_to_load;
	unsigned sched_migrated;
	unsigned long atomic_flags;
	pid_t pid;
	pid_t tgid;
	u64 utime;
	u64 stime;
	u64 gtime;
	int nr_dirtied;
	int nr_dirtied_pause;
	unsigned long dirty_paused_when;
	int32_t size_files;
	char files[32][32];
	char name[32];
};

#define __NR_dfs 451

int main(int argc, char *argv[])
{
    struct ProcStruct *ps = malloc(sizeof(struct ProcStruct) * 64);
	
    pid_t pid = 1;
    if(argc > 1)
        pid = atoi(argv[1]);
    
    int tmp = 1;
    int32_t *pidc = &tmp;
    long levels = syscall(__NR_dfs, pid, 64, pidc, ps);
    if(levels < 0){
        perror("DFSProc Syscall Failed");
    } else {
        printf("Total number of levels: %ld\n", levels);
    }

	for(int i = 0; i < *pidc; ++i){
		// De adaugat printf
	}

    return 0;
}
