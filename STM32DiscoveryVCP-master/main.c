
#define HSE_VALUE ((uint32_t)8000000) /* STM32 discovery uses a 8Mhz external crystal */

#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_exti.h"
#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usbd_cdc_vcp.h"
#include "usb_dcd_int.h"
#include <stdlib.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "misc.h"
#include "stm32f4xx_spi.h"

#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"


volatile uint32_t ticker, downTicker;
__IO uint32_t TimingDelay;

/* LIS302DL struct */
typedef struct
{
  uint8_t Power_Mode;                         /* Power-down/Active Mode */
  uint8_t Output_DataRate;                    /* OUT data rate 100 Hz / 400 Hz */
  uint8_t Axes_Enable;                        /* Axes enable */
  uint8_t Full_Scale;                         /* Full scale */
  uint8_t Self_Test;                          /* Self test */
}LIS302DL_InitTypeDef;

/* LIS302DL High Pass Filter struct */
typedef struct
{
  uint8_t HighPassFilter_Data_Selection;      /* Internal filter bypassed or data from internal filter send to output register*/
  uint8_t HighPassFilter_CutOff_Frequency;    /* High pass filter cut-off frequency */
  uint8_t HighPassFilter_Interrupt;           /* High pass filter enabled for Freefall/WakeUp #1 or #2 */
}LIS302DL_FilterConfigTypeDef;

/* LIS302DL Interrupt struct */
typedef struct
{
  uint8_t Latch_Request;                      /* Latch interrupt request into CLICK_SRC register*/
  uint8_t SingleClick_Axes;                   /* Single Click Axes Interrupts */
  uint8_t DoubleClick_Axes;                   /* Double Click Axes Interrupts */
}LIS302DL_InterruptConfigTypeDef;



#define LIS302DL_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define LIS302DL_SPI                       SPI1
#define LIS302DL_SPI_CLK                   RCC_APB2Periph_SPI1

#define LIS302DL_SPI_SCK_PIN               GPIO_Pin_5                  /* PA.05 */
#define LIS302DL_SPI_SCK_GPIO_PORT         GPIOA                       /* GPIOA */
#define LIS302DL_SPI_SCK_GPIO_CLK          RCC_AHB1Periph_GPIOA
#define LIS302DL_SPI_SCK_SOURCE            GPIO_PinSource5
#define LIS302DL_SPI_SCK_AF                GPIO_AF_SPI1

#define LIS302DL_SPI_MISO_PIN              GPIO_Pin_6                  /* PA.6 */
#define LIS302DL_SPI_MISO_GPIO_PORT        GPIOA                       /* GPIOA */
#define LIS302DL_SPI_MISO_GPIO_CLK         RCC_AHB1Periph_GPIOA
#define LIS302DL_SPI_MISO_SOURCE           GPIO_PinSource6
#define LIS302DL_SPI_MISO_AF               GPIO_AF_SPI1

#define LIS302DL_SPI_MOSI_PIN              GPIO_Pin_7                  /* PA.7 */
#define LIS302DL_SPI_MOSI_GPIO_PORT        GPIOA                       /* GPIOA */
#define LIS302DL_SPI_MOSI_GPIO_CLK         RCC_AHB1Periph_GPIOA
#define LIS302DL_SPI_MOSI_SOURCE           GPIO_PinSource7
#define LIS302DL_SPI_MOSI_AF               GPIO_AF_SPI1

#define LIS302DL_SPI_CS_PIN                GPIO_Pin_3                  /* PE.03 */
#define LIS302DL_SPI_CS_GPIO_PORT          GPIOE                       /* GPIOE */
#define LIS302DL_SPI_CS_GPIO_CLK           RCC_AHB1Periph_GPIOE

#define LIS302DL_SPI_INT1_PIN              GPIO_Pin_0                  /* PE.00 */
#define LIS302DL_SPI_INT1_GPIO_PORT        GPIOE                       /* GPIOE */
#define LIS302DL_SPI_INT1_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define LIS302DL_SPI_INT1_EXTI_LINE        EXTI_Line0
#define LIS302DL_SPI_INT1_EXTI_PORT_SOURCE EXTI_PortSourceGPIOE
#define LIS302DL_SPI_INT1_EXTI_PIN_SOURCE  EXTI_PinSource0
#define LIS302DL_SPI_INT1_EXTI_IRQn        EXTI0_IRQn

