
#include "Flash_UserIf.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_flash_ex.h"



// const FLS_SECTOR_INFORMATION_TYPE FlsSectorInfo[FLS_SECTOR_NUM] = {
//     {0, 0x08000000u, 16, 0x08003FFFu},
//     {1, 0x08004000u, 16, 0x08007FFFu},
//     {2, 0x08008000u, 16, 0x0800BFFFu},
//     {3, 0x0800C000u, 16, 0x0800FFFFu},
//     {4, 0x08010000u, 64, 0x0801FFFFu},
//     {5, 0x08020000u,128, 0x0803FFFFu},
// };




// uint32 IapBuffer[256] = {0};    /* 256*4 = 1024 bytes = 1K buffer */



#if 0





#define FLS_START_ADDRESS       (0x0800C000u)
#define FLS_SIZE_MAX            (1024*16)


#define FLASH_WAITETIME  50000
FlashDataType UserFlashData = {0};
#define  FLASH_SIZE_BYTES    (sizeof(UserFlashData))




void FlashIf_ReadUserData(void)
{
    uint32 AddrStart = FLS_START_ADDRESS;
    uint32 FlashSize_Words = FLASH_SIZE_BYTES/4 + ((FLASH_SIZE_BYTES%4)?1:0);
    uint32* BufferPtr = (uint32*)&UserFlashData;
    uint32 i=0;
    for(i=0;i<FlashSize_Words;i++)
    {
        BufferPtr[i] = *(volatile uint32*)AddrStart;
        AddrStart += 4u;
    }
}


StdRetType FlsIf_EraseSector(uint8 SectorIndex)
{
    StdRetType Ret = RET_OK;
    FLASH_EraseInitTypeDef  FlsEraseInit = {0};
    uint32 SectorErrorCode = 0;

    if(FLS_SECTOR_NUM <= SectorIndex)   /* 检查扇区索引是否合法*/
    {
        Ret = RET_VALUE_RANGE;
    }
    else
    {
        Ret = HAL_FLASH_Unlock();/* Step1: unlock*/
        if(HAL_OK == Ret)
        {
            __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | \
                       FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
            Ret = FLASH_WaitForLastOperation(FLASH_WAITETIME);
            FlsEraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;
            FlsEraseInit.Sector = SectorIndex;
            FlsEraseInit.NbSectors = 1;
            FlsEraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;
            Ret |= HAL_FLASHEx_Erase(&FlsEraseInit,&SectorErrorCode);
        }else{/* do nothing*/}
        if(HAL_OK == Ret)
        {
            Ret = FLASH_WaitForLastOperation(FLASH_WAITETIME);
            Ret |= HAL_FLASH_Lock();/* Step4: Lock*/
        }else{/* do nothing*/}
    }
    return Ret;
}


uint8 FlsWriteRet = 0u;
HAL_StatusTypeDef FlashIf_WriteUserData(void)
{
    HAL_StatusTypeDef ret = HAL_OK;
    FLASH_EraseInitTypeDef  FlsEraseInit = {0};
    uint32 SectorErrorCode = 0;
    uint32 AddrStart = FLS_START_ADDRESS;
    uint32 FlashSize_Words = FLASH_SIZE_BYTES/4 + ((FLASH_SIZE_BYTES%4)?1:0);
    uint32* BufferPtr = (uint32*)&UserFlashData;
    uint32 i=0;

    FlsWriteRet = 0u;
    /* Step1: unlock*/
    FlsWriteRet = HAL_FLASH_Unlock();

    FlsWriteRet |FLASH_WaitForLastOperation(FLASH_WAITETIME);

    /* Step2: Erase old data*/
    FlsEraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;
    FlsEraseInit.Sector = 3;
    FlsEraseInit.NbSectors = 1;
    FlsEraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    FlsWriteRet |= HAL_FLASHEx_Erase(&FlsEraseInit,&SectorErrorCode);

    FlsWriteRet |FLASH_WaitForLastOperation(FLASH_WAITETIME);

    /* Step3: Write data*/
    if(HAL_OK == FlsWriteRet)
    {
        FlsWriteRet = FLASH_WaitForLastOperation(FLASH_WAITETIME);
        if(HAL_OK == FlsWriteRet)
        {
            for(i=0;i<FlashSize_Words;i++)
            {
                FlsWriteRet |= HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,AddrStart,BufferPtr[i]);
                if(HAL_OK != FlsWriteRet)
                {
                    break;
                }
                AddrStart += 4u;
            }
        }
    }

    /* Step4: Lock*/
    FlsWriteRet |HAL_FLASH_Lock();

    return ret;
}







uint8 Fls_Request = 0;
void FlsIf_MainFunction(void)
{

    if(Fls_Request_NULL != Fls_Request)
    {
        if(Fls_Request_Write == Fls_Request)
        {
            FlashIf_WriteUserData();
        }
        else if(Fls_Request_Read == Fls_Request)
        {
            FlashIf_ReadUserData();
        }
        Fls_Request = 0u;
    }

}




__asm void MSR_MSP(uint32 addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}

StdRetType IAP_JumpToApplication(void)
{
    StdRetType Ret = RET_OK;
    IAPfun jump2app;



	if(((*(volatile uint32*)FLS_APP_START_ADDRESS)&0x2FF00000)==0x20000000)	//检查栈顶地址是否合法.
	{ 
		jump2app=(IAPfun)*(volatile uint32*)(FLS_APP_START_ADDRESS+4);		//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP(*(volatile uint32*)FLS_APP_START_ADDRESS);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		jump2app();									//跳转到APP.
	}
    else
    {
        Ret = RET_VALUE_RANGE;
    }

    return Ret;
}




StdRetType Fls_GetSectorIndex(uint32 Addr, uint8* SectorIndex)
{
    StdRetType Ret = RET_OK;
    uint8 i=0;
    uint8 Found = 0u;

    for(i=0;i<FLS_SECTOR_NUM;i++)
    {
        if((Addr >= FlsSectorInfo[i].FlsSectorStartAddr)&&(Addr <= FlsSectorInfo[i].FlsSectorEndAddr))
        {
            *SectorIndex = FlsSectorInfo[i].FlsSectorIndex;
            Found = 1u;
            break;
        }
    }

    if(0u == Found)
    {
        Ret = RET_VALUE_RANGE;
    }

    return Ret;
}




#endif


void FlsIf_Init(void)
{
    // FlashIf_ReadUserData();
}


/**********************End of files**********************/



