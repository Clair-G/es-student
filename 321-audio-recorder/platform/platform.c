#include "platform.h"

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

static const uint STATE_BUTTON_PIN = 15;
#define ADC_PIN 26
#define ADC_CHANNEL 0
#define ADC_TEMP_CHANNEL 4
#define ADC_VREF_V 3.3f
#define ADC_LEVELS 4096
#define CHARGE_PIN 22
#define LED_PIN PICO_DEFAULT_LED_PIN

const platform_button_t buttons[] = {
    { STATE_BUTTON_PIN },
};

const uint32_t buttons_count = sizeof(buttons) / sizeof(buttons[0]);

static repeating_timer_t blink_timer;
static bool blink_running;
static volatile bool blink_on;
static volatile uint32_t blink_toggles;

// кто сейчас работает с АЦП; меняют и функции, и обработчики прерываний
typedef enum {
    ADC_FREE,
    ADC_CAPTURE,
} adc_user_t;

static volatile adc_user_t adc_user;

static repeating_timer_t capture_timer;
static uint16_t capture_samples[PLATFORM_CAPTURE_MAX];
static uint32_t capture_times[PLATFORM_CAPTURE_MAX];
static uint32_t capture_count;
static uint32_t capture_period_us;
static volatile uint32_t capture_index;
static volatile uint32_t capture_handler_us;


static void buttons_init(void)
{
	for (int button_id = 0; button_id < buttons_count; button_id++)
	{
		gpio_init(buttons[button_id].pin);
		gpio_set_dir(buttons[button_id].pin, GPIO_IN);
		gpio_pull_up(buttons[button_id].pin);
	}
	
}

static void led_init(void)
{
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);
	

}

// запускает стандартный ввод-вывод и настраивает выводы
void platform_init(void)
{
	stdio_init_all();
	buttons_init();
	led_init();
	adc_init();
	adc_gpio_init(ADC_PIN);
	adc_set_temp_sensor_enabled(true);
	gpio_init(CHARGE_PIN);
	gpio_set_dir(CHARGE_PIN, GPIO_OUT);

}


void platform_led_set(bool on)
{
	gpio_put(LED_PIN, on);
		
}

bool platform_led_get(void)
{
	return gpio_get(LED_PIN);
		
}

// нажата ли кнопка сейчас, без подавления дребезга
bool platform_button_read(uint32_t button_id) //
{
	if (!gpio_get(buttons[button_id].pin)) 
		return true;
	
	return false;
}

float platform_voltage_read(void)
{
	adc_select_input(ADC_CHANNEL);
	
	return (float)adc_read() * ADC_VREF_V / ADC_LEVELS;
}

float platform_temp_read(void)
{
	adc_select_input(ADC_TEMP_CHANNEL);
	
	float temp_V = adc_read() * ADC_VREF_V / ADC_LEVELS;
	
	return (float)27.0f - (temp_V - 0.706f) / 0.001721f;
}

void platform_charge_set(bool on)
{
	gpio_put(CHARGE_PIN, on);
} 


// обработчик будильника индикатора
static bool on_blink(repeating_timer_t *timer)
{
    blink_on = !blink_on;
    gpio_put(LED_PIN, blink_on);
    blink_toggles++;

    // true — вызвать снова через тот же период
    return true;
}

bool platform_blink_start(uint32_t period_ms)
{
    if (period_ms < 10 || period_ms > 60000)
    {
        return false;
    }

    platform_blink_stop();
    blink_toggles = 0;

    // минус перед периодом — отсчитывать от начала прошлого вызова
    blink_running = add_repeating_timer_ms(-(int32_t)period_ms, on_blink, NULL, &blink_timer);
    return blink_running;
}

uint32_t platform_blink_stop(void)
{
    if (blink_running)
    {
        cancel_repeating_timer(&blink_timer);
        blink_running = false;
    }

    blink_on = false;
    gpio_put(LED_PIN, 0);
    return blink_toggles;
}


// обработчик будильника захвата: время и отсчёт, затем время своей работы
static bool on_capture(repeating_timer_t *timer)
{
    uint32_t start_us = time_us_32();
    capture_times[capture_index] = start_us;
    capture_samples[capture_index] = adc_read();
    capture_index++;
    capture_handler_us += time_us_32() - start_us;

    if (capture_index < capture_count)
    {
        return true;
    }

    // захват окончен: АЦП свободен, будильник снимается
    adc_user = ADC_FREE;
    return false;
}

bool platform_capture_start(uint32_t count, uint32_t period_us)
{
    if (adc_user != ADC_FREE || count == 0 || count > PLATFORM_CAPTURE_MAX || period_us == 0)
    {
        return false;
    }

    adc_select_input(ADC_CHANNEL);
    capture_count = count;
    capture_period_us = period_us;
    capture_index = 0;
    capture_handler_us = 0;
    adc_user = ADC_CAPTURE;

    if (!add_repeating_timer_us(-(int64_t)period_us, on_capture, NULL, &capture_timer))
    {
        adc_user = ADC_FREE;
        return false;
    }
    return true;
}