
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/i2c.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/unaligned.h>
#include <linux/regmap.h>

#include <linux/iio/iio.h>
#include <linux/iio/buffer.h>
#include <linux/iio/trigger.h>
#include <linux/iio/trigger_consumer.h>
#include <linux/iio/triggered_buffer.h>

#define VL53L1X_CHUNK_SIZE 32

#define VL53L1X_REG_CONFIG_START_ADDR			0x002D

#define VL53L1X_REG_SYSTEM__MODE_START 			0x0087

#define VL53L1X_REG_IDENTIFICATION__MODEL_ID	0x010F

#define VL53L1X_MODE_START						0x40
#define VL53L1X_MODE_STOP						0x00
#define VL53L1X_MODEL_ID_VAL					0xEA


// default configuration
const u8 VL51L1X_DEFAULT_CONFIGURATION[] = {
0x00, /* 0x2d : set bit 2 and 5 to 1 for fast plus mode (1MHz I2C), else don't touch */
0x00, /* 0x2e : bit 0 if I2C pulled up at 1.8V, else set bit 0 to 1 (pull up at AVDD) */
0x00, /* 0x2f : bit 0 if GPIO pulled up at 1.8V, else set bit 0 to 1 (pull up at AVDD) */
0x01, /* 0x30 : set bit 4 to 0 for active high interrupt and 1 for active low (bits 3:0 must be 0x1), use SetInterruptPolarity() */
0x02, /* 0x31 : bit 1 = interrupt depending on the polarity, use CheckForDataReady() */
0x00, /* 0x32 : not user-modifiable */
0x02, /* 0x33 : not user-modifiable */
0x08, /* 0x34 : not user-modifiable */
0x00, /* 0x35 : not user-modifiable */
0x08, /* 0x36 : not user-modifiable */
0x10, /* 0x37 : not user-modifiable */
0x01, /* 0x38 : not user-modifiable */
0x01, /* 0x39 : not user-modifiable */
0x00, /* 0x3a : not user-modifiable */
0x00, /* 0x3b : not user-modifiable */
0x00, /* 0x3c : not user-modifiable */
0x00, /* 0x3d : not user-modifiable */
0xff, /* 0x3e : not user-modifiable */
0x00, /* 0x3f : not user-modifiable */
0x0F, /* 0x40 : not user-modifiable */
0x00, /* 0x41 : not user-modifiable */
0x00, /* 0x42 : not user-modifiable */
0x00, /* 0x43 : not user-modifiable */
0x00, /* 0x44 : not user-modifiable */
0x00, /* 0x45 : not user-modifiable */
0x20, /* 0x46 : interrupt configuration 0->level low detection, 1-> level high, 2-> Out of window, 3->In window, 0x20-> New sample ready , TBC */
0x0b, /* 0x47 : not user-modifiable */
0x00, /* 0x48 : not user-modifiable */
0x00, /* 0x49 : not user-modifiable */
0x02, /* 0x4a : not user-modifiable */
0x0a, /* 0x4b : not user-modifiable */
0x21, /* 0x4c : not user-modifiable */
0x00, /* 0x4d : not user-modifiable */
0x00, /* 0x4e : not user-modifiable */
0x05, /* 0x4f : not user-modifiable */
0x00, /* 0x50 : not user-modifiable */
0x00, /* 0x51 : not user-modifiable */
0x00, /* 0x52 : not user-modifiable */
0x00, /* 0x53 : not user-modifiable */
0xc8, /* 0x54 : not user-modifiable */
0x00, /* 0x55 : not user-modifiable */
0x00, /* 0x56 : not user-modifiable */
0x38, /* 0x57 : not user-modifiable */
0xff, /* 0x58 : not user-modifiable */
0x01, /* 0x59 : not user-modifiable */
0x00, /* 0x5a : not user-modifiable */
0x08, /* 0x5b : not user-modifiable */
0x00, /* 0x5c : not user-modifiable */
0x00, /* 0x5d : not user-modifiable */
0x01, /* 0x5e : not user-modifiable */
0xcc, /* 0x5f : not user-modifiable */
0x0f, /* 0x60 : not user-modifiable */
0x01, /* 0x61 : not user-modifiable */
0xf1, /* 0x62 : not user-modifiable */
0x0d, /* 0x63 : not user-modifiable */
0x01, /* 0x64 : Sigma threshold MSB (mm in 14.2 format for MSB+LSB), use SetSigmaThreshold(), default value 90 mm  */
0x68, /* 0x65 : Sigma threshold LSB */
0x00, /* 0x66 : Min count Rate MSB (MCPS in 9.7 format for MSB+LSB), use SetSignalThreshold() */
0x80, /* 0x67 : Min count Rate LSB */
0x08, /* 0x68 : not user-modifiable */
0xb8, /* 0x69 : not user-modifiable */
0x00, /* 0x6a : not user-modifiable */
0x00, /* 0x6b : not user-modifiable */
0x00, /* 0x6c : Intermeasurement period MSB, 32 bits register, use SetIntermeasurementInMs() */
0x00, /* 0x6d : Intermeasurement period */
0x0f, /* 0x6e : Intermeasurement period */
0x89, /* 0x6f : Intermeasurement period LSB */
0x00, /* 0x70 : not user-modifiable */
0x00, /* 0x71 : not user-modifiable */
0x00, /* 0x72 : distance threshold high MSB (in mm, MSB+LSB), use SetD:tanceThreshold() */
0x00, /* 0x73 : distance threshold high LSB */
0x00, /* 0x74 : distance threshold low MSB ( in mm, MSB+LSB), use SetD:tanceThreshold() */
0x00, /* 0x75 : distance threshold low LSB */
0x00, /* 0x76 : not user-modifiable */
0x01, /* 0x77 : not user-modifiable */
0x0f, /* 0x78 : not user-modifiable */
0x0d, /* 0x79 : not user-modifiable */
0x0e, /* 0x7a : not user-modifiable */
0x0e, /* 0x7b : not user-modifiable */
0x00, /* 0x7c : not user-modifiable */
0x00, /* 0x7d : not user-modifiable */
0x02, /* 0x7e : not user-modifiable */
0xc7, /* 0x7f : ROI center, use SetROI() */
0xff, /* 0x80 : XY ROI (X=Width, Y=Height), use SetROI() */
0x9B, /* 0x81 : not user-modifiable */
0x00, /* 0x82 : not user-modifiable */
0x00, /* 0x83 : not user-modifiable */
0x00, /* 0x84 : not user-modifiable */
0x01, /* 0x85 : not user-modifiable */
0x00, /* 0x86 : clear interrupt, use ClearInterrupt() */
0x00  /* 0x87 : start ranging, use StartRanging() or StopRanging(), If you want an automatic start after VL53L1X_init() call, put 0x40 in location 0x87 */
};

