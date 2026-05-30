

#ifndef     Flash_UserIf_H
#define     Flash_UserIf_H



#include "Common_Types.h"

#define FLS_SECTOR_NUM          (6u)



#define FLS_APP_START_ADDRESS   (0x08010000u)


#define ADDR_FLASH_BASE         (0x08000000u)   /* start address of Flash */
#define CHIP_FLASH_SIZE         (256*1024)      /* Flash size = 256K */
#define CHIP_RAM_SIZE           (128*1024)      /* RAM size = 128K */


#if 0

typedef enum
{
    Fls_Request_NULL = 0u,
    Fls_Request_Write,
    Fls_Request_Read,
}FLS_RequestType;



extern FLS_RequestType Fls_Request;




typedef struct{
    uint8 FlsSectorIndex;
    uint32 FlsSectorStartAddr;
    uint8 FlsSectorSizeKB;
    uint32 FlsSectorEndAddr;
}FLS_SECTOR_INFORMATION_TYPE;
extern const FLS_SECTOR_INFORMATION_TYPE FlsSectorInfo[FLS_SECTOR_NUM];







typedef struct{
    uint32 Data_001;
    uint32 Data_002;
    uint32 Data_003;
    uint32 Data_004;
    uint32 Data_005;
    uint32 Data_006;

    uint8   Buffer[3];
    /* ............. */
}__attribute__((aligned(4))) FlashDataType;
extern FlashDataType UserFlashData;










extern void FlsIf_MainFunction(void);







typedef  void (*IAPfun)(void);


extern StdRetType IAP_JumpToApplication(void);

extern StdRetType Fls_GetSectorIndex(uint32 Addr, uint8* SectorIndex);
extern StdRetType FlsIf_EraseSector(uint8 SectorIndex);


#endif


extern void FlsIf_Init(void);



#endif
/**********************End of files**********************/



