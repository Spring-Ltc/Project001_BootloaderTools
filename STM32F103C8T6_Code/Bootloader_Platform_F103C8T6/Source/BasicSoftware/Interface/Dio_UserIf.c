
#include "Dio_UserIf.h"








const static DioConfig_Type DioConfig[DioChannel_MAXNUM] = {
    {DioChannel_SysLED,     SysLED_GPIO_Port,       SysLED_Pin,     DioPermission_OUTPUT},
};





void DioIf_Init(void)
{

    /* Turn off all LED */
    DioIf_WritePin(DioChannel_SysLED,STD_HIGH);

}



StdRetType DioIf_WritePin(DioChannel_Type DioChannel,uint8 DioLevel)
{
    StdRetType Ret = RET_OK;
    if(DioChannel >= DioChannel_MAXNUM)
    {
        Ret = RET_PARAMETER_INVALID;
    }
    else if(DioPermission_OUTPUT != DioConfig[DioChannel].Dio_Permission)
    {
        Ret = RET_OPERATION_NOT_SUPPORTED;
    }
    else
    {
        if(STD_LOW != DioLevel)
        {
            HAL_GPIO_WritePin(DioConfig[DioChannel].Dio_Port,DioConfig[DioChannel].Dio_Pin,GPIO_PIN_SET);
        }
        else
        {
            HAL_GPIO_WritePin(DioConfig[DioChannel].Dio_Port,DioConfig[DioChannel].Dio_Pin,GPIO_PIN_RESET);
        }
    }
    return Ret;
}



StdRetType DioIf_ReadPin(DioChannel_Type DioChannel,uint8 *DioLevel)
{
    StdRetType Ret = RET_OK;

    if(DioChannel >= DioChannel_MAXNUM)
    {
        Ret = RET_PARAMETER_INVALID;
    }
    else if(DioPermission_INPUT != DioConfig[DioChannel].Dio_Permission)
    {
        Ret = RET_OPERATION_NOT_SUPPORTED;
    }
    else
    {
        if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(DioConfig[DioChannel].Dio_Port,DioConfig[DioChannel].Dio_Pin))
        {
            *DioLevel = STD_LOW;
        }
        else
        {
            *DioLevel = STD_HIGH;
        }
    }
    return Ret;
}



StdRetType DioIf_TogglePin(DioChannel_Type DioChannel)
{
    StdRetType Ret = RET_OK;

    if(DioChannel >= DioChannel_MAXNUM)
    {
        Ret = RET_PARAMETER_INVALID;
    }
    else if(DioPermission_OUTPUT != DioConfig[DioChannel].Dio_Permission)
    {
        Ret = RET_OPERATION_NOT_SUPPORTED;
    }
    else
    {
        HAL_GPIO_TogglePin(DioConfig[DioChannel].Dio_Port,DioConfig[DioChannel].Dio_Pin);
    }
    return Ret;  
}


/**********************End of files**********************/