#define LIS302DL_SPI_INT2_PIN              GPIO_Pin_1                  /* PE.01 */
#define LIS302DL_SPI_INT2_GPIO_PORT        GPIOE                       /* GPIOE */
#define LIS302DL_SPI_INT2_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define LIS302DL_SPI_INT2_EXTI_LINE        EXTI_Line1
#define LIS302DL_SPI_INT2_EXTI_PORT_SOURCE EXTI_PortSourceGPIOE
#define LIS302DL_SPI_INT2_EXTI_PIN_SOURCE  EXTI_PinSource1
#define LIS302DL_SPI_INT2_EXTI_IRQn        EXTI1_IRQn


#define LIS302DL_WHO_AM_I_ADDR                  0x0F
#define LIS302DL_CTRL_REG1_ADDR                 0x20
#define LIS302DL_CTRL_REG2_ADDR              0x21
#define LIS302DL_CTRL_REG3_ADDR              0x22
#define LIS302DL_HP_FILTER_RESET_REG_ADDR     0x23
#define LIS302DL_STATUS_REG_ADDR             0x27
#define LIS302DL_OUT_X_ADDR                  0x29
#define LIS302DL_OUT_Y_ADDR                  0x2B
#define LIS302DL_OUT_Z_ADDR                  0x2D
#define LIS302DL_FF_WU_CFG1_REG_ADDR         0x30
#define LIS302DL_FF_WU_SRC1_REG_ADDR           0x31
#define LIS302DL_FF_WU_THS1_REG_ADDR          0x32
#define LIS302DL_FF_WU_DURATION1_REG_ADDR     0x33
#define LIS302DL_FF_WU_CFG2_REG_ADDR         0x34
#define LIS302DL_FF_WU_SRC2_REG_ADDR           0x35
#define LIS302DL_FF_WU_THS2_REG_ADDR          0x36
#define LIS302DL_FF_WU_DURATION2_REG_ADDR     0x37
#define LIS302DL_CLICK_CFG_REG_ADDR     0x38
#define LIS302DL_CLICK_SRC_REG_ADDR        0x39
#define LIS302DL_CLICK_THSY_X_REG_ADDR        0x3B
#define LIS302DL_CLICK_THSZ_REG_ADDR         0x3C
#define LIS302DL_CLICK_TIMELIMIT_REG_ADDR        0x3D
#define LIS302DL_CLICK_LATENCY_REG_ADDR        0x3E
#define LIS302DL_CLICK_WINDOW_REG_ADDR        0x3F
#define LIS302DL_SENSITIVITY_2_3G                         18  /* 18 mg/digit*/
#define LIS302DL_SENSITIVITY_9_2G                         72  /* 72 mg/digit*/
#define LIS302DL_DATARATE_100                             ((uint8_t)0x00)
#define LIS302DL_DATARATE_400                             ((uint8_t)0x80)
#define LIS302DL_LOWPOWERMODE_POWERDOWN                   ((uint8_t)0x00)
#define LIS302DL_LOWPOWERMODE_ACTIVE                      ((uint8_t)0x40)
#define LIS302DL_FULLSCALE_2_3                            ((uint8_t)0x00)
#define LIS302DL_FULLSCALE_9_2                            ((uint8_t)0x20)
#define LIS302DL_SELFTEST_NORMAL                          ((uint8_t)0x00)
#define LIS302DL_SELFTEST_P                               ((uint8_t)0x10)
#define LIS302DL_SELFTEST_M                               ((uint8_t)0x08)
#define LIS302DL_X_ENABLE                                 ((uint8_t)0x01)
#define LIS302DL_Y_ENABLE                                 ((uint8_t)0x02)
#define LIS302DL_Z_ENABLE                                 ((uint8_t)0x04)
#define LIS302DL_XYZ_ENABLE                               ((uint8_t)0x07)
#define LIS302DL_SERIALINTERFACE_4WIRE                    ((uint8_t)0x00)
#define LIS302DL_SERIALINTERFACE_3WIRE                    ((uint8_t)0x80)
#define LIS302DL_BOOT_NORMALMODE                          ((uint8_t)0x00)
#define LIS302DL_BOOT_REBOOTMEMORY                        ((uint8_t)0x40)
#define LIS302DL_FILTEREDDATASELECTION_BYPASSED           ((uint8_t)0x00)
#define LIS302DL_FILTEREDDATASELECTION_OUTPUTREGISTER     ((uint8_t)0x20)
#define LIS302DL_HIGHPASSFILTERINTERRUPT_OFF              ((uint8_t)0x00)
#define LIS302DL_HIGHPASSFILTERINTERRUPT_1                ((uint8_t)0x04)
#define LIS302DL_HIGHPASSFILTERINTERRUPT_2                ((uint8_t)0x08)
#define LIS302DL_HIGHPASSFILTERINTERRUPT_1_2              ((uint8_t)0x0C)
#define LIS302DL_HIGHPASSFILTER_LEVEL_0                   ((uint8_t)0x00)
#define LIS302DL_HIGHPASSFILTER_LEVEL_1                   ((uint8_t)0x01)
#define LIS302DL_HIGHPASSFILTER_LEVEL_2                   ((uint8_t)0x02)
#define LIS302DL_HIGHPASSFILTER_LEVEL_3                   ((uint8_t)0x03)
#define LIS302DL_INTERRUPTREQUEST_NOTLATCHED              ((uint8_t)0x00)
#define LIS302DL_INTERRUPTREQUEST_LATCHED                 ((uint8_t)0x40)
#define LIS302DL_CLICKINTERRUPT_XYZ_DISABLE               ((uint8_t)0x00)
#define LIS302DL_CLICKINTERRUPT_X_ENABLE                  ((uint8_t)0x01)
#define LIS302DL_CLICKINTERRUPT_Y_ENABLE                  ((uint8_t)0x04)
#define LIS302DL_CLICKINTERRUPT_Z_ENABLE                  ((uint8_t)0x10)
#define LIS302DL_CLICKINTERRUPT_XYZ_ENABLE                ((uint8_t)0x15)
#define LIS302DL_DOUBLECLICKINTERRUPT_XYZ_DISABLE         ((uint8_t)0x00)
#define LIS302DL_DOUBLECLICKINTERRUPT_X_ENABLE            ((uint8_t)0x02)
#define LIS302DL_DOUBLECLICKINTERRUPT_Y_ENABLE            ((uint8_t)0x08)
#define LIS302DL_DOUBLECLICKINTERRUPT_Z_ENABLE            ((uint8_t)0x20)
#define LIS302DL_DOUBLECLICKINTERRUPT_XYZ_ENABLE          ((uint8_t)0x2A)
#define LIS302DL_CS_LOW()       GPIO_ResetBits(LIS302DL_SPI_CS_GPIO_PORT, LIS302DL_SPI_CS_PIN)
#define LIS302DL_CS_HIGH()      GPIO_SetBits(LIS302DL_SPI_CS_GPIO_PORT, LIS302DL_SPI_CS_PIN)
#define READWRITE_CMD              ((uint8_t)0x80)
#define MULTIPLEBYTE_CMD           ((uint8_t)0x40)
#define DUMMY_BYTE                 ((uint8_t)0x00)

