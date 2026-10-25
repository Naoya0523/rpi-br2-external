VL53L1X_APP_VERSION = 1.0
VL53L1X_APP_SITE = $(BR2_EXTERNAL_MYBOARD_PATH)/package/vl53l1x-app/src
VL53L1X_APP_SITE_METHOD = local

# インクルードパスとソースファイルを指定してクロスコンパイル
define VL53L1X_APP_BUILD_CMDS
	$(TARGET_CC) $(TARGET_CFLAGS) \
		$(@D)/main.c \
		$(@D)/platform/src/vl53l1_platform.c \
		$(@D)/core/src/vl53l1_api.c \
		$(@D)/core/src/vl53l1_api_core.c \
		$(@D)/core/src/vl53l1_api_calibration.c \
		$(@D)/core/src/vl53l1_api_debug.c \
		$(@D)/core/src/vl53l1_api_preset_modes.c \
		$(@D)/core/src/vl53l1_api_strings.c \
		$(@D)/core/src/vl53l1_core.c \
		$(@D)/core/src/vl53l1_core_support.c \
		$(@D)/core/src/vl53l1_error_strings.c \
		$(@D)/core/src/vl53l1_register_funcs.c \
		$(@D)/core/src/vl53l1_silicon_core.c \
		$(@D)/core/src/vl53l1_wait.c \
		-I$(@D)/core/inc \
		-I$(@D)/platform/inc \
		-o $(@D)/vl53l1x_app $(TARGET_LDFLAGS)
endef

# ターゲットの /usr/bin/ に実行ファイルをインストールする
define VL53L1X_APP_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/vl53l1x_app $(TARGET_DIR)/usr/bin/vl53l1x_app
endef

$(eval $(generic-package))