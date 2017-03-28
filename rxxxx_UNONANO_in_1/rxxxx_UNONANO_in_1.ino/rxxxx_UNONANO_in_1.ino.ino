
#include <EasyTransfer.h>
#include <SoftwareSerial.h>
#include <NewPing.h>
#include <TinyGPS.h>

#include <Wire.h>
#include <EasyTransferI2C.h>





////////////////////// PPM CONFIGURATION//////////////////////////
#define channel_number 6  //set the number of channels
#define sigPin 2  //set PPM signal output pin on the arduino
#define PPM_FrLen 27000  //set the PPM frame length in microseconds (1ms = 1000µs)
#define PPM_PulseLen 400  //set the pulse length
//////////////////////////////////////////////////////////////////



int ppm[channel_number];
const uint64_t pipeIn =  0xE8E8F0F0E1LL;




TinyGPS gps;
//SoftwareSerial ss(12, 13);
EasyTransferI2C ET; 


NewPing sonarBack(5,4, 500); // NewPing setup of pins and maximum distance.
NewPing sonarFront(7,6, 500);
NewPing sonarRight(9,8, 500);
NewPing sonarLeft(11,10, 500);
//create two objects
EasyTransfer ETin, ETout;  
//SoftwareSerial mySerial(2,3);

struct RECEIVE_DATA_STRUCTURE{
  byte throttle;                            
  byte yaw;                             
  byte pitch;                             
  byte roll;                              
  byte sprayFlag = 0;
};

struct SEND_DATA_STRUCTURE{
float sprayLvl;                              
float battLvl;                             
float gpsLat;                             
float gpsLong;                              
float gpsAlt;                             
float SonarFront;                           
float SonarBack;                            
float SonarLeft;                           
float SonarRight;
};

struct SEND_DATA_NANO_STRUCTURE{
  byte throttle;                            
  byte yaw;                             
  byte pitch;                             
  byte roll;                              
  byte sprayFlag;
};


//name to the group of data
RECEIVE_DATA_STRUCTURE rxdata;
SEND_DATA_STRUCTURE txdata;
SEND_DATA_NANO_STRUCTURE mydata;


void resetData() 
{
  // 'safe' values to use when no radio input is detected
  mydata.throttle = 0;
  mydata.yaw = 127;
  mydata.pitch = 127;
  mydata.roll = 127;
  mydata.sprayFlag = 0;
  setPPMValuesFromData();
}


void setPPMValuesFromData()
{
  ppm[0] = map(mydata.throttle, 0, 255, 1000, 2000);
  ppm[1] = map(mydata.yaw,      0, 255, 1000, 2000);
  ppm[2] = map(mydata.pitch,    0, 255, 1000, 2000);
  ppm[3] = map(mydata.roll,     0, 255, 1000, 2000);  
  ppm[4] = 1000;
  ppm[5] = 1000;
}



void setupPPM() {
  pinMode(sigPin, OUTPUT);
  digitalWrite(sigPin, 0);  //set the PPM signal pin to the default state (off)

  cli();
  TCCR1A = 0; // set entire TCCR1 register to 0
  TCCR1B = 0;

  OCR1A = 100;  // compare match register (not very important, sets the timeout for the first interrupt)
  TCCR1B |= (1 << WGM12);  // turn on CTC mode
  TCCR1B |= (1 << CS11);  // 8 prescaler: 0,5 microseconds at 16mhz
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  sei();
}

void setup(){
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  resetData();
  setupPPM();
  
  
   
   Serial.begin(115200);
  // mySerial.begin(9600);
  // ss.begin(9600);
  //start the library, pass in the data details and the name of the serial port. Can be Serial, Serial1, Serial2, etc.

  
  ETin.begin(details(rxdata), &Serial);
  ETout.begin(details(txdata), &Serial);
 
}

unsigned long lastRecvTime = 0;



