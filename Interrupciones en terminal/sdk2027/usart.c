
#include "usart.h"
#include "system_stm32f10x.h"

// Configuracion del USART1
void USART1_Config(void) {

    uint32_t pclk2;
    uint32_t prescaler;

    // 1. Habilitar reloj USART1, GPIOA y AFIO
    RCC->APB2ENR |= (1 << 14) |
                    (1 << 2)  |
                    (1 << 0);

    // 2. Limpiar configuracion PA9 y PA10
    GPIOA->CRH &= ~((0xF << 4) |
                    (0xF << 8));

    // 3. PA9 como TX, alternativa Push-Pull
    GPIOA->CRH |= (0xB << 4);

    // 4. PA10 como RX, entrada flotante
    GPIOA->CRH |= (0x4 << 8);

    // 5. Deshabilitar USART1 para configurarlo
    USART1->CR1 = 0;
    USART1->CR2 = 0;
    USART1->CR3 = 0;

    // 6. Obtener frecuencia de PCLK2
    SystemCoreClockUpdate();

    pclk2 = SystemCoreClock;

    prescaler = (RCC->CFGR >> 11) & 0x7;

    if (prescaler >= 4) {
        pclk2 >>= (prescaler - 3);
    }

    // 7. Configurar 9600 baudios
    USART1->BRR = (pclk2 + 4800) / 9600;

    // 8. Habilitar USART1, TX y RX
    USART1->CR1 = (1 << 13) |
                  (1 << 3)  |
                  (1 << 2);
}

// Enviar un caracter
void USART1_EnviarByte(uint8_t dato) {

    // Esperar registro de transmision libre
    while (!(USART1->SR & (1 << 7)));

    // Enviar dato
    USART1->DR = dato;
}

// Recibir un caracter
uint8_t USART1_RecibirByte(void) {

    // Esperar hasta recibir un dato
    while (!(USART1->SR & (1 << 5)));

    // Retornar caracter recibido
    return (uint8_t)(USART1->DR & 0xFF);
}

// Enviar una cadena de caracteres
void USART1_EnviarCadena(const char *cadena) {

    while (*cadena != '\0') {

        USART1_EnviarByte((uint8_t)*cadena);

        cadena++;
    }
}

// Enviar un numero entero a RealTerm
void USART1_EnviarNumero(uint32_t numero) {

    char buffer[11];
    uint8_t i = 0;

    // Si el numero es cero
    if (numero == 0) {

        USART1_EnviarByte('0');
        return;
    }

    // Convertir numero a ASCII
    while (numero > 0) {

        buffer[i++] = (char)('0' + (numero % 10));

        numero /= 10;
    }

    // Enviar caracteres en orden correcto
    while (i > 0) {

        i--;

        USART1_EnviarByte((uint8_t)buffer[i]);
    }
}

// Recibir un caracter y devolverlo
void USART1_Eco(void) {

    uint8_t dato;

    dato = USART1_RecibirByte();

    USART1_EnviarByte(dato);
}
