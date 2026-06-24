ENTRY(bootload_reboot_stub)
SECTIONS
{
	. = 0x40377fe0;  /* S3: code lands at 0x40378000 (S2 was 0x40070000) */
	.inst : ALIGN(4) {

		/* esptool.py header */

		LONG( 0x000001e9 )  /* Magic + we want to have a "jump to" location once we write */
		LONG( bootload_reboot_stub )
		LONG( 0x00000000 )
		LONG( 0x02000009 )  /* extended header: chip_id 0x0009 = ESP32-S3 (S2 was 0x0002) */
		LONG( 0x000000e9 )
		LONG( 0x00000000 )
		LONG( __IMAGE_BEGIN__ )
		LONG( ( __IMAGE_END__ - __IMAGE_BEGIN__ ) )

		/* The actual firmware image */

		__IMAGE_BEGIN__ = .;
		*(.initial_jump)
		*(.entry.text)
		*(.init.literal)
		*(.init)
		*(.literal .text .literal.* .text.* .stub)
		*(.out_jump.literal.*)
		*(.out_jump.*)
		__IMAGE_END__ = .;

		BYTE( 0 ) /* There's a janky edge case with esptool.py that this solves */
	}
	.checksum : ALIGN(16) {
		BYTE( 0 )
	}
}
/* The S2 version PROVIDE()d ROM addresses for chip_usb_set_persist_flags /
   software_reset here. The S3 stub is pure MMIO (RTC watchdog register writes)
   and calls no ROM functions, so there are no symbols left to resolve. */

