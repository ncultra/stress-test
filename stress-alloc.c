
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
static uint32_t zone = 0; /* 0 dma, 1 dma32, 2 highmem, 3 all */
static uint32_t leak = 1; /* should leak pages as part of the test */
static uint32_t nofail = 0;


module_param(node,   int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
module_param(zone,   int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
module_param(leak,   int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
module_param(nofail, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);


int count = 0;

gfp_t flags[] = {
	OPT_ZONE_DMA     | GFP_ATOMIC | __GFP_REPEAT,
	OPT_ZONE_DMA32   | GFP_ATOMIC | __GFP_REPEAT,
	OPT_ZONE_HIGHMEM | GFP_KERNEL | __GFP_REPEAT,

	OPT_ZONE_DMA     | GFP_ATOMIC | __GFP_NOFAIL,
	OPT_ZONE_DMA32   | GFP_ATOMIC | __GFP_NOFAIL,
	OPT_ZONE_HIGHMEM | GFP_KERNEL | __GFP_NOFAIL	
};


static inline int derive_zone_flags(void) 
{
	if (zone < 3 && zone > -1) {
		if (!nofail)
			return flags[zone];
		else
			return flags[zone + 3];
	}
	if (zone == 3) { /* all zones */
		return 0xffffffffu;
		
	}
	
	return flags[0];
}

static uint32_t alloc_counts[6];
static uint32_t fail_counts[6];

int gfp_flag_count = sizeof(flags) / sizeof(flags[0]); 	

struct task_struct *et;
int should_stop = 0;

int exhuast_hw_zones(void *data) 
{
	int count = 0;
	struct page *leaked = NULL;
	uint32_t flag = 0;
	
	
	do {
		flag = derive_zone_flags();
		
		if (flag == 0xffffffffu) {
			flag = flags[count % 6];
		}
		
		
                /* "leak" is the only option now */
		leaked = alloc_pages_node(node, flag, 0);
		
		if (leaked  != NULL) {
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