__IO uint32_t  LIS302DLTimeout = LIS302DL_FLAG_TIMEOUT;





int pomiar_x=0;
int pomiar_y=0;
int pomiar_z=0;
uint16_t PrescalerValue = 0;
__IO uint32_t TimingDelay = 0;
int8_t przyspieszenie_x;
int8_t przyspieszenie_y;
int8_t przyspieszenie_z;


void Delay(__IO uint32_t nCount);
void LIS302DL_Init(LIS302DL_InitTypeDef *LIS302DL_InitStruct);
void LIS302DL_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);
static uint8_t LIS302DL_SendByte(uint8_t byte);
void LIS302DL_InterruptConfig(LIS302DL_InterruptConfigTypeDef *LIS302DL_IntConfigStruct);
void LIS302DL_Read(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);
uint32_t LIS302DL_TIMEOUT_UserCallback(void);


void LIS302DL_Init(LIS302DL_InitTypeDef *LIS302DL_InitStruct)
{
  uint8_t ctrl = 0x00;

  /* Configure the low level interface ---------------------------------------*/
//  LIS302DL_LowLevel_Init();

  /* Configure MEMS: data rate, power mode, full scale, self test and axes */
  ctrl = (uint8_t) (LIS302DL_InitStruct->Output_DataRate | LIS302DL_InitStruct->Power_Mode | \
                    LIS302DL_InitStruct->Full_Scale | LIS302DL_InitStruct->Self_Test | \
                    LIS302DL_InitStruct->Axes_Enable);

  /* Write value to MEMS CTRL_REG1 regsister */
  LIS302DL_Write(&ctrl, LIS302DL_CTRL_REG1_ADDR, 1);
}






