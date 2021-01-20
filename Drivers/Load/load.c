/******************************************************************************
 * @file load
 * @brief driver example a simple load
 * @author Luos
 * @version 0.0.0
 ******************************************************************************/
#include "main.h"
#include "load.h"
#include "HX711.h"
#include "string.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
uint8_t new_data_ready = 0;
volatile force_t load = 0.0;
char have_to_tare = 0;

/*******************************************************************************
 * Function
 ******************************************************************************/
static void Void_MsgHandler(container_t *container, msg_t *msg)
{}

/******************************************************************************
 * @brief init must be call in project init
 * @param None
 * @return None
 ******************************************************************************/
void Load_Init(void)
{
    revision_t revision = {.unmap = REV};
    
    char alias[15];
    for (uint8_t i=0; i < 15; i++)
    {
        sprintf(alias, "fan_%d", i);
        Luos_CreateContainer(Void_MsgHandler, LOAD_MOD, alias, revision);
    }
}
/******************************************************************************
 * @brief loop must be call in project loop
 * @param None
 * @return None
 ******************************************************************************/
void Load_Loop(void)
{
    static uint8_t led = 0;
    static uint32_t last = 0;
    uint32_t t = HAL_GetTick();

    if ((t - last) > 500)
    {
        led = 1 - led;
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, led);
        last = t;
    }
}
