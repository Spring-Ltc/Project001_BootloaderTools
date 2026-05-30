
#include "Uart_UserIf.h"

#include "App_Boot.h"

// #include "string.h"
#include "UserPublic.h"

/* 再维护一个TX队列，更常用 */
#define UART_TX_SEQUENCE_SIZE		(64u)	/* 单次发送的最大长度，单位为字节，根据实际需求调整 */
#define UART_TX_SEQUENCE_DEPTH		(8u)	/* 队列的深度 */
static uint8 Uart_Tx_Sequence_Buffer[UART_TX_SEQUENCE_DEPTH][UART_TX_SEQUENCE_SIZE];	/* 用于存储待发送的UART数据的队列 */
static UART_Sequence_Type Uart_Tx_Sequence[UART_TX_SEQUENCE_DEPTH];	
static uint8 Uart_Tx_SequenceFullFlag = 0u; 
static uint8 Uart_Tx_SequenceOutIndex = 0u;	/* 当前正在发送的队列索引 */
static uint8 Uart_Tx_SequenceInIndex = 0u;	/* 当前正在写入的队列索引 */
static uint8 Uart_Tx_SequenceEmptyFlag = 1u;	/* 发送队列空标志，初始为1，表示发送队列为空 */

#define UART_RX_SEQUENCE_SIZE		(64u)	/* 单次接收的最大长度，单位为字节，根据实际需求调整 */
#define UART_RX_SEQUENCE_DEPTH		(8u)	/* 队列的深度 */
static uint8 Uart_Rx_Sequence_Buffer[UART_RX_SEQUENCE_DEPTH][UART_RX_SEQUENCE_SIZE];	/* 用于存储接收到的UART数据的队列 */
static UART_Sequence_Type Uart_Rx_Sequence[UART_RX_SEQUENCE_DEPTH];	
static uint8 Uart_Rx_SequenceFullFlag = 0u;
static uint8 Uart_Rx_SequenceOutIndex = 0u;	/* 当前正在读出的队列索引 */
static uint8 Uart_Rx_SequenceInIndex = 0u;	/* 当前正在写入的队列索引 */



/* ============================================ static local functions ========================================*/

static void Uart_Sequence_Init(void)
{
	uint8 i = 0u;
	for(i=0u;i<UART_RX_SEQUENCE_DEPTH;i++)
	{
		Uart_Rx_Sequence[i].State = UART_SEQUENCE_IDLE;
		Uart_Rx_Sequence[i].DataLength = 0u;
		Uart_Rx_Sequence[i].DataPtr = Uart_Rx_Sequence_Buffer[i];
	}
	Uart_Rx_SequenceInIndex = 0u;
	Uart_Rx_SequenceOutIndex = 0u;

	for(i=0u;i<UART_TX_SEQUENCE_DEPTH;i++)
	{
		Uart_Tx_Sequence[i].State = UART_SEQUENCE_IDLE;
		Uart_Tx_Sequence[i].DataLength = 0u;
		Uart_Tx_Sequence[i].DataPtr = Uart_Tx_Sequence_Buffer[i];
	}
	Uart_Tx_SequenceInIndex = 0u;
	Uart_Tx_SequenceOutIndex = 0u;
}


static StdRetType UartIf_Sequence_GetIdleIndex_Rx(uint8 *IdleIndex)
{
	StdRetType Ret = RET_OK;
	uint8 NextIndex = (*IdleIndex + 1u) % UART_RX_SEQUENCE_DEPTH;	/* 计算下一个队列索引，循环使用队列 */
	if(UART_SEQUENCE_IDLE == Uart_Rx_Sequence[NextIndex].State)	/* 检查下一个队列是否空闲 */
	{
		*IdleIndex = NextIndex;	/* 更新正在使用的队列索引 */
	}
	else
	{
		Ret = RET_ERROR;	/* 没有空闲队列，返回错误 */
	}
	return Ret;
}


