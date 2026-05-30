

#ifndef __OSTASK_H__
#define __OSTASK_H__


#include "Common_Types.h"


#define ASW_TASK_ENABLE          (STD_OFF)




#define OS_TASK_TIME_MONITOR    (STD_OFF)






typedef struct{
    uint8   TaskID;
    uint32  TaskPeriod; /* unit is ms */
    uint32  TaskDelay;  /* unit is ms */
    void    (*TaskFun)(void);  
}OsTask_CfgInfo_Type;


typedef struct{
    uint8 TaskRunFlag;
    uint32 TaskCnt;
    #if(STD_ON == OS_TASK_TIME_MONITOR)
    uint32  MonitorCycleTime;
    uint32  MonitorRunningTime;
    uint32  MonitorCycleTimeMax;
    uint32  MonitorRunningTimeMax;
    #endif
}OsTask_Text_Type;





typedef struct{
    const uint8 TaskNum;
    const OsTask_CfgInfo_Type *pTaskCfgList;
    OsTask_Text_Type *pTaskText;
}OsTask_Control_Type;
extern OsTask_Control_Type OsTaskCtrl;












extern uint32 gOsTick_1msCnt;

extern void OsTick_1ms_ISR(void);
extern void BSW_Init(void);
extern void ASW_Init(void);
extern void StartOS(void);




extern void OSTask_Main(void);

















#endif
















