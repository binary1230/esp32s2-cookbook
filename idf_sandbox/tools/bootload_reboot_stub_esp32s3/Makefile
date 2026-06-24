all : reboot

# Reboot a board that is sitting in ROM download mode into its freshly-flashed
# app. On the S3 you can usually skip this stub entirely and just append esptool's
# built-in flag to whatever command you run, e.g.:
#   esptool.py --chip esp32s3 --before no-reset --after watchdog-reset chip_id
# The stub does the identical RTC-watchdog write itself, independent of esptool.
reboot : bootload_reboot_stub.bin
	esptool.py --chip esp32s3 --no-stub --before no_reset --after no_reset --port /dev/ttyACM0 load_ram bootload_reboot_stub.bin

bootload_reboot_stub.bin : bootload_reboot_stub.o
	xtensa-esp32s3-elf-objcopy -O binary bootload_reboot_stub.o bootload_reboot_stub.bin

#IDF_PATH:=/home/cnlohr/esp/esp-idf

# Only three IDF include dirs are needed -- the S3 soc register headers plus
# esp_common (for esp_assert.h, pulled in by soc.h). No app/component headers.
bootload_reboot_stub.o : bootload_reboot_stub.c
	xtensa-esp32s3-elf-gcc -mlongcalls -I$(IDF_PATH)/components/soc/esp32s3/include -I$(IDF_PATH)/components/soc/include -I$(IDF_PATH)/components/esp_common/include -Wall -Wextra -O2 -fno-jump-tables -fno-tree-switch-conversion bootload_reboot_stub.c -T esp32_s3_stub.ld -o bootload_reboot_stub.o -nodefaultlibs -nostartfiles
	xtensa-esp32s3-elf-objdump -S bootload_reboot_stub.o > bootload_reboot_stub.txt

clean :
	rm -rf *.o ~* bootload_reboot_stub.bin bootload_reboot_stub.o bootload_reboot_stub.txt
