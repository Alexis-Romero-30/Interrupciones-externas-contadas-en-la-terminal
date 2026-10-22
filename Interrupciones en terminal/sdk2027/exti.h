
#ifndef EXTI_H
#define EXTI_H

#include "stm32f10x.h"

extern volatile uint32_t contador_interrupciones;
extern volatile uint8_t interrupcion_pendiente;

void init_exti_pin(void);
void init_exti(void);
void exti_procesar(void);

#endif