void loop(){

 //set values to send to flight controller_rx
 mydata.throttle   =  rxdata.throttle;
 mydata.yaw        =  rxdata.yaw;
 mydata.pitch      =  rxdata.pitch;
 mydata.roll       =  rxdata.roll;
 mydata.sprayFlag  =  rxdata.sprayFlag;


 
if(mydata.sprayFlag == 1){

 digitalWrite(13, LOW);
  }
  else{digitalWrite(13, HIGH); 
}




    float flat, flon;
//    unsigned long age;
//    gps.f_get_position(&flat, &flon, &age);
//   
//    flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat;
//    flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon;
  

  
 txdata.sprayLvl      =  80;                             
 txdata.battLvl       =  100;                              
 txdata.gpsLat        =  flat;                          
 txdata.gpsLong       =  flon;
 txdata.gpsAlt        =  0;                   
 txdata.SonarFront    =  sonarFront.ping_cm();                     
 txdata.SonarBack     =  sonarBack.ping_cm();                     
 txdata.SonarLeft     =  sonarLeft.ping_cm();                      
 txdata.SonarRight    =  sonarRight.ping_cm();


//Serial.print("front:");
//Serial.print(txdata.SonarFront );
//Serial.print("  Back:");
//Serial.print(txdata.SonarBack );
//Serial.print("  Left:");
//Serial.print(txdata.SonarLeft );
//Serial.print("  Right:");
//Serial.println(txdata.SonarRight );





  
 //then we will go ahead and send that data out
 ETout.sendData();
 
  for(int i=0; i<5; i++){
    //remember, you could use an if() here to check for new data, this time it's not needed.
    ETin.receiveData();

    
    lastRecvTime = millis();
    //delay
    delay(10);
  }
  
  unsigned long now = millis();
  if ( now - lastRecvTime > 2000 || now - lastRecvTime <= 3000 ) {
            mydata.throttle = 1500;
            mydata.yaw = 127;
            mydata.pitch = 127;
            mydata.roll = 127;
            mydata.sprayFlag = 0;  
  }
  else if ( now - lastRecvTime > 3000 || now - lastRecvTime <= 4000 ) {
            mydata.throttle = 1200;
            mydata.yaw = 127;
            mydata.pitch = 127;
            mydata.roll = 127;
            mydata.sprayFlag = 0;  
  }
  else if ( now - lastRecvTime > 4000 || now - lastRecvTime <= 5000 ) {
            mydata.throttle = 900;
            mydata.yaw = 127;
            mydata.pitch = 127;
            mydata.roll = 127;
            mydata.sprayFlag = 0;  
  }
  else if ( now - lastRecvTime > 5000 || now - lastRecvTime <= 6000 ) {
            mydata.throttle = 0;
            mydata.yaw = 127;
            mydata.pitch = 127;
            mydata.roll = 127;
            mydata.sprayFlag = 0;  
  }



setPPMValuesFromData();

//
//
//mySerial.print("Thrt:");
//mySerial.print(rxdata.throttle);
//mySerial.print("  Yaw:");-
//mySerial.print("  Pitch:");
//mySerial.print(rxdata.pitch);
//mySerial.print("  Roll:");
//mySerial.print(rxdata.roll);
//mySerial.print("  SprFlag:");
//mySerial.println(rxdata.sprayFlag);
//  




  //delay for good measure
  delay(10);
}





#define clockMultiplier 2 // set this to 2 if you are using a 16MHz arduino, leave as 1 for an 8MHz arduino

ISR(TIMER1_COMPA_vect){
  static boolean state = true;

  TCNT1 = 0;

  if ( state ) {
    //end pulse
    PORTD = PORTD & ~B00000100; // turn pin 2 off. Could also use: digitalWrite(sigPin,0)
    OCR1A = PPM_PulseLen * clockMultiplier;
    state = false;
  }
  else {
    //start pulse
    static byte cur_chan_numb;
    static unsigned int calc_rest;

    PORTD = PORTD | B00000100; // turn pin 2 on. Could also use: digitalWrite(sigPin,1)
    state = true;

    if(cur_chan_numb >= channel_number) {
      cur_chan_numb = 0;
      calc_rest += PPM_PulseLen;
      OCR1A = (PPM_FrLen - calc_rest) * clockMultiplier;
      calc_rest = 0;
    }
    else {
      OCR1A = (ppm[cur_chan_numb] - PPM_PulseLen) * clockMultiplier;
      calc_rest += ppm[cur_chan_numb];
      cur_chan_numb++;
    }     
  }
}

