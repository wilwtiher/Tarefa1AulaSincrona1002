#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "hardware/adc.h"

// para o I2C
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

// para o DISPLAY
#define IS_RGBW false
#define NUM_PIXELS 25

// Pinos
#define led_RED 13   // Red=13, Blue=12, Green=11
#define led_BLUE 12  // Red=13, Blue=12, Green=11
#define led_GREEN 11 // Red=13, Blue=12, Green=11
#define botao_pinA 5 // Botão A = 5, Botão B = 6 , BotãoJoy = 22
#define botao_pinB 6 // Botão A = 5, Botão B = 6 , BotãoJoy = 22
#define joybutton 22 // Botoa do joystick
#define VRY_PIN 26   // Pino do Joystick Y
#define VRX_PIN 27   // Pino do Joystick X

// Variáveis globais
static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)
static volatile int contador = 0;
bool cor = true;
bool Vermelho = false;
bool Verde = false;
bool Azul = false;

// Função de interrupção com debouncing
void gpio_irq_handler(uint gpio, uint32_t events)
{
    if(gpio == joybutton){
        gpio_put(led_GREEN, gpio_get(led_GREEN));
    }
    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    // Verifica se passou tempo suficiente desde o último evento
    if (current_time - last_time > 200000) // 200 ms de debouncing
    {
        if(gpio == joybutton){
            gpio_put(led_GREEN, gpio_get(led_GREEN));
        }
        last_time = current_time; // Atualiza o tempo do último evento
    }
}

// Funcao para iniciar pino PWM
uint pwm_init_gpio(uint gpio, uint wrap)
{
    gpio_set_function(gpio, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice_num, wrap);

    pwm_set_enabled(slice_num, true);
    return slice_num;
}

int main()
{
    stdio_init_all();

    gpio_init(joybutton);
    gpio_set_dir(joybutton, GPIO_IN); // Configura o pino como entrada
    gpio_pull_up(joybutton);          // Habilita o pull-up interno
    // Configuração da interrupção com callback
    gpio_set_irq_enabled_with_callback(joybutton, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    gpio_init(botao_pinA);
    gpio_set_dir(botao_pinA, GPIO_IN); // Configura o pino como entrada
    gpio_pull_up(botao_pinA);          // Habilita o pull-up interno
    // Configuração da interrupção com callback
    gpio_set_irq_enabled_with_callback(botao_pinA, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    
    adc_init();

    adc_gpio_init(VRY_PIN);
    adc_gpio_init(VRX_PIN);
    uint pwm_wrap = 4096;
    uint pwm_slice_red = pwm_init_gpio(led_RED, pwm_wrap);
    uint pwm_slice_blue = pwm_init_gpio(led_BLUE, pwm_wrap);

    uint32_t last_print_time = 0;

    // Para o display
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA);                                        // Pull up the data line
    gpio_pull_up(I2C_SCL);                                        // Pull up the clock line
    ssd1306_t ssd;                                                // Inicializa a estrutura do display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd);                                         // Configura o display
    ssd1306_send_data(&ssd);                                      // Envia os dados para o display
    // Limpa o display. O display inicia com um retangulo
    ssd1306_fill(&ssd, false);
    ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo
    ssd1306_send_data(&ssd);                      // Atualiza o display

    while (true)
    {
        // Para o led VERMELHO
        adc_select_input(1);
        int16_t vrx_value = adc_read();

        // ser mais intenso nos extremos
        vrx_value = vrx_value - 2048;
        vrx_value = abs(vrx_value); // Valor absoluto para criar simetria
        vrx_value = vrx_value * 2;
        // Limitar o valor máximo para evitar overflow
        if (vrx_value > 4095)
        {
            vrx_value = 4095;
        }
        // Valor minimo para o led acender
        if (vrx_value < 119)
        {
            vrx_value = 0;
        }
        pwm_set_gpio_level(led_RED, vrx_value);

        // para o led AZUL
        adc_select_input(0);
        int16_t vry_value = adc_read();

        // ser mais intenso nos extremos
        vry_value = vry_value - 2048;
        vry_value = abs(vry_value); // Valor absoluto para criar simetria
        vry_value = vry_value * 2;
        // Limitar o valor máximo para evitar overflow
        if (vry_value > 4095)
        {
            vry_value = 4095;
        }
        // Valor minimo para o led acender
        if (vry_value < 119)
        {
            vry_value = 0;
        }
        pwm_set_gpio_level(led_BLUE, vry_value);

        float duty_cycle0 = (vrx_value / 4095.0) * 100;
        float duty_cycle1 = (vry_value / 4095.0) * 100;

        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        if (current_time - last_print_time >= 1000)
        {
            printf("VRX: %u\n", vrx_value);
            printf("VRY: %u\n", vry_value);
            printf("Duty Cycle LED VERMELHO: %.2f%%\n", duty_cycle0);
            printf("Duty Cycle LED VERMELHO: %.2f%%\n", duty_cycle1);
            last_print_time = current_time;
        }

        sleep_ms(100);
    }
}
