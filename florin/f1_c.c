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
    struct ProcStruct *ps = malloc(sizeof(struct ProcStruct) * 128);
	
    pid_t pid = 1;
    if(argc > 1)
        pid = atoi(argv[1]);
    
    int tmp = 1;
    int32_t *pidc = &tmp;
    long levels = syscall(__NR_dfs, pid, 128, pidc, ps);
    if(levels < 0){
        perror("DFSProc Syscall Failed");
    } else {
        printf("Total number of levels: %ld\n", levels);
    }

	for(int i = 0; i < *pidc; ++i){
		printf("\nprocID: %d level: %d state: %d flags: %d ptrace: %d on_rq: %d prio: %d static_prio: %d normal_prio: %d rt_priority: %d policy: %d nr_cpus_allowed: %d migration_flags: %d exit_state: %d exit_code: %d exit_signal: %d pdeath_signal: %d jobctl: %lu personality: %d sched_reset_on_fork: %d sched_contributes_to_load: %d sched_migrated: %d atomic_flags: %lu pid: %d tgid: %d utime: %lu stime: %lu gtime: %lu nr_dirtied: %d nr_dirtied_pause: %d dirty_paused_when: %lu size_files: %d name: %s \n", ps[i].procID, ps[i].level, ps[i].state, ps[i].flags, ps[i].ptrace, ps[i].on_rq, ps[i].prio, ps[i].static_prio, ps[i].normal_prio, ps[i].rt_priority, ps[i].policy, ps[i].nr_cpus_allowed, ps[i].migration_flags, ps[i].exit_state, ps[i].exit_code, ps[i].exit_signal, ps[i].pdeath_signal, ps[i].jobctl, ps[i].personality, ps[i].sched_reset_on_fork, ps[i].sched_contributes_to_load, ps[i].sched_migrated, ps[i].atomic_flags, ps[i].pid, ps[i].tgid, ps[i].utime, ps[i].stime, ps[i].gtime, ps[i].nr_dirtied, ps[i].nr_dirtied_pause, ps[i].dirty_paused_when, ps[i].size_files, ps[i].name);
	}

    return 0;
}