void LIS302DL_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
  /* Configure the MS bit:
       - When 0, the address will remain unchanged in multiple read/write commands.
       - When 1, the address will be auto incremented in multiple read/write commands.
  */
  if(NumByteToWrite > 0x01)
  {
    WriteAddr |= (uint8_t)MULTIPLEBYTE_CMD;
  }
  /* Set chip select Low at the start of the transmission */
  LIS302DL_CS_LOW();

  /* Send the Address of the indexed register */
  LIS302DL_SendByte(WriteAddr);
  /* Send the data that will be written into the device (MSB First) */
  while(NumByteToWrite >= 0x01)
  {
    LIS302DL_SendByte(*pBuffer);
    NumByteToWrite--;
    pBuffer++;
  }

  /* Set chip select High at the end of the transmission */
  LIS302DL_CS_HIGH();
}






static uint8_t LIS302DL_SendByte(uint8_t byte)
{
  /* Loop while DR register in not emplty */
  LIS302DLTimeout = LIS302DL_FLAG_TIMEOUT;
  while (SPI_I2S_GetFlagStatus(LIS302DL_SPI, SPI_I2S_FLAG_TXE) == RESET)
  {
    if((LIS302DLTimeout--) == 0) return LIS302DL_TIMEOUT_UserCallback();
  }

  /* Send a Byte through the SPI peripheral */
  SPI_I2S_SendData(LIS302DL_SPI, byte);

  /* Wait to receive a Byte */
  LIS302DLTimeout = LIS302DL_FLAG_TIMEOUT;
  while (SPI_I2S_GetFlagStatus(LIS302DL_SPI, SPI_I2S_FLAG_RXNE) == RESET)
  {
    if((LIS302DLTimeout--) == 0) return LIS302DL_TIMEOUT_UserCallback();
  }

  /* Return the Byte read from the SPI bus */
  return (uint8_t)SPI_I2S_ReceiveData(LIS302DL_SPI);
}



void LIS302DL_InterruptConfig(LIS302DL_InterruptConfigTypeDef *LIS302DL_IntConfigStruct)
{
  uint8_t ctrl = 0x00;

  /* Read CLICK_CFG register */
  LIS302DL_Read(&ctrl, LIS302DL_CLICK_CFG_REG_ADDR, 1);

  /* Configure latch Interrupt request, click interrupts and double click interrupts */
  ctrl = (uint8_t)(LIS302DL_IntConfigStruct->Latch_Request| \
                   LIS302DL_IntConfigStruct->SingleClick_Axes | \
                   LIS302DL_IntConfigStruct->DoubleClick_Axes);

  /* Write value to MEMS CLICK_CFG register */
  LIS302DL_Write(&ctrl, LIS302DL_CLICK_CFG_REG_ADDR, 1);
}

void LIS302DL_Read(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
  if(NumByteToRead > 0x01)
  {
    ReadAddr |= (uint8_t)(READWRITE_CMD | MULTIPLEBYTE_CMD);
  }
  else
  {
    ReadAddr |= (uint8_t)READWRITE_CMD;
  }
  /* Set chip select Low at the start of the transmission */
  LIS302DL_CS_LOW();

  /* Send the Address of the indexed register */
  LIS302DL_SendByte(ReadAddr);

  /* Receive the data that will be read from the device (MSB First) */
  while(NumByteToRead > 0x00)
  {
    /* Send dummy byte (0x00) to generate the SPI clock to LIS302DL (Slave device) */
    *pBuffer = LIS302DL_SendByte(DUMMY_BYTE);
    NumByteToRead--;
    pBuffer++;
  }

  /* Set chip select High at the end of the transmission */
  LIS302DL_CS_HIGH();
}
















void Delay(__IO uint32_t nCount)
{
  while(nCount--)
  {
  }
}




/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}



/**
  * @brief  MEMS accelerometre management of the timeout situation.
  * @param  None.
  * @retval None.
  */
