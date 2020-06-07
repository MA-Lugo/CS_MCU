///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////                             BME280_Lib.c                          ////
////                                                                   ////
////                      Driver for CCS C compiler                    ////
////                                                                   ////
//// Driver for Bosch BME280 sensor. This sensor can read temperature, ////
//// humidity and pressure.                                            ////
//// This driver only supports I2C mode, it doesn't support SPI mode.  ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////                     https://simple-circuit.com/                   ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////


#include <stdint.h>

#ifndef BME280_I2C_ADDRESS
  #define BME280_I2C_ADDRESS  0xEE
#endif

#define BME280_CHIP_ID        0x60

#define BME280_REG_DIG_T1     0x88
#define BME280_REG_DIG_T2     0x8A
#define BME280_REG_DIG_T3     0x8C

#define BME280_REG_DIG_P1     0x8E
#define BME280_REG_DIG_P2     0x90
#define BME280_REG_DIG_P3     0x92
#define BME280_REG_DIG_P4     0x94
#define BME280_REG_DIG_P5     0x96
#define BME280_REG_DIG_P6     0x98
#define BME280_REG_DIG_P7     0x9A
#define BME280_REG_DIG_P8     0x9C
#define BME280_REG_DIG_P9     0x9E

#define BME280_REG_DIG_H1     0xA1
#define BME280_REG_DIG_H2     0xE1
#define BME280_REG_DIG_H3     0xE3
#define BME280_REG_DIG_H4     0xE4
#define BME280_REG_DIG_H5     0xE5
#define BME280_REG_DIG_H6     0xE7

#define BME280_REG_CHIPID     0xD0
#define BME280_REG_SOFTRESET  0xE0

#define BME280_REG_CTRLHUM    0xF2
#define BME280_REG_STATUS     0xF3
#define BME280_REG_CONTROL    0xF4
#define BME280_REG_CONFIG     0xF5
#define BME280_REG_PRESS_MSB  0xF7

int32_t adc_T, adc_P, adc_H, t_fine;

// BME280 sensor modes, register ctrl_meas mode[1:0]
enum bme280_mode
{
  MODE_SLEEP  = 0x00,  // sleep mode
  MODE_FORCED = 0x01,  // forced mode
  MODE_NORMAL = 0x03   // normal mode
} ;

// oversampling setting. osrs_h[2:0], osrs_t[2:0], osrs_p[2:0]
enum bme280_sampling
{
  SAMPLING_SKIPPED = 0x00,  //skipped, output set to 0x80000 (0x8000 for humidity)
  SAMPLING_X1      = 0x01,  // oversampling x1
  SAMPLING_X2      = 0x02,  // oversampling x2
  SAMPLING_X4      = 0x03,  // oversampling x4
  SAMPLING_X8      = 0x04,  // oversampling x8
  SAMPLING_X16     = 0x05   // oversampling x16
} ;

// filter setting filter[2:0]
enum bme280_filter
{
  FILTER_OFF = 0x00,  // filter off
  FILTER_2   = 0x01,  // filter coefficient = 2
  FILTER_4   = 0x02,  // filter coefficient = 4
  FILTER_8   = 0x03,  // filter coefficient = 8
  FILTER_16  = 0x04   // filter coefficient = 16
} ;

// standby (inactive) time in ms (used in normal mode), t_sb[2:0]
enum standby_time
{
  STANDBY_0_5   =  0x00,  // standby time = 0.5 ms
  STANDBY_62_5  =  0x01,  // standby time = 62.5 ms
  STANDBY_125   =  0x02,  // standby time = 125 ms
  STANDBY_250   =  0x03,  // standby time = 250 ms
  STANDBY_500   =  0x04,  // standby time = 500 ms
  STANDBY_1000  =  0x05,  // standby time = 1000 ms
  STANDBY_10    =  0x06,  // standby time = 10 ms
  STANDBY_20    =  0x07   // standby time = 20 ms
} ;

struct
{
  uint16_t dig_T1;
  int16_t  dig_T2;
  int16_t  dig_T3;
  uint16_t dig_P1;
  int16_t  dig_P2;
  int16_t  dig_P3;
  int16_t  dig_P4;
  int16_t  dig_P5;
  int16_t  dig_P6;
  int16_t  dig_P7;
  int16_t  dig_P8;
  int16_t  dig_P9;

