#include <Display.hpp>
#include <driver/gpio.h>
#include <driver/spi_master.h>
#include "freertos/FreeRTOS.h"
#include <sdkconfig.h>
#include <esp_log.h>
#include "freertos/timers.h"

constexpr gpio_num_t SSD1322_CS_PIN = GPIO_NUM_5;
constexpr gpio_num_t SSD1322_DC_PIN = GPIO_NUM_17;
constexpr gpio_num_t SSD1322_RESET_PIN = GPIO_NUM_15;



Display::Display() = delete;
void Display::sendCommand(Commands command)
{
    gpio_set_level(SSD1322_DC_PIN, 0);
    spi_transaction_t transaction = {};
	uint8_t command_to_send = static_cast<uint8_t>(command);
    transaction.tx_buffer = &command_to_send;
    transaction.length = 8;
	
	ESP_ERROR_CHECK(spi_device_transmit(display_handle, &transaction));
}
void Display::sendData(uint8_t data)
{
    gpio_set_level(SSD1322_DC_PIN, 1);
    spi_transaction_t transaction = {};
    transaction.tx_buffer = &data;
    transaction.length = 8;
	
	ESP_ERROR_CHECK(spi_device_transmit(display_handle, &transaction));
}
void Display::initialize()
{
    gpio_config_t pin_cfg;
	pin_cfg.pin_bit_mask = (1UL << SSD1322_DC_PIN);
	pin_cfg.mode = GPIO_MODE_OUTPUT;
	pin_cfg.pull_up_en = GPIO_PULLUP_DISABLE;
	pin_cfg.pull_down_en = GPIO_PULLDOWN_ENABLE;
	pin_cfg.intr_type = GPIO_INTR_DISABLE;
	ESP_ERROR_CHECK(gpio_config(&pin_cfg));
	pin_cfg.pin_bit_mask = (1UL << SSD1322_RESET_PIN);
	ESP_ERROR_CHECK(gpio_config(&pin_cfg));
	


	
	spi_bus_config_t bus_conf = {};
    bus_conf.mosi_io_num = GPIO_NUM_23;
    bus_conf.sclk_io_num = GPIO_NUM_18;
    bus_conf.miso_io_num = GPIO_NUM_NC;
	bus_conf.quadhd_io_num = GPIO_NUM_NC;
	bus_conf.quadwp_io_num = GPIO_NUM_NC;
	bus_conf.max_transfer_sz = 256 * 64 / 2;
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &bus_conf, SPI_DMA_CH_AUTO));


	spi_device_interface_config_t dev_conf = {};
	dev_conf.clock_source = SPI_CLK_SRC_DEFAULT;     
	dev_conf.clock_speed_hz = SPI_MASTER_FREQ_8M;      
    dev_conf.spics_io_num = SSD1322_CS_PIN;
    dev_conf.queue_size = 1;
    dev_conf.pre_cb = NULL;
    dev_conf.post_cb = NULL;
	dev_conf.mode = 0;
	dev_conf.command_bits = 0;
	dev_conf.address_bits = 0;
	dev_conf.dummy_bits = 0;
	dev_conf.cs_ena_posttrans = 0;
	dev_conf.cs_ena_pretrans = 0;
	dev_conf.duty_cycle_pos = 128;
    
	ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &dev_conf, &display_handle));


    vTaskDelay(100 /  portTICK_PERIOD_MS);  //100ms delay
	gpio_set_level(SSD1322_RESET_PIN, 0); //Reset pin low
    vTaskDelay(100 /  portTICK_PERIOD_MS);  //100ms delay
	gpio_set_level(SSD1322_RESET_PIN, 1); //Reset pin high
    vTaskDelay(500 /  portTICK_PERIOD_MS);  //500ms delay
    spi_device_acquire_bus(display_handle, portMAX_DELAY);
	sendCommand(Commands::COMMAND_LOCK);
	sendData(0x12);
	sendCommand(Commands::DISPLAY_OFF);
	sendCommand(Commands::SET_FRONT_CLOCK_DIVIDER);
	sendData(0x91);
	sendCommand(Commands::SET_MULTIPLEX_RATIO);
	sendData(0x3F);
	sendCommand(Commands::SET_DISPLAY_OFFSET);
	sendData(0x00);
	sendCommand(Commands::SET_DISPLAY_START_LINE);
	sendData(0x00);
	sendCommand(Commands::SET_REMAP_DUAL_COM_LINE_MODE);
	sendData(0x14);
	sendData(0x11);
	sendCommand(Commands::SET_GPIO);//disable IO input
	sendData(0x00);
	sendCommand(Commands::SET_FUNCTION_SELECTION);
	sendData(0x01);
	sendCommand(Commands::DISPLAY_ENCHANCEMENT_A);   //enable VSL extern
	sendData(0xA0);
	sendData(0xFD);
	sendCommand(Commands::SET_CONTRAST_CURRENT);
	sendData(0xFF);
	sendCommand(Commands::MASTER_CURRENT_CONTROL);
	sendData(0x0F);
	sendCommand(Commands::RESET_GRAY_SCALE_TABLE);
	sendCommand(Commands::SET_PHASE_LENGTH);
	sendData(0xE2);
	sendCommand(Commands::DISPLAY_ENCHANCEMENT_B);
	sendData(0x82);
	sendData(0x20);
	sendCommand(Commands::SET_PRE_CHARGE_VOLTAGE);
	sendData(0x1F);
	sendCommand(Commands::SET_PRE_CHARGE_PERIOD);
	sendData(0x08);
	sendCommand(Commands::SET_Vcomh_VOLTAGE);
	sendData(0x07);
	sendCommand(Commands::SET_MODE_NORMAL);
	sendCommand(Commands::EXIT_PARTIAL_DISPLAY);
    vTaskDelay(100 /  portTICK_PERIOD_MS);//wait for voltage Vdd to sabilize
    buffer = (uint8_t*)heap_caps_malloc(sizeof(uint8_t) * BUFFER_SIZE, MALLOC_CAP_DMA); //div by 2, because there are 2 pixels in one uint8_t
    if(buffer == nullptr)
	{
		esp_restart();
	}
	clear();
	sendCommand(Commands::DISPLAY_ON);
    vTaskDelay(500 /  portTICK_PERIOD_MS);//wait for voltage Vdd to sabilize
    spi_device_release_bus(display_handle);

    //TODO boot animation
}
void Display::update()
{
	spi_device_acquire_bus(display_handle, portMAX_DELAY);
    sendCommand(Commands::WRITE_RAM);
	gpio_set_level(SSD1322_DC_PIN, 1);

	spi_transaction_t transaction = {};
	transaction.tx_buffer = buffer;
	transaction.length = BUFFER_SIZE * sizeof(uint8_t) * 8; // number of BITS send

	ESP_ERROR_CHECK(spi_device_transmit(display_handle, &transaction));
    spi_device_release_bus(display_handle);
}
void Display::clear()
{
    memset(buffer, 0, sizeof(uint8_t) * BUFFER_SIZE);
}
void Display::drawLine(int16_t from_x, int16_t from_y, int16_t to_x, int16_t to_y, uint8_t color);
// static void Display::drawRect();
// static void Display::drawTriangle();
// static void Display::drawCircle();
void Display::drawText(int16_t x, int16_t y, const char* text, Font* font, uint8_t size, uint8_t color);
void Display::drawPixel(int16_t x, int16_t y, uint8_t color);
void Display::drawBitmap(int16_t x, int16_t y, uint8_t* bitmap, size_t width, size_t height);
void Display::drawIcon(int16_t x, int16_t y, Icon* icon, uint8_t color);