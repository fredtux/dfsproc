#ifndef PROCSTRUCT

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
#endif /* ifndef PROCSTRUCT */
