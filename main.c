#include <16F887.h>
#device PASS_STRINGS = IN_RAM 
#include <reg.h>
#use rtos(timer=1,minor_cycle=100ms)
#fuses HS, NOWDT, NOPROTECT, NOBROWNOUT, NOPUT, NOLVP 
#use delay(crystal=20M)
#use rs232(baud=9600,xmit=PIN_c6,rcv=PIN_c7,stream=GPS, parity=N, bits=8)
#use rs232(UART1,baud=57600,xmit=PIN_C0,rcv=PIN_C1,stream=GS, parity=N, bits=8,)
#use i2c (MASTER,SDA=PIN_C4, SCL=PIN_C3,force_SW,fast=400000,stream=BME280_STREAM)

#include <GPS_Lib.c>
#include <INA219_lib.c>
int t = 0,send_period = 2;
float LOAD_VOLTAGE;
float CURRENT;

#task(rate=1s,max=1us)
   void BLINK_LED();
#task(rate=1s,max=100ms)
   void SEND_DATA();

#int_rda
void serial_isr() {
GPSRead();
}

void inicializar();

void main(){
   
   inicializar();
   delay_ms(5000);
   pc5 = 1;
   RTOS_RUN();
}

void inicializar(){
   enable_interrupts(INT_RDA);
   enable_interrupts(GLOBAL);
   TRISA=0xFF;
   TRISB=0xFF;
   TRISC=0x9E;
   TRISD=0xFF;
   TRISE=0xFF;
   ANSEL = 0X00;
   ANSELH = 0X00;
   RBPU =1;
   pc5 = 0;
   INA219_init();
}

void GET_DATA(){


LOAD_VOLTAGE = get_bus_voltage();
CURRENT = get_current();

}

void CLEAR_DATA(){
rawTime[0]=0;rawTime[1]=0;rawTime[2]=0;rawTime[3]=0;rawTime[4]=0;rawTime[5]=0;

rawLatitude[0]=0;rawLatitude[1]=0;rawLatitude[2]=0;rawLatitude[3]=0;
rawLatitude[4]=0;rawLatitude[5]=0;rawLatitude[6]=0;rawLatitude[7]=0;
rawLatitude[8]=0;rawLatitude[9]=0;rawLatitude[10]=0;rawLatitude[11]=0;

rawLongitude[0]=0;rawLongitude[1]=0;rawLongitude[2]=0;rawLongitude[3]=0;
rawLongitude[4]=0;rawLongitude[5]=0;rawLongitude[6]=0;rawLongitude[7]=0;
rawLongitude[8]=0;rawLongitude[9]=0;rawLongitude[10]=0;rawLongitude[11]=0;

rawSatellites[0]=0;rawSatellites[1]=0;
}
void SEND_DATA(){ 
   
   t++;
   GET_DATA();
   if(t == send_period){
      fprintf(GS,"#CAN,");
      fprintf(GS,"%c%c,",rawTime[0],rawTime[1]);
      fprintf(GS,"%c%c,",rawTime[2],rawTime[3]);
      fprintf(GS,"%c%c,",rawTime[4],rawTime[5]);
      fprintf(GS,"%c%c%c",rawLatitude[0],rawLatitude[1],rawLatitude[2]);
      fprintf(GS,"%c%c%c",rawLatitude[3],rawLatitude[4],rawLatitude[5]);
      fprintf(GS,"%c%c%c",rawLatitude[6],rawLatitude[7],rawLatitude[8]);
      fprintf(GS,"%c%c%c,",rawLatitude[9],rawLatitude[10],rawLatitude[11]);
      fprintf(GS,"%c%c%c",rawLongitude[0],rawLongitude[1],rawLongitude[2]);
      fprintf(GS,"%c%c%c",rawLongitude[3],rawLongitude[4],rawLongitude[5]);
      fprintf(GS,"%c%c%c",rawLongitude[6],rawLongitude[7],rawLongitude[8]);
      fprintf(GS,"%c%c%c,",rawLongitude[9],rawLongitude[10],rawLongitude[11]);
      fprintf(GS,"%c%c,",rawSatellites[0],rawSatellites[1]);
      fprintf(GS,"%.2f,",LOAD_VOLTAGE);
      fprintf(GS,"%.2f",CURRENT);
      fprintf(GS,"\n\r");
      fprintf(GS,"#SAT,");
      fprintf(GS,"\n\r");
      t=0;
      CLEAR_DATA();
   }
   
}
void BLINK_LED(){
   output_toggle(PIN_C5);
}

