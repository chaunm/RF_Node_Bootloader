#include "bootloader.h"
#include "system.h"
int main()
{
	InitSystem();;
	BootloaderSwitchApplicationTask(NULL);
	return 0;
}