  uint8_t  dig_H1;
  int16_t  dig_H2;
  uint8_t  dig_H3;
  int16_t  dig_H4;
  int16_t  dig_H5;
  int8_t   dig_H6;
} BME280_calib;

// writes 1 byte '_data' to register 'reg_addr'
void BME280_Write(uint8_t reg_addr, uint8_t _data)
{
  I2C_Start(BME280_STREAM);
  I2C_Write(BME280_STREAM, BME280_I2C_ADDRESS);
  I2C_Write(BME280_STREAM, reg_addr);
  I2C_Write(BME280_STREAM, _data);
  I2C_Stop(BME280_STREAM);
}

// reads 8 bits from register 'reg_addr'
uint8_t BME280_Read8(uint8_t reg_addr)
{
  uint8_t ret;

  I2C_Start(BME280_STREAM);
  I2C_Write(BME280_STREAM, BME280_I2C_ADDRESS);
  I2C_Write(BME280_STREAM, reg_addr);
  I2C_Start(BME280_STREAM);
  I2C_Write(BME280_STREAM, BME280_I2C_ADDRESS | 1);
  ret = I2C_Read(BME280_STREAM, 0);
  I2C_Stop(BME280_STREAM);

  return ret;
}

// reads 16 bits from register 'reg_addr'
uint16_t BME280_Read16(uint8_t reg_addr)
{
  union
  {
    uint8_t  b[2];
    uint16_t w;
  } ret;

  I2C_Start(BME280_STREAM);
  I2C_Write(BME280_STREAM, BME280_I2C_ADDRESS);
  I2C_Write(BME280_STREAM, reg_addr);
  I2C_Start(BME280_STREAM);
  I2C_Write(BME280_STREAM, BME280_I2C_ADDRESS | 1);
  ret.b[0] = I2C_Read(BME280_STREAM, 1);
  ret.b[1] = I2C_Read(BME280_STREAM, 0);
  I2C_Stop(BME280_STREAM);

  return(ret.w);
}

// BME280 sensor configuration function
void BME280_Configure(bme280_mode mode, bme280_sampling T_sampling, bme280_sampling H_sampling,
                      bme280_sampling P_sampling, bme280_filter filter, standby_time standby)
{
  uint8_t _ctrl_hum, _ctrl_meas, _config;

  _ctrl_hum = H_sampling;
  _config = ((standby << 5) | (filter << 2)) & 0xFC;
  _ctrl_meas = (T_sampling << 5) | (P_sampling << 2) | mode;

  BME280_Write(BME280_REG_CTRLHUM, _ctrl_hum);
  BME280_Write(BME280_REG_CONFIG,  _config);
  BME280_Write(BME280_REG_CONTROL, _ctrl_meas);
}

