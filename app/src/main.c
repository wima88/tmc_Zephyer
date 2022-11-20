#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/logging/log.h>
#include <version.h>
#include <TMC2130.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define LOG_LEVEL 4
LOG_MODULE_REGISTER(main);

#define SPI1_NODE           DT_NODELABEL(spi1)
  
/* remove me to use pwm instead gpio toggle */
#define SLEEP_TIME_US   5000
  
/*TMC2130 configurations*/
#ifndef SLEEP_TIME_US
#define PWM1_NODE           DT_NODELABEL(tmc1)
#include <zephyr/drivers/pwm.h>
const struct device *const pwm1_dev = DEVICE_DT_GET(PWM1_NODE);
#else
#include <zephyr/drivers/gpio.h>
#define STEP_NODE DT_NODELABEL(tmc1)
static const struct gpio_dt_spec step = GPIO_DT_SPEC_GET(STEP_NODE,step_gpios);
#endif


const struct device *const spi1_dev = DEVICE_DT_GET(SPI1_NODE);
struct spi_config spi_cfg;
TMC2130TypeDef tmc2130;
ConfigurationTypeDef tmc_config ;
const int32_t registerResetState[TMC2130_REGISTER_COUNT];

/*TMC2130 read write function*/
void tmc2130_readWriteArray(uint8_t channel, uint8_t *data, size_t length);

void main(void)
{
    LOG_INF("Version: %s", CONFIG_APP_VERSION);

    int ret;
  
    if (!device_is_ready(step.port)) {
        return;
    }   
  
    ret = gpio_pin_configure_dt(&step, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        return;
    }   
    
    /*Initialization for TMC2130*/
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

    /*minimal settings to turn motor -refer tmc2130 data sheet*/
    tmc2130_writeInt(&tmc2130,0xEC-0x80,0x80100c3);
    tmc2130_writeInt(&tmc2130,0x90-0x80,0x61f0a);
    tmc2130_writeInt(&tmc2130,0x91-0x80,0xa);
    tmc2130_writeInt(&tmc2130,0x80-0x80,0x4);
    tmc2130_writeInt(&tmc2130,0x93-0x80,0x0f4);
    tmc2130_writeInt(&tmc2130,0xf0-0x80,0x401c8);

#ifndef SLEEP_TIME_US     
    if(!pwm1_dev) {
        
        LOG_ERR("cannot find pwm1 device");
        return ;
    }
    else {
        
        LOG_INF("initializing the pwm");
        const struct pwm_dt_spec pwm_spec = PWM_DT_SPEC_GET_BY_IDX(DT_NODELABEL(tmc1),0);
        int ret = pwm_set_dt(&pwm_spec,PWM_KHZ(50),2000);
        if (ret != 0 ) {
            LOG_ERR("pwm error %d",ret);
            return; 
        } 
    }
#endif

    while (1) {

#ifdef SLEEP_TIME_US          
        ret = gpio_pin_toggle_dt(&step);
        if (ret < 0) {
            return;
        }   
        k_usleep(SLEEP_TIME_US);
#endif

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
}

