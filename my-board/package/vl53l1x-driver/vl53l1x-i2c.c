
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/i2c.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/unaligned.h>
#include <linux/string.h>

#include <linux/iio/iio.h>
#include <linux/iio/buffer.h>
#include <linux/iio/trigger.h>
#include <linux/iio/trigger_consumer.h>
#include <linux/iio/triggered_buffer.h>

#define VL53L1X_CHUNK_SIZE = 32

#define VL_REG_CONFIG_START_ADDR	0x002D

// data class
struct vl53l1x_data {
	struct i2c_client *client;
	struct completion completion;
	struct regulator *vdd_supply;
	struct gpio_desc *reset_gpio;
	struct iio_trigger *trig;
};

int vl53l1x_write_byte(struct i2c_client *client, const u16 reg, const u8 data)
{
	int ret;

	ret = i2c_smbus_write_byte_data(client, reg, data);
	if (ret < 0) {
		dev_err(&client->dev, "Faild to write byte to reg 0x%04X: %d\n", reg, ret);
		return ret;
	}

	return 0;
}

int vl53l1x_write_multi(struct i2c_client *client, u16 reg, const u8 *data, size_t count)
{
	int ret;
	size_t offset = 0;

	while (offset < count) {
		int remaining = count - offset;
		int current_chunk = (remaining < VL53L1X_CHUNK_SIZE) ? remaining : VL53L1X_CHUNK_SIZE;

		u8 buf[VL53L1X_CHUNK_SIZE + 2];
		u16 current_reg = reg + offset;

		buf[0] = (current_reg >> 8) & 0xFF;
		buf[1] = current_reg & 0xFF;

		memcpy(&buf[2], &data[offset], current_chunk);

		ret = i2c_smbus_write_block_data(client, buf, current_chunk + 2);
		if (ret < 0) {
			dev_err(&client->dev, "Faild to write block to reg 0x%04X: %d\n", current_reg, ret);
			return ret;
		}

		offset += current_chunk;
	}

	return 0;
}

// iio channel spec
static const struct iio_chan_spec vl53l1x_channels[] = {
	{
		.type = IIO_DISTANCE,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) |
				      BIT(IIO_CHAN_INFO_SCALE),
		.scan_index = 0,
		.scan_type = {
			.sign = 'u',
			.realbits = 16,
			.storagebits = 16,
		},
	},
	IIO_CHAN_SOFT_TIMESTAMP(1),
};

// probe
static int vl53l1x_probe(struct i2c_client *client)
{

};


// device ID
static const struct i2c_device_id vl53l1x_id[] = {
    { "vl53l1x" },
    { }
};
MODULE_DEVICE_TABLE(i2c, vl53l1x_id);

// DTS match
static const struct of_device_id st_vl53l1x_dt_match[] = {
    { .compatible = "st,vl53l1x"},
    { }
};
MODULE_DEVICE_TABLE(of, st_vl53l1x_dt_match);

// driver
static struct i2c_driver vl53l1x_driver = {
    .driver = {
        .name = "vl53l1x-i2c",
        .of_match_table = st_vl53l1x_dt_match,
    },
    .probe = vl53l1x_probe,
    .id_table = vl53l1x_id,
};
module_i2c_driver(vl53l1x_driver);

// LICENSE
MODULE_AUTHOR("Naoya");
MODULE_DESCRIPTION("ST vl53l1x ToF ranging sensor driver");
MODULE_LICENSE("GPL v2");
