#define LINUX

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gfp.h>
#include <linux/mmzone.h>
#include <linux/nodemask.h>
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


static int node = 0; /* node id */
static int zone = 0; /* 0 dma, 1 dma32, 2 highmem, 3 all */
static int leak = 1; /* should leak pages as part of the test */
static int nofail = 0;
static int dry_run = 0;


module_param(node,    int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
module_param(zone,    int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
module_param(leak,    int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
module_param(nofail,  int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
module_param(dry_run, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);


int count = 0;

unsigned int  flags[] = {
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
		return 0xffffffff;
	}

	return flags[0];
}

static uint32_t alloc_counts[6];
static uint32_t fail_counts[6];

int gfp_flag_count = sizeof(flags) / sizeof(flags[0]);
static struct page *pptrs[4096];
int ppage_count = sizeof(pptrs) / sizeof(pptrs[0]);


struct task_struct *et;
int should_stop = 0;


void meminfo_zones(int nid)
{
	int zone_type;
	unsigned long managed_pages = 0, spanned_pages = 0, present_pages = 0;
	unsigned long free_pages = 0;
	
	pg_data_t *pgdat = NODE_DATA(nid);

	for (zone_type = 0; zone_type < MAX_NR_ZONES; zone_type++) {
		managed_pages += pgdat->node_zones[zone_type].managed_pages;
		spanned_pages += pgdat->node_zones[zone_type].spanned_pages;
		present_pages += pgdat->node_zones[zone_type].present_pages;
		free_pages += node_page_state(nid, NR_FREE_PAGES);
	}
	printk(KERN_DEBUG "STRESS: node %d free pages %lu, managed pages %lu",
	       nid, free_pages, managed_pages);
	
}



int meminfo_nodes(void *data) 
{
	int node = first_memory_node;

	for_each_online_node(node) {
		meminfo_zones(node);
	}
	
	do {
		msleep(100);
	} while (!should_stop);
	return 0;
	
}


static int __init sa_init(void)
{
	
	printk(KERN_DEBUG "STRESS: flags array entry is %d elements\n", gfp_flag_count);
	if (dry_run) {
		printk(KERN_DEBUG "cmd line: node %d, zone %d, leak %d, nofail %d\n",
		       node, zone, leak, nofail);
	} else {
		et = kthread_run(meminfo_nodes, NULL, "stress allocator");
	}
	
	return 0;
	
}

static void __exit sa_exit(void)
{
	should_stop = 1;

	printk(KERN_DEBUG "STRESS: stress-alloc alloc counts: %d %d %d\n",
	       alloc_counts[0], alloc_counts[1], alloc_counts[2]);
	printk(KERN_DEBUG "STRESS: stress-alloc failures: %d %d %d\n",
	       fail_counts[0], fail_counts[1], fail_counts[2]);

}

module_init(sa_init);
module_exit(sa_exit);

/*********************************************
 * TODO:
 *       gen_pool_create/alloc
 * https://lwn.net/Articles/125842/
 *************************/



