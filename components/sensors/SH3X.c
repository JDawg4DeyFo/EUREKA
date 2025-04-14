/* MIT License
*
* Copyright (c) 2022 ma-lwa-re
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "sht3x.h"
#include "math.h"
#include "esp_log.h"

static const char *SHT3X_TAG = "sht3x";

typedef struct measurements {
    sht3x_sensor_value_t temperature;
    sht3x_sensor_value_t humidity;
} measurements_t;

// Single Shot Data Acquisition
uint8_t clock_stretching_enabled_repeatability_high[]     = {0x2C, 0x06};
uint8_t clock_stretching_enabled_repeatability_medium[]   = {0x2C, 0x0D};
uint8_t clock_stretching_enabled_repeatability_low[]      = {0x2C, 0x10};
uint8_t clock_stretching_disabled_repeatability_high[]    = {0x24, 0x00};
uint8_t clock_stretching_disabled_repeatability_medium[]  = {0x24, 0x0B};
uint8_t clock_stretching_disabled_repeatability_low[]     = {0x24, 0x16};

// Periodic Data Acquisition Mode
uint8_t mps_0_5_repeatability_high[]    = {0x20, 0x32};
uint8_t mps_0_5_repeatability_medium[]  = {0x20, 0x24};
uint8_t mps_0_5_repeatability_low[]     = {0x20, 0x2F};
uint8_t mps_1_repeatability_high[]      = {0x21, 0x30};
uint8_t mps_1_repeatability_medium[]    = {0x21, 0x26};
uint8_t mps_1_repeatability_low[]       = {0x21, 0x2D};
uint8_t mps_2_repeatability_high[]      = {0x22, 0x36};
uint8_t mps_2_repeatability_medium[]    = {0x22, 0x20};
uint8_t mps_2_repeatability_low[]       = {0x22, 0x2B};
uint8_t mps_4_repeatability_high[]      = {0x23, 0x34};
uint8_t mps_4_repeatability_medium[]    = {0x23, 0x22};
uint8_t mps_4_repeatability_low[]       = {0x23, 0x29};
uint8_t mps_10_repeatability_high[]     = {0x27, 0x37};
uint8_t mps_10_repeatability_medium[]   = {0x27, 0x21};
uint8_t mps_10_repeatability_low[]      = {0x27, 0x2A};

// Commands
uint8_t read_measurement[]              = {0xE0, 0x00};
uint8_t periodic_measurement_with_art[] = {0x2B, 0x32};
uint8_t stop_periodic_measurement[]     = {0x30, 0x93};
uint8_t soft_reset[]                    = {0x30, 0xA2};
uint8_t general_call_reset[]            = {0x00, 0x06};
uint8_t heater_enable[]                 = {0x30, 0x6D};
uint8_t heater_disable[]                = {0x30, 0x66};
uint8_t read_status_register[]          = {0xF3, 0x2D};
uint8_t clear_status_register[]         = {0x30, 0x41};

/*
* Delay the execution in ms
*/
void delay_ms(uint16_t delay) {
    ets_delay_us(delay * 1000);
}

/*
* The 8-bit CRC checksum transmitted after each data word is generated by a CRC algorithm.
* The CRC covers the contents of the two previously transmitted data bytes.
* To calculate the checksum only these two previously transmitted data bytes are used.
* Note that command words are not followed by CRC.
*/
uint8_t sht3x_generate_crc(const uint8_t* data, uint16_t count) {
    uint16_t current_byte;
    uint8_t crc = CRC8_INIT;
    uint8_t crc_bit;

    for(current_byte = 0; current_byte < count; ++current_byte) {
        crc ^= (data[current_byte]);
        for(crc_bit = 8; crc_bit > 0; --crc_bit) {

            if (crc & 0x80) {
                crc = (crc << 1) ^ CRC8_POLYNOMIAL;
            } else {
                crc = (crc << 1);
            }
        }
    }
    return crc;
}

/*
* For the send command sequences, after writing the address and/or data to the sensor
* and sending the ACK bit, the sensor needs the execution time to respond to the I2C read header with an ACK bit.
* Hence, it is required to wait the command execution time before issuing the read header.
* Commands must not be sent while a previous command is being processed.
*/
esp_err_t sht3x_send_command(uint8_t *command, i2c_device_handle_t *Dev_Handle) {
    esp_err_t err;

	err = ESP_ERROR_CHECK(i2c_master_transmit_receive(Dev))

    return err;
}

/*
* Data sent to and received from the sensor consists of a sequence of 16-bit commands and/or 16-bit words
* (each to be interpreted as unsigned integer, most significant byte transmitted first). Each data word is
* immediately succeeded by an 8-bit CRC. In write direction it is mandatory to transmit the checksum.
* In read direction it is up to the master to decide if it wants to process the checksum.
*/
esp_err_t sht3x_read(uint8_t *hex_code, uint8_t *measurements, uint8_t size, i2c_device_handle_t *Dev_Handle) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (SHT3X_SENSOR_ADDR << 1) | I2C_MASTER_WRITE, I2C_ACK_CHECK_EN));
    ESP_ERROR_CHECK(i2c_master_write(cmd, hex_code, SHT3X_HEX_CODE_SIZE, I2C_ACK_CHECK_EN));

    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (SHT3X_SENSOR_ADDR << 1) | I2C_MASTER_READ, I2C_ACK_CHECK_EN));
    ESP_ERROR_CHECK(i2c_master_read(cmd, measurements, size, I2C_MASTER_LAST_NACK));

    ESP_ERROR_CHECK(i2c_master_stop(cmd));
    esp_err_t err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

    i2c_cmd_link_delete(cmd);
    return err;
}

