/*
 * Copyright (c) 2017 Linaro Limited
 * Copyright (c) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/pwm.h>
#include<version.h>
#include <TMC2130.h>

/*Loggin settings*/
#define LOG_LEVEL 4
LOG_MODULE_REGISTER(main);

#define SPI1_NODE           DT_NODELABEL(spi1)
#define PWM1_NODE           DT_NODELABEL(tmc1)

/*TMC2130 configurations*/
const struct device *const spi1_dev = DEVICE_DT_GET(SPI1_NODE);
const struct device *const pwm1_dev = DEVICE_DT_GET(PWM1_NODE);
struct spi_config spi_cfg;
TMC2130TypeDef tmc2130;
ConfigurationTypeDef tmc_config ;
const int32_t registerResetState[TMC2130_REGISTER_COUNT];

/*TMC2130 read write function*/
void tmc2130_readWriteArray(uint8_t channel, uint8_t *data, size_t length);

#define DELAY_TIME K_MSEC(100)

void main(void) {
    
    LOG_INF("Version: %s", CONFIG_APP_VERSION);

    
    //hardware initialization
    tmc2130_init(&tmc2130,0,&tmc_config,registerResetState);
    LOG_INF("reseting TMC2130");
    tmc2130_reset(&tmc2130);

    int tick = 0;
    LOG_INF("Loading registers");
    while (tmc2130.config->state !=CONFIG_READY )
    {
        tmc2130_periodicJob(&tmc2130,tick);
        tick ++;
    }
    LOG_INF("number of loaded registers %d",tick);

    //LOG_INF("0x04 = %d",tmc2130_readInt(&tmc2130,0x04));
    tmc2130_writeInt(&tmc2130,0xEC-0x80,0x100c3);
    tmc2130_writeInt(&tmc2130,0x90-0x80,0x61f0a);
    tmc2130_writeInt(&tmc2130,0x91-0x80,0xa);
    tmc2130_writeInt(&tmc2130,0x80-0x80,0x4);
    tmc2130_writeInt(&tmc2130,0x93-0x80,0x1f4);
    tmc2130_writeInt(&tmc2130,0xf0-0x80,0x401c8);

    if(!pwm1_dev)
    {
        LOG_ERR("cannot find pwm1 device");
        return ;
    }
    else
    {
        LOG_INF("initializing the pwm");
        const struct pwm_dt_spec pwm_spec = PWM_DT_SPEC_GET_BY_IDX(DT_NODELABEL(tmc1),0);
        pwm_set_pulse_dt(&pwm_spec,50000);
        
    }


    
    while (1) {

	}
}


void tmc2130_readWriteArray(uint8_t channel, uint8_t *data, size_t length) {
    const struct gpio_dt_spec cs_spec = GPIO_DT_SPEC_GET_BY_IDX(SPI1_NODE,cs_gpios,0);
    
    struct spi_cs_control ctrl = {
        .gpio = cs_spec,
        .delay = 2,
    };

        spi_cfg.frequency = 500000;
        spi_cfg.operation = SPI_OP_MODE_MASTER | SPI_TRANSFER_MSB | SPI_WORD_SET(8) | SPI_LINES_SINGLE | SPI_LOCK_ON;
        spi_cfg.cs = &ctrl;

    
    struct spi_buf bufs[] = {
            {
                    .buf = data,
                    .len = length
            },
    };


    struct spi_buf_set tx = {
            .buffers =  bufs,
            .count = 1
    };

    struct spi_buf_set rx = {
            .buffers =  bufs,
            .count = 1
    };
    if (spi_transceive(spi1_dev, &spi_cfg, &tx, &rx) !=0) {
        LOG_INF("Error in SPI transaction");
    }
    //spi_write(spi1_dev, &spi_cfg, &tx);

}
