// GPS Library for CCS PIC C compiler
// http://simple-circuit.com/



#define _GPRMC_  1
#define _GPGGA_  2
#define _OTHER_  3

#include <stdint.h>
#include <stdlib.h>
#include <string.h>


int1 GPRMC_ok = 0, GPGGA_ok = 0;
uint8_t char_number = 0, SentenceType = 0, Term;
char sentence[6], rawTime[6], rawSatellites[2],
     rawLatitude[13], rawLongitude[13], buffer[13];

void stringcpy(char *str1, char *str2, int1 dir = 0) {
  uint8_t chr = 0;
  do {
    str2[chr + dir] = str1[chr];
  } while(str1[chr++] != '\0');
}

int1 GPSRead() {
  uint8_t c = fgetc(GPS);

  switch(c) {
    case '\r':  // sentence end
      if(SentenceType == _GPRMC_)
        GPRMC_ok = 1;
      if(SentenceType == _GPGGA_)
        GPGGA_ok = 1;
      if(GPRMC_ok && GPGGA_ok) {
        GPRMC_ok = GPGGA_ok = 0;
        return 1;
      }
      break;

    case '$': // sentence start
      Term = char_number = 0;
      break;

    case ',':  // term end (new term start)
      buffer[char_number] = '\0';
      if(Term == 0) {
        stringcpy(buffer, sentence);
        if(strcmp(sentence, "GPRMC") == 0)
          SentenceType = _GPRMC_;
        else if(strcmp(sentence, "GPGGA") == 0)
               SentenceType = _GPGGA_;
             else
               SentenceType = _OTHER_;
      }

      // Time
      if(Term == 1 && SentenceType == _GPRMC_) {
        stringcpy(buffer, rawTime);
      }

      // Latitude
      if((Term == 3) && (SentenceType == _GPRMC_)) {
        stringcpy(buffer, rawLatitude, 2);
      }
      // Latitude N/S
      if((Term == 4) && (SentenceType == _GPRMC_)) {
         if(buffer[0] == 'N'){
            rawLatitude[1] = 'N';}
         else
            rawLatitude[1] = 'S';

      }

      // Longitude
      if((Term == 5) && (SentenceType == _GPRMC_)) {
        stringcpy(buffer, rawLongitude, 1);
      }
      // Longitude E/W
      if((Term == 6) && (SentenceType == _GPRMC_)) {
        if(buffer[0] == 'E'){
          rawLongitude[0] = 'E';}
        else
          rawLongitude[0] = 'W';

      }    
      
      // Satellites
      if((Term == 7) && (SentenceType == _GPGGA_)) {
        stringcpy(buffer, rawSatellites);
      }


      Term++;
      char_number = 0;
      break;

    default:
      buffer[char_number++] = c;
      break;
  }

  return 0;
}




