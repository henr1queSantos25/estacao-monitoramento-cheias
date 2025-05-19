# Estação Inteligente de Monitoramento de Cheias

A **Estação Inteligente de Monitoramento de Cheias** é um sistema embarcado desenvolvido para simular o monitoramento em tempo real de níveis de água e volume de chuva utilizando a plataforma **BitDogLab** com o microcontrolador **RP2040**. O projeto integra o sistema operacional **FreeRTOS** para gerenciamento de múltiplas tarefas concorrentes e utiliza filas para comunicação entre os componentes do sistema, gerando alertas visuais e sonoros quando situações de risco são detectadas.

---

## Funcionalidades Principais

- **Monitoramento em Tempo Real**: Simula a leitura de sensores de nível de água e volume de chuva por meio de entradas analógicas (joystick).
- **Sistema de Alerta Multi-sensorial**:
  - Indicações visuais através de LED RGB e matriz de LEDs 5x5.
  - Alertas sonoros com padrões distintos via buzzer.
  - Informações detalhadas no display OLED.
- **Modos de Operação Automáticos**:
  - **Modo Normal**: Ativo quando níveis estão dentro de limites seguros.
  - **Modo Alerta**: Acionado quando nível da água atinge ≥70% ou volume de chuva ≥80%.
- **Multitarefa com FreeRTOS**: Tarefas concorrentes comunicando-se exclusivamente por filas.
- **Processamento de Dados**: Normalização de leituras para escalas percentuais (0-100%).

---

## Tecnologias Utilizadas

- **Linguagem de Programação**: C  
- **Microcontrolador**: Raspberry Pi Pico (RP2040)  
- **Sistema Operacional**: FreeRTOS  
- **Componentes Utilizados**:
  - Display OLED SSD1306 128x64 (I2C)
  - LED RGB para indicação de estado
  - Matriz de LEDs 5x5 para alertas visuais dinâmicos
  - Buzzer com PWM para alertas sonoros
  - Joystick para simulação de sensores (ADC)
- **Bibliotecas**:
  - FreeRTOS para gerenciamento de tarefas e filas
  - Pico SDK para acesso ao hardware (GPIO, ADC, PWM e I2C)
  - Bibliotecas personalizadas para controle dos periféricos

---

## Como Funciona

### Aquisição de Dados
- O joystick simula dois sensores diferentes:
  - Um eixo representa o nível da água (0-100%)
  - O outro eixo representa o volume de chuva (0-100%)
- A leitura é feita via ADC e normalizada para valores percentuais.

### Processamento e Tomada de Decisão
- O sistema verifica constantemente se os valores ultrapassam limites predefinidos:
  - Nível da água ≥ 70%
  - Volume de chuva ≥ 80%
- Quando um ou ambos os limites são excedidos, o sistema entra em modo de alerta.

### Feedback e Alertas
- **Display OLED**:
  - Exibe valores atuais de nível de água e volume de chuva.
  - No modo normal: exibição em texto padrão.
  - No modo alerta: exibição em letras maiúsculas para maior visibilidade.

- **LED RGB**:
  - Verde no modo normal.
  - Vermelho no modo alerta.

- **Matriz de LEDs 5x5**:
  - Modo normal: animação de scanner horizontal.
  - Modo alerta por nível de água: símbolo de exclamação (!).
  - Modo alerta por volume de chuva: símbolo de exclamação (!).
  - Modo alerta por ambos: símbolo "X".

- **Buzzer**:
  - Inativo no modo normal.
  - Dois bipes curtos para alerta de nível de água.
  - Som grave contínuo para alerta de volume de chuva.
  - Som intermitente em diferentes frequências quando ambos os alertas estão ativos.

---

## Configuração do Hardware

| Componente            | Pino do RP2040 | Função                                   |
|-----------------------|----------------|------------------------------------------|
| Display OLED (I2C)    | GP14 (SDA), GP15 (SCL) | Exibição de informações e alertas      |
| Joystick (eixo X)     | GP26 (ADC0)    | Simulação do sensor de nível de água     |
| Joystick (eixo Y)     | GP27 (ADC1)    | Simulação do sensor de volume de chuva   |
| LED RGB               | Pinos dedicados| Indicação visual de estado normal/alerta |
| Matriz de LEDs 5x5    | PIO            | Exibição de animações e símbolos de alerta|
| Buzzer                | GP10           | Emissão de alertas sonoros               |

---

## Estrutura do Repositório

- **`estacao_monitoramento_cheias.c`**: Código-fonte principal do projeto.
- **`CMakeLists.txt`**: Arquivo de configuração para o sistema de build CMake, definindo as dependências, bibliotecas externas e configurações de compilação do projeto.
- **`lib/ssd1306.h` e `lib/ssd1306.c`**: Biblioteca para controle do display OLED.
- **`lib/led_5x5.h` e `lib/led_5x5.c`**: Biblioteca para controle da matriz de LEDs 5x5 via PIO.
- **`lib/buzzer.h` e `lib/buzzer.c`**: Biblioteca para geração de sons via PWM.
- **`lib/ledRGB.h` e `lib/ledRGB.c`**: Biblioteca para controle do LED RGB.
- **`lib/font.h`**: Definição da fonte utilizada no display OLED.
- **`README.md`**: Documentação do projeto.

---

## Organização das Tarefas FreeRTOS

- **vTaskJoystick**: Lê os valores do joystick, converte para percentuais (0-100%), determina o estado de alerta e envia os dados para a fila compartilhada.
- **vTaskDisplay**: Atualiza o display OLED com os valores recebidos da fila, alternando entre formatação normal e de alerta conforme o estado do sistema.
- **vTaskLED**: Controla o LED RGB, alternando entre verde (modo normal) e vermelho (modo alerta) com base nos dados recebidos da fila.
- **vTaskMatrizLED**: Gerencia a matriz de LEDs 5x5, exibindo diferentes animações e símbolos de acordo com o estado atual do sistema.
- **vTaskBuzzer**: Controla o buzzer para emitir diferentes padrões sonoros baseados no tipo de alerta ativo.

---

## Conceitos Aplicados

- **Multitarefa com FreeRTOS**: Gerenciamento de múltiplas tarefas concorrentes.
- **Comunicação por Filas**: Transferência segura de dados entre tarefas sem uso de variáveis compartilhadas.
- **Conversão Analógico-Digital (ADC)**: Leitura de sinais analógicos do joystick.
- **Modulação por Largura de Pulso (PWM)**: Geração de diferentes padrões sonoros.
- **Interface I2C**: Comunicação com o display OLED.
- **Programable IO (PIO)**: Controle eficiente da matriz de LEDs 5x5.
- **Detecção de Limiares**: Lógica de tomada de decisão baseada em valores predefinidos.
- **Feedback Multi-sensorial**: Combinação de estímulos visuais e auditivos para alertas.

---

## Objetivos Alcançados

- **Prototipação Realista**: Simulação funcional de um sistema de monitoramento ambiental.
- **Responsividade**: Sistema reage rapidamente a mudanças nos valores monitorados.
- **Acessibilidade**: Múltiplos canais de alerta para atender diferentes necessidades.
- **Modularidade**: Código organizado em tarefas independentes e bibliotecas reutilizáveis.
- **Comunicação Eficiente**: Uso de filas para troca de informações entre componentes.
- **Aplicação Educativa**: Demonstração prática de conceitos de sistemas embarcados e tempo real.

---

## Demonstração do Projeto

[Clique aqui](#)

---

## Desenvolvido por

Henrique Oliveira dos Santos  
[LinkedIn](https://www.linkedin.com/in/dev-henriqueo-santos/)
