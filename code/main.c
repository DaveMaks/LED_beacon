#include "stm8s.h"
#include "main.h"

uint32_t TimerCount=0;

uint32_t NextStart=0;

bool is_ONLed=TRUE;
bool is_LedStart=TRUE;

uint16_t LED_Ligher=10; //яркость

uint32_t TimerClikBTN=0;
uint8_t BTN_SETCountClik=0;
bool BTN_SETLastClik=FALSE;

uint8_t BTN_PWRCountClik=0;

uint32_t TimerADCUpdate=0;

u16 voltage;
typedef struct config
{
  uint16_t Lighit;
  uint16_t DelayHi;
  uint16_t DelayLow;
} config_t;

config_t localConfig={5,10,2000};


void main( void )
{
  CLK_Config();
  GPIO_Config();
  enableInterrupts();
  TIM4_Config();
  TIM2_Config();
  ReadConfig();
  loading(3);
  bool FirstLED=FALSE;
  while (1)
  {

    if (TimerCount>TimerADCUpdate){
        voltage=GetADC();
        TimerADCUpdate=TimerCount+500;
    }

    /**Отлов нажатие кнопки SET*/
    if ((GPIO_ReadInputPin(BTN_PORT,BTN_2)==RESET) && !BTN_SETLastClik){
        TimerClikBTN=TimerCount+(100);
        BTN_SETCountClik++;
        BTN_SETLastClik=TRUE;
    }
    if ((GPIO_ReadInputPin(BTN_PORT,BTN_2)) && BTN_SETLastClik==TRUE){
      BTN_SETLastClik=FALSE;
    }

    if (BTN_SETCountClik>0 && TimerCount>TimerClikBTN){
      loading(BTN_SETCountClik);
      switch (BTN_SETCountClik)
      {
      case 2:
        ConfigSET_Lighet();
        break;
      case 3:
        ConfigSET_DelayHi();
        break;
      case 4:
        ConfigSET_DelayLow();
        break;
      }
      BTN_SETCountClik=0;
      TimerClikBTN=0;
      loading(1);
    }
    //################################

    /**Отлов нажатие кнопки PWR*/
    if (GPIO_ReadInputPin(BTN_PORT,BTN_POWER)==RESET){
      BTN_PWRCountClik++;
    }
    else
    {
      BTN_PWRCountClik=0;
    }
    //###############################

    /** Мизаем */
    if (is_LedStart && is_ONLed && TimerCount>NextStart){
      if (FirstLED){
        TIM2_SetCompare1(localConfig.Lighit);
        FirstLED=FALSE;
      }
      else
      {
        TIM2_SetCompare2(localConfig.Lighit);
        FirstLED=TRUE;
      }
      NextStart=TimerCount+(localConfig.DelayHi);
      is_ONLed=FALSE;
    }
    if (is_LedStart && !is_ONLed && TimerCount>NextStart){
      TIM2_SetCompare2(0);
      TIM2_SetCompare1(0);
      NextStart=TimerCount+(localConfig.DelayLow);
      is_ONLed=!FALSE;
    }
    //#################################


    /**Power*/
    if (BTN_PWRCountClik>(100*2) || voltage<MIN_VOLTAGE){
      loading(10);
      GPIO_WriteLow(PWR_PORT,(GPIO_Pin_TypeDef)PWR_PIN);
      return;
    }
    //################################
   //wfi();
    delay_cs(1);
  }
}

void ConfigSET_Lighet(){
  while (GPIO_ReadInputPin(BTN_PORT,BTN_POWER)!=RESET)
  {
    if (GPIO_ReadInputPin(BTN_PORT,BTN_2)==RESET){
      localConfig.Lighit+=100;
      if (localConfig.Lighit>1000){
        TIM2_SetCompare2(localConfig.Lighit);
        delay_cs(100);
      }
      else
      {
         TIM2_SetCompare2(0);
      }
      if (localConfig.Lighit>1200){
          localConfig.Lighit=0;
      }
      TIM2_SetCompare1(localConfig.Lighit);
    }
    delay_cs(50);
  }
  WriteConfig();
}
void ConfigSET_DelayHi(){
 
 while (GPIO_ReadInputPin(BTN_PORT,BTN_POWER)!=RESET)
  {
   LEDFlash();
    if (GPIO_ReadInputPin(BTN_PORT,BTN_2)==RESET){
      localConfig.DelayHi+=10;
      if (localConfig.DelayHi>100){
        TIM2_SetCompare2(localConfig.Lighit);
        delay_cs(100);
      }
      else
      {
         TIM2_SetCompare2(0);
      }
      if (localConfig.DelayHi>110){
          localConfig.DelayHi=10;
      }
        
    }
    delay_cs(50);
  }
 WriteConfig();
 
}
void ConfigSET_DelayLow(){
while (GPIO_ReadInputPin(BTN_PORT,BTN_POWER)!=RESET)
  {
    LEDFlash();
    if (GPIO_ReadInputPin(BTN_PORT,BTN_2)==RESET){
      localConfig.DelayLow+=100;
      if (localConfig.DelayLow>1000){
        TIM2_SetCompare2(localConfig.Lighit);
        delay_cs(100);
      }
      else
      {
         TIM2_SetCompare2(0);
      }
     
      if (localConfig.DelayLow>1100){
          localConfig.DelayLow=100;
      }
        
    }
    delay_cs(50);
  }
WriteConfig();
}

