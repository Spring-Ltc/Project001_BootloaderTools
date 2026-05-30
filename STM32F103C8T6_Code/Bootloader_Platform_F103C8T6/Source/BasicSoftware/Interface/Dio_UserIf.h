
#ifndef     Dio_UserIf_H
#define     Dio_UserIf_H

#include "Common_Types.h"

#include "gpio.h"
#include "stm32f1xx.h"

typedef enum{
    DioChannel_SysLED   = 0,
    DioChannel_MAXNUM
}DioChannel_Type;

#define DioPermission_INPUT      (0x01u)
#define DioPermission_OUTPUT     (0x02u)

typedef struct{
    DioChannel_Type     Dio_Name;
    GPIO_TypeDef        *Dio_Port;
    uint16              Dio_Pin;
    uint8               Dio_Permission;
}DioConfig_Type;











extern void DioIf_Init(void);

extern StdRetType DioIf_WritePin(DioChannel_Type DioChannel,uint8 DioLevel);
extern StdRetType DioIf_ReadPin(DioChannel_Type DioChannel,uint8 *DioLevel);
extern StdRetType DioIf_TogglePin(DioChannel_Type DioChannel);








#endif
/**********************End of files**********************/



