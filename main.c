/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Chuong trinh doc do am tu cam bien va hien thi len LCD I2C
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "CLCD_I2C.h"
#include <stdio.h>

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
ADC_HandleTypeDef hadc1;

/* USER CODE BEGIN PV */
// tao doi tuong LCD
CLCD_I2C_Name LCD1;

// bien luu moc thoi gian
uint32_t lastTick = 0;

// gia tri adc tuong ung voi trang thai uot va kho
const uint32_t adcUot = 4095;
const uint32_t adcKho = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_ADC1_Init(void);

/* USER CODE BEGIN 0 */

// ham doc gia tri ADC
uint32_t Read_ADC(void)
{
  HAL_ADC_Start(&hadc1);                                   // bat dau chuyen doi
  HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);        // cho den khi co ket qua
  uint32_t value = HAL_ADC_GetValue(&hadc1);               // doc gia tri
  HAL_ADC_Stop(&hadc1);                                    // dung ADC
  return value;
}

/* USER CODE END 0 */

int main(void)
{
  // khoi tao he thong
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_ADC1_Init();

  // doi LCD on dinh
  HAL_Delay(100);

  // khoi tao LCD 
  CLCD_I2C_Init(&LCD1, &hi2c1, 0x4e, 16, 2);

  // hien thi thong bao ban dau
  CLCD_I2C_SetCursor(&LCD1, 0, 0);
  CLCD_I2C_WriteString(&LCD1, "LCD I2C STM32");
  CLCD_I2C_SetCursor(&LCD1, 0, 1);
  CLCD_I2C_WriteString(&LCD1, "Khoi dong OK");

  // luu moc thoi gian ban dau
  lastTick = HAL_GetTick();

  // vong lap vo tan
  while (1)
  {
    // neu du 3 giay thi cap nhat man hinh
    if (HAL_GetTick() - lastTick >= 3000)
    {
      char buffer[20];

      // doc gia tri adc
      uint32_t adcValue = Read_ADC();

      // chuyen doi sang do am %
      uint32_t doAm = ( (adcUot - adcValue) * 100 ) / (adcUot - adcKho);

      // tao chuoi hien thi
      snprintf(buffer, sizeof(buffer), "Do am: %u%%", doAm);

      // xoa man hinh va in lai
      CLCD_I2C_Clear(&LCD1);
      CLCD_I2C_SetCursor(&LCD1, 0, 0);
      CLCD_I2C_WriteString(&LCD1, buffer);

      // cap nhat moc thoi gian
      lastTick = HAL_GetTick();
    }
  }
}

/* Ham cau hinh clock he thong */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
}

/* Ham khoi tao I2C1 */
static void MX_I2C1_Init(void)
{
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;     // toc do 100kHz
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  HAL_I2C_Init(&hi2c1);
}

/* Ham khoi tao ADC1 */
static void MX_ADC1_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  HAL_ADC_Init(&hadc1);

  // cau hinh kenh adc (vi du: kenh 0)
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

/* Ham khoi tao GPIO */
static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  // neu chi dung I2C va ADC thi khong can init them pin khac
}
