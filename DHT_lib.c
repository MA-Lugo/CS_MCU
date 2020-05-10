//////Driver modified by M.Lugo
#define DHT11_pin PIN_E2 

unsigned char values[5];
/* For DHT11:
 *	values[0] = Integer RH
 * 	values[1] = Decimal RH = 0
 * 	values[2] = Integer Temp
 * 	values[3] = Decimal Temp
 * 	values[4] = Check Sum
 */ 

void DHT_init(); 
unsigned char get_byte(); 
unsigned char get_dht11(); 
   
float dht22_Temp;
float dht22_Rh;

void DHT_init() 
{ 
   output_float(DHT11_pin); 
   delay_ms(1000); 
} 

unsigned char get_byte() 
{ 
   unsigned char s = 0; 
   unsigned char value = 0; 

   for(s = 0; s < 8; s += 1) 
   { 
      value <<= 1; 
      while(!input(DHT11_pin)); 
      delay_us(30); 

      if(input(DHT11_pin)) 
      { 
          value |= 1; 
      } 
      while(input(DHT11_pin)); 
   } 
   return value; 
} 


unsigned char get_dht11() 
{ 
   short chk = 0; 
   unsigned char s = 0; 
   unsigned char check_sum = 0; 

   output_high(DHT11_pin); 
   output_low(DHT11_pin); 
   delay_ms(18); 
   output_high(DHT11_pin); 
   delay_us(26); 
    
   chk = input(DHT11_pin); 
   if(chk) 
   { 
      return 1; 
   } 
   delay_us(80); 
    
   chk = input(DHT11_pin); 
   if(!chk) 
   { 
      return 2; 
   } 
   delay_us(80); 

   for(s = 0; s <= 4; s += 1) 
   { 
       values[s] = get_byte(); 
   } 
  
   output_high(DHT11_pin); 
    
   for(s = 0; s < 4; s += 1) 
   { 
       check_sum += values[s]; 
   } 
    
   if(check_sum != values[4]) 
   { 
      return 3; 
   } 
   else 
   { 
      return 0; 
   } 
} 

unsigned char get_dht22() 
{
unsigned char z;   

z = get_dht11();
      switch(z) 
      { 
            case 1: 
            {
	       return 1;
            } 
            case 2: 
            { 
               return 2;	//No sensor
	       
               break; 
            } 
            case 3: 
            { 

               return 3;	// Check sum error
               break; 
            } 
            default: 
	    {
	       dht22_Rh = make16(values[0],values[1]);
	       dht22_Rh = dht22_Rh/10;
	       
	       dht22_Temp = Make16 (values [2] & 0x7F, values [3]);
	       if (values[2] & 0x80)  dht22_Temp = dht22_Temp *-1;
	       dht22_Temp = dht22_Temp / 10;
	       
	       return 0; 
	       
	    }
      }
}