static StdRetType UartIf_Sequence_GetIdleIndex_Tx(uint8 *IdleIndex)
{
	StdRetType Ret = RET_OK;
	uint8 SearchCnt = UART_TX_SEQUENCE_DEPTH;	/* 最多搜索队列深度次，避免死循环 */
	while((UART_SEQUENCE_IDLE != Uart_Tx_Sequence[*IdleIndex].State)&&(SearchCnt--))
	{
		*IdleIndex = (*IdleIndex + 1u) % UART_TX_SEQUENCE_DEPTH;	/* 计算下一个队列索引，循环使用队列 */
	}
	if(UART_SEQUENCE_IDLE != Uart_Tx_Sequence[*IdleIndex].State)/* 当前队列是空闲的，直接返回当前队列*/
	{
		Ret = RET_ERROR;	/* 没有空闲队列，返回错误 */
	}
	return Ret;
}


static StdRetType UartIf_Sequence_GetReadyIndex_Tx(uint8 *ReadyIndex)
{
	StdRetType Ret = RET_OK;
	uint8 SearchCnt = UART_TX_SEQUENCE_DEPTH;	/* 最多搜索队列深度次，避免死循环 */
	while((UART_SEQUENCE_READY != Uart_Tx_Sequence[*ReadyIndex].State)&&(SearchCnt--))
	{
		*ReadyIndex = (*ReadyIndex + 1u) % UART_TX_SEQUENCE_DEPTH;	/* 计算下一个队列索引，循环使用队列 */
	}
	if(UART_SEQUENCE_READY != Uart_Tx_Sequence[*ReadyIndex].State)
	{
		Ret = RET_ERROR;	/* 没有就绪队列，返回错误 */
	}
	return Ret;
}



/* ============================================ global functions ========================================*/

void UartIf_Init(void)
{
	Uart_Sequence_Init();	/* 初始化UART接收和发送队列 */

	Uart_Rx_Sequence[Uart_Rx_SequenceInIndex].State = UART_SEQUENCE_BUSY; /* 初始化设置第一个队列为正在接收*/

	HAL_UART_Receive_DMA(&BOOT_UART_CHANNEL,Uart_Rx_Sequence[Uart_Rx_SequenceInIndex].DataPtr,UART_RX_SEQUENCE_SIZE);

	__HAL_UART_ENABLE_IT(&BOOT_UART_CHANNEL,UART_IT_IDLE);   
}


void UartCallback_USART_BOOT_IRQHandler(void)
{
	if(__HAL_UART_GET_FLAG(&BOOT_UART_CHANNEL,UART_FLAG_IDLE) != RESET)
	{
		__HAL_UART_CLEAR_IDLEFLAG(&BOOT_UART_CHANNEL);
		HAL_UART_DMAStop(&BOOT_UART_CHANNEL);
		/* 接收过程，最早设置队列状态*/
		Uart_Rx_Sequence[Uart_Rx_SequenceInIndex].State = UART_SEQUENCE_READY;	/* 当前队列接收完成，标记为READY状态，等待主函数处理 */
		Uart_Rx_Sequence[Uart_Rx_SequenceInIndex].DataLength = UART_RX_SEQUENCE_SIZE - __HAL_DMA_GET_COUNTER(&BOOT_UART_CHANNEL_DMA_RX);	/* 记录实际接收的数据长度 */
		
		if(RET_OK == UartIf_Sequence_GetIdleIndex_Rx(&Uart_Rx_SequenceInIndex))	/* 获取下一个空闲队列的索引 */
		{
			Uart_Rx_Sequence[Uart_Rx_SequenceInIndex].State = UART_SEQUENCE_BUSY; /* 初始化设置第一个队列为正在接收*/
			HAL_UART_Receive_DMA(&BOOT_UART_CHANNEL,Uart_Rx_Sequence[Uart_Rx_SequenceInIndex].DataPtr,UART_RX_SEQUENCE_SIZE);
		}
		else
		{
			Uart_Rx_SequenceFullFlag = STD_TRUE;	/* 没有空闲队列，设置队列满标志，等待主函数处理完数据后清除 */
		}
	}
}


