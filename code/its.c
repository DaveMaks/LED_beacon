
#include "its.h"
/**
  * @brief  Timer4 Update/Overflow Interrupt routine
  * @param  None
  * @retval None
  */
extern uint32_t TimerCount;


INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23)
{
  TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
  TimerCount++;
}