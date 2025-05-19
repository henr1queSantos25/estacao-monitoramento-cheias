#include "led_5x5.h"

// ARQUIVO .pio
#include "build/ws2812.pio.h"

// INICIALIZAÇÃO E CONFIGURAÇÃO DO PIO
void setup_PIO() {
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &ws2812_program);
    uint sm = pio_claim_unused_sm(pio, true);
    ws2812_program_init(pio, sm, offset, OUT_PIN);
    matrix_rgb(0.0, 0.0, 0.0); // Inicializa a matriz com todos os LEDs apagados
}

// FUNÇÃO PARA ENVIAR O VALOR RGB PARA A MATRIZ
uint32_t matrix_rgb(double r, double g, double b) {
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// ACENDE TODOS OS LEDS COM UMA COR ESPECÍFICA
void drawMatrix(uint cor) {
    uint32_t valor_led;

    /*
    "0. Azul", 
    "1. Verde", 
    "2. Vermelho",     
    "3. Desligado"
    */

    switch (cor)
    {
    case 0:
        for (int16_t i = 0; i < NUM_PIXELS; i++) {
            valor_led = matrix_rgb(0.0, 0.0, 0.2);
            pio_sm_put_blocking(pio0, 0, valor_led);
        }
        break;
    case 1:
        for (int16_t i = 0; i < NUM_PIXELS; i++) {
            valor_led = matrix_rgb(0.0, 0.2, 0.0);
            pio_sm_put_blocking(pio0, 0, valor_led);
        }
        break;
    case 2:
        for (int16_t i = 0; i < NUM_PIXELS; i++) {
            valor_led = matrix_rgb(0.2, 0.0, 0.0);
            pio_sm_put_blocking(pio0, 0, valor_led);
        }
        break;
    case 3:
        for (int16_t i = 0; i < NUM_PIXELS; i++) {
            valor_led = matrix_rgb(0.0, 0.0, 0.0);
            pio_sm_put_blocking(pio0, 0, valor_led);
        }
        break;
    }
    
    

}

void apagarMatriz() {
    for (int i = 0; i < NUM_PIXELS; i++) {
        uint32_t valor_led = matrix_rgb(0.0, 0.0, 0.0);
        pio_sm_put_blocking(pio0, 0, valor_led);
    }
}

// DESENHO NOS LEDS CENTRAIS
bool isCentroMatriz(int i) {
    return i == 6 || i == 7 || i == 8 || i == 11 || i == 12 || i == 13 || i == 16 || i == 17 || i == 18;
}
void desenharCorNaMatriz(float r, float g, float b) {
    for (int i = 0; i < NUM_PIXELS; i++) {
        uint32_t valor_led;

        valor_led = isCentroMatriz(i) ? matrix_rgb(r, g, b) : matrix_rgb(0.0, 0.0, 0.0);
        
        pio_sm_put_blocking(pio0, 0, valor_led);
    }
}

// SCANNER HORIZONTAL
int linha_atual = 0;
bool descendo = true;
absolute_time_t ultimo_update_scanner;

void atualizarScannerHorizontal() {
    const uint32_t cor = matrix_rgb(0.0, 0.01, 0.0); // Verde médio
    const uint32_t apagado = matrix_rgb(0.0, 0.0, 0.0);

    // Só atualiza a cada 150ms
    if (!time_reached(ultimo_update_scanner)) return;
    ultimo_update_scanner = make_timeout_time_ms(150);

    for (int i = 0; i < NUM_PIXELS; i++) {
        if (i / 5 == linha_atual)
            pio_sm_put_blocking(pio0, 0, cor);
        else
            pio_sm_put_blocking(pio0, 0, apagado);
    }

    if (descendo) {
        linha_atual++;
        if (linha_atual >= 5) {
            linha_atual = 3; // volta a subir
            descendo = false;
        }
    } else {
        linha_atual--;
        if (linha_atual < 0) {
            linha_atual = 1; // volta a descer
            descendo = true;
        }
    }

}

// ANIMAÇÃO DO ALARME (X)
absolute_time_t proximo_flash_x;
bool x_aceso = false;

void animacao_alarme_x() {
    if (!time_reached(proximo_flash_x)) return;

    proximo_flash_x = make_timeout_time_ms(500); // Pisca a cada 200 ms
    x_aceso = !x_aceso;

    uint32_t cor = x_aceso ? matrix_rgb(0.01, 0.0, 0.0) : matrix_rgb(0.0, 0.0, 0.0); // vermelho / apagado

    for (int i = 0; i < NUM_PIXELS; i++) {
        // Índices que formam o X
        if (i == 0 || i == 4 || i == 6 || i == 8 || i == 12 || i == 16 || i == 18 || i == 20 || i == 24)
            pio_sm_put_blocking(pio0, 0, cor);
        else
            pio_sm_put_blocking(pio0, 0, matrix_rgb(0.0, 0.0, 0.0)); // Apaga os demais
    }
}

// ANIMAÇÃO DO ALARME (!)
absolute_time_t proximo_flash_exclamacao;
bool exclamacao_acesa = false;

void animacao_alarme_exclamacao() {
    if (!time_reached(proximo_flash_exclamacao)) return;

    proximo_flash_exclamacao = make_timeout_time_ms(500); // Pisca a cada 200ms
    exclamacao_acesa = !exclamacao_acesa;

    uint32_t cor = exclamacao_acesa ? matrix_rgb(0.01, 0.0, 0.0) : matrix_rgb(0.0, 0.0, 0.0); // vermelho / apagado

    for (int i = 0; i < NUM_PIXELS; i++) {
        // Índices que formam o ponto de exclamação (!)
        // A linha vertical do ponto de exclamação
        if (i == 2 || i == 17 || i == 12 || 
            // O ponto do ponto de exclamação
            i == 22) 
        {
            pio_sm_put_blocking(pio0, 0, cor);
        } else {
            pio_sm_put_blocking(pio0, 0, matrix_rgb(0.0, 0.0, 0.0)); // Apaga os demais
        }
    }
}

// Adicione estas variáveis globais
absolute_time_t ultimo_update_radar;
int posicao_radar = 0;

void animacao_radar() {
    const uint32_t cor = matrix_rgb(0.0, 0.01, 0.0);  // Verde suave
    const uint32_t apagado = matrix_rgb(0.0, 0.0, 0.0);

    // Atualiza a cada 150ms
    if (!time_reached(ultimo_update_radar)) return;
    ultimo_update_radar = make_timeout_time_ms(150);

    // Arrays com as posições dos LEDs para cada posição do radar
    const int radar_positions[][5] = {
        {0, 1, 2, 3, 4},           // Posição 0 (base)
        {4, 5, 6, 7, 8},           // Posição 1 (direita-baixo)
        {8, 13, 18, 23, 24},       // Posição 2 (direita)
        {24, 23, 22, 21, 20},      // Posição 3 (topo)
        {20, 15, 10, 5, 0}         // Posição 4 (esquerda)
    };

    // Limpa a matriz
    for (int i = 0; i < NUM_PIXELS; i++) {
        pio_sm_put_blocking(pio0, 0, apagado);
    }

    // Acende os LEDs da posição atual do radar
    for (int i = 0; i < 5; i++) {
        int led_pos = radar_positions[posicao_radar][i];
        for (int j = 0; j < NUM_PIXELS; j++) {
            if (j == led_pos) {
                pio_sm_put_blocking(pio0, 0, cor);
            } else {
                pio_sm_put_blocking(pio0, 0, apagado);
            }
        }
    }

    // Avança para a próxima posição do radar
    posicao_radar = (posicao_radar + 1) % 5;
}





