#ifndef _RA01S_H
#define _RA01S_H

#include "driver/spi_master.h"

#define MAX_BUFF 256

//return values
#define ERR_NONE                        0
#define ERR_PACKET_TOO_LONG             1
#define ERR_UNKNOWN                     2
#define ERR_TX_TIMEOUT                  3
#define ERR_RX_TIMEOUT                  4
#define ERR_CRC_MISMATCH                5
#define ERR_WRONG_MODEM                 6
#define ERR_INVALID_BANDWIDTH           7
#define ERR_INVALID_SPREADING_FACTOR    8
#define ERR_INVALID_CODING_RATE         9
#define ERR_INVALID_FREQUENCY_DEVIATION 10
#define ERR_INVALID_BIT_RATE            11
#define ERR_INVALID_RX_BANDWIDTH        12
#define ERR_INVALID_DATA_SHAPING        13
#define ERR_INVALID_SYNC_WORD           14
#define ERR_INVALID_OUTPUT_POWER        15
#define ERR_INVALID_MODE                16
#define ERR_INVALID_TRANCEIVER          17
#define ERR_INVALID_SETRX_STATE         18
#define ERR_INVALID_SETTX_STATE         19
#define ERR_IDLE_TIMEOUT                20
#define ERR_SPI_TRANSACTION             21

// SX126X physical layer properties
#define XTAL_FREQ                       ( double )32000000
#define FREQ_DIV                        ( double )pow( 2.0, 25.0 )
#define FREQ_STEP                       ( double )( XTAL_FREQ / FREQ_DIV )

#define LOW                             0
#define HIGH                            1
#define BUSY_WAIT                       5000

// SX126X Model
#define SX1261_TRANCEIVER                             0x01
#define SX1262_TRANCEIVER                             0x02
#define SX1268_TRANCEIVER                             0x08

// SX126X SPI commands
// operational modes commands
#define SX126X_CMD_NOP                                0x00
#define SX126X_CMD_SET_SLEEP                          0x84
#define SX126X_CMD_SET_STANDBY                        0x80
#define SX126X_CMD_SET_FS                             0xC1
#define SX126X_CMD_SET_TX                             0x83
#define SX126X_CMD_SET_RX                             0x82
#define SX126X_CMD_STOP_TIMER_ON_PREAMBLE             0x9F
#define SX126X_CMD_SET_RX_DUTY_CYCLE                  0x94
#define SX126X_CMD_SET_CAD                            0xC5
#define SX126X_CMD_SET_TX_CONTINUOUS_WAVE             0xD1
#define SX126X_CMD_SET_TX_INFINITE_PREAMBLE           0xD2
#define SX126X_CMD_SET_REGULATOR_MODE                 0x96
#define SX126X_CMD_CALIBRATE                          0x89
#define SX126X_CMD_CALIBRATE_IMAGE                    0x98
#define SX126X_CMD_SET_PA_CONFIG                      0x95
#define SX126X_CMD_SET_RX_TX_FALLBACK_MODE            0x93

// register and buffer access commands
#define SX126X_CMD_WRITE_REGISTER                     0x0D
#define SX126X_CMD_READ_REGISTER                      0x1D
#define SX126X_CMD_WRITE_BUFFER                       0x0E
#define SX126X_CMD_READ_BUFFER                        0x1E

// DIO and IRQ control
#define SX126X_CMD_SET_DIO_IRQ_PARAMS                 0x08
#define SX126X_CMD_GET_IRQ_STATUS                     0x12
#define SX126X_CMD_CLEAR_IRQ_STATUS                   0x02
#define SX126X_CMD_SET_DIO2_AS_RF_SWITCH_CTRL         0x9D
#define SX126X_CMD_SET_DIO3_AS_TCXO_CTRL              0x97

// RF, modulation and packet commands
#define SX126X_CMD_SET_RF_FREQUENCY                   0x86
#define SX126X_CMD_SET_PACKET_TYPE                    0x8A
#define SX126X_CMD_GET_PACKET_TYPE                    0x11
#define SX126X_CMD_SET_TX_PARAMS                      0x8E
#define SX126X_CMD_SET_MODULATION_PARAMS              0x8B
#define SX126X_CMD_SET_PACKET_PARAMS                  0x8C
#define SX126X_CMD_SET_CAD_PARAMS                     0x88
#define SX126X_CMD_SET_BUFFER_BASE_ADDRESS            0x8F
#define SX126X_CMD_SET_LORA_SYMB_NUM_TIMEOUT          0xA0

