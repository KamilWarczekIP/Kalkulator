/**
 ****************************************************************************************
 *
 * \file SSD1322_HW_Driver.c
 *
 * \brief Hardware dependent functions for SSD1322 OLED display.
 *
 * This file contains functions that rely on hardware of used MCU. In this example functions
 * are filled with STM32F411RE hardware implementation. To use this library on any other MCU
 * you just have to provide its hardware implementations of functions from this file and higher
 * level functions should work without modification.
 *
 * Copyright (C) 2020 Wojciech Klimek
 * MIT license:
 * https://github.com/wjklimek1/SSD1322_OLED_library
 *
 ****************************************************************************************
 */

//=================== Include hardware HAL libraries =====================//
#include <driver/gpio.h>
#include <driver/spi_master.h>
#include "freertos/FreeRTOS.h"
#include <sdkconfig.h>
#include <esp_log.h>
#include "freertos/timers.h"

#include "../include/SSD1322_HW_Driver.h"
#define SSD1322_CS_PIN GPIO_NUM_5
#define SSD1322_DC_PIN GPIO_NUM_17
#define SSD1322_RESET_PIN GPIO_NUM_15


spi_device_handle_t SSD1322_HW_SPI_DEVICE;

//====================== CS pin low ========================//
/**
 *  @brief Drives CS (Chip Select) pin of SPI interface low.
 *
 *  CS pin may be also signed as "SS" or "NSS"
 */
void SSD1322_HW_drive_CS_low()
{
	//50000000000gpio_set_level(SSD1322_CS_PIN, 0);
}

//====================== CS pin high ========================//
/**
 *  @brief Drives CS (Chip Select) pin of SPI interface high.
 *
 *  CS pin may be also signed as "SS" or "NSS"
 */
void SSD1322_HW_drive_CS_high()
{
	//gpio_set_level(SSD1322_CS_PIN, 1);
}

//====================== DC pin low ========================//
/**
 *  @brief Drives DC (Data/Command) pin of OLED driver low.
 *
 *  High state is for data and low state is for command.
 */
void SSD1322_HW_drive_DC_low()
{
	gpio_set_level(SSD1322_DC_PIN, 0);
}

//====================== DC pin high ========================//
/**
 *  @brief Drives DC (Data/Command) pin of of OLED driver high.
 *
 *  High state is for data and low state is for command.
 */
void SSD1322_HW_drive_DC_high()
{
	gpio_set_level(SSD1322_DC_PIN, 1);
}

//====================== RESET pin low ========================//
/**
 *  @brief Drives RESET pin of of OLED driver low.
 *
 *  Logic low on RESET resets OLED driver.
 */
void SSD1322_HW_drive_RESET_low()
{
	gpio_set_level(SSD1322_RESET_PIN, 0);
}

//====================== RESET pin high ========================//
/**
 *  @brief Drives RESET pin of of OLED driver high.
 *
 *  Logic low on RESET resets OLED driver.
 */
void SSD1322_HW_drive_RESET_high()
{
	gpio_set_level(SSD1322_RESET_PIN, 1);
}

//====================== Send single SPI byte ========================//
/**
 *  @brief Transmits single byte through SPI interface.
 *
 *  @param[in] byte_to_transmit byte that will be transmitted through SPI interface
 */
void SSD1322_HW_SPI_send_byte(uint8_t byte_to_transmit)
{
	spi_device_acquire_bus(SSD1322_HW_SPI_DEVICE, portMAX_DELAY);
	
	spi_transaction_t transaction = {
		.tx_buffer = &byte_to_transmit,
		.rx_buffer = NULL,
		.rxlength = 0,
		//.tx_data[0] = byte_to_transmit,
		.length = 8,
		.flags = 0,
		.addr = 0,
		.cmd = 0,
		//.flags = SPI_TRANS_USE_TXDATA,
	};
	

	ESP_ERROR_CHECK(spi_device_transmit(SSD1322_HW_SPI_DEVICE, &transaction));
	
	spi_device_release_bus(SSD1322_HW_SPI_DEVICE);

}

//====================== Send array of SPI bytes ========================//
/**
 *  @brief Transmits array of bytes through SPI interface.
 *
 *  @param[in] array_to_transmit array of bytes that will be transmitted through SPI interface
 *  @param[in] array_size amount of bytes to transmit
 */
void SSD1322_HW_SPI_send_array(uint8_t *array_to_transmit, uint32_t array_size)
{
	spi_device_acquire_bus(SSD1322_HW_SPI_DEVICE, portMAX_DELAY);

	spi_transaction_t transaction = {
		.tx_buffer = array_to_transmit,
		.rx_buffer = NULL,
		.length = array_size * 8,
		.rxlength = 0,
		.flags = 0,
		.addr = 0,
		.cmd = 0,
	};
	ESP_ERROR_CHECK(spi_device_transmit(SSD1322_HW_SPI_DEVICE, &transaction));

	spi_device_release_bus(SSD1322_HW_SPI_DEVICE);

}

//====================== Milliseconds delay ========================//
/**
 *  @brief Wait for x milliseconds.
 *
 *  NOTE: This function is only used in initialization sequence to pull reset down and high
 *  and later to wait for Vdd stabilization. It takes time ONLY on startup (around 200ms)
 *  and is NOT USED during normal operation.
 *
 *  @param[in] milliseconds time to wait
 */
void SSD1322_HW_msDelay(uint32_t milliseconds)
{
	vTaskDelay(milliseconds /  portTICK_PERIOD_MS);
}
void SSD1322_HW_INIT()
{
	gpio_config_t pin_cfg = {
		.mode = GPIO_MODE_OUTPUT,
		.pin_bit_mask = (1UL << SSD1322_DC_PIN),
		.intr_type = GPIO_INTR_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_ENABLE,
		.pull_up_en = GPIO_PULLUP_DISABLE,
	};

	gpio_config(&pin_cfg);
	//pin_cfg.pin_bit_mask = (1UL << SSD1322_CS_PIN);
	//gpio_config(&pin_cfg);
	pin_cfg.pin_bit_mask = (1UL << SSD1322_RESET_PIN);
	gpio_config(&pin_cfg);

	
	spi_bus_config_t bus_conf = {0};

    bus_conf.miso_io_num = GPIO_NUM_NC;
    bus_conf.mosi_io_num = GPIO_NUM_23;
    bus_conf.sclk_io_num = GPIO_NUM_18;
	bus_conf.quadhd_io_num = GPIO_NUM_NC;
	bus_conf.quadwp_io_num = GPIO_NUM_NC;
	bus_conf.max_transfer_sz = 256 * 64 / 2 + 16;
	//.flags = SPICOMMON_BUSFLAG_GPIO_PINS,
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &bus_conf, SPI_DMA_CH_AUTO));


	spi_device_interface_config_t dev_conf = {0};
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
    
	
	ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &dev_conf, &SSD1322_HW_SPI_DEVICE));

}