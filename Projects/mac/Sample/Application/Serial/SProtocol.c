/*********************************************************************************************************
*FileName:	SProtocol.c
*Processor: zigbee RF MCU
*Complier:	IAR |KEIL |AVR GCC|HT-TECH
*Company:   
**********************************************************************************************************
*Author			Date				Comment
*LXP			2014/3/11			Original
*********************************************************************************************************/
#include "Serial.h"
#include "hal_uart.h"
#include "OSAL.h"
/********************************************************************************************************

********************************************************************************************************/
static uint8 *ComRxBuf[USE_MAX_UART];
/********************************************************************************************************

********************************************************************************************************/
static uint8 Len[USE_MAX_UART];
static uint8 RxLen[USE_MAX_UART];
static uint8 RxCrc[USE_MAX_UART];
static uint8 AddCrc[USE_MAX_UART];
static uint8 dirCmd[USE_MAX_UART];
static uint8 RxStep[USE_MAX_UART];
/********************************************************************************************************

********************************************************************************************************/
void Serial_Init(void)
{
    uint8 i;
    
    for(i=0;i<USE_MAX_UART;i++)
    {
        Len[i]      = 0;
        RxLen[i]    = 0;
        RxCrc[i]    = 0;
        AddCrc[i]   = 0;
        dirCmd[i]   = 0;
        RxStep[i]   = COM_DEBUG_STEP_HEAD;
        
        ComRxBuf[i] = (uint8*)osal_msg_allocate(COM_RX_BUFFTER_SIZE);
        if(ComRxBuf[i] == NULL)
        {
            while(1);
        }
    }
}
/********************************************************************************************************
UINT8 dir,  传输数据方向
UINT8 len,  传输数据长度
UINT8 *pBuf 传输数据缓存
装载串行发送数据
********************************************************************************************************/
void Serial_vLoadingTx(uint8 dir,uint8 len,uint8 *pBuf)
{
    uint8 i,n,crc=0;
    uint8 *ComTxBuf = NULL;
    
    if((dir&0xF0) == 0x00)
    {
        n = 6+(len<<1)+1;
    }
    else
    {
        n = 4+len+1;
    }

    ComTxBuf = (uint8*)osal_msg_allocate(n);
    if(ComTxBuf != NULL)
    {
        ComTxBuf[0]= COM_DEBUG_LABEL_HEAD;
        ComTxBuf[1]= dir;
        
        /**< 通信协议模式选择 */
        if(dir&0xF0 == 0x00)
        {
            ComTxBuf[2]= (len<<1)&0x0F;
            ComTxBuf[3]= (len<<1)&0xF0;

            crc = 0x00;
            for(i=0;i<len;i++)
            {
                ComTxBuf[4+(i<<1)]= (pBuf[i]&0x0F);
                ComTxBuf[5+(i<<1)]= (pBuf[i]&0xF0);

                crc ^=	pBuf[i];
            }
            ComTxBuf[4+(len<<1)] = 	crc&0x0F;
            ComTxBuf[5+(len<<1)] = 	crc&0xF0;

            ComTxBuf[6+(len<<1)] = 	COM_DEBUG_LABEL_END;
            
            len = 6+(len<<1)+1;
        }
        else
        {
            ComTxBuf[2]= len;

            crc = 0x00;
            for(i=0;i<len;i++)
            {
                ComTxBuf[3+i]= pBuf[i];
                crc ^=	pBuf[i];
            }
            ComTxBuf[3+len] = crc;

            ComTxBuf[4+len] = COM_DEBUG_LABEL_END;

            len = 4+len+1;
        }
        
        if((dir&0x0F) == 0x02)
        {
            HalUARTWrite(0,ComTxBuf,len);
        }
        else
        {
            HalUARTWrite(1,ComTxBuf,len);
        }
        
        osal_msg_deallocate((uint8 *)ComTxBuf);
    }
}
/********************************************************************************************************/
void Serial_vRx(uint8 port,uint8 len,uint8 *pBuf)
{
	uint8 i = port;
    uint8 k,buf;

	//for(i=0;i<USE_MAX_UART;i++)
	{
		/** 读取主串口缓冲区中的数据 while */
		//while(HalUARTRead(0,&buf,1) > 0)
        for(k=0;k<len;k++)  
		{
            buf = *pBuf++;
                
			switch(RxStep[i])
			{
				case COM_DEBUG_STEP_HEAD:		/**< 数据头标示 */
					 if(buf == COM_DEBUG_LABEL_HEAD)
					 {
						Len[i] = 0;
						AddCrc[i] = 0;
						RxCrc[i]  = 0;
						RxLen[i]  = 0;
						dirCmd[i] = 0;
						RxStep[i] = COM_DEBUG_STEP_CMD;
					 }
					 else
					 {
						RxStep[i] = COM_DEBUG_STEP_HEAD;
					 }
					break;
				case COM_DEBUG_STEP_CMD:		/**< 数据命令标示 */
					switch(buf)
					{
						case 0x00:
						case 0x01:
						case 0x02:
						case 0x03:
						case 0x04:
						case 0x05:
						case 0x06:
						case 0x07:
						case 0x08:
						case 0x09:
						case 0x0A:
						case 0x0B:
						case 0x0C:
						case 0x0D:
						case 0x0E:
						case 0x0F:
						
							dirCmd[i] = buf;
							RxStep[i] = COM_DEBUG_STEP_LEN1;
							break;
                        case 0xF0:
                        case 0xF1:
                        case 0xF2:
                        case 0xF3:
                        case 0xF4:
                        case 0xF5:
                        case 0xF6:
                        case 0xF7:
                        case 0xF8:
                        case 0xF9:
                        case 0xFA:
                        case 0xFB:
                        case 0xFC:
                        case 0xFD:
                        case 0xFE:
                        case 0xFF:
                        
							dirCmd[i] = buf;
							RxStep[i] = COM_DEBUG_STEP_LEN;
							break;
						case COM_DEBUG_LABEL_HEAD:
							RxStep[i] = COM_DEBUG_STEP_CMD;
							break;
						case COM_DEBUG_LABEL_END:
						default:
							RxStep[i] = COM_DEBUG_STEP_HEAD;
							break;
					}
					break;
				case COM_DEBUG_STEP_LEN:
				        RxLen[i] = buf;
						if(RxLen[i] == 0x00)		/**< 判断数据长度是否为零 */
						{
							RxStep[i] = COM_DEBUG_STEP_END;
						}
						else
						{
							RxStep[i] = COM_DEBUG_STEP_DATA;
						}
				    break;
				case COM_DEBUG_STEP_LEN1:		    /**< 数据长度低4位 */
					if(!(buf&0xF0))
					{
						RxLen[i]  = buf;
						RxStep[i] = COM_DEBUG_STEP_LEN2;
					}
					else
					{
						switch(buf)
						{
							case COM_DEBUG_LABEL_HEAD:
								RxStep[i] = COM_DEBUG_STEP_CMD;
								break;
							case COM_DEBUG_LABEL_END:
							default:
								RxStep[i] = COM_DEBUG_STEP_HEAD;
								break;
						}
					}
					break;
				case COM_DEBUG_STEP_LEN2:			/**< 数据长度高4位 */
					if(!(buf&0x0F))
					{
						RxLen[i]  |= buf;
						if(RxLen[i] == 0x00)		/**< 判断数据长度是否为零 */
						{
							RxStep[i] = COM_DEBUG_STEP_END;
						}
						else if(RxLen[i]&0x01)		/**< 判断数据长度是否为奇数 */
						{
							RxStep[i] = COM_DEBUG_LABEL_HEAD;
						}
						else
						{
							RxStep[i] = COM_DEBUG_STEP_DATA1;
						}
					}
					else
					{
						switch(buf)
						{
							case COM_DEBUG_LABEL_HEAD:
								RxStep[i] = COM_DEBUG_STEP_CMD;
								break;
							case COM_DEBUG_LABEL_END:
							default:
								RxStep[i] = COM_DEBUG_STEP_HEAD;
								break;
						}
					}
					break;
			    case COM_DEBUG_STEP_DATA:
			    
                    ComRxBuf[i][Len[i]] = buf;
                    AddCrc[i] ^=ComRxBuf[i][Len[i]];
                    Len[i]++;
                    if(RxLen[i] == Len[i])
                    {
                        RxStep[i] = COM_DEBUG_STEP_CRC;
                    }
					break;
				case COM_DEBUG_STEP_DATA1:		/**< 数据 */
				case COM_DEBUG_STEP_DATA2:		/**< 数据 */
					if(!(Len[i]&0x01))			/**< 判断是否是数据低4位 */
					{
						if(!(buf&0xF0)) 		/**< 数据低4位 */
						{
							ComRxBuf[i][Len[i]>>1] = buf;
							Len[i]++;
						}
						else
						{
							switch(buf)
							{
								case COM_DEBUG_LABEL_HEAD:
									RxStep[i] = COM_DEBUG_STEP_CMD;
									break;
								case COM_DEBUG_LABEL_END:
								default:
									RxStep[i] = COM_DEBUG_STEP_HEAD;
									break;
							}
						}
					}
					else						/**< 判断是否是数据高4位 */
					{
						if(!(buf&0x0F)) 		/**< 数据高4位 */
						{
							ComRxBuf[i][Len[i]>>1] |= buf;
							AddCrc[i] ^=ComRxBuf[i][Len[i]>>1];
							Len[i]++;
							if(RxLen[i] == Len[i])
							{
								RxStep[i] = COM_DEBUG_STEP_CRC1;
							}
						}
						else
						{
							switch(buf)
							{
								case COM_DEBUG_LABEL_HEAD:
									RxStep[i] = COM_DEBUG_STEP_CMD;
									break;
								case COM_DEBUG_LABEL_END:
								default:
									RxStep[i] = COM_DEBUG_STEP_HEAD;
									break;
							}
						}
					}
					break;
			    case COM_DEBUG_STEP_CRC:
			    
                    RxCrc[i] = buf;
                    RxStep[i] = COM_DEBUG_STEP_END;
			        break;
				case COM_DEBUG_STEP_CRC1:
					if(!(buf&0xF0))
					{
						RxCrc[i] = buf;
						RxStep[i] = COM_DEBUG_STEP_CRC2;
					}
					else
					{
						switch(buf)
						{
							case COM_DEBUG_LABEL_HEAD:
								RxStep[i] = COM_DEBUG_STEP_CMD;
								break;
							case COM_DEBUG_LABEL_END:
							default:
								RxStep[i] = COM_DEBUG_STEP_HEAD;
								break;
						}
					}
					break;
				case COM_DEBUG_STEP_CRC2:
					if(!(buf&0x0F))
					{
						RxCrc[i] |= buf;
						RxStep[i] = COM_DEBUG_STEP_END;
						RxLen[i] >>=1;
					}
					else
					{
						switch(buf)
						{
							case COM_DEBUG_LABEL_HEAD:
								RxStep[i] = COM_DEBUG_STEP_CMD;
								break;
							case COM_DEBUG_LABEL_END:
							default:
								RxStep[i] = COM_DEBUG_STEP_HEAD;
								break;
						}
					}
					break;
				case COM_DEBUG_STEP_END:
					switch(buf)
					{
						case COM_DEBUG_LABEL_HEAD:
							RxStep[i] = COM_DEBUG_STEP_CMD;
							break;
						case COM_DEBUG_LABEL_END:

							if(AddCrc[i] == RxCrc[i])		/**处理接收的数据	*/
							{
                                Serial_vRxSplit(dirCmd[i],(RxLen[i]),&ComRxBuf[i][0]);
							}
						default:
							RxStep[i] = COM_DEBUG_STEP_HEAD;
							break;
					}
					break;
				default:
					RxStep[i] = COM_DEBUG_STEP_HEAD;
					break;
			}
		}
	}
}

/********************************************************************************************************

********************************************************************************************************/
