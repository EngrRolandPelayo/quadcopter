#include <EasyTransfer.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
//create two objects

LiquidCrystal lcd(12, 11,10, 9, 8, 7);
EasyTransfer ETin, ETout; 
SoftwareSerial mySerial(2,3);

struct RECEIVE_DATA_STRUCTURE{
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

struct SEND_DATA_STRUCTURE{
  byte throttle;                            
  byte yaw;                             
  byte pitch;                             
  byte roll;                              
  byte sprayFlag;   
};

//give a name to the group of data
RECEIVE_DATA_STRUCTURE rxdata;
SEND_DATA_STRUCTURE txdata;


void setup(){
  Serial.begin(115200);
  mySerial.begin(9600);
  lcd.begin(16, 2);
  lcd.clear();
  
  //start the library, pass in the data details and the name of the serial port. Can be Serial, Serial1, Serial2, etc.
  ETin.begin(details(rxdata), &Serial);
  ETout.begin(details(txdata), &Serial);
}




int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse)
{
  val = constrain(val, lower, upper);
  if ( val < middle )
    val = map(val, lower, middle, 0, 128);
  else
    val = map(val, middle, upper, 128, 255);
  return ( reverse ? 255 - val : val );
}

void loop(){
  

  txdata.throttle = mapJoystickValues( analogRead(0), 340, 527  , 720, false ); 
  txdata.yaw      = mapJoystickValues( analogRead(1), 284, 560, 827, true );
  txdata.pitch    = mapJoystickValues( analogRead(3), 50, 530, 975, true );
  txdata.roll     = mapJoystickValues( analogRead(2), 27, 532, 1015, true );
  if(digitalRead(13) == 0){txdata.sprayFlag =1;}else{txdata.sprayFlag =0;}


  //then we will go ahead and send that data out
  ETout.sendData();


//    

//----------------LCD-----------------------
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SPR:");
  lcd.setCursor(4, 0);
  lcd.print(rxdata.sprayLvl);


  lcd.setCursor(9, 0);
  lcd.print("BAT:");
  lcd.setCursor(13, 0);
  lcd.print(rxdata.battLvl);


  lcd.setCursor(0, 1);
  lcd.print("Front:");
  lcd.setCursor(6, 1);
  lcd.print(rxdata.sonarFront);


  lcd.setCursor(9, 1);
  lcd.print("Spray:");
  lcd.setCursor(12, 1);
  lcd.print(txdata.sprayFlag);
//------------------------------------------
    
 for(int i=0; i<5; i++){
    //remember, you could use an if() here to check for new data, this time it's not needed.
    ETin.receiveData(); 
     delay(10);
  }
  //delay for good measure
  delay(10);
}
