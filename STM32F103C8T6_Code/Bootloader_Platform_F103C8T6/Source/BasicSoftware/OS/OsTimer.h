

#ifndef __OSTIMER_H__
#define __OSTIMER_H__

#include "Common_Types.h"

#include "tim.h"

// /** FC4150_SysTick - Register Layout Typedef */
// typedef struct {
//     __IO uint32 CSR;                               /**< SysTick Control and Status Register, offset: 0x0 */
//     __IO uint32 RVR;                               /**< SysTick Reload Value Register, offset: 0x4 */
//     __IO uint32 CVR;                               /**< SysTick Current Value Register, offset: 0x8 */
//     __I  uint32 CALIB;                             /**< SysTick Calibration Value Register, offset: 0xC */
//   } FC4150_SysTick_Type, *FC4150_SysTick_MemMapPtr;




/* FC4150_SysTick - Peripheral instance base addresses */
/** Peripheral FC4150_SysTick base address */
#define FC4150_SysTick_BASE                         (0xE000E010u)
/** Peripheral FC4150_SysTick base pointer */
// #define FC4150_SysTick                              ((FC4150_SysTick_Type *)FC4150_SysTick_BASE)






// #define     OsTick_1ms_ISR   SysTick_Handler


extern void OsTimer_Start(void);
extern void OsTimer_Stop(void);
extern void OsTimer_Init(void);
extern void OsTimer_DeInit(void);












#endif



