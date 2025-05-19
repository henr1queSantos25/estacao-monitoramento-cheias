#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>
#include "hardware/adc.h"

// Bibliotecas personalizadas
#include "lib/led_5x5.h"
#include "lib/buzzer.h"
#include "lib/ledRGB.h"
#include "lib/ssd1306.h"
#include "lib/font.h"

// Definições de pinos e configurações
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define nivel_agua_pin 26
#define volume_chuva_pin 27
#define BUZZER 10

// Definição da estrutura de dados para monitoramento
typedef struct {
    uint16_t nivel_agua;
    uint16_t volume_chuva;
    bool alertaAtivo;
} DadosMonitoramento;

// Fila para comunicação entre tarefas
QueueHandle_t xQueueDadosMonitoramento;

void vTaskJoystick(void *pvParameters) {
    
    adc_gpio_init(nivel_agua_pin);
    adc_gpio_init(volume_chuva_pin);
    adc_init();
    
    DadosMonitoramento dadosMonitoramento;

    while (true) {
        adc_select_input(0); // GPIO 26 = ADC0
        dadosMonitoramento.nivel_agua = adc_read();

        adc_select_input(1); // GPIO 27 = ADC1
        dadosMonitoramento.volume_chuva = adc_read();

        dadosMonitoramento.nivel_agua = (dadosMonitoramento.nivel_agua * 100) / 4095; 
        dadosMonitoramento.volume_chuva = (dadosMonitoramento.volume_chuva * 100) / 4095; 

        if (dadosMonitoramento.nivel_agua >= 70 || dadosMonitoramento.volume_chuva >= 80) {
            dadosMonitoramento.alertaAtivo = true;
        } else {
            dadosMonitoramento.alertaAtivo = false;
        }

        // Envia os dados para a fila
        xQueueSend(xQueueDadosMonitoramento, &dadosMonitoramento, 0);

        vTaskDelay(pdMS_TO_TICKS(100)); 
    }
}


void vTaskDisplay(void *pvParameters) {
    ssd1306_t ssd;
    setup_I2C(I2C_PORT, I2C_SDA, I2C_SCL, 400 * 1000);
    setup_ssd1306(&ssd, endereco, I2C_PORT);

    DadosMonitoramento dadosMonitoramento;
    bool cor = false;
    char buffer[50];

    while (true) {
        
        if (xQueueReceive(xQueueDadosMonitoramento, &dadosMonitoramento, portMAX_DELAY) == pdTRUE) {
            ssd1306_fill(&ssd, false);
            ssd1306_rect(&ssd, 1, 1, 125, 62, !cor, cor); 
            
            if (!dadosMonitoramento.alertaAtivo) {
                snprintf(buffer, sizeof(buffer), "Nivel da agua");
                ssd1306_draw_string(&ssd, buffer, 10, 5);
    
                snprintf(buffer, sizeof(buffer), "%d%%", dadosMonitoramento.nivel_agua);
                ssd1306_draw_string(&ssd, buffer, 50, 18);
    
                ssd1306_line(&ssd, 2, 28, 123, 28, !cor);
    
                snprintf(buffer, sizeof(buffer), "Volume de chuva");
                ssd1306_draw_string(&ssd, buffer, 3, 32);
                snprintf(buffer, sizeof(buffer), "%d%%", dadosMonitoramento.volume_chuva);
                ssd1306_draw_string(&ssd, buffer, 50, 45);
            } else {
                ssd1306_rect(&ssd, 1, 1, 125, 62, !cor, cor); 
                snprintf(buffer, sizeof(buffer), "NIVEL DA AGUA");
                ssd1306_draw_string(&ssd, buffer, 10, 5);
    
                snprintf(buffer, sizeof(buffer), "%d%%", dadosMonitoramento.nivel_agua);
                ssd1306_draw_string(&ssd, buffer, 50, 18);
    
                ssd1306_line(&ssd, 2, 28, 123, 28, !cor);
    
                snprintf(buffer, sizeof(buffer), "VOLUME DE CHUVA");
                ssd1306_draw_string(&ssd, buffer, 3, 32);
                snprintf(buffer, sizeof(buffer), "%d%%", dadosMonitoramento.volume_chuva);
                ssd1306_draw_string(&ssd, buffer, 50, 45);
            }

            ssd1306_send_data(&ssd);
        }

    }

}

void vTaskLED(void *pvParameters) {
    setupLED(LED_GREEN);
    setupLED(LED_RED);
    setupLED(LED_BLUE);

    DadosMonitoramento dadosMonitoramento;

    while (true) {
        if (xQueueReceive(xQueueDadosMonitoramento, &dadosMonitoramento, portMAX_DELAY) == pdTRUE) {
            if (dadosMonitoramento.alertaAtivo) {
                setLeds(1, 0, 0); // Liga o LED vermelho
            } else {
                setLeds(0, 1, 0); // Liga o LED verde
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50)); // Aumentado o delay para melhor visualização
    }
}

void vTaskMatrizLED(void *pvParameters) {
    setup_PIO();


    DadosMonitoramento dadosMonitoramento;

    while (true) {
        if (xQueuePeek(xQueueDadosMonitoramento, &dadosMonitoramento, portMAX_DELAY) == pdTRUE) {
            if (dadosMonitoramento.nivel_agua >= 70 && dadosMonitoramento.volume_chuva >= 80) {
                animacao_alarme_x();
            } else if (dadosMonitoramento.nivel_agua >= 70) {
                animacao_alarme_exclamacao();
            } else if (dadosMonitoramento.volume_chuva >= 80) {
                animacao_alarme_exclamacao();
            } else {
                atualizarScannerHorizontal();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50)); // Aumentado o delay para melhor visualização
    }
}


void vTaskBuzzer(void *pvParameters) {
    init_pwm_buzzer(BUZZER);

    DadosMonitoramento dadosMonitoramento;

    while (true) {
        if (xQueuePeek(xQueueDadosMonitoramento, &dadosMonitoramento, portMAX_DELAY) == pdTRUE) {
            if(dadosMonitoramento.nivel_agua >= 70 && dadosMonitoramento.volume_chuva >= 80) {
                tocar_beep_intermitente(BUZZER);
            } else if (dadosMonitoramento.nivel_agua >= 70) {
                tocar_dois_bipes_curtos(BUZZER);
            } else if( dadosMonitoramento.volume_chuva >= 80) {
                tocar_beep_freqMenor(BUZZER);
            } else {
                parar_buzzer(BUZZER);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50)); // Aumentado o delay para melhor visualização
    }
}



int main() {
    stdio_init_all();

    // Inicializando a fila
    xQueueDadosMonitoramento = xQueueCreate(5, sizeof(DadosMonitoramento));

    // Tasks
    xTaskCreate(vTaskJoystick, "Task Joystick", 256, NULL, 2, NULL);
    xTaskCreate(vTaskDisplay, "Task Display", 512, NULL, 1, NULL);
    xTaskCreate(vTaskLED, "Task LED", 256, NULL, 1, NULL);
    xTaskCreate(vTaskMatrizLED, "Task Matriz LED", 256, NULL, 1, NULL);
    xTaskCreate(vTaskBuzzer, "Task Buzzer", 256, NULL, 1, NULL);


    vTaskStartScheduler();
}