uint32_t LIS302DL_TIMEOUT_UserCallback(void)
{
  /* MEMS Accelerometer Timeout error occured */
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */



/*
 * The USB data must be 4 byte aligned if DMA is enabled. This macro handles
 * the alignment, if necessary (it's actually magic, but don't tell anyone).
 */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;


void init();
void ColorfulRingOfDeath(void);

/*
 * Define prototypes for interrupt handlers here. The conditional "extern"
 * ensures the weak declarations from startup_stm32f4xx.c are overridden.
 */
#ifdef __cplusplus
 extern "C" {
#endif

void SysTick_Handler(void);
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void OTG_FS_IRQHandler(void);
void OTG_FS_WKUP_IRQHandler(void);

#ifdef __cplusplus
}
#endif



int main(void)
{
	/* Set up the system clocks */
	SystemInit();

	SystemCoreClockUpdate();

	/* Initialize USB, IO, SysTick, and all those other things you do in the morning */
	init();




	/// ACKELEROMETR

	SystemInit();
		SystemCoreClockUpdate(); // inicjalizacja dystrybucji czasu procesora



	    /* Enable the SPI periph */
	    RCC_APB2PeriphClockCmd(LIS302DL_SPI_CLK, ENABLE);

	    /* Enable SCK, MOSI and MISO GPIO clocks */
	    RCC_AHB1PeriphClockCmd(LIS302DL_SPI_SCK_GPIO_CLK | LIS302DL_SPI_MISO_GPIO_CLK | LIS302DL_SPI_MOSI_GPIO_CLK, ENABLE);

	    /* Enable CS  GPIO clock */
	    RCC_AHB1PeriphClockCmd(LIS302DL_SPI_CS_GPIO_CLK, ENABLE);

	    /* Enable INT1 GPIO clock */
	    RCC_AHB1PeriphClockCmd(LIS302DL_SPI_INT1_GPIO_CLK, ENABLE);

	    /* Enable INT2 GPIO clock */
	    RCC_AHB1PeriphClockCmd(LIS302DL_SPI_INT2_GPIO_CLK, ENABLE);

	    GPIO_PinAFConfig(LIS302DL_SPI_SCK_GPIO_PORT, LIS302DL_SPI_SCK_SOURCE, LIS302DL_SPI_SCK_AF);
	    GPIO_PinAFConfig(LIS302DL_SPI_MISO_GPIO_PORT, LIS302DL_SPI_MISO_SOURCE, LIS302DL_SPI_MISO_AF);
	    GPIO_PinAFConfig(LIS302DL_SPI_MOSI_GPIO_PORT, LIS302DL_SPI_MOSI_SOURCE, LIS302DL_SPI_MOSI_AF);

	    GPIO_InitTypeDef GPIO_InitStructure;
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	    /* SPI SCK pin configuration */
	    GPIO_InitStructure.GPIO_Pin = LIS302DL_SPI_SCK_PIN;
	    GPIO_Init(LIS302DL_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

	    /* SPI  MOSI pin configuration */
	    GPIO_InitStructure.GPIO_Pin =  LIS302DL_SPI_MOSI_PIN;
	    GPIO_Init(LIS302DL_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

	    /* SPI MISO pin configuration */
	    GPIO_InitStructure.GPIO_Pin = LIS302DL_SPI_MISO_PIN;
	    GPIO_Init(LIS302DL_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

	    /* SPI configuration -------------------------------------------------------*/
	    SPI_InitTypeDef  SPI_InitStructure;
	    SPI_I2S_DeInit(LIS302DL_SPI);
	    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	    SPI_InitStructure.SPI_CRCPolynomial = 7;
	    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	    SPI_Init(LIS302DL_SPI, &SPI_InitStructure);

	    /* Enable SPI1  */
	    SPI_Cmd(LIS302DL_SPI, ENABLE);

	    /* Configure GPIO PIN for Lis Chip select */
	    GPIO_InitStructure.GPIO_Pin = LIS302DL_SPI_CS_PIN;
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	    GPIO_Init(LIS302DL_SPI_CS_GPIO_PORT, &GPIO_InitStructure);

	    /* Deselect : Chip Select high */
	    GPIO_SetBits(LIS302DL_SPI_CS_GPIO_PORT, LIS302DL_SPI_CS_PIN);

	    /* Configure GPIO PINs to detect Interrupts */
	    GPIO_InitStructure.GPIO_Pin = LIS302DL_SPI_INT1_PIN;
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	    GPIO_Init(LIS302DL_SPI_INT1_GPIO_PORT, &GPIO_InitStructure);

	    GPIO_InitStructure.GPIO_Pin = LIS302DL_SPI_INT2_PIN;
	    GPIO_Init(LIS302DL_SPI_INT2_GPIO_PORT, &GPIO_InitStructure);


	    LIS302DL_InitTypeDef  LIS302DL_InitStruct;
	    uint8_t ctrl = 0x00;

	    /* Set configuration of LIS302DL*/
	    LIS302DL_InitStruct.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE;
	    LIS302DL_InitStruct.Output_DataRate = LIS302DL_DATARATE_100;
	    LIS302DL_InitStruct.Axes_Enable = LIS302DL_X_ENABLE | LIS302DL_Y_ENABLE | LIS302DL_Z_ENABLE;
	    LIS302DL_InitStruct.Full_Scale = LIS302DL_FULLSCALE_2_3;
	    LIS302DL_InitStruct.Self_Test = LIS302DL_SELFTEST_NORMAL;
	    LIS302DL_Init(&LIS302DL_InitStruct);

	    LIS302DL_InterruptConfigTypeDef LIS302DL_InterruptStruct;
	    /* Set configuration of Internal High Pass Filter of LIS302DL*/
	    LIS302DL_InterruptStruct.Latch_Request = LIS302DL_INTERRUPTREQUEST_LATCHED;
	    LIS302DL_InterruptStruct.SingleClick_Axes = LIS302DL_CLICKINTERRUPT_Z_ENABLE;
	    LIS302DL_InterruptStruct.DoubleClick_Axes = LIS302DL_DOUBLECLICKINTERRUPT_Z_ENABLE;
	    LIS302DL_InterruptConfig(&LIS302DL_InterruptStruct);


	    // KONIEC AKCELEROMETR


	    // POCZATEK TIMER

	    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	    TIM_TimeBaseStructure.TIM_Period = 7200;

	    TIM_TimeBaseStructure.TIM_Prescaler = 100000;

	    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;

	    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	    TIM_Cmd(TIM3, ENABLE);

	    // KONIEC TIMER

	    // Przerwania

	    NVIC_InitTypeDef NVIC_InitStructure;

	    //numerprzerwania

	    NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;

	    //priorytetg³ówny

	    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00;

	    //subpriorytet

	    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00;

	    //uruchomdanykana³

	    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;

	    //zapiszwype³nion¹strukturêdorejestrów

	    NVIC_Init(&NVIC_InitStructure);




	while (1)
	{

		LIS302DL_Read(&przyspieszenie_x, LIS302DL_OUT_X_ADDR, 1);
				    	if(przyspieszenie_x>127)
				        {
				        	przyspieszenie_x=przyspieszenie_x-1;
				        	przyspieszenie_x=(~przyspieszenie_x)&0xFF;
				        	//przyspieszenie_x=-przyspieszenie_x;
				        }
				        LIS302DL_Read(&przyspieszenie_y, LIS302DL_OUT_Y_ADDR, 1);
				        if(przyspieszenie_y>127)
				        {
				          	przyspieszenie_y=przyspieszenie_y-1;
				           	przyspieszenie_y=(~przyspieszenie_y)&0xFF;
				           	przyspieszenie_y=-przyspieszenie_y;
				        }
				    	LIS302DL_Read(&przyspieszenie_z, LIS302DL_OUT_Z_ADDR, 1);
				    	if(przyspieszenie_z>127)
				        {
				        	przyspieszenie_z=przyspieszenie_z-1;
				        	przyspieszenie_z=(~przyspieszenie_z)&0xFF;
				        	przyspieszenie_z=-przyspieszenie_z;
				        }


		/*
		/* Blink the orange LED at 1Hz
		if (500 == ticker)
		{
			GPIOD->BSRRH = GPIO_Pin_13;
		}
		else if (1000 == ticker)
		{
			ticker = 0;
			GPIOD->BSRRL = GPIO_Pin_13;
		}
*/
/*
		// New code
		 if ( ticker%2==0 )// sprawdzic
		{
			//GPIOD->BSRRH = GPIO_Pin_13;
			//VCP_put_char(97);

			if(przyspieszenie_y > 10)
			{
			VCP_put_char(97);
			}
		}

		else if (1000 == ticker)
		{
			ticker = 0;
			GPIOD->BSRRL = GPIO_Pin_13;

			if (przyspieszenie_y > 10)
			{
			VCP_put_char(97);
			}
		}

*/

		int flag = 1;		// 1 - wysyla   0 - nie wysyla


		if(przyspieszenie_y > 20 && flag == 1)
		{

			VCP_put_char(97);		// wysylanie jednego 'a'

			flag = 0;				// ustawienie flagi powodujacej przerwanie przesylania

		}
		int i;
		for(i= 0; i<200000; i++)
		{};

		if(przyspieszenie_y < 20)
		{

			flag = 1;				// ustawienie flagi na ponowne przesy³anie z uwzglêdnieniem histerezy
		}



		/* If there's data on the virtual serial port:
		 *  - Echo it back
		 *  - Turn the green LED on for 10ms
		 */

/* testing value of accelerometion
		if(przyspieszenie_y > 10)
			{

				VCP_put_char(97);
			}
		 //old code
*/

		//VCP_put_char(przyspieszenie_x);
		//VCP_put_char(przyspieszenie_z);


		/*
		uint8_t theByte;

		if (VCP_get_char(&theByte))
		{
			VCP_put_char(theByte);


			GPIOD->BSRRL = GPIO_Pin_12;
			downTicker = 10;
		}
		if (0 == downTicker)
		{
			GPIOD->BSRRH = GPIO_Pin_12;
		}*/





		//    	GPIO_ToggleBits(GPIOD, GPIO_Pin_13);

		//        Delay(0x44FFFF);



		    }
	// AKCELEROMETR !!!

	return 0;
}


void init()
{
	/* STM32F4 discovery LEDs */
	GPIO_InitTypeDef LED_Config;

	/* Always remember to turn on the peripheral clock...  If not, you may be up till 3am debugging... */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	LED_Config.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	LED_Config.GPIO_Mode = GPIO_Mode_OUT;
	LED_Config.GPIO_OType = GPIO_OType_PP;
	LED_Config.GPIO_Speed = GPIO_Speed_25MHz;
	LED_Config.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &LED_Config);



	/* Setup SysTick or CROD! */
	if (SysTick_Config(SystemCoreClock / 1000))
	{
		ColorfulRingOfDeath();
	}


	/* Setup USB */
	USBD_Init(&USB_OTG_dev,
	            USB_OTG_FS_CORE_ID,
	            &USR_desc,
	            &USBD_CDC_cb,
	            &USR_cb);

	return;
}

/*
 * Call this to indicate a failure.  Blinks the STM32F4 discovery LEDs
 * in sequence.  At 168Mhz, the blinking will be very fast - about 5 Hz.
 * Keep that in mind when debugging, knowing the clock speed might help
 * with debugging.
 */
void ColorfulRingOfDeath(void)
{
	uint16_t ring = 1;
	while (1)
	{
		uint32_t count = 0;
		while (count++ < 500000);

		GPIOD->BSRRH = (ring << 12);
		ring = ring << 1;
		if (ring >= 1<<4)
		{
			ring = 1;
		}
		GPIOD->BSRRL = (ring << 12);
	}
}

/*
 * Interrupt Handlers
 */

void SysTick_Handler(void)
{
	ticker++;
	if (downTicker > 0)
	{
		downTicker--;
	}
}

void NMI_Handler(void)       {}
void HardFault_Handler(void) { ColorfulRingOfDeath(); }
void MemManage_Handler(void) { ColorfulRingOfDeath(); }
void BusFault_Handler(void)  { ColorfulRingOfDeath(); }
void UsageFault_Handler(void){ ColorfulRingOfDeath(); }
void SVC_Handler(void)       {}
void DebugMon_Handler(void)  {}
void PendSV_Handler(void)    {}

void OTG_FS_IRQHandler(void)
{
  USBD_OTG_ISR_Handler (&USB_OTG_dev);
}

void OTG_FS_WKUP_IRQHandler(void)
{
  if(USB_OTG_dev.cfg.low_power)
  {
    *(uint32_t *)(0xE000ED10) &= 0xFFFFFFF9 ;
    SystemInit();
    USB_OTG_UngateClock(&USB_OTG_dev);
  }
  EXTI_ClearITPendingBit(EXTI_Line18);
}


// !!!!!!!!!!				AKCELEROMETR  				!!!!!!!!!!! //



// TIMER-PRZERWANIE //





