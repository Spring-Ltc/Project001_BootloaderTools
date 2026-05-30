
#ifndef     Uart_UserIf_H
#define     Uart_UserIf_H

#include "Common_Types.h"
#include "usart.h"



#define BOOT_UART_CHANNEL 		(huart2)
#define BOOT_UART_CHANNEL_DMA_RX	(hdma_usart2_rx)
#define BOOT_UART_CHANNEL_DMA_TX	(hdma_usart2_tx)


#define UART_SEQUENCE_TX_POLLING_MODE			(0u)	/* 轮询模式，调用发送接口后等待之前的发送完成再进行下一次发送，适用于发送频率较高的情况，避免数据丢失 */		
#define UART_SEQUENCE_TX_IMMEDIATELY_MODE		(1u)	/* 立即模式，调用发送接口后立即发送，不管之前的发送是否完成，适用于发送频率较低的情况，避免等待之前的发送完成；如果发送频率较高，建议使用轮询模式，等待之前的发送完成后再进行下一次发送，避免数据丢失 */		



typedef enum{
	UART_SEQUENCE_IDLE = 0u,	/* 空闲状态，如果是TX，表示允许往Buffer写数据；如果是RX，表示允许将该Buffer用于接收数据 */
	UART_SEQUENCE_BUSY,			/* 忙碌状态，如果是TX，表示该Buffer的数据正在发送；如果是RX，表示接收数据正在往该Buffer存储 */
	UART_SEQUENCE_READY,		/* 就绪状态，如果是TX，表示该Buffer的数据发送完成；如果是RX，表示接收完一帧并存好在该Buffer，允许在MainFunction中读取处理 */
}Uart_Sequence_StateEnumType;

typedef struct{
	Uart_Sequence_StateEnumType State;	
	uint8 DataLength;	
	uint8 *DataPtr;	
}UART_Sequence_Type;



extern void UartCallback_USART_BOOT_IRQHandler(void);
extern StdRetType UartIf_Sequence_Transmit(const uint8 *Data,uint16 Len,uint8 TxMode);	/* 通过Sequence发送数据*/
extern void UartIf_Init(void);
extern void UartIf_MainFunction_Tx(void);
extern void UartIf_MainFunction_Rx(void);


#endif
/**********************End of files**********************/



