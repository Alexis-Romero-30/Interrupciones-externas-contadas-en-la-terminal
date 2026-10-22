
#include "stm32f10x.h"
#include "system_stm32f10x.h"
#include "exti.h"
#include "leds.h"
#include "usart.h"

// Variables para registrar interrupciones
volatile uint32_t contador_interrupciones = 0;
volatile uint8_t interrupcion_pendiente = 0;

// Retardo en milisegundos usando SysTick
static void esperar_ms(uint32_t ms) {

    uint32_t recarga;

    recarga = (SystemCoreClock / 1000U) - 1U;

    SysTick->LOAD = recarga;
    SysTick->CTRL = (1U << 2) | (1U << 0);

    while (ms > 0) {

        SysTick->VAL = 0;

        while (!(SysTick->CTRL & (1U << 16))) {
        }

        ms--;
    }

    SysTick->CTRL = 0;
}

// Configurar PA0 como entrada Pull-Up
void init_exti_pin(void) {

    // Habilitar reloj GPIOA y AFIO
    RCC->APB2ENR |= (1 << 2) | (1 << 0);

    // Limpiar configuracion PA0
    GPIOA->CRL &= ~(0xF << 0);

    // Configurar PA0 como entrada Pull-Up
    GPIOA->CRL |= (0x8 << 0);

    // Activar Pull-Up interno
    GPIOA->ODR |= (1 << 0);
}

// Configurar interrupcion EXTI0
void init_exti(void) {

    // Deshabilitar EXTI0 temporalmente
    EXTI->IMR &= ~(1 << 0);

    // Seleccionar PA0 para EXTI0
    AFIO->EXTICR[0] &= ~(0xF << 0);

    // Deshabilitar flanco ascendente
    EXTI->RTSR &= ~(1 << 0);

    // Habilitar flanco descendente
    EXTI->FTSR |= (1 << 0);

    // Limpiar bandera pendiente
    EXTI->PR = (1 << 0);

    // Configurar NVIC
    NVIC_ClearPendingIRQ(EXTI0_IRQn);
    NVIC_SetPriority(EXTI0_IRQn, 2);
    NVIC_EnableIRQ(EXTI0_IRQn);

    // Habilitar interrupcion EXTI0
    EXTI->IMR |= (1 << 0);
}

// Rutina de servicio de interrupcion
void EXTI0_IRQHandler(void) {

    if (EXTI->PR & (1 << 0)) {

        // Bloquear nuevas interrupciones
        EXTI->IMR &= ~(1 << 0);

        // Limpiar bandera pendiente
        EXTI->PR = (1 << 0);

        // Registrar evento pendiente
        interrupcion_pendiente = 1;
    }
}

// Procesar interrupciones y mostrar contador
void exti_procesar(void) {

    if (interrupcion_pendiente == 1) {

        interrupcion_pendiente = 0;

        // Antirrebote de 30 ms
        esperar_ms(30);

        // Verificar que PA0 siga en bajo
        if ((GPIOA->IDR & (1 << 0)) == 0) {

            // Incrementar contador
            contador_interrupciones++;

            // Cambiar estado del LED PC13
            led_on_off();

            // Mostrar mensaje solo la primera vez
            if (contador_interrupciones == 1) {

                USART1_EnviarCadena(
                    "\r\nInterrupcion detectada. Total: "
                );
            }

            // Enviar numero de interrupciones
            USART1_EnviarNumero(
                contador_interrupciones
            );

            USART1_EnviarCadena("\r\n");

            // Esperar a que se suelte el boton
            do {

                while ((GPIOA->IDR & (1 << 0)) == 0) {
                }

                // Antirrebote al liberar
                esperar_ms(30);

            } while ((GPIOA->IDR & (1 << 0)) == 0);
        }

        // Limpiar solicitudes pendientes
        EXTI->PR = (1 << 0);
        NVIC_ClearPendingIRQ(EXTI0_IRQn);

        // Habilitar nuevamente EXTI0
        EXTI->IMR |= (1 << 0);
    }
}
