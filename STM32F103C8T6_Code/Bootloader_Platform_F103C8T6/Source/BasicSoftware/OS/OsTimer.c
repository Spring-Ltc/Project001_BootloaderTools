#include "OsTimer.h"


void OsTimer_Start(void)
{
    // FC4150_SysTick->CSR |= 0x00000001u;
    HAL_TIM_Base_Start_IT(&htim4);
}

void OsTimer_Stop(void)
{
    // FC4150_SysTick->CSR &= 0xFFFFFFFEu;
}


void OsTimer_Init(void)
{
    // //	FC4150_SCB->SHPR3 |= FC4150_SCB_SHPR3_PRI_15(pCfgPtr->Priority);
    // FC4150_SysTick->CSR = 0U;  
    // FC4150_SysTick->CVR = 0U;  
    // FC4150_SysTick->RVR = ((uint32)(150000000U / 1000U))& (0x00FFFFFFu);
    // FC4150_SysTick->CSR |= 0x00000006u;
}


void OsTimer_DeInit(void)
{
	// FC4150_SysTick->CSR = 0U;  
	// FC4150_SysTick->CVR = 0U;  
	// FC4150_SysTick->RVR = 0U;
	// FC4150_SCB->ICSR = 1u << 25u; 
}



