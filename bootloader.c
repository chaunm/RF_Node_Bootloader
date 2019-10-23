#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"
#include "bootloader.h"
#include "partition.h"
#include "flash.h"

#ifdef STM32F091
#define WD_GPIO_CLK 		(RCC->AHBENR)
#define WD_GPIO_CLK_EN		RCC_AHBENR_GPIOAEN
#define WD_GPIO_PORT		GPIOA
#define WD_GPIO_PIN			GPIO_Pin_5
#endif

#ifdef STM32F030
#define WD_GPIO_CLK 		(RCC->AHBENR)
#define WD_GPIO_CLK_EN		RCC_AHBENR_GPIOBEN
#define WD_GPIO_PORT		GPIOB
#define WD_GPIO_PIN			GPIO_Pin_0
#endif

uint8_t buffer[COPY_BUFFER_SIZE];

/*!
* @brief switch to application
*/
static void BootloaderSwitchApplication(uint32_t programAddr)
{
	// disable interrupt
	__disable_irq();
	//configure vector table
	//set stack pointer to application stack pointer
	__set_MSP(*(__IO PDWORD)programAddr);
	//get address of reset vector of application (@ program addr + 4)
	__asm volatile("mov r0, %0\n\t" ::"r"(programAddr));
	__asm volatile("ldr r0, [r0, #4]");
	//jump to application
	__asm volatile("bx r0");
	
}

static uint32_t firmwareKey[2];
/*!
* @brief task to check firmware and switch to application
*/
void BootloaderSwitchApplicationTask(void* param)
{
	uint32_t copySize = 0;
	
	uint32_t checkBlank[48];
	uint8_t isImageBlank = 1;
	uint8_t i;
	/* Init internal flash */
	InitFlash();
	// Init GPIO to reset external watchdog
	WD_GPIO_CLK |= WD_GPIO_CLK_EN;
	GPIO_InitTypeDef gpioInitStruct;
	GPIO_StructInit(&gpioInitStruct);
	gpioInitStruct.GPIO_Pin = WD_GPIO_PIN;
	gpioInitStruct.GPIO_Mode = GPIO_Mode_OUT; 
	gpioInitStruct.GPIO_OType = GPIO_OType_PP;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_Level_1;
	GPIO_Init(WD_GPIO_PORT, &gpioInitStruct);
	// Read firmware key
	GPIO_WriteBit(WD_GPIO_PORT, WD_GPIO_PIN, (BitAction)(1 - GPIO_ReadInputDataBit(WD_GPIO_PORT, WD_GPIO_PIN)));
	ReadFlash((void*)firmwareKey, sizeof(firmwareKey), (INFORMATION_START_ADDR / 4));
	GPIO_WriteBit(WD_GPIO_PORT, WD_GPIO_PIN, (BitAction)(1 - GPIO_ReadInputDataBit(WD_GPIO_PORT, WD_GPIO_PIN)));
	// check if new image exists
	if ((firmwareKey[0] != FIRMWARE_KEY_1) || (firmwareKey[1] != FIRMWARE_KEY_2))
	{
		goto APPLICATION;
	}
	//check blank in image partition
	GPIO_WriteBit(WD_GPIO_PORT, WD_GPIO_PIN, (BitAction)(1 - GPIO_ReadInputDataBit(WD_GPIO_PORT, WD_GPIO_PIN)));
	ReadFlash((void*)checkBlank, sizeof(checkBlank), IMAGE_START_ADDR / 4);
	GPIO_WriteBit(WD_GPIO_PORT, WD_GPIO_PIN, (BitAction)(1 - GPIO_ReadInputDataBit(WD_GPIO_PORT, WD_GPIO_PIN)));
	for (i = 0; i < 48; i++)
	{
		if (checkBlank[i] != 0xFFFFFFFF)
		{
			isImageBlank = 0;
			break;
		}
	}
	
	if (isImageBlank == 0)
	{
		GPIO_WriteBit(WD_GPIO_PORT, WD_GPIO_PIN, (BitAction)(1 - GPIO_ReadInputDataBit(WD_GPIO_PORT, WD_GPIO_PIN)));
		// delete appliaction partition
		for (i = APPLICATION_START_PAGE; i <= APPLIACTION_END_PAGE; i++)
		{
			EraseFlash(i, 1);
			GPIO_WriteBit(WD_GPIO_PORT, WD_GPIO_PIN, (BitAction)(1 - GPIO_ReadInputDataBit(WD_GPIO_PORT, WD_GPIO_PIN))); 
		}        
		// copy from image partition to appliaction partition
		copySize = 0;
		while (copySize < APPLICATION_SIZE)
		{
			ReadFlash(buffer, COPY_BUFFER_SIZE, (IMAGE_START_ADDR + copySize) / 4);
			WriteFlash(buffer, COPY_BUFFER_SIZE, (APPLICATION_START_ADDR + copySize) / 4);
			copySize += COPY_BUFFER_SIZE;
			GPIO_WriteBit(WD_GPIO_PORT, WD_GPIO_PIN, (BitAction)(1 - GPIO_ReadInputDataBit(WD_GPIO_PORT, WD_GPIO_PIN)));
		}
		// erase image partition
		for (i = IMAGE_START_PAGE; i <= INFORMATION_END_PAGE; i++)
		{
			EraseFlash(i, 1);
			GPIO_WriteBit(WD_GPIO_PORT, WD_GPIO_PIN, (BitAction)(1 - GPIO_ReadInputDataBit(WD_GPIO_PORT, WD_GPIO_PIN)));
		}   
	}
APPLICATION:
	GPIO_WriteBit(WD_GPIO_PORT, WD_GPIO_PIN, (BitAction)(1 - GPIO_ReadInputDataBit(WD_GPIO_PORT, WD_GPIO_PIN)));
	//    	PRINTF("Run to appliaction @ 0x%x\r\n", APPLICATION_START_ADDR);
	BootloaderSwitchApplication(APPLICATION_MEM_ADDRESS);
}