#define SX126X_PA_CONFIG_SX1261                       0x01
#define SX126X_PA_CONFIG_SX1262                       0x00

// status commands
#define SX126X_CMD_GET_STATUS                         0xC0
#define SX126X_CMD_GET_RSSI_INST                      0x15
#define SX126X_CMD_GET_RX_BUFFER_STATUS               0x13
#define SX126X_CMD_GET_PACKET_STATUS                  0x14
#define SX126X_CMD_GET_DEVICE_ERRORS                  0x17
#define SX126X_CMD_CLEAR_DEVICE_ERRORS                0x07
#define SX126X_CMD_GET_STATS                          0x10
#define SX126X_CMD_RESET_STATS                        0x00


// SX126X register map
#define SX126X_REG_HOPPING_ENABLE                     0x0385
#define SX126X_REG_PACKECT_LENGTH                     0x0386
#define SX126X_REG_NB_HOPPING_BLOCKS                  0x0387
#define SX126X_REG_NB_SYMBOLS0                        0x0388
#define SX126X_REG_FREQ0                              0x038A
#define SX126X_REG_NB_SYMBOLS15                       0x03E2
#define SX126X_REG_FREQ15                             0x03E4
#define SX126X_REG_DIOX_OUTPUT_ENABLE                 0x0580
#define SX126X_REG_DIOX_INPUT_ENABLE                  0x0583
#define SX126X_REG_DIOX_PILL_UP_CONTROL               0x0584
#define SX126X_REG_DIOX_PULL_DOWN_CONTROL             0x0585
#define SX126X_REG_WHITENING_INITIAL_MSB              0x06B8
#define SX126X_REG_WHITENING_INITIAL_LSB              0x06B9
#define SX126X_REG_CRC_INITIAL_MSB                    0x06BC
#define SX126X_REG_CRC_INITIAL_LSB                    0x06BD
#define SX126X_REG_CRC_POLYNOMIAL_MSB                 0x06BE
#define SX126X_REG_CRC_POLYNOMIAL_LSB                 0x06BF
#define SX126X_REG_SYNC_WORD_0                        0x06C0
#define SX126X_REG_SYNC_WORD_1                        0x06C1
#define SX126X_REG_SYNC_WORD_2                        0x06C2
#define SX126X_REG_SYNC_WORD_3                        0x06C3
#define SX126X_REG_SYNC_WORD_4                        0x06C4
#define SX126X_REG_SYNC_WORD_5                        0x06C5
#define SX126X_REG_SYNC_WORD_6                        0x06C6
#define SX126X_REG_SYNC_WORD_7                        0x06C7
#define SX126X_REG_NODE_ADDRESS                       0x06CD
#define SX126X_REG_BROADCAST_ADDRESS                  0x06CE
#define SX126X_REG_IQ_POLARITY_SETUP                  0x0736
#define SX126X_REG_LORA_SYNC_WORD_MSB                 0x0740
#define SX126X_REG_LORA_SYNC_WORD_LSB                 0x0741
#define SX126X_REG_RANDOM_NUMBER_0                    0x0819
#define SX126X_REG_RANDOM_NUMBER_1                    0x081A
#define SX126X_REG_RANDOM_NUMBER_2                    0x081B
#define SX126X_REG_RANDOM_NUMBER_3                    0x081C
#define SX126X_REG_TX_MODULETION                      0x0889
#define SX126X_REG_RX_GAIN                            0x08AC
#define SX126X_REG_TX_CLAMP_CONFIG                    0x08D8
#define SX126X_REG_OCP_CONFIGURATION                  0x08E7
#define SX126X_REG_RTC_CONTROL                        0x0902
#define SX126X_REG_XTA_TRIM                           0x0911
#define SX126X_REG_XTB_TRIM                           0x0912
#define SX126X_REG_DIO3_OUTPUT_VOLTAGE_CONTROL        0x0920
#define SX126X_REG_EVENT_MASK                         0x0944


