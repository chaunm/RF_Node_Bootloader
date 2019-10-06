#ifndef __BOOTLOADER_H__
#define __BOOTLOADER_H__

#define __APPLICATION_ENTRY     0x00010000
#define FIRMWARE_KEY_1			0x55AAAA55
#define FIRMWARE_KEY_2 			0x2112FEEF

void BootloaderSwitchApplicationTask(void* param);

#endif //__BOOTLOADER_H__bootload.h