/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "SSD1322_GFX.h"
#include "SSD1322_API.h"

static const char *TAG = "example";


void app_main(void)
{
    ESP_LOGI(TAG, "Before init");

    SSD1322_API_init();
    ESP_LOGI(TAG, "After init!");

    uint8_t *buffer = (uint8_t*)heap_caps_malloc(sizeof(uint8_t) * OLED_HEIGHT * OLED_WIDTH / 2, MALLOC_CAP_DMA);
    if(buffer == NULL)
        exit(3);
    for (size_t i = 0; i < OLED_HEIGHT * OLED_WIDTH / 2; i++)
    {
        buffer[i] = 0;
    }  
    while (1) {
        ESP_LOGI(TAG, "Turning the LED!");
        
        draw_line(buffer, 0, 0, 255, 63, 15);
        
        send_buffer_to_OLED(buffer, 0, 0);
        

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
