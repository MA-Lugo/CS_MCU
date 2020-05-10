 
#byte PORTA = 0x005
   #bit pa0 = 0x005.0
   #bit pa1 = 0x005.1
   #bit pa2 = 0x005.2
   #bit pa3 = 0x005.3
   #bit pa4 = 0x005.4
   #bit pa5 = 0x005.5
   #bit pa6 = 0x005.6
   #bit pa7 = 0x005.7
#byte TRISA = 0x085
//PUERTO B
#byte PORTB = 0x006
   #bit pb0 = 0x006.0
   #bit pb1 = 0x006.1
   #bit pb2 = 0x006.2
   #bit pb3 = 0x006.3
   #bit pb4 = 0x006.4
   #bit pb5 = 0x006.5
   #bit pb6 = 0x006.6
   #bit pb7 = 0x006.7
#byte TRISB = 0x086

//PUERTO C
#byte PORTC = 0x007
   #bit pc0 = 0x007.0
   #bit pc1 = 0x007.1
   #bit pc2 = 0x007.2
   #bit pc3 = 0x007.3
   #bit pc4 = 0x007.4
   #bit pc5 = 0x007.5
   #bit pc6 = 0x007.6
   #bit pc7 = 0x007.7
#byte TRISC = 0x087
// PUERTO D
#byte PORTD = 0x008
   #bit pd0 = 0x008.0
   #bit pd1 = 0x008.1
   #bit pd2 = 0x008.2
   #bit pd3 = 0x008.3
   #bit pd4 = 0x008.4
   #bit pd5 = 0x008.5
   #bit pd6 = 0x008.6
   #bit pd7 = 0x008.7
#byte TRISD =0x088
//PUERTO E
#byte PORTE = 0x009
   #bit pe0 = 0x009.0
   #bit pe1 = 0x009.1
   #bit pe2 = 0x009.2
#byte TRISE = 0x089

#byte ANSEL = 0X188
#byte ANSELH = 0X189
#byte RCREG = 0X01A

//R_PULL-UP PORTB-->>0_ENABLE
#bit RBPU   = 0X81.7
//GLOBAL INTERUPTS
#bit GEI   = 0X00B.7 
//FLAG GLOBAL INTERRUPTS
#bit INTF   = 0X00B.1 
//PHERIP. INTERRUPTS
#bit PEIE   = 0X00B.6 
//UART INTERUPTS RX
#bit RCIE   = 0X08C.5 
//UART INTERUPTS RX FLAG
#bit RCIF   = 0X00C.5
//UART INTERUPTS RX FLAG
#bit TXIF   = 0X00C.4


