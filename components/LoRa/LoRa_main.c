/**
 * @file LoRa_main.c
 * @author Edouard Valenzuela (ecvalenz@ucsc.edu)
 * @brief Consolidation of LoRa functions
 * @version 1.0
 * @date 2025-04-27
 * 
 * @copyright Copyright (c) 2025
 * 
 * 
 *  
 */
#include "../../include/LoRa_main.h"

static sx1262_handle_t test_LoRa_handle;

/**
 * @brief  sx1262 lora irq
 * @return status code
 *         - 0 success
 *         - 1 run failed
 * @note   none
 */
uint8_t sx1262_lora_irq_handler(void)
{
    if (sx1262_irq_handler(&test_LoRa_handle) != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief     call basic functions such as rf freq and tx params in one function
 * @return    status code
 *            - 0 success
 *            - 1 init failed
 * @note      none
 */
uint8_t sx1262_lora_begin(void)
{
    uint8_t res;
    uint32_t reg;
    uint8_t modulation;
    uint8_t config;
    
    
    /* init the sx1262 */
    res = sx1262_device_init(&test_LoRa_handle);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: init failed.\n");
       
        return 1;
    }
    
    /* enter standby */
    res = sx1262_set_standby(&test_LoRa_handle, SX1262_CLOCK_SOURCE_XTAL_32MHZ);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set standby failed.\n");
        (void)sx1262_deinit(&test_LoRa_handle);
        
        return 1;
    }
    
    /* set stop timer on preamble */
    res = sx1262_set_stop_timer_on_preamble(&test_LoRa_handle, SX1262_LORA_DEFAULT_STOP_TIMER_ON_PREAMBLE);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: stop timer on preamble failed.\n");
        (void)sx1262_deinit(&test_LoRa_handle);
        
        return 1;
    }
    
    /* set regulator mode */
    res = sx1262_set_regulator_mode(&test_LoRa_handle, SX1262_LORA_DEFAULT_REGULATOR_MODE);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set regulator mode failed.\n");
        (void)sx1262_deinit(&test_LoRa_handle);
        
        return 1;
    }
    
    /* set pa config */
    res = sx1262_set_pa_config(&test_LoRa_handle, SX1262_LORA_DEFAULT_PA_CONFIG_DUTY_CYCLE, SX1262_LORA_DEFAULT_PA_CONFIG_HP_MAX);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set pa config failed.\n");
        (void)sx1262_deinit(&test_LoRa_handle);
        
        return 1;
    }
    
    /* enter to stdby xosc mode */
    res = sx1262_set_rx_tx_fallback_mode(&test_LoRa_handle, SX1262_RX_TX_FALLBACK_MODE_STDBY_XOSC);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set rx tx fallback mode failed.\n");
        (void)sx1262_deinit(&test_LoRa_handle);
        
        return 1;
    }
    
    /* set dio irq */
    res = sx1262_set_dio_irq_params(&test_LoRa_handle, 0x03FF, 0x03FF, 0x0000, 0x0000);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set dio irq params failed.\n");
        (void)sx1262_deinit(&test_LoRa_handle);
        
        return 1;
    }
    
    /* clear irq status */
    res = sx1262_clear_irq_status(&test_LoRa_handle, 0x03FF);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: clear irq status failed.\n");
        (void)sx1262_deinit(&test_LoRa_handle);
        
        return 1;
    }
    
    /* set lora mode */
    res = sx1262_set_packet_type(&test_LoRa_handle, SX1262_PACKET_TYPE_LORA);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set packet type failed.\n");
        (void)sx1262_deinit(&test_LoRa_handle);
        
        return 1;
    }
    
    /* set tx params */
    res = sx1262_set_tx_params(&test_LoRa_handle, SX1262_LORA_DEFAULT_TX_DBM, SX1262_LORA_DEFAULT_RAMP_TIME);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set tx params failed.\n");
        (void)sx1262_deinit(&test_LoRa_handle);
        
        return 1;
    }
    
    /* set lora modulation params */
    res = sx1262_set_lora_modulation_params(&test_LoRa_handle, SX1262_LORA_DEFAULT_SF, SX1262_LORA_DEFAULT_BANDWIDTH, 
                                            SX1262_LORA_DEFAULT_CR, SX1262_LORA_DEFAULT_LOW_DATA_RATE_OPTIMIZE);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set lora modulation params failed.\n");
        (void)sx1262_deinit(&test_LoRa_handle);
        
        return 1;
    }
    
    /* convert the frequency */
    res = sx1262_frequency_convert_to_register(&test_LoRa_handle, SX1262_LORA_DEFAULT_RF_FREQUENCY, (uint32_t *)&reg);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: convert to register failed.\n");
        (void)sx1262_deinit(&test_LoRa_handle);
        
        return 1;
    }
    
    /* set the frequency */
    res = sx1262_set_rf_frequency(&test_LoRa_handle, reg);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set rf frequency failed.\n");
        (void)sx1262_deinit(&test_LoRa_handle);
        
        return 1;
    }
    
    /* set base address */
    res = sx1262_set_buffer_base_address(&test_LoRa_handle, 0x00, 0x00);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set buffer base address failed.\n");
        (void)sx1262_deinit(&test_LoRa_handle);
        
        return 1;
    }
    
    /* set lora symb num */
    res = sx1262_set_lora_symb_num_timeout(&test_LoRa_handle, SX1262_LORA_DEFAULT_SYMB_NUM_TIMEOUT);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set lora symb num timeout failed.\n");
        (void)sx1262_deinit(&test_LoRa_handle);
        
        return 1;
    }
    
    /* reset stats */
    res = sx1262_reset_stats(&test_LoRa_handle, 0x0000, 0x0000, 0x0000);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: reset stats failed.\n");
        (void)sx1262_deinit(&test_LoRa_handle);
        
        return 1;
    }
    
    /* clear device errors */
    res = sx1262_clear_device_errors(&test_LoRa_handle);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: clear device errors failed.\n");
        (void)sx1262_deinit(&test_LoRa_handle);
        
        return 1;
    }
    
    /* set the lora sync word */
    res = sx1262_set_lora_sync_word(&test_LoRa_handle, SX1262_LORA_DEFAULT_SYNC_WORD);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set lora sync word failed.\n");
        (void)sx1262_deinit(&test_LoRa_handle);
        
        return 1;
    }
    
    /* get tx modulation */
    res = sx1262_get_tx_modulation(&test_LoRa_handle, (uint8_t *)&modulation);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get tx modulation failed.\n");
        (void)sx1262_deinit(&test_LoRa_handle);
        
        return 1;
    }
    modulation |= 0x04;
    
    /* set the tx modulation */
    res = sx1262_set_tx_modulation(&test_LoRa_handle, modulation);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set tx modulation failed.\n");
        (void)sx1262_deinit(&test_LoRa_handle);
        
        return 1;
    }
    
    /* set the rx gain */
    res = sx1262_set_rx_gain(&test_LoRa_handle, SX1262_LORA_DEFAULT_RX_GAIN);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set rx gain failed.\n");
        (void)sx1262_deinit(&test_LoRa_handle);
        
        return 1;
    }
    
    /* set the ocp */
    res = sx1262_set_ocp(&test_LoRa_handle, SX1262_LORA_DEFAULT_OCP);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set ocp failed.\n");
        (void)sx1262_deinit(&test_LoRa_handle);
        
        return 1;
    }
    
    /* get the tx clamp config */
    res = sx1262_get_tx_clamp_config(&test_LoRa_handle, (uint8_t *)&config);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get tx clamp config failed.\n");
        (void)sx1262_deinit(&test_LoRa_handle);
        
        return 1;
    }
    config |= 0x1E;
    
    /* set the tx clamp config */
    res = sx1262_set_tx_clamp_config(&test_LoRa_handle, config);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set tx clamp config failed.\n");
        (void)sx1262_deinit(&test_LoRa_handle);
        
        return 1;
    }
    
    return 0;
}

