
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gfp.h>
#include <linux/mmzone.h>
#include <linux/percpu-defs.h>
#include <linux/memory.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mempool.h>
#include <linux/percpu.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/delay.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mike Day");


static uint32_t node = 0; /* node id */
static uint32_t zone = 0; /* 0 dma, 1 dma32, 2 highmem */

int count = 0;

gfp_t flags[] = {
	OPT_ZONE_DMA     | GFP_ATOMIC | __GFP_REPEAT,
	OPT_ZONE_DMA32   | GFP_ATOMIC | __GFP_REPEAT,
	OPT_ZONE_HIGHMEM | GFP_KERNEL | __GFP_REPEAT,

	OPT_ZONE_DMA     | GFP_ATOMIC | __GFP_NOFAIL,
	OPT_ZONE_DMA32   | GFP_ATOMIC | __GFP_NOFAIL,
	OPT_ZONE_HIGHMEM | GFP_KERNEL | __GFP_NOFAIL	
};

	

static uint32_t alloc_counts[6];
static uint32_t fail_counts[6];

int gfp_flag_count = sizeof(flags) / sizeof(flags[0]); 	

struct task_struct *et;
int should_stop = 0;

int exhuast_hw_zones(void *data) 
{
	int count = 0;
	void *ret = NULL;
	
	do {

		ret = kzalloc(0x100, flags[count % 3]);
		
		if (ret != NULL) {
			alloc_counts[count % 3]++;
			if (!count % 1000) {
				printk(KERN_DEBUG "STRESS: stress-alloc alloc counts: %d %d %d\n",
				       alloc_counts[0], alloc_counts[1], alloc_counts[2]);
			}
		} else {
			fail_counts[count % 3]++;
			if (!count % 1000) {
				printk(KERN_DEBUG "STRESS: stress-alloc failures: %d %d %d\n",
				       fail_counts[0], fail_counts[1], fail_counts[2]);
			}
		}
		msleep(100);
		count++;

	} while (!should_stop);
	
	return 0;
	
}

module_param(node, uint32_t, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP0;
module_param(zone, uint32_t, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);





static int __init sa_init(void)
{
	
	printk(KERN_DEBUG "STRESS: flags array entry is %d elements\n", gfp_flag_count);
	et = kthread_run(exhuast_hw_zones, NULL, "stress allocator");
	
	return 0;
	
}


static void __exit sa_exit(void)
{
	should_stop = 1;
	
	printk(KERN_DEBUG "STRESS: stress-alloc alloc counts: %d %d %d\n",
	       alloc_counts[0], alloc_counts[1], alloc_counts[2]);
	printk(KERN_DEBUG "STRESS: stress-alloc failures: %d %d %d\n",
	       fail_counts[0], fail_counts[1], fail_counts[2]);

	printk(KERN_DEBUG "STRESS: kthread ptr %p\n", et);
	

}

module_init(sa_init);
module_exit(sa_exit);

/*********************************************
 * TODO: 
 * current flag definitions mix unlike flag values
 * page alloc
 * GFP_NOFAIL 
 * GFP_REPEAT
 *       gen_pool_create/alloc 
 * https://lwn.net/Articles/125842/
 *************************/



