
#include "OsTask.h"



#include "Dio_UserIf.h"
#include "Uart_UserIf.h"
#include "Flash_UserIf.h"










void BSW_Init(void)
{

    DioIf_Init();
    UartIf_Init();
    FlsIf_Init();
    
}

void ASW_Init(void)
{

}












/* ====================================== Task for BSW ================================= */
static void Task_BSW_1ms(void)
{
    UartIf_MainFunction_Tx();
}

static void Task_BSW_5ms(void)
{
    UartIf_MainFunction_Rx();

}

static void Task_BSW_10ms(void)
{

}

static void Task_BSW_20ms(void)
{
    // AdcIf_MainFunction();
}

static void Task_BSW_50ms(void)
{
    // #if(STD_ON == DEVTEST_UART_ENABLE)
    // DevTest_Uart_MainFunction();
    // #endif
}


// uint8 UartRead_Cmd = 0u;
static void Task_BSW_100ms(void)
{
    // #if (MCAL_TEST_DIO == STD_ON)
    // DioIf_MainFunction_TEST();
    // #endif

    // if(0u != UartRead_Cmd)
    // {
    //     UartRead_Cmd = 0u;
    //     UartIf_MainFunction_Rx();
    // }
    


}

static void Task_BSW_500ms(void)
{
    DioIf_TogglePin(DioChannel_SysLED);
}

uint32 SystemTime_Seconds = 0u;
static void Task_BSW_1000ms(void)
{
    SystemTime_Seconds++;
}

/* ====================================== Task for ASW ================================= */
#if (STD_ON == ASW_TASK_ENABLE)
static void Task_ASW_1ms(void)
{

}

static void Task_ASW_5ms(void)
{

}

static void Task_ASW_10ms(void)
{

}

static void Task_ASW_20ms(void)
{

}

static void Task_ASW_50ms(void)
{

}

static void Task_ASW_100ms(void)
{

}

static void Task_ASW_500ms(void)
{

}

static void Task_ASW_1000ms(void)
{

}
#endif



/* ====================================== Task for BSW ================================= */

const OsTask_CfgInfo_Type OsTaskCfg[] = {
    /* Task config for BSW */
    {.TaskID = 00u, .TaskPeriod = 1u,      .TaskDelay = 000u,    .TaskFun = Task_BSW_1ms},
    {.TaskID = 01u, .TaskPeriod = 5u,      .TaskDelay = 000u,    .TaskFun = Task_BSW_5ms},
    {.TaskID = 02u, .TaskPeriod = 10u,     .TaskDelay = 000u,    .TaskFun = Task_BSW_10ms},
    {.TaskID = 03u, .TaskPeriod = 20u,     .TaskDelay = 000u,    .TaskFun = Task_BSW_20ms},
    {.TaskID = 04u, .TaskPeriod = 50u,     .TaskDelay = 000u,    .TaskFun = Task_BSW_50ms},
    {.TaskID = 05u, .TaskPeriod = 100u,    .TaskDelay = 000u,    .TaskFun = Task_BSW_100ms},
    {.TaskID = 06u, .TaskPeriod = 500u,    .TaskDelay = 000u,    .TaskFun = Task_BSW_500ms},
    {.TaskID = 07u, .TaskPeriod = 1000u,   .TaskDelay = 000u,    .TaskFun = Task_BSW_1000ms},

    /* Task config for ASW */
    #if (STD_ON == ASW_TASK_ENABLE)
    {.TaskID = 100u, .TaskPeriod = 1u,      .TaskDelay = 000u,    .TaskFun = Task_ASW_1ms},
    {.TaskID = 101u, .TaskPeriod = 5u,      .TaskDelay = 002u,    .TaskFun = Task_ASW_5ms},
    {.TaskID = 102u, .TaskPeriod = 10u,     .TaskDelay = 004u,    .TaskFun = Task_ASW_10ms},
    {.TaskID = 103u, .TaskPeriod = 20u,     .TaskDelay = 005u,    .TaskFun = Task_ASW_20ms},
    {.TaskID = 104u, .TaskPeriod = 50u,     .TaskDelay = 005u,    .TaskFun = Task_ASW_50ms},
    {.TaskID = 105u, .TaskPeriod = 100u,    .TaskDelay = 005u,    .TaskFun = Task_ASW_100ms},
    {.TaskID = 106u, .TaskPeriod = 500u,    .TaskDelay = 050u,    .TaskFun = Task_ASW_500ms},
    {.TaskID = 107u, .TaskPeriod = 1000u,   .TaskDelay = 100u,    .TaskFun = Task_ASW_1000ms},
    #endif
};

static OsTask_Text_Type OsTaskText[sizeof(OsTaskCfg)/sizeof(OsTask_CfgInfo_Type)];

OsTask_Control_Type OsTaskCtrl = {
    .TaskNum = sizeof(OsTaskCfg)/sizeof(OsTask_CfgInfo_Type),
    .pTaskCfgList = OsTaskCfg,
    .pTaskText = OsTaskText,
};

