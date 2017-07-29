/* Low-level parallel port routines for GPIO.
 *
 * Author: Jim Garlick <garlick.jim@gmail.com>
 *
 * Based on parport_sunbpp.c.
 *
 * A PC parallel port style interface is cobbled together from GPIO pins:
 * 8-bit data out, 5-bit status in, 4 bit control out
 * No interrupts or dma; SPP mode only.
 *
 * See https://lwn.net/Articles/533632/ for info on the gpiod_ API used here.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/parport.h>
#include <linux/slab.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/of_device.h>

struct parport_gpio_ctx {
	struct gpio_descs *data;
	struct gpio_descs *status;
	struct gpio_descs *control;
	spinlock_t lock;
};

static unsigned char
parport_gpio_read_data(struct parport *p)
{
	struct parport_gpio_ctx *ctx = p->private_data;
	unsigned char data;
	unsigned long flags;

	spin_lock_irqsave (&ctx->lock, flags);

	data = gpiod_get_raw_value (ctx->data->desc[0]);
	data |= (gpiod_get_raw_value (ctx->data->desc[1]) << 1);
	data |= (gpiod_get_raw_value (ctx->data->desc[2]) << 2);
	data |= (gpiod_get_raw_value (ctx->data->desc[3]) << 3);
	data |= (gpiod_get_raw_value (ctx->data->desc[4]) << 4);
	data |= (gpiod_get_raw_value (ctx->data->desc[5]) << 5);
	data |= (gpiod_get_raw_value (ctx->data->desc[6]) << 6);
	data |= (gpiod_get_raw_value (ctx->data->desc[7]) << 7);

	spin_unlock_irqrestore (&ctx->lock, flags);

	return data;
}

static void
parport_gpio_write_data(struct parport *p, unsigned char data)
{
	struct parport_gpio_ctx *ctx = p->private_data;
	int val[8];
	unsigned long flags;

	spin_lock_irqsave (&ctx->lock, flags);

	val[0] = data & 1;
	val[1] = (data >> 1) & 1;
	val[2] = (data >> 2) & 1;
	val[3] = (data >> 3) & 1;
	val[4] = (data >> 4) & 1;
	val[5] = (data >> 5) & 1;
	val[6] = (data >> 6) & 1;
	val[7] = (data >> 7) & 1;
	gpiod_set_raw_array_value (ctx->data->ndescs, ctx->data->desc, val);

	spin_unlock_irqrestore (&ctx->lock, flags);
}

static unsigned char
parport_gpio_read_control(struct parport *p)
{
	struct parport_gpio_ctx *ctx = p->private_data;
	unsigned char control;
	unsigned long flags;

	spin_lock_irqsave (&ctx->lock, flags);

	control = gpiod_get_raw_value (ctx->control->desc[0]);
	control |= (gpiod_get_raw_value (ctx->control->desc[1]) << 1);
	control |= (gpiod_get_raw_value (ctx->control->desc[2]) << 2);
	control |= (gpiod_get_raw_value (ctx->control->desc[3]) << 3);

	spin_unlock_irqrestore (&ctx->lock, flags);

	return control;
}

static void
parport_gpio_write_control(struct parport *p, unsigned char control)
{
	struct parport_gpio_ctx *ctx = p->private_data;
	int value[4];
	unsigned long flags;

	spin_lock_irqsave (&ctx->lock, flags);

	value[0] = ~control & 1; // ~nStrobe
	value[1] = (~control >> 1) & 1; // ~nAutoLF
	value[2] = (control >> 2) & 1; // nInitialize
	value[3] = (~control >> 3) & 1; // ~nSelect
	gpiod_set_raw_array_value (ctx->control->ndescs, ctx->control->desc, value);

	spin_unlock_irqrestore (&ctx->lock, flags);
}

static unsigned char
parport_gpio_frob_control(struct parport *p, unsigned char mask,
			   unsigned char val)
{
	unsigned char old = parport_gpio_read_control(p);
	parport_gpio_write_control(p, (old & ~mask) ^ val);
	return old;
}

static unsigned char
parport_gpio_read_status(struct parport *p)
{
	struct parport_gpio_ctx *ctx = p->private_data;
	unsigned char status;
	unsigned long flags;

	spin_lock_irqsave (&ctx->lock, flags);

	status = (gpiod_get_raw_value (ctx->status->desc[0]) << 3); // nError
	status |= (gpiod_get_raw_value (ctx->status->desc[1]) << 4); // Select
	status |= (gpiod_get_raw_value (ctx->status->desc[2]) << 5); // Paperout
	status |= (gpiod_get_raw_value (ctx->status->desc[3]) << 6); // nAck
	status |= (~gpiod_get_raw_value (ctx->status->desc[4]) << 7); // ~Busy

	spin_unlock_irqrestore (&ctx->lock, flags);

	return status;
}

static void
parport_gpio_init_state(struct pardevice *d, struct parport_state *s)
{
}

static void
parport_gpio_save_state(struct parport *p, struct parport_state *s)
{
}

static void
parport_gpio_restore_state(struct parport *p, struct parport_state *s)
{
}

static void
parport_gpio_enable_irq(struct parport *p)
{
}

static void
parport_gpio_disable_irq(struct parport *p)
{
}

static void
parport_gpio_data_forward(struct parport *p)
{
}

static void
parport_gpio_data_reverse(struct parport *p)
{
}

static struct parport_operations parport_gpio_ops = {
	.write_data	= parport_gpio_write_data,
	.read_data	= parport_gpio_read_data,

	.write_control	= parport_gpio_write_control,
	.read_control	= parport_gpio_read_control,
	.frob_control	= parport_gpio_frob_control,

	.read_status	= parport_gpio_read_status,

	.enable_irq	= parport_gpio_enable_irq,
	.disable_irq	= parport_gpio_disable_irq,

	.data_forward	= parport_gpio_data_forward,
	.data_reverse	= parport_gpio_data_reverse,

	.init_state	= parport_gpio_init_state,
	.save_state	= parport_gpio_save_state,
	.restore_state	= parport_gpio_restore_state,

	.epp_write_data	= parport_ieee1284_epp_write_data,
	.epp_read_data	= parport_ieee1284_epp_read_data,
	.epp_write_addr	= parport_ieee1284_epp_write_addr,
	.epp_read_addr	= parport_ieee1284_epp_read_addr,

	.ecp_write_data	= parport_ieee1284_ecp_write_data,
	.ecp_read_data	= parport_ieee1284_ecp_read_data,
	.ecp_write_addr	= parport_ieee1284_ecp_write_addr,

	.compat_write_data	= parport_ieee1284_write_compat,
	.nibble_read_data	= parport_ieee1284_read_nibble,
	.byte_read_data		= parport_ieee1284_read_byte,

	.owner		= THIS_MODULE,
};

static void parport_gpio_detach (struct parport_gpio_ctx *ctx)
{
	if (ctx) {
		if (ctx->data)
			gpiod_put_array (ctx->data);
		if (ctx->status)
			gpiod_put_array (ctx->status);
		if (ctx->control)
			gpiod_put_array (ctx->control);
		kfree (ctx);
	}
}

static int parport_gpio_attach (struct device *dev,
                                struct parport_gpio_ctx **ctxp)
{
	struct parport_gpio_ctx *ctx;
	int i;

	if (!(ctx = kzalloc (sizeof (*ctx), GFP_KERNEL))) {
		pr_err ("parport_gpio: out of memory\n");
		goto out;
	}
	ctx->data = gpiod_get_array_optional (dev, "data", GPIOD_OUT_LOW);
	if (!ctx->data || ctx->data->ndescs != 8) {
		pr_err ("parport_gpio: could not get data pins\n");
		goto out;
        }
	ctx->status = gpiod_get_array_optional (dev, "status", GPIOD_IN);
	if (!ctx->status|| ctx->status->ndescs != 5) {
		pr_err ("parport_gpio: could not get status pins\n");
		goto out;
	}
	ctx->control = gpiod_get_array_optional (dev, "control",
						 GPIOD_OUT_LOW);
	if (!ctx->control || ctx->control->ndescs != 4) {
		pr_err ("parport_gpio: could not get control pins\n");
		goto out;
	}
	for (i = 0; i < ctx->data->ndescs; i++) {
		if (gpiod_cansleep (ctx->data->desc[i]))
			goto out_cansleep;
	}
	for (i = 0; i < ctx->status->ndescs; i++) {
		if (gpiod_cansleep (ctx->status->desc[i]))
			goto out_cansleep;
	}
	for (i = 0; i < ctx->control->ndescs; i++) {
		if (gpiod_cansleep (ctx->control->desc[i]))
			goto out_cansleep;
	}
	pr_info ("parport_gpio: data on gpio pins [%d,%d,%d,%d,%d,%d,%d,%d]\n",
                 desc_to_gpio (ctx->data->desc[7]),
                 desc_to_gpio (ctx->data->desc[6]),
                 desc_to_gpio (ctx->data->desc[5]),
                 desc_to_gpio (ctx->data->desc[4]),
                 desc_to_gpio (ctx->data->desc[3]),
                 desc_to_gpio (ctx->data->desc[2]),
                 desc_to_gpio (ctx->data->desc[1]),
                 desc_to_gpio (ctx->data->desc[0]));
	pr_info ("parport_gpio: status on gpio pins [%d,%d,%d,%d,%d]\n",
                 desc_to_gpio (ctx->status->desc[4]),
                 desc_to_gpio (ctx->status->desc[3]),
                 desc_to_gpio (ctx->status->desc[2]),
                 desc_to_gpio (ctx->status->desc[1]),
                 desc_to_gpio (ctx->status->desc[0]));
	pr_info ("parport_gpio: control on gpio pins [%d,%d,%d,%d]\n",
                 desc_to_gpio (ctx->control->desc[3]),
                 desc_to_gpio (ctx->control->desc[2]),
                 desc_to_gpio (ctx->control->desc[1]),
                 desc_to_gpio (ctx->control->desc[0]));
	spin_lock_init (&ctx->lock);
	*ctxp = ctx;
	return 0;
out_cansleep:
	pr_err ("parport_gpio: inappropriate gpio pin (can sleep)\n");
out:
	parport_gpio_detach (ctx);
	return -1;
}

static int parport_gpio_probe(struct platform_device *op)
{
	struct parport_gpio_ctx *ctx = NULL;
	struct parport *p;
	int irq = PARPORT_IRQ_NONE;
	int dma = PARPORT_DMA_NONE;
	unsigned long base = 0;

	if (parport_gpio_attach (&op->dev, &ctx) < 0)
		goto out;
	if (!(p = parport_register_port (base, irq, dma, &parport_gpio_ops))) {
		pr_err ("parport_gpio: parport_register_port\n");
		goto out_detach;
	}
	p->private_data = ctx;
	p->modes = PARPORT_MODE_PCSPP;
	p->dev = &op->dev;

	dev_set_drvdata (&op->dev, p);

	parport_announce_port (p);
	return 0;
out_detach:
	parport_gpio_detach (ctx);
out:
	return -1;
}

static int parport_gpio_remove(struct platform_device *op)
{
	struct parport *p = dev_get_drvdata (&op->dev);

	parport_gpio_detach (p->private_data);
	p->private_data = NULL;

	parport_remove_port (p);
	parport_put_port (p);

	dev_set_drvdata (&op->dev, NULL);

	return 0;
}

static const struct of_device_id parport_gpio_match[] = {
	{ .compatible = "parport-gpio", },
	{},
};

MODULE_DEVICE_TABLE(of, parport_gpio_match);

static struct platform_driver parport_gpio_driver = {
	.driver = {
		.name = "parport-gpio",
		.of_match_table = parport_gpio_match,
	},
	.probe = parport_gpio_probe,
	.remove = parport_gpio_remove,
};

module_platform_driver(parport_gpio_driver);

MODULE_AUTHOR("Jim Garlick");
MODULE_DESCRIPTION("Parport Driver for Raspberry Pi GPIO Parallel Port HAT");
MODULE_SUPPORTED_DEVICE("Raspberry Pi GPIO Parallel Port HAT");
MODULE_VERSION("1.0");
MODULE_LICENSE("GPL");
