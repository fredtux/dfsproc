/**
 * error codes:
 * -1: when the number of proceses read is 0
 * -2: when no pid counter was read from user space into kernel space
 * -3: when no bytes of data were copied from kernel space to user space, within the number of processes
 * -4: when no bytes of data were copied from kernel space to user space regarding process info struct
 * -5: error in dfs
 * -6: when the allocation didn't happen
*/
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/fdtable.h>
#include <linux/string.h>

int files_to_str(char result[32][32], int32_t *size_files,
		 struct files_struct *files)
{
	char *str = kmalloc(256, GFP_KERNEL);

	// each process has a number of files associated with
	// files_fdtable returns a pointer to the fdtable, taking as input the files_struct.
	// once we have fdtable we can iterate over the fdt array, printing the file path of each elem from fdt
	struct fdtable *fdt = files_fdtable(files);

	(*size_files) = 0;

// if we can't allocate memory to our string
	if (!str)
		return -1;

// we initialize str
	memset(str, 0, 32);

// we iterate through the files till the max files that files_fdtable has within the limit of 32 positions
	for (int i = 0; i < fdt->max_fds && i < 32; ++i) {
		if (fdt->fd[i]) {
	// we copy the name of our file in tmp
			char *tmp = d_path(&fdt->fd[i]->f_path, str, 256);
			if (tmp) {
				for (int j = 0; j < strlen(tmp) && j < 32; ++j) {
					result[(*size_files)][j] = tmp[j];
				}
				//we limit to 32 characters through '\0'
				result[(*size_files)][strlen(tmp) > 31 ? 31 : strlen(tmp)] = '\0';
				(*size_files)++;

			} else {
				break;
			}
		}
	}

	kfree(str);

	return 0;
}

/*
* task is in essence de current process
* show is a flag for printing/or not something process related  
*/
int dfs(struct task_struct *task, int32_t *pidc, struct ProcStruct *ps,
	int32_t level, int32_t show, long *maxLevel)
{
	struct task_struct *child;
	struct list_head *list;

	if (level > (*maxLevel))
		(*maxLevel) = level;
	
/*
* for each attribute from the current process info
* it fills in our data structure
* the data attributes is taken from the kernel processes structure and populates our structure 
*/
	ps[*pidc].procID = task->pid;
	ps[*pidc].level = level;
	ps[*pidc].state = task->__state;
	ps[*pidc].flags = task->flags;
	ps[*pidc].on_rq = task->on_rq;
	ps[*pidc].prio = task->prio;
	ps[*pidc].static_prio = task->static_prio;
	ps[*pidc].normal_prio = task->normal_prio;
	ps[*pidc].rt_priority = task->rt_priority;
	ps[*pidc].nr_cpus_allowed = task->nr_cpus_allowed;
	ps[*pidc].migration_flags = task->migration_flags;
	ps[*pidc].exit_state = task->exit_state;
	ps[*pidc].exit_code = task->exit_code;
	ps[*pidc].exit_signal = task->exit_signal;
	ps[*pidc].pdeath_signal = task->pdeath_signal;
	ps[*pidc].jobctl = task->jobctl;
	ps[*pidc].personality = task->personality;
	ps[*pidc].sched_reset_on_fork = task->sched_reset_on_fork;
	ps[*pidc].sched_contributes_to_load = task->sched_contributes_to_load;
	ps[*pidc].sched_migrated = task->sched_migrated;
	ps[*pidc].atomic_flags = task->atomic_flags;
	ps[*pidc].pid = task->pid;
	ps[*pidc].tgid = task->tgid;
	ps[*pidc].utime = task->utime;
	ps[*pidc].stime = task->stime;
	ps[*pidc].gtime = task->gtime;
	ps[*pidc].nr_dirtied = task->nr_dirtied;
	ps[*pidc].nr_dirtied_pause = task->nr_dirtied_pause;
	ps[*pidc].dirty_paused_when = task->dirty_paused_when;

	// it brings all the files in our structure
	if (files_to_str(ps[*pidc].files, &ps[*pidc].size_files, task->files) <
	    0)
		return -1;

// it takes the first 16 chars from the process name to associate the name of the process  with our data structure
	for (int i = 0; i < 16; ++i) {
		ps[*pidc].name[i] = task->comm[i];
	}
	ps[*pidc].name[16] = '\0';

// a debug parameter in order to display some info about the current process: name, id, level
	if (show) {
		printk(KERN_INFO "Name: %-16s Process ID: %d Level: %d\n",
		       ps[*pidc].name, task->pid, level);
	}

	++(*pidc);

//it loops through a list of processes and takes the children and applies dfs on them
	list_for_each(list, &task->children) {
		child = list_entry(list, struct task_struct, sibling);

		if (dfs(child, pidc, ps, level + 1, show, maxLevel) < 0) {
			return -1;
		}
	}

	return 0;
}

/**
 *  Main 4 stages defined in our syscall:
 * memory allocation
 * dfs creation using the struct info of the proceeses
 * copy made from kernel to user
 * deallocation of memory with kfree
 * 
 * Note:
 *  everything that has k as prefix is referring to kernel space
 * 
 * pid_t-> the process that we start with
 * numPs-> the limit number of processes that we want print (in case of any memory leaks)
 * pidc-> the number of processes read for real, numPs is its upper limit
 * ps-> a vector with our struct info about a process as component (defined in dfsproc.h) 
*/

SYSCALL_DEFINE4(dfsproc, pid_t, pid, int32_t, numPs, int32_t *, pidc, struct ProcStruct *, ps)
{//a kernel structure that takes a pid and returns a structure with the respective process
	struct task_struct *task = find_task_by_vpid(pid); 

	int32_t *kpidc = kmalloc(sizeof(int32_t), GFP_KERNEL);//we allocate memory in the kernel space
	struct ProcStruct *kps = kmalloc(sizeof(struct ProcStruct) * (numPs > 256 ? 256 : numPs), GFP_KERNEL);

	int32_t show;
	long *maxLevel;
	long ml = 0;
	maxLevel = &ml;

	if(!kps){  
		kps = kmalloc(sizeof(struct ProcStruct) * (numPs > 256 ? 64 : numPs / 2), GFP_KERNEL);
		if(!kps){
			return -6;
		}
	}

	if (!kpidc)
		return -1;

	if (copy_from_user(kpidc, pidc, sizeof(int32_t))) {
		return -2;
	}

// used a counter at this point
	if ((show = *kpidc))
		*kpidc = 0;

// in kps it will be all the processes seen within a dfs structure
	if (dfs(task, kpidc, kps, 0, show, maxLevel) < 0) {
		printk(KERN_INFO "Error in dfs");
		return -5;
	}

	if (copy_to_user(pidc, kpidc, sizeof(int32_t))) {
		return -3;
	}
	if (copy_to_user(ps, kps, sizeof(struct ProcStruct) * (*kpidc))) {
		return -4;
	}

	kfree(kpidc);
	kfree(kps);

	return (*maxLevel);
}
