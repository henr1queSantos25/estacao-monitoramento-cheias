#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hardware/pwm.h"
#include "buzzer.h"

// Macros auxiliares
#define BUZZER_PWM_SLICE(pin) pwm_gpio_to_slice_num(pin)

// Inicialização simples do pino do buzzer
void init_buzzer(uint buzzer_pin) {
    gpio_init(buzzer_pin);
    gpio_set_dir(buzzer_pin, GPIO_OUT);
}

// Liga o buzzer (nível alto)
void ligar_buzzer(uint buzzer_pin) {
    gpio_put(buzzer_pin, 1);
}

// Desliga o buzzer (nível baixo)
void desligar_buzzer(uint buzzer_pin) {
    gpio_put(buzzer_pin, 0);
}

// Gera som no buzzer por um tempo e frequência específicos (modo digital)
void buzzer_on(uint buzzer_pin, float frequency, uint duration_ms) {
    float period = 1.0f / frequency;
    float half_period_us = (period / 2) * 1e6f;
    uint cycles = (duration_ms * frequency) / 1000;

    for (uint i = 0; i < cycles; i++) {
        gpio_put(buzzer_pin, 1);
        sleep_us((uint)half_period_us);
        gpio_put(buzzer_pin, 0);
        sleep_us((uint)half_period_us);
    }
}



// Inicializa o PWM do buzzer
void init_pwm_buzzer(uint buzzer_pin) {
    gpio_set_function(buzzer_pin, GPIO_FUNC_PWM);
    uint slice = BUZZER_PWM_SLICE(buzzer_pin);
    pwm_set_clkdiv(slice, 4.0f);
    pwm_set_enabled(slice, true);
}

// Ativa o PWM do buzzer com frequência e duty cycle específicos
void buzzer_pwm_on(uint buzzer_pin, float freq_hz, float duty_cycle) {
    uint slice = BUZZER_PWM_SLICE(buzzer_pin);
    uint32_t clock_hz = 125000000;
    uint32_t wrap = clock_hz / (4 * freq_hz);
    if (wrap < 10) wrap = 10;
    if (wrap > 65535) wrap = 65535;

    pwm_set_wrap(slice, wrap);
    pwm_set_chan_level(slice, pwm_gpio_to_channel(buzzer_pin), wrap * duty_cycle);
    pwm_set_enabled(slice, true);
}

// Desativa o PWM do buzzer
void buzzer_pwm_off(uint buzzer_pin) {
    uint slice = BUZZER_PWM_SLICE(buzzer_pin);
    pwm_set_chan_level(slice, pwm_gpio_to_channel(buzzer_pin), 0);
    pwm_set_enabled(slice, false);
}

// SONS PADRÃO

void tocar_bipe_curto(uint buzzer_pin) {
    buzzer_pwm_on(buzzer_pin, 2000.0f, 0.5f);
    vTaskDelay(pdMS_TO_TICKS(150));
    buzzer_pwm_off(buzzer_pin);
    vTaskDelay(pdMS_TO_TICKS(850));
}

void tocar_beep_intermitente(uint buzzer_pin) {
    const uint total_duration = 900;
    const uint beep = 50, pausa = 50;
    uint tempo = 0;
    while (tempo + beep + pausa <= total_duration) {
        buzzer_pwm_on(buzzer_pin, 2000.0f, 0.5f);
        vTaskDelay(pdMS_TO_TICKS(beep));
        buzzer_pwm_off(buzzer_pin);
        vTaskDelay(pdMS_TO_TICKS(pausa));
        tempo += beep + pausa;
    }
}

void tocar_tom_continuo(uint buzzer_pin) {
    buzzer_pwm_on(buzzer_pin, 1000.0f, 0.5f);
    vTaskDelay(pdMS_TO_TICKS(500));
    buzzer_pwm_off(buzzer_pin);
    vTaskDelay(pdMS_TO_TICKS(1500));
}

void tocar_beep_freqMenor(uint buzzer_pin) {
    buzzer_pwm_on(buzzer_pin, 800.0f, 0.4f);
    vTaskDelay(pdMS_TO_TICKS(250));
    buzzer_pwm_off(buzzer_pin);
    vTaskDelay(pdMS_TO_TICKS(1750));
}

void parar_buzzer(uint buzzer_pin) {
    buzzer_pwm_off(buzzer_pin);
}

void tocar_dois_bipes_curtos(uint buzzer_pin) {
    // Primeiro bipe
    buzzer_pwm_on(buzzer_pin, 2000.0f, 0.5f);
    vTaskDelay(pdMS_TO_TICKS(200));
    buzzer_pwm_off(buzzer_pin);
    vTaskDelay(pdMS_TO_TICKS(100)); // Pequena pausa entre bipes

    // Segundo bipe
    buzzer_pwm_on(buzzer_pin, 2000.0f, 0.5f);
    vTaskDelay(pdMS_TO_TICKS(200));
    buzzer_pwm_off(buzzer_pin);
    vTaskDelay(pdMS_TO_TICKS(800)); // Pausa final
}