#pragma once
#include "pico/stdlib.h"


// Funções GPIO
void init_buzzer(uint buzzer_pin);
void ligar_buzzer(uint buzzer_pin);
void desligar_buzzer(uint buzzer_pin);
void buzzer_on(uint buzzer_pin, float frequency, uint duration_ms);

// Funções PWM
void init_pwm_buzzer(uint buzzer_pin);
void buzzer_pwm_on(uint buzzer_pin, float freq_hz, float duty_cycle);
void buzzer_pwm_off(uint buzzer_pin);

// SONS PADRÃO
void tocar_bipe_curto(uint buzzer_pin);
void tocar_beep_intermitente(uint buzzer_pin);
void tocar_tom_continuo(uint buzzer_pin);
void tocar_beep_freqMenor(uint buzzer_pin);
void parar_buzzer(uint buzzer_pin);
void tocar_dois_bipes_curtos(uint buzzer_pin);