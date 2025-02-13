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
#define VRY_PIN 26   // Pino do Joystick Y
#define VRX_PIN 27   // Pino do Joystick X

// Variáveis globais
static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)
static volatile int contador = 0;
bool cor = true;
bool Vermelho = false;
bool Verde = false;
bool Azul = false;

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

    adc_init();

    adc_gpio_init(VRX_PIN);
    adc_gpio_init(VRY_PIN);
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
    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    while (true)
    {
        // Para o led VERMELHO
        adc_select_input(0);
        int16_t vrx_value0 = adc_read();

        // ser mais intenso nos extremos
        vrx_value0 = vrx_value0 - 2048;
        vrx_value0 = abs(vrx_value0); // Valor absoluto para criar simetria
        vrx_value0 = vrx_value0 * 2;
        // Limitar o valor máximo para evitar overflow
        if (vrx_value0 > 4095)
        {
            vrx_value0 = 4095;
        }
        pwm_set_gpio_level(led_RED, vrx_value0);

        // para o led AZUL
        adc_select_input(1);
        int16_t vrx_value1 = adc_read();

        // ser mais intenso nos extremos
        vrx_value1 = vrx_value1 - 2048;
        vrx_value1 = abs(vrx_value1); // Valor absoluto para criar simetria
        vrx_value1 = vrx_value1 * 2;
        // Limitar o valor máximo para evitar overflow
        if (vrx_value1 > 4095)
        {
            vrx_value1 = 4095;
        }
        pwm_set_gpio_level(led_BLUE, vrx_value1);

        float duty_cycle = (vrx_value1 / 4095.0) * 100;

        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        if (current_time - last_print_time >= 1000)
        {
            printf("VRX0: %u\n", vrx_value0);
            printf("VRX0: %u\n", vrx_value1);
            printf("Duty Cycle LED0: %.2f%%\n", duty_cycle);
            last_print_time = current_time;
        }

        sleep_ms(100);
    }
}
