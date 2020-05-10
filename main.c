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

int t = 0,send_period = 2;




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
}

void GET_DATA(){


}

void SEND_DATA(){ 
   
   t++;
   GET_DATA();
   if(t == send_period){
      fprintf(GS,"#CAN,");
      fprintf(GS,"%02u,",GPSHour());
      fprintf(GS,"%02u,",GPSMinute());
      fprintf(GS,"%02u,",GPSSecond());
      fprintf(GS,"%.6f,", Latitude());
      fprintf(GS,"%.6f,", Longitude());
      fprintf(GS,"\n\r");
      fprintf(GS,"#SAT");
      fprintf(GS,"\n\r");
      t=0;
   }
   
}
void BLINK_LED(){
   output_toggle(PIN_C5);
}
