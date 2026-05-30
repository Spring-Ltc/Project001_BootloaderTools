
#include "OsTask.h"
#include "OsTimer.h"





uint32 gOsTick_1msCnt = 0u;



void OsTick_1ms_ISR(void)
{
    uint8 i = 0u;
    gOsTick_1msCnt++;
    for(i=0u;i<OsTaskCtrl.TaskNum;i++)
    {
        OsTaskCtrl.pTaskText[i].TaskCnt++;
        if(OsTaskCtrl.pTaskText[i].TaskCnt >= OsTaskCtrl.pTaskCfgList[i].TaskPeriod)
        {
            OsTaskCtrl.pTaskText[i].TaskRunFlag = 1u;
            OsTaskCtrl.pTaskText[i].TaskCnt = 0u;
        }
    }
}



static void OsTask_Init(void)
{
    uint8 i=0u;
    for(i=0u;i<OsTaskCtrl.TaskNum;i++)
    {
        OsTaskCtrl.pTaskText[i].TaskCnt = OsTaskCtrl.pTaskCfgList[i].TaskDelay;
        OsTaskCtrl.pTaskText[i].TaskRunFlag = 0u;
    }
}




static void OsTask_Schedule(void)
{
    uint8 i=0u;
    for(i=0u;i<OsTaskCtrl.TaskNum;i++)
    {
        if(OsTaskCtrl.pTaskText[i].TaskRunFlag != 0u)
        {
            OsTaskCtrl.pTaskText[i].TaskRunFlag = 0u;
            #if(STD_ON == OS_TASK_TIME_MONITOR)
            // TaskRun_Pre(OsTaskCtrl.pTaskCfgList[i].TaskID);
            #endif
            OsTaskCtrl.pTaskCfgList[i].TaskFun();
            #if(STD_ON == OS_TASK_TIME_MONITOR)
            // TaskRun_Post(OsTaskCtrl.pTaskCfgList[i].TaskID);
            #endif
        }
    }
}








void StartOS(void)
{
    OsTask_Init();


    OsTimer_Init();
    OsTimer_Start();

    while(1u)
    {
       OsTask_Schedule(); 
    }
}




void OSTask_Main(void)
{
    BSW_Init();
    ASW_Init();

    StartOS();
    while(1)
    {
        /* Report Os Error*/
    }
}



