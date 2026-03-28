/*======================================================================
    A globalmem driver as an example of char device drivers
    There are two same globalmems in this driver
    This example is to introduce the function of file->private_data

    The initial developer of the original code is Baohua Song
    <author@linuxdriver.cn>. All Rights Reserved.
======================================================================*/
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/uaccess.h>

#define GLOBALMEM_SIZE		0x1000
#define GLOBALMEM_DEVICE_COUNT	2
#define GLOBALMEM_MAJOR		254

#define GLOBALMEM_IOC_MAGIC	'g'

/* No payload: clear the whole device buffer. */
#define GLOBALMEM_IOCTL_CLEAR \
	_IO(GLOBALMEM_IOC_MAGIC, 0)
/* Read-only payload: return device size to userspace. */
#define GLOBALMEM_IOCTL_GET_SIZE \
	_IOR(GLOBALMEM_IOC_MAGIC, 1, int)
/* Write-only payload: fill the whole device buffer with one byte. */
#define GLOBALMEM_IOCTL_SET_FILL \
	_IOW(GLOBALMEM_IOC_MAGIC, 2, unsigned char)
/* Read/write payload: write a byte and return the old value. */
#define GLOBALMEM_IOCTL_EXCHANGE_BYTE \
	_IOWR(GLOBALMEM_IOC_MAGIC, 3, struct globalmem_ioc_byte)
/* Raw _IOC example: same semantics as EXCHANGE_BYTE. */
#define GLOBALMEM_IOCTL_RAW_EXCHANGE_BYTE \
	_IOC(_IOC_READ | _IOC_WRITE, GLOBALMEM_IOC_MAGIC, 4, \
	     sizeof(struct globalmem_ioc_byte))

#define GLOBALMEM_IOC_MAXNR	4

struct globalmem_ioc_byte {
	unsigned int index;
	unsigned char value;
};

struct globalmem_dev {
	struct cdev cdev;
	struct mutex lock;
	unsigned char mem[GLOBALMEM_SIZE];
};

static int globalmem_major = GLOBALMEM_MAJOR;
module_param(globalmem_major, int, 0444);

static struct globalmem_dev *globalmem_devp;

static int globalmem_open(struct inode *inode, struct file *filp)
{
	struct globalmem_dev *dev;

	dev = container_of(file_inode(filp)->i_cdev, struct globalmem_dev, cdev);
	filp->private_data = dev;

	return 0;
}

static int globalmem_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static long globalmem_ioctl_exchange_byte(struct globalmem_dev *dev,
					  void __user *argp)
{
	struct globalmem_ioc_byte data;
	unsigned char old_value;

	if (copy_from_user(&data, argp, sizeof(data)))
		return -EFAULT;

	if (data.index >= GLOBALMEM_SIZE)
		return -EINVAL;

	mutex_lock(&dev->lock);
	old_value = dev->mem[data.index];
	dev->mem[data.index] = data.value;
	data.value = old_value;
	mutex_unlock(&dev->lock);

	if (copy_to_user(argp, &data, sizeof(data)))
		return -EFAULT;

	return 0;
}

static long globalmem_ioctl(struct file *filp, unsigned int cmd,
			    unsigned long arg)
{
	struct globalmem_dev *dev = filp->private_data;
	void __user *argp = (void __user *)arg;

	if (_IOC_TYPE(cmd) != GLOBALMEM_IOC_MAGIC || _IOC_NR(cmd) > GLOBALMEM_IOC_MAXNR)
		return -ENOTTY;

	switch (cmd) {
	case GLOBALMEM_IOCTL_CLEAR:
		mutex_lock(&dev->lock);
		memset(dev->mem, 0, sizeof(dev->mem));
		mutex_unlock(&dev->lock);
		pr_info("globalmem: cleared device buffer\n");
		return 0;

	case GLOBALMEM_IOCTL_GET_SIZE: {
		int size = GLOBALMEM_SIZE;

		if (copy_to_user(argp, &size, sizeof(size)))
			return -EFAULT;
		return 0;
	}

	case GLOBALMEM_IOCTL_SET_FILL: {
		unsigned char value;

		if (copy_from_user(&value, argp, sizeof(value)))
			return -EFAULT;

		mutex_lock(&dev->lock);
		memset(dev->mem, value, sizeof(dev->mem));
		mutex_unlock(&dev->lock);
		return 0;
	}

	case GLOBALMEM_IOCTL_EXCHANGE_BYTE:
	case GLOBALMEM_IOCTL_RAW_EXCHANGE_BYTE:
		return globalmem_ioctl_exchange_byte(dev, argp);

	default:
		return -ENOTTY;
	}
}