// initializes the BME280 sensor, returns 1 if OK and 0 if error
int1 BME280_begin(bme280_mode mode,
                  bme280_sampling T_sampling = SAMPLING_X1,
                  bme280_sampling H_sampling = SAMPLING_X1,
                  bme280_sampling P_sampling = SAMPLING_X1,
                  bme280_filter filter       = FILTER_OFF,
                  standby_time  standby      = STANDBY_0_5)
{
  if(BME280_Read8(BME280_REG_CHIPID) != BME280_CHIP_ID)
    return 0;

  // reset the BME280 with soft reset
  BME280_Write(BME280_REG_SOFTRESET, 0xB6);
  delay_ms(100);

  // if NVM data are being copied to image registers, wait 100 ms
  while( (BME280_Read8(BME280_REG_STATUS) & 0x01) == 0x01 )
    delay_ms(100);

  BME280_calib.dig_T1 = BME280_Read16(BME280_REG_DIG_T1);
  BME280_calib.dig_T2 = BME280_Read16(BME280_REG_DIG_T2);
  BME280_calib.dig_T3 = BME280_Read16(BME280_REG_DIG_T3);

  BME280_calib.dig_P1 = BME280_Read16(BME280_REG_DIG_P1);
  BME280_calib.dig_P2 = BME280_Read16(BME280_REG_DIG_P2);
  BME280_calib.dig_P3 = BME280_Read16(BME280_REG_DIG_P3);
  BME280_calib.dig_P4 = BME280_Read16(BME280_REG_DIG_P4);
  BME280_calib.dig_P5 = BME280_Read16(BME280_REG_DIG_P5);
  BME280_calib.dig_P6 = BME280_Read16(BME280_REG_DIG_P6);
  BME280_calib.dig_P7 = BME280_Read16(BME280_REG_DIG_P7);
  BME280_calib.dig_P8 = BME280_Read16(BME280_REG_DIG_P8);
  BME280_calib.dig_P9 = BME280_Read16(BME280_REG_DIG_P9);

  BME280_calib.dig_H1 = BME280_Read8(BME280_REG_DIG_H1);
  BME280_calib.dig_H2 = BME280_Read16(BME280_REG_DIG_H2);
  BME280_calib.dig_H3 = BME280_Read8(BME280_REG_DIG_H3);
  BME280_calib.dig_H4 = ((uint16_t)BME280_Read8(BME280_REG_DIG_H4) << 4) | (BME280_Read8(BME280_REG_DIG_H4 + 1) & 0x0F);
  if (BME280_calib.dig_H4 & 0x0800)    // if BME280_calib.dig_H4 < 0
      BME280_calib.dig_H4 |= 0xF000;
  BME280_calib.dig_H5 = ((uint16_t)BME280_Read8(BME280_REG_DIG_H5 + 1) << 4) | (BME280_Read8(BME280_REG_DIG_H5) >> 4);
  if (BME280_calib.dig_H5 & 0x0800)    // if BME280_calib.dig_H5 < 0
      BME280_calib.dig_H5 |= 0xF000;
  BME280_calib.dig_H6 = BME280_Read8(BME280_REG_DIG_H6);

  BME280_Configure(mode, T_sampling, H_sampling, P_sampling, filter, standby);

  return 1;
}

// takes a new measurement, for forced mode only!
// Returns 1 if ok and 0 if error (sensor is not in sleep mode)
int1 BME280_ForcedMeasurement()
{
  uint8_t ctrl_meas_reg = BME280_Read8(BME280_REG_CONTROL);

  if ( (ctrl_meas_reg & 0x03) != 0x00 )
    return 0;   // sensor is not in sleep mode

  // set sensor to forced mode
  BME280_Write(BME280_REG_CONTROL, ctrl_meas_reg | 1);
  // wait for conversion complete
  while (BME280_Read8(BME280_REG_STATUS) & 0x08)
    delay_ms(1);

  return 1;
}

// read (updates) adc_P, adc_T and adc_H from BME280 sensor
void BME280_Update()
{
  union
  {
    uint8_t  b[4];
    uint32_t dw;
  } ret;
  ret.b[3] = 0x00;

  I2C_Start(BME280_STREAM);
  I2C_Write(BME280_STREAM, BME280_I2C_ADDRESS);
  I2C_Write(BME280_STREAM, BME280_REG_PRESS_MSB);
  I2C_Start(BME280_STREAM);
  I2C_Write(BME280_STREAM, BME280_I2C_ADDRESS | 1);
  ret.b[2] = I2C_Read(BME280_STREAM, 1);
  ret.b[1] = I2C_Read(BME280_STREAM, 1);
  ret.b[0] = I2C_Read(BME280_STREAM, 1);

  adc_P = (ret.dw >> 4) & 0xFFFFF;

  ret.b[2] = I2C_Read(BME280_STREAM, 1);
  ret.b[1] = I2C_Read(BME280_STREAM, 1);
  ret.b[0] = I2C_Read(BME280_STREAM, 1);

  adc_T = (ret.dw >> 4) & 0xFFFFF;

  ret.b[2] = 0x00;
  ret.b[1] = I2C_Read(BME280_STREAM, 1);
  ret.b[0] = I2C_Read(BME280_STREAM, 0);
  I2C_Stop(BME280_STREAM);

  adc_H = ret.dw & 0xFFFF;
}