// SX126X SPI command variables
//SX126X_CMD_SET_SLEEP
#define SX126X_SLEEP_START_COLD                       0b00000000  //  2     2     sleep mode: cold start, configuration is lost (default)
#define SX126X_SLEEP_START_WARM                       0b00000100  //  2     2                 warm start, configuration is retained
#define SX126X_SLEEP_RTC_OFF                          0b00000000  //  0     0     wake on RTC timeout: disabled
#define SX126X_SLEEP_RTC_ON                           0b00000001  //  0     0                          enabled

//SX126X_CMD_SET_STANDBY
#define SX126X_STANDBY_RC                             0x00        //  7     0     standby mode: 13 MHz RC oscillator
#define SX126X_STANDBY_XOSC                           0x01        //  7     0                   32 MHz crystal oscillator

//SX126X_CMD_SET_RX
#define SX126X_RX_TIMEOUT_NONE                        0x000000    //  23    0     Rx timeout duration: no timeout (Rx single mode)
#define SX126X_RX_TIMEOUT_INF                         0xFFFFFF    //  23    0                          infinite (Rx continuous mode)

//SX126X_CMD_SET_RXGAIN
#define SX126X_RX_GAIN_OFF                            0x94        //Rx Power Saving gain
#define SX126X_RX_GAIN_ON                             0x96        //Rx Boosted gain

//SX126X_CMD_STOP_TIMER_ON_PREAMBLE
#define SX126X_STOP_ON_PREAMBLE_OFF                   0x00        //  7     0     stop timer on: sync word or header (default)
#define SX126X_STOP_ON_PREAMBLE_ON                    0x01        //  7     0                    preamble detection

//SX126X_CMD_SET_REGULATOR_MODE
#define SX126X_REGULATOR_LDO                          0x00        //  7     0     set regulator mode: LDO (default)
#define SX126X_REGULATOR_DC_DC                        0x01        //  7     0                         DC-DC

//SX126X_CMD_CALIBRATE
#define SX126X_CALIBRATE_IMAGE_OFF                    0b00000000  //  6     6     image calibration: disabled
#define SX126X_CALIBRATE_IMAGE_ON                     0b01000000  //  6     6                        enabled
#define SX126X_CALIBRATE_ADC_BULK_P_OFF               0b00000000  //  5     5     ADC bulk P calibration: disabled
#define SX126X_CALIBRATE_ADC_BULK_P_ON                0b00100000  //  5     5                             enabled
#define SX126X_CALIBRATE_ADC_BULK_N_OFF               0b00000000  //  4     4     ADC bulk N calibration: disabled
#define SX126X_CALIBRATE_ADC_BULK_N_ON                0b00010000  //  4     4                             enabled
#define SX126X_CALIBRATE_ADC_PULSE_OFF                0b00000000  //  3     3     ADC pulse calibration: disabled
#define SX126X_CALIBRATE_ADC_PULSE_ON                 0b00001000  //  3     3                            enabled
#define SX126X_CALIBRATE_PLL_OFF                      0b00000000  //  2     2     PLL calibration: disabled
#define SX126X_CALIBRATE_PLL_ON                       0b00000100  //  2     2                      enabled
#define SX126X_CALIBRATE_RC13M_OFF                    0b00000000  //  1     1     13 MHz RC osc. calibration: disabled
#define SX126X_CALIBRATE_RC13M_ON                     0b00000010  //  1     1                                 enabled
#define SX126X_CALIBRATE_RC64K_OFF                    0b00000000  //  0     0     64 kHz RC osc. calibration: disabled
#define SX126X_CALIBRATE_RC64K_ON                     0b00000001  //  0     0                                 enabled

//SX126X_CMD_CALIBRATE_IMAGE
#define SX126X_CAL_IMG_430_MHZ_1                      0x6B
#define SX126X_CAL_IMG_430_MHZ_2                      0x6F
#define SX126X_CAL_IMG_470_MHZ_1                      0x75
#define SX126X_CAL_IMG_470_MHZ_2                      0x81
#define SX126X_CAL_IMG_779_MHZ_1                      0xC1
#define SX126X_CAL_IMG_779_MHZ_2                      0xC5
#define SX126X_CAL_IMG_863_MHZ_1                      0xD7
#define SX126X_CAL_IMG_863_MHZ_2                      0xDB
#define SX126X_CAL_IMG_902_MHZ_1                      0xE1
#define SX126X_CAL_IMG_902_MHZ_2                      0xE9

