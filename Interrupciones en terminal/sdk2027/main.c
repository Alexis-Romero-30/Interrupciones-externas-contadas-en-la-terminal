
#include "stm32f10x.h"
#include "leds.h"
#include "exti.h"
#include "usart.h"

int main(void) {

    init_led_pin();
    init_exti_pin();
    USART1_Config();
    init_exti();

    while(1) {
        exti_procesar();
    }
}
