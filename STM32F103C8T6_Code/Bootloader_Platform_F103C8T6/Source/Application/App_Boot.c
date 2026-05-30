
#include "App_Boot.h"


#include "Uart_UserIf.h"


uint8 AppBuf[64] = {0};


void Asw_Callback_Receive_Uart1(const uint8 *Str,uint32 Len)
{
    uint8 i = 0u;
    for(i=0u;i<Len;i++)
    {
        AppBuf[i] = Str[i];
    }

    UartIf_Sequence_Transmit(AppBuf,Len,0u);


}



