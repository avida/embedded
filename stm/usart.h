
#include <stm32f10x_usart.h>

void InitializeUSART()
{
    USART_InitTypeDef usartConfig;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 |
                           RCC_APB2Periph_GPIOA |
                           RCC_APB2Periph_AFIO, ENABLE);
    USART_Cmd(USART1, ENABLE);

    usartConfig.USART_BaudRate = 2*115200;
    usartConfig.USART_WordLength = USART_WordLength_8b;
    usartConfig.USART_StopBits = USART_StopBits_1;
    usartConfig.USART_Parity = USART_Parity_No;
    usartConfig.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usartConfig.USART_HardwareFlowControl =
         USART_HardwareFlowControl_None;
    USART_Init(USART1, &usartConfig);

    GPIO_InitTypeDef gpioConfig;

    //PA9 = USART1.TX => Alternative Function Output
    gpioConfig.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioConfig.GPIO_Pin = GPIO_Pin_9;
    gpioConfig.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &gpioConfig);

    //PA10 = USART1.RX => Input
    gpioConfig.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpioConfig.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &gpioConfig);
}

unsigned char USART_ReadByteSync(USART_TypeDef *USARTx)
{
    while ((USARTx->SR & USART_SR_RXNE) == 0)
    {
    }

    return (unsigned char)USART_ReceiveData(USARTx);
}

void sendByte(USART_TypeDef *USARTx, char byte)
{
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
	USART_SendData(USARTx, byte);
}
void sendString(char* string)
{
	while(*string)
	{
		sendByte(USART1, *string);
		string ++;
	}
}

