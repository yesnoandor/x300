
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/percpu.h>
#include <linux/sched.h>


static struct proc_dir_entry *entry;

static void *l_start(struct seq_file *m, loff_t * pos)
{
	loff_t index = *pos;

	printk(KERN_DEBUG "%s::+++++++++++++++\r\n",__FUNCTION__);

	if (index == 0) {
		seq_printf(m, "Current all the processes in system:\n"
                           "%-24s%-5s\n", "name", "pid");
		return &init_task;
	}
	else {
		return NULL;
	}

	printk(KERN_DEBUG "%s::---------------\r\n",__FUNCTION__);
}

static void *l_next(struct seq_file *m, void *p, loff_t * pos)
{
	struct task_struct * task = (struct task_struct *)p;

	printk(KERN_DEBUG "%s::+++++++++++++++\r\n",__FUNCTION__);

	task = next_task(task);

	if ((*pos != 0) && (task == &init_task)) {
		return NULL;
	}
	
	++*pos;

	printk(KERN_DEBUG "%s::---------------\r\n",__FUNCTION__);
	
	return task;
}

static void l_stop(struct seq_file *m, void *p)
{
	
	printk(KERN_DEBUG "%s::+++++++++++++++\r\n",__FUNCTION__);
	printk(KERN_DEBUG "%s::---------------\r\n",__FUNCTION__);
}

static int l_show(struct seq_file *m, void *p)
{
	struct task_struct * task = (struct task_struct *)p;

	
	printk(KERN_DEBUG "%s::+++++++++++++++\r\n",__FUNCTION__);

	seq_printf(m, "%-24s%-5d\n", task->comm, task->pid);

	printk(KERN_DEBUG "%s::---------------\r\n",__FUNCTION__);
		
	return 0;
}

static struct seq_operations fleetyinfo_seq_op = {
	.start = l_start,
	.next  = l_next,
	.stop  = l_stop,
	.show  = l_show
};

static int fleetyinfo_seq_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &fleetyinfo_seq_op);
}

static struct file_operations fleetyinfo_seq_fops = {
	.open = fleetyinfo_seq_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

static int __init proc_fleetyinfo_init(void)
{
	printk(KERN_DEBUG "%s::+++++++++++++++\r\n",__FUNCTION__);

	entry = proc_create("fleetyinfo", 0, NULL,&fleetyinfo_seq_fops);
	if(!entry)
	{
		printk(KERN_ERR "can't create fleetyinfo proc inode!\r\n"); 
			
		return -ENOMEM;
	}	

	printk(KERN_DEBUG "%s::---------------\r\n",__FUNCTION__);

	return 0;
}

static void __exit proc_fleetyinfo_exit(void)
{
	printk(KERN_DEBUG "%s::+++++++++++++++\r\n",__FUNCTION__);

	remove_proc_entry("fleetyinfo", NULL);
	
	printk(KERN_DEBUG "%s::---------------\r\n",__FUNCTION__);
}

module_init(proc_fleetyinfo_init);
module_exit(proc_fleetyinfo_exit);
MODULE_LICENSE("GPL");
