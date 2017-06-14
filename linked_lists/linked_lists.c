#include <linux/init.h>
#include <linux/module.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/rwlock.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/list.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Visnja Krsmanovic");

struct identity {
	struct list_head list;
	char name[32];
	int id;
	bool busy;
};


static struct identity *identity_list;
//LIST_HEAD(&identity_list);

int identity_create(char* name, int id)
{
	//struct identity new_elem;
	struct list_head head;

	INIT_LIST_HEAD(&head);
	identity_list = kmalloc(sizeof(struct identity *),GFP_KERNEL);
	if(identity_list == NULL)
		return -1; //todo
	strcpy(identity_list->name, name);
	identity_list->id = id;

	list_add_tail(&identity_list->list, &head);

	return 0;
}
struct identity *identity_find(int id)
{

}
void identity_destroy(int id)
{
	if(!list_empty(&identity_list->list))
		list_del_init(&identity_list->list);
//list_del_init(struct list_head *entry)
}

/////////////////////////////////////
static int __init llist_init(void)
{
	//INIT_LIST_HEAD(&identity_list.list);

	return 0;
}

static void __exit llist_exit(void)
{

}

module_init(llist_init);
module_exit(llist_exit);
