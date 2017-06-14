#include <linux/init.h>
#include <linux/module.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/rwlock.h>
#include <linux/slab.h>



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Visnja Krsmanovic");

static const char buffer[] = "ZuehlkeCamp2017\n\0";
static const int length_bf = strlen(buffer);
static uint8_t *data;
static rwlock_t data_lock;



//static int jif;
//static int foo;
//static int id;


static struct kobject *kobj;

/* File id */
static ssize_t id_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	return sprintf(buf, "%s", buffer);
}
static ssize_t id_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
	char temp_buf[length_bf + 1];

	if (length_bf != count)
		return -EINVAL;
	memcpy(temp_buf, buf, count);
	if (strncmp(temp_buf, buffer, length_bf) == 0)
		return length_bf;
	else
		return -EINVAL;
}
static struct kobj_attribute id_attribute = __ATTR(id, 0664, id_show, id_store);
//////////////////////////////////////////////////////////////////////////////
/* File jiffies */
static ssize_t jiffies_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	return sprintf(buf, "%llu\n", get_jiffies_64());
}

static struct kobj_attribute jiffies_attribute = __ATTR_RO(jiffies);

/////////////////////////////////////////////////////////////////////////////
/* File foo */
static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	int res = 0;

	read_lock(&data_lock);
	res = sprintf(buf, "%s\n", data);
//	res = simple_read_from_buffer(buf, len, ppos, data, PAGE_SIZE);
	read_unlock(&data_lock);
	return res;
}
static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
	int res = 0;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;
	write_lock(&data_lock);
	res = sprintf(data, "%s\n", buf);
//	res = simple_write_to_buffer(data, PAGE_SIZE, ppos, buf, len);
	write_unlock(&data_lock);
	return res;
}
static struct kobj_attribute foo_attribute = __ATTR(foo, 0644,
			foo_show, foo_store);


static struct attribute *attrs[] = {
	&id_attribute.attr,
	&jiffies_attribute.attr,
	&foo_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

//---------------------------------------------------------------------------
static int __init sysFS_init(void)
{
	int ret = 0;
	data = NULL;
	rwlock_init(&data_lock);
	data = kzalloc(PAGE_SIZE, GFP_KERNEL);

	kobj = kobject_create_and_add("zuehlke", kernel_kobj);
	if (!kobj)
		return -ENOMEM;

	/* Create the files associated with this kobject */
	ret = sysfs_create_group(kobj, &attr_group);
	if (ret) {
		kobject_put(kobj);
		if (data != NULL)
			kzfree(data);
	}

	return ret;
}

static void __exit sysFS_exit(void)
{
	if (data != NULL)
		kzfree(data);
	kobject_put(kobj);
}

module_init(sysFS_init);
module_exit(sysFS_exit);