StdRetType UartIf_Sequence_Transmit(const uint8 *Data,uint16 Len,uint8 TxMode)
{
	StdRetType Ret = RET_OK;
	uint32 TimeoutCnt = 1000;
	if(UART_SEQUENCE_TX_IMMEDIATELY_MODE == TxMode)	/* 立即模式，取消正在发送的Sequence，优先发送当前的*/
	{
		if(UART_SEQUENCE_BUSY == Uart_Tx_Sequence[Uart_Tx_SequenceOutIndex].State)	/* 当前存在正在发送的队列*/
		{
			/* 判断是否已经发送完成*/
			if(BOOT_UART_CHANNEL.gState == HAL_UART_STATE_READY)	/* 已经发送完成，不需要取消,更新当前队列的状态即可 */
			{
				Uart_Tx_Sequence[Uart_Tx_SequenceOutIndex].State = UART_SEQUENCE_IDLE;	/* 发送完成，标记当前队列为空闲状态 */
				Uart_Tx_Sequence[Uart_Tx_SequenceOutIndex].DataLength = 0u;	/* 清除数据长度 */
			}
			else /* 还在发送中，需要取消当前发送，优先发送新的数据 */
			{
				Uart_Tx_Sequence[Uart_Tx_SequenceOutIndex].State = UART_SEQUENCE_READY;	/* 重新设置为就绪状态，表示需要重新发送 */
				HAL_UART_DMAStop(&BOOT_UART_CHANNEL);	/* 取消当前的发送 */
			}
			while((BOOT_UART_CHANNEL.gState != HAL_UART_STATE_READY)&&(TimeoutCnt--));
			if(TimeoutCnt >0)
			{
				HAL_UART_Transmit_DMA(&BOOT_UART_CHANNEL,Data,Len);
			}
			else
			{
				Ret = RET_ERROR;	/* 发送失败，返回错误 */
			}
		}
	}
	else /* UART_SEQUENCE_TX_POLLING_MODE */ /* 轮询模式，写入Sequence的Buffer，然后设置状态即可，等待在MainFunction中处理 */
	{
		/* 获取空闲队列*/
		if(RET_OK == UartIf_Sequence_GetIdleIndex_Tx(&Uart_Tx_SequenceInIndex))	/* 获取到空闲队列的索引 */
		{
			Uart_Tx_Sequence[Uart_Tx_SequenceInIndex].DataLength = (Len > UART_TX_SEQUENCE_SIZE)?UART_TX_SEQUENCE_SIZE:Len;	/* 记录数据长度，超过最大长度则截断 */
			/* 内存拷贝*/
			fast_memcpy(Uart_Tx_Sequence[Uart_Tx_SequenceInIndex].DataPtr, Data, Uart_Tx_Sequence[Uart_Tx_SequenceInIndex].DataLength);
			// memcpy(Uart_Tx_Sequence[Uart_Tx_SequenceInIndex].DataPtr,Data,Uart_Tx_Sequence[Uart_Tx_SequenceInIndex].DataLength);	/* 将数据拷贝到队列的Buffer中 */
			Uart_Tx_Sequence[Uart_Tx_SequenceInIndex].State = UART_SEQUENCE_READY;	/* 标记当前队列为就绪状态，等待在MainFunction中处理 */
			Uart_Tx_SequenceEmptyFlag = STD_FALSE;	/* 发送队列不空了 */
		}
		else /* 没有空闲队列了，返回错误 */
		{
			Uart_Tx_SequenceFullFlag = STD_TRUE;	/* 设置队列满标志，等待主函数处理完数据后清除 */
			return RET_ERROR;
		}
	}
	return Ret;
}


