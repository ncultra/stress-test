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


void meminfo_node(int nid)
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
	printk(KERN_DEBUG "STRESS: node %d free pages %d, managed pages %d",
	       nid, free_pages, managed_pages);
	
}



void fill_drain_node(int nid) 
{
	struct zone *zone;

	for_each_zone(zone)   {
		if (!populated_zone(zone)) {
			printk(KERN_DEBUG "STRESS: zone %s is not populated\n",
			       zone->name);
			continue;
		}
                /*
		zone_start_pfn
			zone name, type 
			managed, spanned, present
		  zone type (enum zone_type) mmzone.h:274
		  zone stats: zone.vm_stat[] mmzone.h:529 atomic_long_t
		NR_PAGES_FREE, NUMA_HIT, NUMA_MISS, NUMA_LOCAL, NUMA_OTHER
		(FREE / SPANNED) is % of zone allocation
		alloc pages until hit the low watermark
		free pages
		*/
		
	}
	
}


int exhuast_hw_zones(void *data)
{
	unsigned int count = 0;
	struct page *leaked = NULL;
	uint32_t flag = 0;


	do {
		flag = derive_zone_flags();

		if (flag == 0xffffffff) {
			flag = flags[count % 6];
		}

		leaked = alloc_pages_node(node, flag, 0);
		/* leaked mode is the only supported mode right now */
		if (leaked  != NULL) {
		  pptrs[count] = leaked;
			alloc_counts[count % 6]++;
                       if (!count % 1000) {
				printk(KERN_DEBUG "STRESS: stress-alloc alloc counts: %d %d %d %d %d %d\n",
				       alloc_counts[0], alloc_counts[1], alloc_counts[2],
					alloc_counts[3], alloc_counts[4], alloc_counts[5]);
		       }

		} else {
			fail_counts[count % 6]++;
				if (!count % 1000) {
				printk(KERN_DEBUG "STRESS: stress-alloc failures: %d %d %d %d %d %d\n",
				       fail_counts[0], fail_counts[1], fail_counts[2],
				       fail_counts[3], fail_counts[4], fail_counts[5]);
			}
		}


		if (!count % 1000) {
			printk(KERN_DEBUG "STRESS: zone alloc with flags 0x%ul\n", flag);
		}
		msleep(100);
		count++;

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
		et = kthread_run(exhuast_hw_zones, NULL, "stress allocator");
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