//SX126X_CMD_SET_PA_CONFIG
#define SX126X_PA_CONFIG_HP_MAX                       0x07
#define SX126X_PA_CONFIG_SX1268                       0x01
#define SX126X_PA_CONFIG_PA_LUT                       0x01

//SX126X_CMD_SET_RX_TX_FALLBACK_MODE
#define SX126X_RX_TX_FALLBACK_MODE_FS                 0x40        //  7     0     after Rx/Tx go to: FS mode
#define SX126X_RX_TX_FALLBACK_MODE_STDBY_XOSC         0x30        //  7     0                        standby with crystal oscillator
#define SX126X_RX_TX_FALLBACK_MODE_STDBY_RC           0x20        //  7     0                        standby with RC oscillator (default)

//SX126X_CMD_SET_DIO_IRQ_PARAMS
#define SX126X_IRQ_TIMEOUT                            0b1000000000  //  9     9     Rx or Tx timeout
#define SX126X_IRQ_CAD_DETECTED                       0b0100000000  //  8     8     channel activity detected
#define SX126X_IRQ_CAD_DONE                           0b0010000000  //  7     7     channel activity detection finished
#define SX126X_IRQ_CRC_ERR                            0b0001000000  //  6     6     wrong CRC received
#define SX126X_IRQ_HEADER_ERR                         0b0000100000  //  5     5     LoRa header CRC error
#define SX126X_IRQ_HEADER_VALID                       0b0000010000  //  4     4     valid LoRa header received
#define SX126X_IRQ_SYNC_WORD_VALID                    0b0000001000  //  3     3     valid sync word detected
#define SX126X_IRQ_PREAMBLE_DETECTED                  0b0000000100  //  2     2     preamble detected
#define SX126X_IRQ_RX_DONE                            0b0000000010  //  1     1     packet received
#define SX126X_IRQ_TX_DONE                            0b0000000001  //  0     0     packet transmission completed
#define SX126X_IRQ_ALL                                0b1111111111  //  9     0     all interrupts
#define SX126X_IRQ_NONE                               0b0000000000  //  9     0     no interrupts

//SX126X_CMD_SET_DIO2_AS_RF_SWITCH_CTRL
#define SX126X_DIO2_AS_IRQ                            0x00        //  7     0     DIO2 configuration: IRQ
#define SX126X_DIO2_AS_RF_SWITCH                      0x01        //  7     0                         RF switch control

//SX126X_CMD_SET_DIO3_AS_TCXO_CTRL
#define SX126X_DIO3_OUTPUT_1_6                        0x00        //  7     0     DIO3 voltage output for TCXO: 1.6 V
#define SX126X_DIO3_OUTPUT_1_7                        0x01        //  7     0                                   1.7 V
#define SX126X_DIO3_OUTPUT_1_8                        0x02        //  7     0                                   1.8 V
#define SX126X_DIO3_OUTPUT_2_2                        0x03        //  7     0                                   2.2 V
#define SX126X_DIO3_OUTPUT_2_4                        0x04        //  7     0                                   2.4 V
#define SX126X_DIO3_OUTPUT_2_7                        0x05        //  7     0                                   2.7 V
#define SX126X_DIO3_OUTPUT_3_0                        0x06        //  7     0                                   3.0 V
#define SX126X_DIO3_OUTPUT_3_3                        0x07        //  7     0                                   3.3 V

//Radio complete Wake-up Time with TCXO stabilisation time
#define RADIO_TCXO_SETUP_TIME                         5000        // [us]

//SX126X_CMD_SET_PACKET_TYPE
#define SX126X_PACKET_TYPE_GFSK                       0x00        //  7     0     packet type: GFSK
#define SX126X_PACKET_TYPE_LORA                       0x01        //  7     0                  LoRa