void UartIf_MainFunction_Tx(void)
{
	if(STD_TRUE == Uart_Tx_SequenceEmptyFlag)	/* 如果发送队列为空，直接退出 */
	{
		/* do nothing */
	}
	else
	{
		if(UART_SEQUENCE_IDLE == Uart_Tx_Sequence[Uart_Tx_SequenceOutIndex].State)	/* 如果当前队列空闲,表明不需要发送，直接退出即可*/
		{
			/* do nothing */
		}
		else if(UART_SEQUENCE_READY == Uart_Tx_Sequence[Uart_Tx_SequenceOutIndex].State)	/* 如果当前队列就绪，说明有数据需要发送 */
		{
			if(BOOT_UART_CHANNEL.gState == HAL_UART_STATE_READY) /* 总线空闲，允许发送*/
			{
				Uart_Tx_Sequence[Uart_Tx_SequenceOutIndex].State = UART_SEQUENCE_BUSY;	/* 标记当前队列为忙碌状态，正在发送 */
				HAL_UART_Transmit_DMA(&BOOT_UART_CHANNEL,Uart_Tx_Sequence[Uart_Tx_SequenceOutIndex].DataPtr,Uart_Tx_Sequence[Uart_Tx_SequenceOutIndex].DataLength);	/* 发送当前队列的数据 */
			}
			else /* 有数据需要发送，但是总线上还有未发完的数据，需要等待*/
			{
				/* do nothing */
			}
		}
		else if(UART_SEQUENCE_BUSY == Uart_Tx_Sequence[Uart_Tx_SequenceOutIndex].State)	/* 如果当前队列忙碌，说明有数据正在发送 */
		{
			if(BOOT_UART_CHANNEL.gState == HAL_UART_STATE_READY)	/* 总线状态空闲，表示当前队列发送完成 */
			{
				Uart_Tx_Sequence[Uart_Tx_SequenceOutIndex].State = UART_SEQUENCE_IDLE;	/* 发送完成，标记当前队列为空闲状态 */
				Uart_Tx_Sequence[Uart_Tx_SequenceOutIndex].DataLength = 0u;	/* 清除数据长度 */
				// Uart_Tx_SequenceOutIndex = (Uart_Tx_SequenceOutIndex + 1u) % UART_TX_SEQUENCE_DEPTH;	/* 更新发送索引，循环使用队列 */
				if(RET_OK != UartIf_Sequence_GetReadyIndex_Tx(&Uart_Tx_SequenceOutIndex))	/* 没有找到处于就绪状态的队列 ，不需发送;如果找到了，就更新 */
				{
					Uart_Tx_SequenceEmptyFlag = STD_TRUE;	/* 发送队列空了 */
				}
				if(STD_TRUE == Uart_Tx_SequenceFullFlag)
				{
					Uart_Tx_SequenceFullFlag = STD_FALSE;
				}
			}
			else /* 总线状态忙碌，说明当前队列的数据正在发送，继续等待 */
			{
				/* do nothing */
			}
		}
	}
}


void UartIf_MainFunction_Rx(void)
{
	uint8 i = 0u;
	if(UART_SEQUENCE_READY == Uart_Rx_Sequence[Uart_Rx_SequenceOutIndex].State)
	{
		/* 处理接收到的数据 */
		Asw_Callback_Receive_Uart1(Uart_Rx_Sequence[Uart_Rx_SequenceOutIndex].DataPtr, Uart_Rx_Sequence[Uart_Rx_SequenceOutIndex].DataLength);
		/* 读取处理过程，最后清除队列状态*/
		Uart_Rx_Sequence[Uart_Rx_SequenceOutIndex].State = UART_SEQUENCE_IDLE;	/* 处理完成，标记当前队列为空闲状态 */
		Uart_Rx_Sequence[Uart_Rx_SequenceOutIndex].DataLength = 0u;	/* 清除数据长度 */
		Uart_Rx_SequenceOutIndex = (Uart_Rx_SequenceOutIndex + 1u) % UART_RX_SEQUENCE_DEPTH;	/* 更新读取索引，循环使用队列 */
	}
	/* 队列满了的话需要不断地检测是否恢复 */
	if(STD_TRUE == Uart_Rx_SequenceFullFlag)	/* 如果之前标记过队列满，说明现在有一个队列空闲了，可以继续接收数据了 */
	{
		for(i=0u;i<UART_RX_SEQUENCE_DEPTH;i++)
		{
			if(UART_SEQUENCE_IDLE == Uart_Rx_Sequence[i].State)	/* 找到一个空闲队列 */
			{
				Uart_Rx_SequenceInIndex = i;	/* 更新正在使用的队列索引 */
				Uart_Rx_SequenceFullFlag = STD_FALSE;	/* 清除队列满标志 */
				Uart_Rx_Sequence[Uart_Rx_SequenceInIndex].State = UART_SEQUENCE_BUSY; /* 初始化设置第一个队列为正在接收*/
				HAL_UART_Receive_DMA(&BOOT_UART_CHANNEL,Uart_Rx_Sequence[Uart_Rx_SequenceInIndex].DataPtr,UART_RX_SEQUENCE_SIZE);	
				break;
			}
		}
	}
}





/**********************End of files**********************/