/**
 * @brief  lora example enter to the sleep mode
 * @return status code
 *         - 0 success
 *         - 1 sleep failed
 * @note   none
 */
uint8_t sx1262_lora_sleep(void)
{
    if (sx1262_set_sleep(&test_LoRa_handle, SX1262_LORA_DEFAULT_START_MODE, SX1262_LORA_DEFAULT_RTC_WAKE_UP) != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief  lora example wake up the chip
 * @return status code
 *         - 0 success
 *         - 1 wake up failed
 * @note   none
 */
uint8_t sx1262_lora_wake_up(void)
{
    uint8_t status;
    
    if (sx1262_get_status(&test_LoRa_handle, (uint8_t *)&status) != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief  lora example enter to the continuous receive mode
 * @return status code
 *         - 0 success
 *         - 1 enter failed
 * @note   none
 */
uint8_t sx1262_lora_set_continuous_receive_mode(void)
{
    uint8_t setup;
    
    /* set dio irq */
    if (sx1262_set_dio_irq_params(&test_LoRa_handle, SX1262_IRQ_RX_DONE | SX1262_IRQ_TIMEOUT | SX1262_IRQ_CRC_ERR | SX1262_IRQ_CAD_DONE | SX1262_IRQ_CAD_DETECTED,
                                  SX1262_IRQ_RX_DONE | SX1262_IRQ_TIMEOUT | SX1262_IRQ_CRC_ERR | SX1262_IRQ_CAD_DONE | SX1262_IRQ_CAD_DETECTED,
                                  0x0000, 0x0000) != 0)
    {
        return 1;
    }
    
    /* clear irq status */
    if (sx1262_clear_irq_status(&test_LoRa_handle, 0x03FFU) != 0)
    {
        return 1;
    }
    
    /* set lora packet params */
    if (sx1262_set_lora_packet_params(&test_LoRa_handle, SX1262_LORA_DEFAULT_PREAMBLE_LENGTH,
                                      SX1262_LORA_DEFAULT_HEADER, SX1262_LORA_DEFAULT_BUFFER_SIZE,
                                      SX1262_LORA_DEFAULT_CRC_TYPE, SX1262_LORA_DEFAULT_INVERT_IQ) != 0)
    {
        return 1;
    }
    
    /* get iq polarity */
    if (sx1262_get_iq_polarity(&test_LoRa_handle, (uint8_t *)&setup) != 0)
    {
        return 1;
    }
    
#if SX1262_LORA_DEFAULT_INVERT_IQ == SX1262_BOOL_FALSE
    setup |= 1 << 2;
#else
    setup &= ~(1 << 2);
#endif
    
    /* set the iq polarity */
    if (sx1262_set_iq_polarity(&test_LoRa_handle, setup) != 0)
    {
        return 1;
    }
    
    /* start receive */
    if (sx1262_continuous_receive(&test_LoRa_handle) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief     lora example enter to the shot receive mode
 * @param[in] us delay in us
 * @return    status code
 *            - 0 success
 *            - 1 set failed
 * @note      none
 */
uint8_t sx1262_lora_set_shot_receive_mode(double us)
{
    uint8_t setup;
    
    /* set dio irq */
    if (sx1262_set_dio_irq_params(&test_LoRa_handle, SX1262_IRQ_RX_DONE | SX1262_IRQ_TIMEOUT | SX1262_IRQ_CRC_ERR | SX1262_IRQ_CAD_DONE | SX1262_IRQ_CAD_DETECTED,
                                  SX1262_IRQ_RX_DONE | SX1262_IRQ_TIMEOUT | SX1262_IRQ_CRC_ERR | SX1262_IRQ_CAD_DONE | SX1262_IRQ_CAD_DETECTED,
                                  0x0000, 0x0000) != 0)
    {
        return 1;
    }
    
    /* clear irq status */
    if (sx1262_clear_irq_status(&test_LoRa_handle, 0x03FFU) != 0)
    {
        return 1;
    }
    
    /* set lora packet params */
    if (sx1262_set_lora_packet_params(&test_LoRa_handle, SX1262_LORA_DEFAULT_PREAMBLE_LENGTH,
                                      SX1262_LORA_DEFAULT_HEADER, SX1262_LORA_DEFAULT_BUFFER_SIZE,
                                      SX1262_LORA_DEFAULT_CRC_TYPE, SX1262_LORA_DEFAULT_INVERT_IQ) != 0)
    {
        return 1;
    }
    
    /* get iq polarity */
    if (sx1262_get_iq_polarity(&test_LoRa_handle, (uint8_t *)&setup) != 0)
    {
        return 1;
    }
    
#if SX1262_LORA_DEFAULT_INVERT_IQ == SX1262_BOOL_FALSE
    setup |= 1 << 2;
#else
    setup &= ~(1 << 2);
#endif
    
    /* set the iq polarity */
    if (sx1262_set_iq_polarity(&test_LoRa_handle, setup) != 0)
    {
        return 1;
    }
    
    /* start receive */
    if (sx1262_single_receive(&test_LoRa_handle, us) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief  lora example enter to the send mode
 * @return status code
 *         - 0 success
 *         - 1 enter failed
 * @note   none
 */
uint8_t sx1262_lora_set_send_mode(void)
{
    /* set dio irq */
    if (sx1262_set_dio_irq_params(&test_LoRa_handle, SX1262_IRQ_TX_DONE | SX1262_IRQ_TIMEOUT | SX1262_IRQ_CAD_DONE | SX1262_IRQ_CAD_DETECTED,
                                  SX1262_IRQ_TX_DONE | SX1262_IRQ_TIMEOUT | SX1262_IRQ_CAD_DONE | SX1262_IRQ_CAD_DETECTED,
                                  0x0000, 0x0000) != 0)
    {
        return 1;
    }
    
    /* clear irq status */
    if (sx1262_clear_irq_status(&test_LoRa_handle, 0x03FFU) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief     lora example send lora data
 * @param[in] *buf pointer to a data buffer
 * @param[in] len data length
 * @return    status code
 *            - 0 success
 *            - 1 send failed
 * @note      none
 */
uint8_t sx1262_lora_send(uint8_t *buf, uint16_t len)
{
    /* send the data */
    if (sx1262_lora_transmit(&test_LoRa_handle, SX1262_CLOCK_SOURCE_XTAL_32MHZ,
                             SX1262_LORA_DEFAULT_PREAMBLE_LENGTH, SX1262_LORA_DEFAULT_HEADER,
                             SX1262_LORA_DEFAULT_CRC_TYPE, SX1262_LORA_DEFAULT_INVERT_IQ,
                            (uint8_t *)buf, len, 0) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief      lora example run the cad
 * @param[out] *enable pointer to a enable buffer
 * @return     status code
 *             - 0 success
 *             - 1 run failed
 * @note       none
 */
uint8_t sx1262_lora_run_cad(sx1262_bool_t *enable)
{
    /* set cad params */
    if (sx1262_set_cad_params(&test_LoRa_handle, SX1262_LORA_DEFAULT_CAD_SYMBOL_NUM,
                              SX1262_LORA_DEFAULT_CAD_DET_PEAK, SX1262_LORA_DEFAULT_CAD_DET_MIN,
                              SX1262_LORA_CAD_EXIT_MODE_ONLY, 0) != 0)
    {
        return 1;
    }
    
    /* run the cad */
    if (sx1262_lora_cad(&test_LoRa_handle, enable) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief      lora example get the status
 * @param[out] *rssi pointer to a rssi buffer
 * @param[out] *snr pointer to a snr buffer
 * @return     status code
 *             - 0 success
 *             - 1 get status failed
 * @note       none
 */
uint8_t sx1262_lora_get_status(float *rssi, float *snr)
{
    uint8_t rssi_pkt_raw;
    uint8_t snr_pkt_raw;
    uint8_t signal_rssi_pkt_raw;
    float signal_rssi_pkt;
    
    /* get the status */
    if (sx1262_get_lora_packet_status(&test_LoRa_handle, (uint8_t *)&rssi_pkt_raw, (uint8_t *)&snr_pkt_raw,
                                     (uint8_t *)&signal_rssi_pkt_raw, (float *)rssi, (float *)snr, (float *)&signal_rssi_pkt) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief      lora example check packet error
 * @param[out] *enable pointer to a bool value buffer
 * @return     status code
 *             - 0 success
 *             - 1 check packet error failed
 * @note       none
 */
uint8_t sx1262_lora_check_packet_error(sx1262_bool_t *enable)
{
    /* check the error */
    if (sx1262_check_packet_error(&test_LoRa_handle, enable) != 0)
    {
        return 1;
    }

    return 0;
}