//SX126X_CMD_SET_TX_PARAMS
#define SX126X_PA_RAMP_10U                            0x00        //  7     0     ramp time: 10 us
#define SX126X_PA_RAMP_20U                            0x01        //  7     0                20 us
#define SX126X_PA_RAMP_40U                            0x02        //  7     0                40 us
#define SX126X_PA_RAMP_80U                            0x03        //  7     0                80 us
#define SX126X_PA_RAMP_200U                           0x04        //  7     0                200 us
#define SX126X_PA_RAMP_800U                           0x05        //  7     0                800 us
#define SX126X_PA_RAMP_1700U                          0x06        //  7     0                1700 us
#define SX126X_PA_RAMP_3400U                          0x07        //  7     0                3400 us

//SX126X_CMD_SET_MODULATION_PARAMS
#define SX126X_GFSK_FILTER_NONE                       0x00        //  7     0     GFSK filter: none
#define SX126X_GFSK_FILTER_GAUSS_0_3                  0x08        //  7     0                  Gaussian, BT = 0.3
#define SX126X_GFSK_FILTER_GAUSS_0_5                  0x09        //  7     0                  Gaussian, BT = 0.5
#define SX126X_GFSK_FILTER_GAUSS_0_7                  0x0A        //  7     0                  Gaussian, BT = 0.7
#define SX126X_GFSK_FILTER_GAUSS_1                    0x0B        //  7     0                  Gaussian, BT = 1
#define SX126X_GFSK_RX_BW_4_8                         0x1F        //  7     0     GFSK Rx bandwidth: 4.8 kHz
#define SX126X_GFSK_RX_BW_5_8                         0x17        //  7     0                        5.8 kHz
#define SX126X_GFSK_RX_BW_7_3                         0x0F        //  7     0                        7.3 kHz
#define SX126X_GFSK_RX_BW_9_7                         0x1E        //  7     0                        9.7 kHz
#define SX126X_GFSK_RX_BW_11_7                        0x16        //  7     0                        11.7 kHz
#define SX126X_GFSK_RX_BW_14_6                        0x0E        //  7     0                        14.6 kHz
#define SX126X_GFSK_RX_BW_19_5                        0x1D        //  7     0                        19.5 kHz
#define SX126X_GFSK_RX_BW_23_4                        0x15        //  7     0                        23.4 kHz
#define SX126X_GFSK_RX_BW_29_3                        0x0D        //  7     0                        29.3 kHz
#define SX126X_GFSK_RX_BW_39_0                        0x1C        //  7     0                        39.0 kHz
#define SX126X_GFSK_RX_BW_46_9                        0x14        //  7     0                        46.9 kHz
#define SX126X_GFSK_RX_BW_58_6                        0x0C        //  7     0                        58.6 kHz
#define SX126X_GFSK_RX_BW_78_2                        0x1B        //  7     0                        78.2 kHz
#define SX126X_GFSK_RX_BW_93_8                        0x13        //  7     0                        93.8 kHz
#define SX126X_GFSK_RX_BW_117_3                       0x0B        //  7     0                        117.3 kHz
#define SX126X_GFSK_RX_BW_156_2                       0x1A        //  7     0                        156.2 kHz
#define SX126X_GFSK_RX_BW_187_2                       0x12        //  7     0                        187.2 kHz
#define SX126X_GFSK_RX_BW_234_3                       0x0A        //  7     0                        234.3 kHz
#define SX126X_GFSK_RX_BW_312_0                       0x19        //  7     0                        312.0 kHz
#define SX126X_GFSK_RX_BW_373_6                       0x11        //  7     0                        373.6 kHz
#define SX126X_GFSK_RX_BW_467_0                       0x09        //  7     0                        467.0 kHz
#define SX126X_LORA_BW_7_8                            0x00        //  7     0     LoRa bandwidth: 7.8 kHz
#define SX126X_LORA_BW_10_4                           0x08        //  7     0                     10.4 kHz
#define SX126X_LORA_BW_15_6                           0x01        //  7     0                     15.6 kHz
#define SX126X_LORA_BW_20_8                           0x09        //  7     0                     20.8 kHz
#define SX126X_LORA_BW_31_25                          0x02        //  7     0                     31.25 kHz
#define SX126X_LORA_BW_41_7                           0x0A        //  7     0                     41.7 kHz
#define SX126X_LORA_BW_62_5                           0x03        //  7     0                     62.5 kHz
#define SX126X_LORA_BW_125_0                          0x04        //  7     0                     125.0 kHz
#define SX126X_LORA_BW_250_0                          0x05        //  7     0                     250.0 kHz
#define SX126X_LORA_BW_500_0                          0x06        //  7     0                     500.0 kHz
#define SX126X_LORA_CR_4_5                            0x01        //  7     0     LoRa coding rate: 4/5
#define SX126X_LORA_CR_4_6                            0x02        //  7     0                       4/6
#define SX126X_LORA_CR_4_7                            0x03        //  7     0                       4/7
#define SX126X_LORA_CR_4_8                            0x04        //  7     0                       4/8
#define SX126X_LORA_LOW_DATA_RATE_OPTIMIZE_OFF        0x00        //  7     0     LoRa low data rate optimization: disabled
#define SX126X_LORA_LOW_DATA_RATE_OPTIMIZE_ON         0x01        //  7     0                                      enabled

