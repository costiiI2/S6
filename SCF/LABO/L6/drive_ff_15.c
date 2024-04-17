#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "de1_io"
#define CLASS_NAME "de1_io"

#define MAX_REGISTERS 10

#define CST_OFFSET 0x0
#define TEST_REG 0x4
#define KEYS 0x8 // not used for this program
#define EDGE_CAP 0xC
#define SWITCHES 0x10
#define LED_OFFSET 0x14
#define LED_SET 0x18 // not used for this program
#define LED_CLR 0x1C // not used for this program
#define HEX3_0_OFFSET 0x20
#define HEX5_4_OFFSET 0x24

#define REG_ARRAY [MAX_REGISTERS] = {CST_OFFSET, TEST_REG, KEYS, EDGE_CAP, SWITCHES, LED_OFFSET, LED_SET, LED_CLR, HEX3_0_OFFSET, HEX5_4_OFFSET}

#define WR_VALUE _IOW('a', 'a', int32_t *)

static int in_use = 0;


static ssize_t hello_read(struct file *filep, char __user *buf, size_t count, loff_t *ppos)
{
        // read from mem point + offset to char buffer
        priv_t *priv = (priv_t *)filep->private_data;
        int value;

        value = *(priv->mem_ptr + REG_ARRAY[priv->reg_index]);

        if (copy_to_user(buf, &value, sizeof(int)) != 0)
        {
                return -EFAULT;
        }

        return 1;
}

static ssize_t hello_write(struct file *filep, const char __user *buf, size_t count, loff_t *ppos)
{
        // write to mem point +  offset  with value in char buffer
        priv_t *priv = (priv_t *)filep->private_data;

        int value;

        if (copy_from_user(&value, buf + sizeof(int), sizeof(int)) != 0)
        {
                return -EFAULT;
        }

        *(priv->mem_ptr + REG_ARRAY[priv->reg_index]) = value;
        return 1;
}

/*
** This function will be called when we write IOCTL on the Device file
*/
static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
        priv_t *priv = (priv_t *)filep->private_data;
        int value;

        switch (cmd)
        {
        case WR_VALUE:
                if (copy_from_user(&value, (int32_t *)arg, sizeof(value)))
                {
                        pr_err("Data Write : Err!\n");
                }
                pr_info("Value = %d\n", value);

                if (value > -1 && value < MAX_REGISTERS)
                {
                        priv->reg_index = value;
                }
                else
                {
                        pr_err("Invalid register index\n");
                        return -EINVAL;
                }
                break;

        default:
                pr_info("Default\n");
                break;
        }
        return 0;
}

static int hello_open(struct inode *inode, struct file *filep)
{
        // open
        if (in_use == 1)
        {
                return -EBUSY;
        }
        in_use = 1;
        filep->private_data = container_of(inodep->i_cdev, priv_t, cdev);

        return 0;
}

static int hello_release(struct inode *inode, struct file *filep)
{
        // release
        in_use = 0;
        filep->private_data = NULL;
        return 0;
}

static const struct file_operations hello_fops = {
    .owner = THIS_MODULE,
    .read = hello_read,
    .write = hello_write,
    .open = hello_open,
    .release = hello_release,
    .unlocked_ioctl = etx_ioctl,
};

typedef struct
{
        struct device *dev;
        struct class *class;
        struct cdev cdev;
        struct timer_list timer;
        void *mem_ptr;
        int dev_num;
        int reg_index;

} priv_t;

static int __init hello_init(struct platform_device *pdev)
{
        int err;
        priv_t *priv;

        dev_info(&pdev->dev, "probe() called!\n");

        priv = devm_kzalloc(&pdev->dev, sizeof(priv_t), GFP_KERNEL);
        if (priv == NULL)
        {
                dev_err(&pdev->dev,
                        "Failed to allocate memory for module!\n");
                err = -ENOMEM;
                return err;
        }

        platform_set_drvdata(pdev, priv);
        priv->dev = &pdev->dev;
        priv->reg_index = 0;
        // Allocate a range of character device numbers
        err = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
        if (err < 0)
        {
                pr_err("Failed to allocate character device region\n");
        }

        // Create a class for the device
        priv->class = class_create(THIS_MODULE, CLASS_NAME);
        if (IS_ERR(priv->class))
        {
                pr_err("Failed to create class\n");
                err = PTR_ERR(priv->class);
        }

        // Initialize the character device
        cdev_init(&priv->cdev, &hello_fops);
        priv->cdev.owner = THIS_MODULE;

        // Add the character device to the kernel
        priv->dev_num = MKDEV(MAJOR(dev_num), 0);
        err = cdev_add(&priv->cdev, priv->dev_num 1);
        if (err < 0)
        {
                pr_err("Failed to add character device\n");
        }

        device_create(priv->class, NULL, priv->dev_num, NULL, DEVICE_NAME);

        // Allocate memory for the memory pointer
        priv->mem_ptr = ioremap_nocache(0xff200000, 0x1000);
        if (priv->mem_ptr == NULL)
        {
                err = -EINVAL;
                goto ioremap_fail;
        }

        pr_info("Hello module initialized\n");
        return 0;
}

static void __exit hello_exit(void)
{
        priv_t *priv = platform_get_drvdata(pdev);

        device_destroy(priv->class, priv->dev_num);
        cdev_del(&priv->cdev);
        class_destroy(priv->class);
        unregister_chrdev_region(priv->dev_num, 1);
        iounmap(priv->mem_ptr);

        pr_info("Hello module removed\n");
}

MODULE_AUTHOR("REDS");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple character device driver");
MODULE_VERSION("1.0");

module_init(hello_init);
module_exit(hello_exit);