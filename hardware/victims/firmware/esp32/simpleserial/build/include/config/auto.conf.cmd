deps_config := \
	C:/msys32/home/adewa/esp32/esp-idf/components/app_trace/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/aws_iot/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/bt/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/driver/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/efuse/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/esp32/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/esp_adc_cal/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/esp_event/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/esp_http_client/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/esp_http_server/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/esp_https_ota/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/espcoredump/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/ethernet/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/fatfs/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/freemodbus/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/freertos/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/heap/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/libsodium/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/log/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/lwip/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/mbedtls/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/mdns/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/mqtt/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/nvs_flash/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/openssl/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/pthread/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/spi_flash/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/spiffs/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/tcpip_adapter/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/unity/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/vfs/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/wear_levelling/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/wifi_provisioning/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/wpa_supplicant/Kconfig \
	C:/msys32/home/adewa/esp32/esp-idf/components/app_update/Kconfig.projbuild \
	C:/msys32/home/adewa/esp32/esp-idf/components/bootloader/Kconfig.projbuild \
	C:/msys32/home/adewa/esp32/esp-idf/components/esptool_py/Kconfig.projbuild \
	C:/msys32/home/adewa/esp32/esp-idf/components/partition_table/Kconfig.projbuild \
	/c/msys32/home/adewa/esp32/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)

ifneq "$(IDF_TARGET)" "esp32"
include/config/auto.conf: FORCE
endif
ifneq "$(IDF_CMAKE)" "n"
include/config/auto.conf: FORCE
endif

$(deps_config): ;