void LEDFlash(){
    if (is_ONLed && TimerCount>NextStart){
      TIM2_SetCompare1(localConfig.Lighit);
      NextStart=TimerCount+localConfig.DelayHi;
      is_ONLed=FALSE;
    }
    if (!is_ONLed && TimerCount>NextStart){
      TIM2_SetCompare1(0);
      NextStart=TimerCount+localConfig.DelayLow;
      is_ONLed=!FALSE;
    }
}


void WriteConfig(){
  FLASH_Unlock(FLASH_MEMTYPE_DATA);
  // запись во флеш
  *((PointerAttr config_t *)(MemoryAddressCast)FLASH_DATA_START_PHYSICAL_ADDRESS) = *((config_t*)(&localConfig));
  FLASH_Lock(FLASH_MEMTYPE_DATA);
}
void ReadConfig(){
  FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);
  FLASH_Unlock(FLASH_MEMTYPE_DATA);
  localConfig=(*(PointerAttr config_t *) (MemoryAddressCast)FLASH_DATA_START_PHYSICAL_ADDRESS); 
  FLASH_Lock(FLASH_MEMTYPE_DATA);
}

void loading(uint8_t count){
  count*=2;
  for(uint8_t i=0;i<count;i+=2){
    TIM2_SetCompare1(localConfig.Lighit);
    TIM2_SetCompare2(localConfig.Lighit);
    delay_cs(LOADING_DELAY);
    TIM2_SetCompare1(0);
    TIM2_SetCompare2(0);
    delay_cs(LOADING_DELAY);
  }
  GPIO_WriteLow(LED_GPIO_PORT,(GPIO_Pin_TypeDef)LEDS);
}

void NextTime(uint32_t addtime){
  
}

void delay_s(uint32_t s) {
  delay_cs((s*100));
}

void delay_cs(uint32_t cs){
    cs+=TimerCount;
    while(cs>=TimerCount){
    nop();
    }
}

void CLK_Config(){
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV8);
  CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV2);
  CLK->PCKENR1=0x00;
  CLK->PCKENR2=0x00;
}

void TIM4_Config(){
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4,ENABLE);
  TIM4_TimeBaseInit(TIM4_PRESCALER_128, 99);
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
  TIM4_Cmd(ENABLE);

}

void TIM2_Config(){
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2,ENABLE);
  TIM2_TimeBaseInit(TIM2_PRESCALER_1, 999);
  TIM2_OC1Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE,0, TIM2_OCPOLARITY_HIGH);
  TIM2_OC2Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE,0, TIM2_OCPOLARITY_HIGH);
  TIM2_ARRPreloadConfig(ENABLE);
  //TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);
  TIM2_Cmd(ENABLE);
  
  
}

void GPIO_Config(void)
{
  GPIO_Init(PWR_PORT,(GPIO_Pin_TypeDef)PWR_PIN,GPIO_MODE_OUT_PP_HIGH_SLOW);
  GPIO_Init(BTN_PORT,(GPIO_Pin_TypeDef)BTN_ALL,GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(LED_GPIO_PORT, (GPIO_Pin_TypeDef)LEDS, GPIO_MODE_OUT_PP_LOW_FAST);
  //GPIO_WriteHigh(BTN_PORT,BTN_1);
  //GPIO(BTN_PORT,BTN_1);  
  /*EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOC, EXTI_TLISENSITIVITY_RISE_ONLY);
  EXTI_SetTLISensitivity(EXTI_TLISENSITIVITY_RISE_ONLY);*/
}

void ADC_Config(){
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC,ENABLE);
 ADC1_DeInit();     
       ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS,
                 ADC1_CHANNEL_0,
                 ADC1_PRESSEL_FCPU_D2,
                 ADC1_EXTTRIG_GPIO,
                 DISABLE,
                 ADC1_ALIGN_RIGHT,
                 ADC1_SCHMITTTRIG_CHANNEL0,
                 DISABLE);
       ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS,
                 ADC1_CHANNEL_1,
                 ADC1_PRESSEL_FCPU_D2,
                 ADC1_EXTTRIG_GPIO,
                 DISABLE,
                 ADC1_ALIGN_RIGHT,
                 ADC1_SCHMITTTRIG_CHANNEL1,
                 DISABLE);
       ADC1_ConversionConfig(ADC1_CONVERSIONMODE_CONTINUOUS,
                 ((ADC1_Channel_TypeDef)(ADC1_CHANNEL_0 | ADC1_CHANNEL_1)),
                 ADC1_ALIGN_RIGHT);
       ADC1_DataBufferCmd(ENABLE);
       ADC1_Cmd(ENABLE);
}

uint16_t GetADC(){
    ADC_Config();
    ADC1_ScanModeCmd(ENABLE);
    ADC1_StartConversion();
    while(ADC1_GetFlagStatus(ADC1_FLAG_EOC) == FALSE);
    ADC1_ClearFlag(ADC1_FLAG_EOC);
    uint16_t ret=(float)2310/ADC1_GetBufferValue(1)*ADC1_GetBufferValue(0);
    ADC1_ScanModeCmd(DISABLE);
    ADC1_DeInit();
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC,DISABLE);
    return ret;
}

/*
INTERRUPT_HANDLER(IRQ_Handler_TIM4, 23)
{
   TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
   //TimerCount++;
}
*/
