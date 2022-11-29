#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/fdtable.h>
#include <linux/string.h>

int files_to_str(char result[32][32], int32_t *size_files,
		 struct files_struct *files)
{
	char *str = kmalloc(256, GFP_KERNEL);
	struct fdtable *fdt = files_fdtable(files);

	(*size_files) = 0;


	if (!str)
		return -1;
	

	memset(str, 0, 32);


	for (int i = 0; i < fdt->max_fds && i < 32; ++i) {
		if (fdt->fd[i]) {
	
			char *tmp = d_path(&fdt->fd[i]->f_path, str, 256);
			if (tmp) {
				for (int j = 0; j < strlen(tmp) && j < 32; ++j) {
					result[(*size_files)][j] = tmp[j];
				}
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

int dfs(struct task_struct *task, int32_t *pidc, struct ProcStruct *ps,
	int32_t level, int32_t show, long *maxLevel)
{
	struct task_struct *child;
	struct list_head *list;

	if (level > (*maxLevel))
		(*maxLevel) = level;
	

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
	
	if (files_to_str(ps[*pidc].files, &ps[*pidc].size_files, task->files) <
	    0)
		return -1;

	for (int i = 0; i < 16; ++i) {
		ps[*pidc].name[i] = task->comm[i];
	}
	ps[*pidc].name[16] = '\0';


	if (show) {
		printk(KERN_INFO "Name: %-16s Process ID: %d Level: %d\n",
		       ps[*pidc].name, task->pid, level);
	}

	++(*pidc);

	list_for_each(list, &task->children) {
		child = list_entry(list, struct task_struct, sibling);

		if (dfs(child, pidc, ps, level + 1, show, maxLevel) < 0) {
			return -1;
		}
	}

	return 0;
}

SYSCALL_DEFINE4(dfsproc, pid_t, pid, int32_t, numPs, int32_t *, pidc, struct ProcStruct *, ps)
{
	struct task_struct *task = find_task_by_vpid(pid);

	int32_t *kpidc = kmalloc(sizeof(int32_t), GFP_KERNEL);
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

	if ((show = *kpidc))
		*kpidc = 0;

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