//SX126X_CMD_SET_PACKET_PARAMS
#define SX126X_GFSK_PREAMBLE_DETECT_OFF               0x00        //  7     0     GFSK minimum preamble length before reception starts: detector disabled
#define SX126X_GFSK_PREAMBLE_DETECT_8                 0x04        //  7     0                                                           8 bits
#define SX126X_GFSK_PREAMBLE_DETECT_16                0x05        //  7     0                                                           16 bits
#define SX126X_GFSK_PREAMBLE_DETECT_24                0x06        //  7     0                                                           24 bits
#define SX126X_GFSK_PREAMBLE_DETECT_32                0x07        //  7     0                                                           32 bits
#define SX126X_GFSK_ADDRESS_FILT_OFF                  0x00        //  7     0     GFSK address filtering: disabled
#define SX126X_GFSK_ADDRESS_FILT_NODE                 0x01        //  7     0                             node only
#define SX126X_GFSK_ADDRESS_FILT_NODE_BROADCAST       0x02        //  7     0                             node and broadcast
#define SX126X_GFSK_PACKET_FIXED                      0x00        //  7     0     GFSK packet type: fixed (payload length known in advance to both sides)
#define SX126X_GFSK_PACKET_VARIABLE                   0x01        //  7     0                       variable (payload length added to packet)
#define SX126X_GFSK_CRC_OFF                           0x01        //  7     0     GFSK packet CRC: disabled
#define SX126X_GFSK_CRC_1_BYTE                        0x00        //  7     0                      1 byte
#define SX126X_GFSK_CRC_2_BYTE                        0x02        //  7     0                      2 byte
#define SX126X_GFSK_CRC_1_BYTE_INV                    0x04        //  7     0                      1 byte, inverted
#define SX126X_GFSK_CRC_2_BYTE_INV                    0x06        //  7     0                      2 byte, inverted
#define SX126X_GFSK_WHITENING_OFF                     0x00        //  7     0     GFSK data whitening: disabled
#define SX126X_GFSK_WHITENING_ON                      0x01        //  7     0                          enabled
#define SX126X_LORA_HEADER_EXPLICIT                   0x00        //  7     0     LoRa header mode: explicit
#define SX126X_LORA_HEADER_IMPLICIT                   0x01        //  7     0                       implicit
#define SX126X_LORA_CRC_OFF                           0x00        //  7     0     LoRa CRC mode: disabled
#define SX126X_LORA_CRC_ON                            0x01        //  7     0                    enabled
#define SX126X_LORA_IQ_STANDARD                       0x00        //  7     0     LoRa IQ setup: standard
#define SX126X_LORA_IQ_INVERTED                       0x01        //  7     0                    inverted

