#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"


// NÚMERO DE LEDS
#define NUM_PIXELS 25

// PINO DA MATRIZ DE LED
#define OUT_PIN 7


// FUNÇÕES
void setup_PIO();
uint32_t matrix_rgb(double b, double r, double g);
void drawMatrix(uint cor);
void apagarMatriz();
bool isCentroMatriz(int i);
void desenharCorNaMatriz(float r, float g, float b);
void atualizarScannerHorizontal();
void animacao_alarme_x();
void animacao_alarme_exclamacao();
void animacao_radar();