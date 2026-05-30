#ifndef Common_Types_H
#define Common_Types_H



#define    NULL_PTR       ((void*)0)

typedef unsigned char           uint8;
typedef unsigned short int      uint16;
typedef unsigned int            uint32;
typedef unsigned long long      uint64;

#define STD_ON          (1u)
#define STD_OFF         (0u)

#define STD_HIGH        (1u)
#define STD_LOW         (0u)

#define STD_TRUE        (1u)
#define STD_FALSE       (0u)



typedef uint8 StdRetType;
#define RET_OK                          (0u)
#define RET_PARAMETER_INVALID           (1u)
#define RET_OPERATION_NOT_SUPPORTED     (2u)
#define RET_VALUE_RANGE                 (3u)
#define RET_ADC_BUSY                    (4u)
#define RET_ADDRESS_ILLEGAL             (5u)
#define RET_TIMEOUT                     (6u)
#define RET_ERROR                       (7u)






#define RESET_REASON_NULL           (0x00)
#define RESET_REASON_LSIRDY         (0x01)
#define RESET_REASON_BORRST         (0x02)
#define RESET_REASON_PINRST         (0x04)
#define RESET_REASON_PORRST         (0x08)
#define RESET_REASON_SFTRST         (0x10)
#define RESET_REASON_IWDGRST        (0x20)
#define RESET_REASON_WWDGRST        (0x40)
#define RESET_REASON_LPWRRST        (0x80)




#define USER_COMPILE_OPTIMIZATION   (STD_OFF)

/* #include "cmsis_compiler.h"  */

#if(STD_ON == USER_COMPILE_OPTIMIZATION)
#define USER_FORCE_INLINE   __attribute__ ( (always_inline))
/* 16902 + 710 + 32 + 2832*/
/* 16898 + 710 + 32 + 2832*/
// #define USER_FORCE_INLINE         __STATIC_FORCEINLINE
#else
#define USER_FORCE_INLINE  
#endif




#endif

