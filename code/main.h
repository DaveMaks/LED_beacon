#ifndef __MAIN_H
#define __MAIN_H

#include "stm8s.h"

#define LED_GPIO_PORT GPIOD
#define LED_1 (GPIO_PIN_3)
#define LED_2 (GPIO_PIN_4)
#define LEDS (LED_1 | LED_2)

#define PWR_PORT GPIOB
#define PWR_PIN GPIO_PIN_7

#define BTN_PORT GPIOC
#define BTN_1 (GPIO_PIN_7)
#define BTN_POWER (GPIO_PIN_7)
#define BTN_2 (GPIO_PIN_6)
#define BTN_SET (GPIO_PIN_6)
#define BTN_ALL (BTN_1 | BTN_2)

#define ADC_1 0 //PORTB GPIO_PIN_0 прямой
#define ADC_2 1 //PORTB GPIO_PIN_1 со стабилизатором

#define DEFAULT_DELAY_SLEEP 5 // задержка между циклами
#define LOADING_DELAY       50 //задержка при миганни уведомлений
#define MIN_VOLTAGE         3300  //минимальный вольтаж отключения в миливольтах

/* Exported functions ------------------------------------------------------- */

void CLK_Config();
void TIM2_Config();
void TIM4_Config();
void TIM1_Config();
void delay_s(uint32_t s);
void delay_cs(uint32_t cs);
void GPIO_Config(void);
void loading(uint8_t count);
void ReadConfig();
void WriteConfig();
void ConfigSET_Lighet();
void ConfigSET_DelayHi();
void ConfigSET_DelayLow();
void LEDFlash();
void ADC_Config();
uint16_t GetADC();


#endif /* __MAIN_H */