#include "string.h"
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_usart.h>
#include <uart.h>


#if _UART_PORT == 1
	#define UART_PORT         USART1
	#define UART_TX_PIN       GPIO_Pin_9    // PA9  (USART1_TX)
	#define UART_RX_PIN       GPIO_Pin_10   // PA10 (USART1_RX)
	#define UART_GPIO_PORT_TX GPIOA
	#define UART_GPIO_PORT_RX UART_GPIO_PORT_TX
#elif _UART_PORT == 2
	#define UART_PORT         USART2
	#define UART_TX_PIN       GPIO_Pin_2    // PA2 (USART2_TX)
	#define UART_RX_PIN       GPIO_Pin_3    // PA3 (USART2_RX)
	#define UART_GPIO_PORT_TX GPIOA
	#define UART_GPIO_PORT_RX UART_GPIO_PORT_TX
#elif _UART_PORT == 3
	#define UART_PORT         USART3
	#define UART_TX_PIN       GPIO_Pin_10    // PB10 (USART3_TX)
	#define UART_RX_PIN       GPIO_Pin_11    // PB11 (USART3_RX)
	#define UART_GPIO_PORT_TX GPIOB
	#define UART_GPIO_PORT_RX UART_GPIO_PORT_TX
#elif _UART_PORT == 4
	#define UART_PORT         UART4
	#define UART_TX_PIN       GPIO_Pin_10    // PC10 (UART4_TX)
	#define UART_RX_PIN       GPIO_Pin_11    // PC11 (UART4_RX)
	#define UART_GPIO_PORT_TX GPIOC
	#define UART_GPIO_PORT_RX UART_GPIO_PORT_TX
#elif _UART_PORT == 5
	#define UART_PORT         UART5
	#define UART_TX_PIN       GPIO_Pin_12    // PC12 (UART5_TX)
	#define UART_RX_PIN       GPIO_Pin_2     // PD2  (UART5_RX)
	#define UART_GPIO_PORT_TX GPIOC
	#define UART_GPIO_PORT_RX GPIOD
#endif

void UART_Init(void) {
	GPIO_InitTypeDef PORT;

	// U(S)ART init
#if _UART_PORT == 1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);
#elif _UART_PORT == 2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
#elif _UART_PORT == 3
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
#elif _UART_PORT == 4
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
#elif _UART_PORT == 5
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
#endif

	PORT.GPIO_Pin = UART_TX_PIN;
	PORT.GPIO_Speed = GPIO_Speed_50MHz;
	PORT.GPIO_Mode = GPIO_Mode_AF_PP; // TX as AF with Push-Pull
	GPIO_Init(UART_GPIO_PORT_TX,&PORT);
	PORT.GPIO_Pin = UART_RX_PIN;
	PORT.GPIO_Speed = GPIO_Speed_50MHz;
	PORT.GPIO_Mode = GPIO_Mode_IN_FLOATING; // RX as in without pull-up
	GPIO_Init(UART_GPIO_PORT_RX,&PORT);

	USART_InitTypeDef UART;
	UART.USART_BaudRate = 115200;
	UART.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // No flow control
	UART.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // RX+TX mode
	UART.USART_Parity = USART_Parity_No; // No parity check
	UART.USART_StopBits = USART_StopBits_1; // 1 stop bit
	UART.USART_WordLength = USART_WordLength_8b; // 8-bit frame
	USART_Init(UART_PORT,&UART);
	USART_Cmd(UART_PORT,ENABLE);
}

void UART_SendChar(char ch) {
	while (!USART_GetFlagStatus(UART_PORT,USART_FLAG_TC)); // wait for "Transmission Complete" flag cleared
	USART_SendData(UART_PORT,ch);
}

void UART_SendHex8(uint16_t num) {
	UART_SendChar(HEX_CHARS[(num >> 4)   % 0x10]);
	UART_SendChar(HEX_CHARS[(num & 0x0f) % 0x10]);
}

void UART_SendHex16(uint16_t num) {
	uint8_t i;
	for (i = 12; i > 0; i -= 4) UART_SendChar(HEX_CHARS[(num >> i) % 0x10]);
	UART_SendChar(HEX_CHARS[(num & 0x0f) % 0x10]);
}

void UART_SendHex32(uint32_t num) {
	uint8_t i;
	for (i = 28; i > 0; i -= 4)	UART_SendChar(HEX_CHARS[(num >> i) % 0x10]);
	UART_SendChar(HEX_CHARS[(num & 0x0f) % 0x10]);
}

void UART_SendStr(char *str) {
	while (*str) UART_SendChar(*str++);
}

void UART_SendBuf(char *buf, uint16_t bufsize) {
	uint16_t i;
	for (i = 0; i < bufsize; i++) UART_SendChar(*buf++);
}

void UART_SendBufPrintable(char *buf, uint16_t bufsize, char subst) {
	uint16_t i;
	char ch;
	for (i = 0; i < bufsize; i++) {
		ch = *buf++;
		UART_SendChar(ch > 32 ? ch : subst);
	}
}

void UART_SendBufHex(char *buf, uint16_t bufsize) {
	uint16_t i;
	char ch;
	for (i = 0; i < bufsize; i++) {
		ch = *buf++;
		UART_SendChar(HEX_CHARS[(ch >> 4)   % 0x10]);
		UART_SendChar(HEX_CHARS[(ch & 0x0f) % 0x10]);
	}
}

void UART_SendBufHexFancy(char *buf, uint16_t bufsize, uint8_t column_width, char subst) {
	uint16_t i = 0,len,pos;
	char buffer[column_width];

	while (i < bufsize) {
		// Line number
		UART_SendHex16(i);
		UART_SendChar(':'); UART_SendChar(' '); // Faster and less code than USART_SendStr(": ");

		// Copy one line
		if (i+column_width >= bufsize) len = bufsize - i; else len = column_width;
		memcpy(buffer,&buf[i],len);

		// Hex data
		pos = 0;
		while (pos < len) UART_SendHex8(buffer[pos++]);
		UART_SendChar(' ');

		// Raw data
		pos = 0;
		do UART_SendChar(buffer[pos] > 32 ? buffer[pos] : subst); while (++pos < len);
		UART_SendChar('\n');

		i += len;
	}
}