static ssize_t globalmem_read(struct file *filp, char __user *buf, size_t count,
			      loff_t *ppos)
{
	struct globalmem_dev *dev = filp->private_data;
	loff_t pos = *ppos;

	if (pos < 0)
		return -EINVAL;
	if (pos >= GLOBALMEM_SIZE)
		return 0;

	if (count > GLOBALMEM_SIZE - pos)
		count = GLOBALMEM_SIZE - pos;

	mutex_lock(&dev->lock);
	if (copy_to_user(buf, dev->mem + pos, count)) {
		mutex_unlock(&dev->lock);
		return -EFAULT;
	}
	mutex_unlock(&dev->lock);

	*ppos = pos + count;
	pr_info("globalmem: read %zu bytes at offset %lld\n", count, pos);

	return count;
}

static ssize_t globalmem_write(struct file *filp, const char __user *buf,
			       size_t count, loff_t *ppos)
{
	struct globalmem_dev *dev = filp->private_data;
	loff_t pos = *ppos;

	if (pos < 0)
		return -EINVAL;
	if (pos >= GLOBALMEM_SIZE)
		return -ENOSPC;

	if (count > GLOBALMEM_SIZE - pos)
		count = GLOBALMEM_SIZE - pos;

	mutex_lock(&dev->lock);
	if (copy_from_user(dev->mem + pos, buf, count)) {
		mutex_unlock(&dev->lock);
		return -EFAULT;
	}
	mutex_unlock(&dev->lock);

	*ppos = pos + count;
	pr_info("globalmem: wrote %zu bytes at offset %lld\n", count, pos);

	return count;
}

static loff_t globalmem_llseek(struct file *filp, loff_t offset, int whence)
{
	loff_t new_pos;

	switch (whence) {
	case SEEK_SET:
		new_pos = offset;
		break;
	case SEEK_CUR:
		new_pos = filp->f_pos + offset;
		break;
	default:
		return -EINVAL;
	}

	if (new_pos < 0 || new_pos > GLOBALMEM_SIZE)
		return -EINVAL;

	filp->f_pos = new_pos;
	return new_pos;
}

static const struct file_operations globalmem_fops = {
	.owner		= THIS_MODULE,
	.open		= globalmem_open,
	.release	= globalmem_release,
	.llseek		= globalmem_llseek,
	.read		= globalmem_read,
	.write		= globalmem_write,
	.unlocked_ioctl	= globalmem_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl	= globalmem_ioctl,
#endif
};

static int globalmem_setup_cdev(struct globalmem_dev *dev, dev_t devno)
{
	cdev_init(&dev->cdev, &globalmem_fops);
	dev->cdev.owner = THIS_MODULE;

	return cdev_add(&dev->cdev, devno, 1);
}

static int __init globalmem_init(void)
{
	dev_t devno = MKDEV(globalmem_major, 0);
	int ret;
	int i;

	if (globalmem_major) {
		ret = register_chrdev_region(devno, GLOBALMEM_DEVICE_COUNT, "globalmem");
	} else {
		ret = alloc_chrdev_region(&devno, 0, GLOBALMEM_DEVICE_COUNT,
					  "globalmem");
		globalmem_major = MAJOR(devno);
	}
	if (ret)
		return ret;

	globalmem_devp = kcalloc(GLOBALMEM_DEVICE_COUNT, sizeof(*globalmem_devp),
				 GFP_KERNEL);
	if (!globalmem_devp) {
		ret = -ENOMEM;
		goto err_unregister_region;
	}

	for (i = 0; i < GLOBALMEM_DEVICE_COUNT; i++) {
		mutex_init(&globalmem_devp[i].lock);
		ret = globalmem_setup_cdev(&globalmem_devp[i],
					   MKDEV(globalmem_major, i));
		if (ret)
			goto err_del_cdevs;
	}

	pr_info("globalmem: registered %d devices on major %d\n",
		GLOBALMEM_DEVICE_COUNT, globalmem_major);
	return 0;

err_del_cdevs:
	while (--i >= 0)
		cdev_del(&globalmem_devp[i].cdev);
	kfree(globalmem_devp);
err_unregister_region:
	unregister_chrdev_region(MKDEV(globalmem_major, 0), GLOBALMEM_DEVICE_COUNT);
	return ret;
}

static void __exit globalmem_exit(void)
{
	int i;

	for (i = 0; i < GLOBALMEM_DEVICE_COUNT; i++)
		cdev_del(&globalmem_devp[i].cdev);

	kfree(globalmem_devp);
	unregister_chrdev_region(MKDEV(globalmem_major, 0), GLOBALMEM_DEVICE_COUNT);
	pr_info("globalmem: unloaded\n");
}

MODULE_AUTHOR("Song Baohua");
MODULE_LICENSE("Dual BSD/GPL");

module_init(globalmem_init);
module_exit(globalmem_exit);
