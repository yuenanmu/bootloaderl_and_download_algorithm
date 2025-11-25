/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "quadspi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "W25Q256.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */
//QSPI_CommandTypeDef QSPI_Command;
//QSPI_MemoryMappedTypeDef QSPI_Mem;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
typedef void (*pFunction)(void);
pFunction JumpToApp;
uint32_t JumpAddress;
//#define APP_FLASH_ADDR	(0x90000000)	//Address of the application
#define APP_FLASH_ADDR	(0x8010000)	//Address of the application
void Jump_to_Application()
{
	if(((*(__IO uint32_t *)APP_FLASH_ADDR) & 0x2FFE0000) == 0x24000000)
	{
		__disable_irq();
		JumpAddress = *(__IO uint32_t*)(APP_FLASH_ADDR + 4);
		__set_MSP(*(__IO uint32_t*)(APP_FLASH_ADDR));
		 SCB->VTOR = APP_FLASH_ADDR;  
		JumpToApp = (pFunction)JumpAddress;
		JumpToApp();
	}
}

void W25Q256_EnableQPI(void) {
    QSPI_CommandTypeDef cmd = {0};
    
    // �˳�QPIģʽ (�������QPIģʽ)
    cmd.Instruction = 0xFF;
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.AddressMode = QSPI_ADDRESS_NONE;
    cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    cmd.DataMode = QSPI_DATA_NONE;
    HAL_QSPI_Command(&hqspi, &cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    HAL_Delay(1);
    
    // ����QPIģʽ
    cmd.Instruction = 0x38; // Enter Quad mode
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    HAL_QSPI_Command(&hqspi, &cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    HAL_Delay(1);
}

void W25Q256_EnterMemoryMappedMode(QSPI_HandleTypeDef *hqspi) {
    W25Q256_EnableQPI(); 
    
    QSPI_CommandTypeDef s_command = {0};
    QSPI_MemoryMappedTypeDef sMemMappedCfg = {0};

    // W25Q256����ȷFast Read Quad I/Oָ��
    s_command.Instruction = 0xEB; // Fast Read Quad I/O
    s_command.InstructionMode = QSPI_INSTRUCTION_4_LINES;
    s_command.AddressSize = QSPI_ADDRESS_24_BITS;
    s_command.AddressMode = QSPI_ADDRESS_4_LINES;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DummyCycles = 6; // 6 dummy cycles for 0xEB command
    s_command.DataMode = QSPI_DATA_4_LINES;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    
    // �ڴ�ӳ������
    sMemMappedCfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
    
    // �������
    SCB_InvalidateICache();
    SCB_CleanInvalidateDCache();
    
    if (HAL_QSPI_MemoryMapped(hqspi, &s_command, &sMemMappedCfg) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  //buzzer_on();
  /* USER CODE END 1 */
  buzzer_on();
  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_QUADSPI_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(100);
  //HAL_UART_Transmit(&huart1,"Test\n",5,100);
  W25Q256_Init();
  W25Q256_EnterMemoryMappedMode(&hqspi);
  HAL_UART_Transmit(&huart1,"Test\n",5,100);
  //SCB_DisableICache();
  //SCB_DisableDCache();
  //SysTick->CTRL=0;
	buzzer_on();
  while (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)!= GPIO_PIN_SET)
  {
	  HAL_UART_Transmit(&huart1,"Ready\n",6,100);
	  HAL_Delay(500);
  }
  //Jump_to_Application();
	//buzzer_on();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_UART_Transmit(&huart1,"Jump Failed\n",5,100);
	  //HAL_UART_Transmit(&huart1,(const uint8_t*)((__IO uint32_t *)APP_FLASH_ADDR),4,100);
	  //HAL_UART_Transmit(&huart1,(const uint8_t*)((__IO uint32_t *)0x10000),4,100);
	  HAL_Delay(500);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 60;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /*
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0x08000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_128KB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.AccessPermission = MPU_REGION_PRIV_RO;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);
	  //Initializes and configures the Region and the memory to be protected
  */
	MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0x08000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_128KB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.AccessPermission = MPU_REGION_PRIV_RO;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
	// --- 新增 Region 2:
	 MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER2;          // Region 2
  MPU_InitStruct.BaseAddress = 0x20000000;              // IRAM1 (DTCM RAM) 起始地址
  MPU_InitStruct.Size = MPU_REGION_SIZE_128KB;         // IRAM1 大小 (128KB = 2^17 bytes)
  MPU_InitStruct.SubRegionDisable = 0x0;               // 不禁用子区域
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;        // 通常用于TCM RAM
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS; // 给予读写权限
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE; // 通常数据区不允许执行代码
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE; // DTCM RAM 通常不缓存
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

	// --- 新增 Region 4: 内部 RAM2 (AXI SRAM) ---
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER4;          // 使用 Region 4 (Region 3 可留给外部Flash)
  MPU_InitStruct.BaseAddress = 0x24000000;              // IRAM2 (AXI SRAM) 起始地址
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;         // IRAM2 大小 (512KB = 2^19 bytes)
  MPU_InitStruct.SubRegionDisable = 0x0;               // 不禁用子区域
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;        // 通常用于AXI SRAM
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS; // 给予读写权限
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE; // 数据区通常不允许执行代码，除非有特殊需求
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;   // AXI SRAM 通常可缓存
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);
	
  // --- 新增 Region 5: 外部 QSPI Flash 映射区域 ---
  MPU_InitStruct.Enable = MPU_REGION_ENABLE; // 启用此Region
  MPU_InitStruct.Number = MPU_REGION_NUMBER3; // 使用Region编号3
  MPU_InitStruct.BaseAddress = 0x90000000; // 外部Flash映射地址 (通常为0x90000000)
  MPU_InitStruct.Size = MPU_REGION_SIZE_32MB; // 【需要修改】：请根据您实际使用的外部Flash芯片容量进行设置 (例如 W25Q128=16MB->MPU_REGION_SIZE_16MB, W25Q64=8MB->MPU_REGION_SIZE_8MB)
  MPU_InitStruct.SubRegionDisable = 0x0; // 通常不禁用子区域
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1; // 通常用于外部存储器
  MPU_InitStruct.AccessPermission = MPU_REGION_PRIV_RO_URO; // 设置为只读 (通常外部Flash不需要写入权限)。也可以设置为 MPU_REGION_FULL_ACCESS 如果需要。
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE; // 【关键】：必须设置为 ENABLE，允许从外部Flash执行代码
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE; // 通常设置为非共享
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE; // 通常可缓存
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE; // 通常可缓冲
  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