//SX126X_CMD_SET_CAD_PARAMS
#define SX126X_CAD_ON_1_SYMB                          0x00        //  7     0     number of symbols used for CAD: 1
#define SX126X_CAD_ON_2_SYMB                          0x01        //  7     0                                     2
#define SX126X_CAD_ON_4_SYMB                          0x02        //  7     0                                     4
#define SX126X_CAD_ON_8_SYMB                          0x03        //  7     0                                     8
#define SX126X_CAD_ON_16_SYMB                         0x04        //  7     0                                     16
#define SX126X_CAD_GOTO_STDBY                         0x00        //  7     0     after CAD is done, always go to STDBY_RC mode
#define SX126X_CAD_GOTO_RX                            0x01        //  7     0     after CAD is done, go to Rx mode if activity is detected

//SX126X_CMD_GET_STATUS
#define SX126X_STATUS_MODE_STDBY_RC                   0b00100000  //  6     4     current chip mode: STDBY_RC
#define SX126X_STATUS_MODE_STDBY_XOSC                 0b00110000  //  6     4                        STDBY_XOSC
#define SX126X_STATUS_MODE_FS                         0b01000000  //  6     4                        FS
#define SX126X_STATUS_MODE_RX                         0b01010000  //  6     4                        RX
#define SX126X_STATUS_MODE_TX                         0b01100000  //  6     4                        TX
#define SX126X_STATUS_DATA_AVAILABLE                  0b00000100  //  3     1     command status: packet received and data can be retrieved
#define SX126X_STATUS_CMD_TIMEOUT                     0b00000110  //  3     1                     SPI command timed out
#define SX126X_STATUS_CMD_INVALID                     0b00001000  //  3     1                     invalid SPI command
#define SX126X_STATUS_CMD_FAILED                      0b00001010  //  3     1                     SPI command failed to execute
#define SX126X_STATUS_TX_DONE                         0b00001100  //  3     1                     packet transmission done
#define SX126X_STATUS_SPI_FAILED                      0b11111111  //  7     0     SPI transaction failed

//SX126X_CMD_GET_PACKET_STATUS
#define SX126X_GFSK_RX_STATUS_PREAMBLE_ERR            0b10000000  //  7     7     GFSK Rx status: preamble error
#define SX126X_GFSK_RX_STATUS_SYNC_ERR                0b01000000  //  6     6                     sync word error
#define SX126X_GFSK_RX_STATUS_ADRS_ERR                0b00100000  //  5     5                     address error
#define SX126X_GFSK_RX_STATUS_CRC_ERR                 0b00010000  //  4     4                     CRC error
#define SX126X_GFSK_RX_STATUS_LENGTH_ERR              0b00001000  //  3     3                     length error
#define SX126X_GFSK_RX_STATUS_ABORT_ERR               0b00000100  //  2     2                     abort error
#define SX126X_GFSK_RX_STATUS_PACKET_RECEIVED         0b00000010  //  2     2                     packet received
#define SX126X_GFSK_RX_STATUS_PACKET_SENT             0b00000001  //  2     2                     packet sent

//SX126X_CMD_GET_DEVICE_ERRORS
#define SX126X_PA_RAMP_ERR                            0b100000000  //  8     8     device errors: PA ramping failed
#define SX126X_PLL_LOCK_ERR                           0b001000000  //  6     6                    PLL failed to lock
#define SX126X_XOSC_START_ERR                         0b000100000  //  5     5                    crystal oscillator failed to start
#define SX126X_IMG_CALIB_ERR                          0b000010000  //  4     4                    image calibration failed
#define SX126X_ADC_CALIB_ERR                          0b000001000  //  3     3                    ADC calibration failed
#define SX126X_PLL_CALIB_ERR                          0b000000100  //  2     2                    PLL calibration failed
#define SX126X_RC13M_CALIB_ERR                        0b000000010  //  1     1                    RC13M calibration failed
#define SX126X_RC64K_CALIB_ERR                        0b000000001  //  0     0                    RC64K calibration failed


// SX126X SPI register variables
//SX126X_REG_LORA_SYNC_WORD_MSB + LSB
#define SX126X_SYNC_WORD_PUBLIC                       0x3444
#define SX126X_SYNC_WORD_PRIVATE                      0x1424

#define SX126x_TXMODE_ASYNC                           0x01
#define SX126x_TXMODE_SYNC                            0x02
#define SX126x_TXMODE_BACK2RX                         0x04