// Reads temperature from BME280 sensor.
// Temperature is stored in hundredths C (output value of "5123" equals 51.23 DegC).
// Temperature value is saved to *temp, returns 1 if OK and 0 if error.
int1 BME280_readTemperature(int32_t *temp)
{
  int32_t var1, var2;

  BME280_Update();

  // calculate temperature
  var1 = ((((adc_T / 8) - ((int32_t)BME280_calib.dig_T1 * 2))) *
         ((int32_t)BME280_calib.dig_T2)) / 2048;

  var2 = (((((adc_T / 16) - ((int32_t)BME280_calib.dig_T1)) *
         ((adc_T / 16) - ((int32_t)BME280_calib.dig_T1))) / 4096) *
         ((int32_t)BME280_calib.dig_T3)) / 16384;

  t_fine = var1 + var2;

  *temp = (t_fine * 5 + 128) / 256;

  return 1;
}

// Reads humidity from BME280 sensor.
// Humidity is stored in relative humidity percent in 1024 steps
// (output value of "47445" represents 47445/1024 = 46.333 %RH).
// Humidity value is saved to *humi, returns 1 if OK and 0 if error.
int1 BME280_readHumidity(uint32_t *humi)
{
  int32_t v_x1_u32r;
  uint32_t H;

  v_x1_u32r = (t_fine - ((int32_t)76800));

  v_x1_u32r = (((((adc_H * 16384) - (((int32_t)BME280_calib.dig_H4) * 1048576) - (((int32_t)BME280_calib.dig_H5) * v_x1_u32r)) +
      ((int32_t)16384)) / 32768) * (((((((v_x1_u32r * ((int32_t)BME280_calib.dig_H6)) / 1024) * (((v_x1_u32r *
      ((int32_t)BME280_calib.dig_H3)) / 2048) + ((int32_t)32768))) / 1024) + ((int32_t)2097152)) *
      ((int32_t)BME280_calib.dig_H2) + 8192) / 16384));
      
  v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r / 32768) * (v_x1_u32r / 32768)) / 128) * ((int32_t)BME280_calib.dig_H1)) / 16));
  v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
  v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
   
  H = (uint32_t)(v_x1_u32r / 4096);
  *humi = H;

  return 1;
}

// Reads pressure from BME280 sensor.
// Pressure is stored in Pa (output value of "96386" equals 96386 Pa = 963.86 hPa).
// Pressure value is saved to *pres, returns 1 if OK and 0 if error.
int1 BME280_readPressure(uint32_t *pres)
{
  int32_t var1, var2;
  uint32_t p;

  // calculate pressure
  var1 = (((int32_t)t_fine) / 2) - (int32_t)64000;
  var2 = (((var1/4) * (var1/4)) / 2048 ) * ((int32_t)BME280_calib.dig_P6);

  var2 = var2 + ((var1 * ((int32_t)BME280_calib.dig_P5)) * 2);
  var2 = (var2/4) + (((int32_t)BME280_calib.dig_P4) * 65536);

  var1 = ((((int32_t)BME280_calib.dig_P3 * (((var1/4) * (var1/4)) / 8192 )) / 8) +
         ((((int32_t)BME280_calib.dig_P2) * var1)/2)) / 262144;

  var1 =((((32768 + var1)) * ((int32_t)BME280_calib.dig_P1)) / 32768);

  if (var1 == 0)
    return 0; // avoid exception caused by division by zero

  p = (((uint32_t)(((int32_t)1048576) - adc_P) - (var2 / 4096))) * 3125;

  if (p < 0x80000000)
    p = (p * 2) / ((uint32_t)var1);

  else
    p = (p / (uint32_t)var1) * 2;

  var1 = (((int32_t)BME280_calib.dig_P9) * ((int32_t)(((p/8) * (p/8)) / 8192))) / 4096;
  var2 = (((int32_t)(p/4)) * ((int32_t)BME280_calib.dig_P8)) / 8192;

  p = (uint32_t)((int32_t)p + ((var1 + var2 + (int32_t)BME280_calib.dig_P7) / 16));

  *pres = p;

  return 1;
}

// end of code.
