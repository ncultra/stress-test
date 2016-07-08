#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gfp.h>
#include <linux/mmzone.h>
#include <linux/percpu-defs.h>

#include <linux/mm.h>
#include <linux/slab.h>
#include <linux malloc.h>
#include <linux vmalloc.h>
#include <linux/mempool.h>
#include <linux/percpu.h>
#include <linux/printk.h>

int node = 0;
int count = 0;

gfp_t flags[] = {
	__GFP_ATOMIC | __GFP_KERNEL | __GFP_HIGH | __GFP_MEMALLOC | __GFP_DMA,
	__GFP_ATOMIC | __GFP_KERNEL | __GFP_HIGH | __GFP_MEMALLOC | __GFP_DMA32, 
	__GFP_ATOMIC | __GFP_KERNEL | __GFP_HIGH | __GFP_MEMALLOC | __GFP_HIGHMEM
};

int gfp_flag_size = sizeof(flags) / sizeof(gfp_t); 	
 
void exhuast_hw_zones(void) 
{
	int lflags = 0;
	void *ret = NULL;
	
	do {
		ret = kmalloc_node(0x1000, flags[lflags % 3], node);
		count++;
	} while (ret != NULL);
}


static int __init sa_init(void)
{
	exhuast_hw_zones();
	return 0;
	
}


static void __exit sa_exit(void)
{
	printk(KERN_INFO, "exuast got %d pages\n", count);
	
}

module_init(sa_init);
module_exit(sa_exit);

MODULE_LICENCE("GPL");

/*********************************************
 * TODO: page alloc
 *       gen_pool_create/alloc 
 * https://lwn.net/Articles/125842/
 *************************/