// Public function
void     LoRaInit(void);
int16_t  LoRaBegin(uint32_t frequencyInHz, int8_t txPowerInDbm, float tcxoVoltage, bool useRegulatorLDO);
void     LoRaConfig(uint8_t spreadingFactor, uint8_t bandwidth, uint8_t codingRate, uint16_t preambleLength, uint8_t payloadLen, bool crcOn, bool invertIrq);
uint8_t  LoRaReceive(uint8_t *pData, int16_t len);
bool     LoRaSend(uint8_t *pData, int16_t len, uint8_t mode);
void     LoRaDebugPrint(bool enable);

// Private function
void     spi_write_byte(uint8_t* Dataout, size_t DataLength );
void     spi_read_byte(uint8_t* Datain, uint8_t* Dataout, size_t DataLength );
uint8_t  spi_transfer(uint8_t address);

bool     ReceiveMode(void);
void     GetPacketStatus(int8_t *rssiPacket, int8_t *snrPacket);
void     SetTxPower(int8_t txPowerInDbm);

void     FixInvertedIQ(uint8_t iqConfig);
void     SetDio3AsTcxoCtrl(float voltage, uint32_t delay);
void     SetDio2AsRfSwitchCtrl(uint8_t enable);
void     Reset(void);
void     SetStandby(uint8_t mode);
void     SetRfFrequency(uint32_t frequency);
void     Calibrate(uint8_t calibParam);
void     CalibrateImage(uint32_t frequency);
void     SetRegulatorMode(uint8_t mode);
void     SetBufferBaseAddress(uint8_t txBaseAddress, uint8_t rxBaseAddress);
void     SetPowerConfig(int8_t power, uint8_t rampTime);
void     SetOvercurrentProtection(float currentLimit);
void     SetSyncWord(int16_t sync);
void     SetPaConfig(uint8_t paDutyCycle, uint8_t hpMax, uint8_t deviceSel, uint8_t paLut);
void     SetDioIrqParams(uint16_t irqMask, uint16_t dio1Mask, uint16_t dio2Mask, uint16_t dio3Mask);
void     SetStopRxTimerOnPreambleDetect(bool enable);
void     SetLoRaSymbNumTimeout(uint8_t SymbNum);
void     SetPacketType(uint8_t packetType);
void     SetModulationParams(uint8_t spreadingFactor, uint8_t bandwidth, uint8_t codingRate, uint8_t lowDataRateOptimize);
void     SetCadParams(uint8_t cadSymbolNum, uint8_t cadDetPeak, uint8_t cadDetMin, uint8_t cadExitMode, uint32_t cadTimeout);
void     SetCad();
void     SetRxGain();
uint8_t  GetStatus(void);
uint16_t GetIrqStatus(void);
void     ClearIrqStatus(uint16_t irq);
void     SetTxEnable(void);
void     SetRxEnable(void);
void     SetRx(uint32_t timeout);
void     SetTx(uint32_t timeoutInMs);
void     SetTxContinuousWave(void);
void     SetRxTxFallbackMode(uint8_t fallback_mode);
int      GetPacketLost();
uint8_t  GetRssiInst();
void     GetRxBufferStatus(uint8_t *payloadLength, uint8_t *rxStartBufferPointer);
void     Wakeup(void);
void     WaitForIdleBegin(unsigned long timeout, char *text);
bool     WaitForIdle(unsigned long timeout, char *text, bool stop);
uint8_t  ReadBuffer(uint8_t *rxData, int16_t rxDataLen);
void     WriteBuffer(uint8_t *txData, int16_t txDataLen);
void     WriteRegister(uint16_t reg, uint8_t* data, uint8_t numBytes);
void     ReadRegister(uint16_t reg, uint8_t* data, uint8_t numBytes);
void     WriteCommand(uint8_t cmd, uint8_t* data, uint8_t numBytes);
uint8_t  WriteCommand2(uint8_t cmd, uint8_t* data, uint8_t numBytes);
void     ReadCommand(uint8_t cmd, uint8_t* data, uint8_t numBytes);
void     SPItransfer(uint8_t cmd, bool write, uint8_t* dataOut, uint8_t* dataIn, uint8_t numBytes, bool waitForBusy);
void     LoRaError(int error);


#endif