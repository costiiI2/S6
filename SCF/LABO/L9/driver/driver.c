#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/io.h>
#include <linux/of.h>

#define DEVICE_NAME "de1_io"
#define CLASS_NAME "de1_io"

#define BASE_ADDR 0xff200000
#define IO_MEM_SIZE 0x1000

#define CNST 0x0

#define KERNEL_0_2_OFFSET 0x4
#define KERNEL_3_5_OFFSET 0x8
#define KERNEL_6_8_OFFSET 0xC

#define IMG_OFFSET 0x10

#define TEST_FIR_OFFSET 0x14
#define SIZES_OFFSET 0x18

#define RETURN_OFFSET 0x1C

#define READ_WRITE_OFFSET 0x20

#define MAX_REGISTERS 8
#define MAX_READ_REGISTERS 8
#define MAX_WRITE_REGISTERS 5

const uint8_t REG_READ_ARRAY[MAX_READ_REGISTERS] = { KERNEL_0_2_OFFSET, KERNEL_3_5_OFFSET, KERNEL_6_8_OFFSET,TEST_FIR_OFFSET,SIZES_OFFSET, RETURN_OFFSET, READ_WRITE_OFFSET,CNST};

const uint8_t REG_WRITE_ARRAY[MAX_WRITE_REGISTERS] = {KERNEL_0_2_OFFSET, KERNEL_3_5_OFFSET, KERNEL_6_8_OFFSET,TEST_FIR_OFFSET,IMG_OFFSET};

#define WR_VALUE _IOW('a', 'a', int32_t *)

static int in_use = 0;

typedef struct
{
        struct platform_device *pdev;
        struct device *dev;
        struct class *class;
        struct cdev cdev;
        struct timer_list timer;
        volatile void *mem_ptr;
        int dev_num;
        int reg_index;

} priv_t;

static ssize_t hello_read(struct file *filep, char __user *buf, size_t count, loff_t *ppos)
{
        // read from mem point + offset to char buffer
        priv_t *priv = (priv_t *)filep->private_data;
        uint32_t value;
        if(priv->reg_index >= MAX_READ_REGISTERS)
        {
                return -EINVAL;
        }
        value = *(uint32_t *)(priv->mem_ptr + (REG_ARRAY[priv->reg_index]));
        if (copy_to_user(buf, &value, count) != 0)
        {
                return -EFAULT;
        }

        return 1;
}

static ssize_t hello_write(struct file *filep, const char __user *buf, size_t count, loff_t *ppos)
{
        // write to mem point +  offset  with value in char buffer
        priv_t *priv = (priv_t *)filep->private_data;

        uint32_t value;
        if (priv->reg_index >= MAX_WRITE_REGISTERS)
        {
                return -EINVAL;
        }

        if (copy_from_user(&value, buf, count) != 0)
        {
                return -EFAULT;
        }

        *(uint32_t *)(priv->mem_ptr + (REG_ARRAY[priv->reg_index])) = value;
 
        return 1;
}

/*
** This function will be called when we write IOCTL on the Device file
*/
static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
        priv_t *priv = (priv_t *)file->private_data;

        switch (cmd)
        {
        case WR_VALUE:
            
                if (arg < MAX_REGISTERS)
                {       
                        priv->reg_index = arg;
                }
                else
                {
                        return -EINVAL;
                }
                break;

        default:
                break;
        }
        return 0;
}

static int hello_open(struct inode *inode, struct file *filep)
{
        if (in_use == 1)
        {
                return -EBUSY;
        }
        in_use = 1;
        filep->private_data = container_of(inode->i_cdev, priv_t, cdev);

        return 0;
}

static int hello_release(struct inode *inode, struct file *filep)
{
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

static int hello_probe(struct platform_device *pdev)
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

        priv->pdev = pdev;
        platform_set_drvdata(pdev, priv);

        priv->dev = &pdev->dev;
        
        priv->reg_index = 0;
        // Allocate a range of character device numbers
        err = alloc_chrdev_region(&priv->dev_num, 0, 1, DEVICE_NAME);
        if (err < 0)
        {
                pr_err("Failed to allocate character device region\n");
                goto err_alloc_chrdev;
        }

        // Create a class for the device
        priv->class = class_create(CLASS_NAME);
        if (IS_ERR(priv->class))
        {
                pr_err("Failed to create class\n");
                err = PTR_ERR(priv->class);
                goto err_create_class;
        }

        // Initialize the character device
        cdev_init(&priv->cdev, &hello_fops);
        priv->cdev.owner = THIS_MODULE;

        // Add the character device to the kernel
        priv->dev_num = MKDEV(MAJOR(priv->dev_num), 0);
        err = cdev_add(&priv->cdev, priv->dev_num, 1);
        if (err < 0)
        {
                pr_err("Failed to add character device\n");
                goto err_create_class;
        }

        device_create(priv->class, NULL, priv->dev_num, NULL, DEVICE_NAME);

        // Allocate memory for the memory pointer
        priv->mem_ptr = ioremap(BASE_ADDR, IO_MEM_SIZE);
        if (priv->mem_ptr == NULL)
        {
                err = -EINVAL;
                goto err_ioremap;
        }

        pr_info("module initialized\n");
        return 0;

err_ioremap:
        device_destroy(priv->class, priv->dev_num);
        cdev_del(&priv->cdev);
err_create_class:
        class_destroy(priv->class);
        unregister_chrdev_region(priv->dev_num, 1);
err_alloc_chrdev:

return  err;

}

static int hello_exit(struct platform_device *pdev)
{
        priv_t *priv = platform_get_drvdata(pdev);

        device_destroy(priv->class, priv->dev_num);
        cdev_del(&priv->cdev);
        class_destroy(priv->class);
        unregister_chrdev_region(priv->dev_num, 1);
        iounmap(priv->mem_ptr);

        pr_info("module removed\n");

        return 0;
}

MODULE_AUTHOR("REDS");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("L6_SCF_LAB");
MODULE_VERSION("1.0");
MODULE_INFO(intree, "Y");

static const struct of_device_id pushbutton_driver_id[] = {
    {.compatible = "SCF"},
    {/* END */},
};

static struct platform_driver hello_driver = {
    .driver = {
        .name = DEVICE_NAME,
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(pushbutton_driver_id),

    },
    .probe = hello_probe,
    .remove = hello_exit,
};

module_platform_driver(hello_driver);