/*
* Data sent to and received from the sensor consists of a sequence of 16-bit commands and/or 16-bit words
* (each to be interpreted as unsigned integer, most significant byte transmitted first). Each data word is
* immediately succeeded by an 8-bit CRC. In write direction it is mandatory to transmit the checksum.
* In read direction it is up to the master to decide if it wants to process the checksum.
*/
esp_err_t sht3x_write(uint8_t *hex_code, uint8_t *measurements, uint8_t size, i2c_device_handle_t *Dev_Handle) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (SHT3X_SENSOR_ADDR << 1) | I2C_MASTER_WRITE, I2C_ACK_CHECK_EN));
    ESP_ERROR_CHECK(i2c_master_write(cmd, hex_code, SHT3X_HEX_CODE_SIZE, I2C_ACK_CHECK_EN));
    ESP_ERROR_CHECK(i2c_master_write(cmd, measurements, size, I2C_ACK_CHECK_EN));

    ESP_ERROR_CHECK(i2c_master_stop(cmd));
    esp_err_t err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

    i2c_cmd_link_delete(cmd);
    return err;
}

/*
* For the send command and fetch results sequences, after writing the address and/or data to the sensor
* and sending the ACK bit, the sensor needs the execution time to respond to the I2C read header with an ACK bit.
* Hence, it is required to wait the command execution time before issuing the read header.
* Commands must not be sent while a previous command is being processed.
*/
esp_err_t sht3x_send_command_and_fetch_result(uint8_t *command, uint8_t *measurements, uint8_t size, i2c_device_handle_t *Dev_Handle) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (SHT3X_SENSOR_ADDR << 1) | I2C_MASTER_WRITE, I2C_ACK_CHECK_EN));
    ESP_ERROR_CHECK(i2c_master_write(cmd, command, sizeof(command), I2C_ACK_CHECK_EN));
    ESP_ERROR_CHECK(i2c_master_stop(cmd));

    delay_ms(1000);

    ESP_ERROR_CHECK(i2c_master_write(cmd, measurements, size, I2C_ACK_CHECK_EN));

    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (SHT3X_SENSOR_ADDR << 1) | I2C_MASTER_READ, I2C_ACK_CHECK_EN));
    ESP_ERROR_CHECK(i2c_master_read(cmd, measurements, size, I2C_MASTER_LAST_NACK));

    ESP_ERROR_CHECK(i2c_master_stop(cmd));
    esp_err_t err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

    i2c_cmd_link_delete(cmd);
    return err;
}

/*
* Start periodic measurement, signal update interval is 5 seconds.
*/
esp_err_t sht3x_start_periodic_measurement() {
    return sht3x_send_command(mps_4_repeatability_high);
}

/*
* Start periodic measurement with the accelerated response time (ART) feature.
*/
esp_err_t sht3x_start_periodic_measurement_with_art() {
    return sht3x_send_command(periodic_measurement_with_art);
}

/*
* Read sensor output. The measurement data can only be read out once per signal update interval
* as the buffer is emptied upon read-out.
*/
esp_err_t sht3x_read_measurement(sht3x_sensors_values_t *sensors_values) {
    measurements_t measurements = {
        .temperature = {{0x00, 0x00}, 0x00},
        .humidity = {{0x00, 0x00}, 0x00}
    };

    esp_err_t err = sht3x_read(read_measurement, (uint8_t *) &measurements, sizeof(measurements));

    sensors_values->temperature = (175.0 * (((measurements.temperature.value.msb << 8) + measurements.temperature.value.lsb) / 65535.0)) - 45.0;
    sensors_values->humidity = 100.0 * ((measurements.humidity.value.msb << 8) + measurements.humidity.value.lsb) / 65535.0;
    return err;
}

/*
* Stop periodic measurement to change the sensor configuration or to save power. Note that the sensor will only
* respond to other commands after waiting 500 ms after issuing the stop_periodic_measurement command.
*/
esp_err_t sht3x_stop_periodic_measurement() {
    return sht3x_send_command(stop_periodic_measurement);
}

/*
* The SHT3x provides a soft reset mechanism that forces the system into a well-defined state without removing
* the power supply. When the system is in idle state the soft reset command can be sent to the SHT3x.
* This triggers the sensor to reset its system controller and reloads calibration data from the memory.
*/
esp_err_t sht3x_soft_reset() {
    return sht3x_send_command(soft_reset);
}

/*
* Additionally, a reset of the sensor can also be generated using the "general call" mode according to
* I2C-bus specification. This generates a reset which is functionally identical to using the nReset pin.
*/
esp_err_t sht3x_general_call_reset() {
    return sht3x_send_command(general_call_reset);
}

/*
* Switch the internal heater on.
*/
esp_err_t sht3x_enable_heater() {
    return sht3x_send_command(heater_enable);
}

/*
* Switch the internal heater off.
*/
esp_err_t sht3x_disable_heater() {
    return sht3x_send_command(heater_disable);
}

/*
* The status register contains information on the operational status of the heater, the alert mode
* and on the execution status of the last command and the last write sequence.
*/
esp_err_t sht3x_read_status_register(sht3x_sensor_value_t *sensors_value) {
    sht3x_sensor_value_t status_register = {
        .value = {0x00, 0x00},
        .crc = 0x00
    };

    esp_err_t err = sht3x_read(read_status_register, (uint8_t *) &status_register, sizeof(status_register));

    if(err != ESP_OK) {
        ESP_LOGE(SHT3X_TAG, "read_status_register failed with status code: %s", esp_err_to_name(err));
        return SHT3X_READ_ERROR;
    }
    return err;
}

/*
* Clear all flags in the status register.
*/
esp_err_t sht3x_clear_status_register() {
    return sht3x_send_command(clear_status_register);
}