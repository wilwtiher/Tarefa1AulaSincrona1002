# Aluno: Wilton Lacerda Silva Júnior
## Grupo 6 Subgrupo 5
# Video explicativo: https://youtu.be/hEfYz17iNUU
# Atividade 1 Aula Sincrona dia 10/02: ADC
# Leitura de ADC no Raspberry Pi Pico
Este projeto demonstra como ler um ADC (Conversor Analógico Digital) no Raspberry Pi Pico. O código faz de acordo com os valores dos ADCs, os leds vermelho e azul brilhar mais forte ou mais fraco, além de mecher um quadrado 8x8 no display.

## Funcionalidades

- **Joystick:**  
  - O Joystick funciona como dois potenciômetros, podendo assim ler os volores do X e Y dele com ADC.
  - Ele será o objeto a ser lido pelo ADC.
- **3 Leds:**
  - Os leds serão retornos visuais para os potenciômetros.
- **Display OLED**
  - O display será outro retorno visual.
- **2 Botões**
  - Os botões serão usados para a implementação de interrupções com debounce e a sua funcionalidade é de alterar os leds e a borda do display.

# Requisitos
## Hardware:

- Raspberry Pi Pico W.
- 2 potenciômetros nas portas 26 e 27.
- 3 LEDS nas portas 11, 12 e 13.
- 2 botões nas portas 5 e 22.
- 1 display ssd1306 com o sda na porta 14 e o scl na porta 15.

## Software:

- Ambiente de desenvolvimento VS Code com extensão Pico SDK.

# Instruções de uso
## Configure o ambiente:
- Certifique-se de que o Pico SDK está instalado e configurado no VS Code.
- Compile o código utilizando a extensão do Pico SDK.
## Teste:
- Utilize a placa BitDogLab para o teste. Caso não tenha, conecte os hardwares informados acima nos pinos correspondentes.