// data class
struct vl53l1x_data {
	struct regmap *regmap;
	struct completion completion;
	struct gpio_desc *reset_gpio;
};

static const struct regmap_config vl53l1x_regmap_config = {
	.reg_bits = 16,
	.val_bits = 8,
	.reg_format_endian = REGMAP_ENDIAN_BIG,
	.cache_type = REGCACHE_NONE,
};

static int vl53l1x_read(struct vl53l1x_data *data, const u16 reg, u8 *val)
{
	int ret;

	ret = regmap_read(data->regmap, reg, val);
	if (ret)
		return ret;

	return 0;
}

static int vl53l1x_read_u16(struct vl53l1x_data *data, const u16 reg, u16 *val)
{
	__be16 buf;
	int ret;

	ret = regmap_bulk_read(data->regmap, reg, &buf, sizeof(buf));
	if (ret)
		return ret;

	*val = be16_to_cpu(buf);
	return 0;
}


static int vl53l1x_write(struct vl53l1x_data *data, const u16 reg, const u8 *val)
{
	return regmap_write(data->regmap, reg, val);
}

static int vl53l1x_write_u16(struct vl53l1x_data *data, const u16 reg, const u16 *val)
{
	__be16 buf = cpu_to_be16(val);

	return regmap_bulk_write(data->regmap, reg, &buf, sizeof(buf));
	
}

static int vl53l1x_start_ranging(struct vl53l1x_data *data)
{
	int ret;
	ret = vl53l1x_write(data->regmap, VL53L1X_REG_SYSTEM__MODE_START, VL53L1X_MODE_START);
	if (ret)
		return ret;
	
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
	struct vl53l1x_data *data;
	struct iio_dev *indio_dev;
	struct device *dev = &client->dev;
	int ret;
	u8 model_id;

	indio_dev = devm_iio_device_alloc(&client->dev, sizeof(*data));
	if (!indio_dev)
		return -ENOMEM;
	
	data = iio_priv(indio_dev);
	
	data->regmap = devm_regmap_init_i2c(client, &vl53l1x_regmap_config);
	if (IS_ERR(data->regmap))
		return dev_err_probe(&client->dev, 
			PTR_ERR(data->regmap), "Failed to initalize regmap\n");

	/* vdd setting */
	ret = devm_regulator_get_enable(dev, "vdd");
	if (ret)
		return dev_err_probe(dev, ret, "Faild to enable VDD regulators\n");

	/* reset GPIO setting */
	data->reset_gpio = devm_gpiod_get_optional(dev, "xshut", GPIOD_OUT_HIGH);
	if (IS_ERR(data->reset_gpio))
		return PTR_ERR(data->reset_gpio);

	/* model check */
	ret = vl53l1x_read(data->regmap, VL53L1X_REG_IDENTIFICATION__MODEL_ID, &model_id);
	if (ret)
		return ret;

	if (model_id != VL53L1X_MODEL_ID_VAL)
		dev_info(dev, "unknown model. id: 0x%04x, continuing\n", model_id);

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
