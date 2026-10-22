
#ifndef USART_H
#define USART_H

#include "stm32f10x.h"

// Configuracion USART1
void USART1_Config(void);

// Transmision de un caracter
void USART1_EnviarByte(uint8_t dato);

// Recepcion de un caracter
uint8_t USART1_RecibirByte(void);

// Transmision de cadenas
void USART1_EnviarCadena(const char *cadena);

// Transmision de numeros enteros
void USART1_EnviarNumero(uint32_t numero);

// Recepcion y retransmision de caracteres
void USART1_Eco(void);

#endif
