// This file is designed to be executed by esptool's load_ram functionality.
// It compiles to ~110 bytes and gets uploaded and executed when compiled using
// the very unusual esp32_s3_stub.ld.
//
// This is the ESP32-S3 port of the ESP32-S2 bootload_reboot_stub in the sibling
// ../bootload_reboot_stub directory.
//
// You usually DON'T need this stub on the S3: esptool's built-in
// `--after watchdog-reset` does exactly the same thing (arms the RTC watchdog)
// with no load_ram step, e.g.
//     esptool.py --chip esp32s3 --before no-reset --after watchdog-reset chip_id
// That flag does NOT work on the S2, which is why the stub is needed there. This
// S3 version is kept for reference / to stay independent of esptool's flags.
//
// Why the S2's software_reset() trick doesn't port: the S2 reaches its ROM
// loader over USB-OTG CDC, where software_reset() is enough to leave download
// mode. The S3 is flashed over the native USB-Serial/JTAG peripheral, which
// lives in the always-on power domain -- software_reset() only resets the
// digital core, so USB-Serial/JTAG keeps holding download mode and you land
// right back in the bootloader. The fix is a full RTC-watchdog *chip* reset,
// which is exactly what esptool writes for --after watchdog-reset.

#include "soc/rtc_cntl_reg.h"
#include "soc/soc.h"  // for REG_WRITE

void bootload_reboot_stub()
{
	// We **must** unset this, otherwise we'll end up back in the bootloader.
	REG_WRITE(RTC_CNTL_OPTION1_REG, 0);

	// On the S2 a ROM software_reset(0) went here ("the first one I tried, it
	// worked"). On the S3 it leaves us in download mode, so instead arm the RTC
	// watchdog for a full chip reset and spin until it fires. These are the same
	// register writes esptool uses for --after watchdog-reset.
	REG_WRITE(RTC_CNTL_WDTWPROTECT_REG, 0x50d83aa1);  // WKEY: unlock the WDT regs
	REG_WRITE(RTC_CNTL_WDTCONFIG1_REG, 2000);         // timeout, RTC slow-clock ticks
	REG_WRITE(RTC_CNTL_WDTCONFIG0_REG, 0xd0000102);   // WDT_EN | CHIP_RESET_EN | stage0=reset
	REG_WRITE(RTC_CNTL_WDTWPROTECT_REG, 0);           // re-lock

	while(1);  // wait for the watchdog to take the whole chip down

	// The S2 original also poked chip_usb_set_persist_flags(0) //USBDC_PERSIST_ENA
	// and left esp_cpu_reset() noted as an alternative -- neither is needed here,
	// the chip reset takes the USB-Serial/JTAG down with it.
}
