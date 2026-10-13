#include <stdint.h>

#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"

/* =========================================
   CAU HINH CHAN LED
   ========================================= */

#define LED1_PIN    GPIO_Pin_0
#define LED2_PIN    GPIO_Pin_1
#define LED3_PIN    GPIO_Pin_2

/* =========================================
   CAU TRUC THAM SO LED
   ========================================= */

typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
    float frequency;
} LED_Config_t;

/* =========================================
   KHOI TAO GPIO BANG THU VIEN STD
   ========================================= */

static void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Bat clock GPIOA */
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA,
        ENABLE
    );

    /* Cau hinh PA0, PA1, PA2 lam output */
    GPIO_InitStructure.GPIO_Pin =
        LED1_PIN | LED2_PIN | LED3_PIN;

    GPIO_InitStructure.GPIO_Speed =
        GPIO_Speed_2MHz;

    GPIO_InitStructure.GPIO_Mode =
        GPIO_Mode_Out_PP;

    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Tat ca LED ban dau tat */
    GPIO_ResetBits(
        GPIOA,
        LED1_PIN | LED2_PIN | LED3_PIN
    );
}

/* =========================================
   HAM DIEU KHIEN NHAP NHAY LED
   ========================================= */

static void LED_BlinkTask(void *pvParameters)
{
    LED_Config_t *led;
    TickType_t halfPeriodTicks;
    uint32_t halfPeriodMs;

    led = (LED_Config_t *)pvParameters;

    /* Kiem tra tham so */
    if ((led == NULL) ||
        (led->port == NULL) ||
        (led->frequency <= 0.0f))
    {
        vTaskDelete(NULL);
    }

    /*
       Tinh thoi gian doi trang thai:

       halfPeriodMs = 1000 / (2 * frequency)
    */

    halfPeriodMs =
        (uint32_t)(1000.0f / (2.0f * led->frequency));

    /* Chuyen mili giay thanh tick FreeRTOS */
    halfPeriodTicks = pdMS_TO_TICKS(halfPeriodMs);

    if (halfPeriodTicks < 1U)
    {
        halfPeriodTicks = 1U;
    }

    while (1)
    {
        /* Dao trang thai LED bang thu vien STD */
        if (GPIO_ReadOutputDataBit(
                led->port,
                led->pin) == Bit_SET)
        {
            GPIO_ResetBits(led->port, led->pin);
        }
        else
        {
            GPIO_SetBits(led->port, led->pin);
        }

        /* Nhuong CPU cho cac task khac */
        vTaskDelay(halfPeriodTicks);
    }
}

/* =========================================
   THAM SO CHO 3 LED
   ========================================= */

static LED_Config_t led1 =
{
    GPIOA,
    LED1_PIN,
    0.1f
};

static LED_Config_t led2 =
{
    GPIOA,
    LED2_PIN,
    1.0f
};

static LED_Config_t led3 =
{
    GPIOA,
    LED3_PIN,
    10.0f
};

/* =========================================
   HAM MAIN
   ========================================= */

int main(void)
{
    GPIO_Config();

    /* Task 1: PA0 - 0.1 Hz */
    if (xTaskCreate(
            LED_BlinkTask,
            "LED_0_1Hz",
            128,
            &led1,
            1,
            NULL) != pdPASS)
    {
        for (;;)
        {
        }
    }

    /* Task 2: PA1 - 1 Hz */
    if (xTaskCreate(
            LED_BlinkTask,
            "LED_1Hz",
            128,
            &led2,
            1,
            NULL) != pdPASS)
    {
        for (;;)
        {
        }
    }

    /* Task 3: PA2 - 10 Hz */
    if (xTaskCreate(
            LED_BlinkTask,
            "LED_10Hz",
            128,
            &led3,
            1,
            NULL) != pdPASS)
    {
        for (;;)
        {
        }
    }

    /* Khoi dong he dieu hanh FreeRTOS */
    vTaskStartScheduler();

    /* Neu scheduler khong khoi dong duoc */
    while (1)
    {
    }
}