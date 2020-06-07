/*
///////////////////////////////////////////////////////////////////////
Name:    INA219_lib.c
Autor:   Mario Lugo
Date:    02/05/20
Description: Library for current/voltage sensor INA219
             v1.0 (POSITIVE VALUES ONLY)
///////////////////////////////////////////////////////////////////////
*/

#define INA219_WRITE_ADDR                      (0x80)// 10000000 (A0+A1=GND)
#define INA219_READ_ADDR                       (0x81)// 10000001 
#define INA219_REG_CONFIG                      (0x00)
#define INA219_REG_SHUNTVOLTAGE                (0x01)
#define INA219_REG_BUSVOLTAGE                  (0x02)
#define INA219_REG_POWER                       (0x03)
#define INA219_REG_CURRENT                     (0x04)
#define INA219_REG_CALIBRATION                 (0x05)

unsigned char data8L,data8H;

void INA219_init(){
   // INA219 initialize; bus 16V, shunt 160mV, 12-bit
   i2c_start();
   i2c_write(INA219_WRITE_ADDR);  // I2C slave address 0x80 WRITE
   i2c_write(INA219_REG_CONFIG);  // INA219 config reg   
   i2c_write(0x11);// bus 16V, shunt 160mV(MAX=1.6A), 12-bit 
   i2c_write(0x9f); //
   i2c_stop();
   //CALIBRATION
   i2c_start();
   i2c_write(INA219_WRITE_ADDR); // I2C slave address 0x80 WRITE
   i2c_write(INA219_REG_CALIBRATION); // INA219 calibration reg 0x05
   i2c_write(0x20);//CALIBRATION pag.5 Datasheet
   i2c_write(0x00);
   i2c_stop();
}

float get_bus_voltage(){
   unsigned int32 busV; 
      ////REGISTER POINTER SET
   i2c_start();
   i2c_write(INA219_WRITE_ADDR);// I2C slave address 0x80 
   i2c_write(INA219_REG_BUSVOLTAGE);//0x02
   i2c_stop();delay_ms(1);
   ////READ WORD FORMAT
   i2c_start();
   i2c_write(INA219_READ_ADDR); // I2C slave address 0x81
   data8H = i2c_read(1); // read INA219 MSB with master Acknowledge
   data8L = i2c_read(0); // read INA219 LSB without master Acknowledge
   i2c_stop();    
   busV = make16(data8H,data8L); // combine MSB LSB, raw value for busVoltage
   busV = busV>>3;//get only the last 12 bits
     
   return busV*0.004;//At full-scale range = 16V(decimal = 4000, hex = 0FA0), and LSB = 4 mV. 
   
      
}

float get_shunt_voltage(){
   unsigned int32 shuntV; 
      ////REGISTER POINTER SET
   i2c_start();
   i2c_write(INA219_WRITE_ADDR);// I2C slave address 0x80 
   i2c_write(INA219_REG_SHUNTVOLTAGE);//0x01
   i2c_stop();delay_ms(1);
   ////READ WORD FORMAT
   i2c_start();
   i2c_write(INA219_READ_ADDR); // I2C slave address 0x81
   data8H = i2c_read(1); // read INA219 MSB with master Acknowledge
   data8L = i2c_read(0); // read INA219 LSB without master Acknowledge
   i2c_stop();    
   shuntV = make16(data8H,data8L); // combine MSB LSB, raw value for busVoltage
   //shuntV = shuntV>>3;//get only the last 12 bits
     
   return shuntV*0.00001;//At full-scale range = 16V(decimal = 4000, hex = 0FA0), and LSB = 4 mV. 
   
      
}


float get_current(){
      unsigned int32 curr;
      ////REGISTER POINTER SET
      i2c_start();
      i2c_write(INA219_WRITE_ADDR);// I2C slave address 0x80 
      i2c_write(INA219_REG_CURRENT);//0x04
      i2c_stop();delay_ms(1);
      ////READ WORD FORMAT
      i2c_start();
      i2c_write(INA219_READ_ADDR); // I2C slave address 0x81
      data8H = i2c_read(1); // read INA219 MSB with master Acknowledge
      data8L = i2c_read(0); // read INA219 LSB without master Acknowledge
      i2c_stop();  
      curr = make16(data8H,data8L); // combine MSB LSB, raw value for Current
      
      return curr*0.05;//mA         (0.00005 for A)
 
}

float get_power(){
      unsigned int32 pow;
      ////REGISTER POINTER SET
      i2c_start();
      i2c_write(INA219_WRITE_ADDR);// I2C slave address 0x80 
      i2c_write(INA219_REG_POWER);//0x03
      i2c_stop();delay_ms(1);
      ////READ WORD FORMAT
      i2c_start();
      i2c_write(INA219_READ_ADDR); // I2C slave address 0x81
      data8H = i2c_read(1); // read INA219 MSB with master Acknowledge
      data8L = i2c_read(0); // read INA219 LSB without master Acknowledge
      i2c_stop();  
      pow = make16(data8H,data8L); // combine MSB LSB, raw value 
      
      return pow*0.001; //
}


