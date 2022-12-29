/***************************************************************************
* Example sketch for the ADS1220_WE library
*
* This sketch shows how to change channels and get data. The sketch uses the 
* the default settings, e.g. the internal voltage reference. This limits the 
* maximum voltage that can be measured to 2.048 volts.
* 
* On my website you find:
*   1) An article about the ADS1220 and the use of this library
*      https://wolles-elektronikkiste.de/ads1220-4-kanal-24-bit-a-d-wandler  (German) 
*      https://wolles-elektronikkiste.de/en/4-channel-24-bit-adc-ads1220     (English)
* 
*   2) An article how to use the ADS1220 for typical applications, such as thermocouples, 
*      NTCs, RTDs or Wheatstone bridges:
*      https://wolles-elektronikkiste.de/ads1220-teil-2-anwendungen          (German)
*      https://wolles-elektronikkiste.de/en/ads1220-part-2-applications      (English)
* 
***************************************************************************/

#include <ADS1220_WE.h>
#include <SPI.h>

#define ADS1220_CS_PIN    7 // chip select pin
#define ADS1220_DRDY_PIN  6 // data ready pin 
#define ADC_MAX_VAL     8388607
//#define ADSC_CONSTANT     0.03692323568    // R = 11.3 ohm
#define ADSC_CONSTANT     0.01069827085      // R = 5.6 ohm
#define CURRENT_OFFSET     0.5      // R = 5.6 ohm


#define LOW_POWER_LED     14
#define WAKEUP_LED        8
#define OVER_LOAD_LED     9

#define SLEEP_CURRENT          5       //uA
#define SLEEP_OVER_CURRENT     500     //uA
#define WAKEUP_CURRENT         14000   //uA

/* Create your ADS1220 object */
ADS1220_WE ads = ADS1220_WE(ADS1220_CS_PIN, ADS1220_DRDY_PIN);
/* Alternatively you can also pass the SPI object as reference */
// ADS1220_WE ads = ADS1220_WE(&SPI, ADS1220_CS_PIN, ADS1220_DRDY_PIN);





void currentCheck(double current){
  if(current < 1){
    digitalWrite(LOW_POWER_LED,0);
    digitalWrite(WAKEUP_LED,0);
    digitalWrite(OVER_LOAD_LED,0);
  }
  else if(current >= 1 && current <= SLEEP_CURRENT){
    digitalWrite(LOW_POWER_LED,1);
    digitalWrite(WAKEUP_LED,0);
    digitalWrite(OVER_LOAD_LED,0);
  }
  else if(current <= SLEEP_OVER_CURRENT){
    digitalWrite(LOW_POWER_LED,0);
    digitalWrite(WAKEUP_LED,0);
    digitalWrite(OVER_LOAD_LED,1);
  }
  else if(current <= WAKEUP_CURRENT){
    digitalWrite(LOW_POWER_LED,0);
    digitalWrite(WAKEUP_LED,1);
    digitalWrite(OVER_LOAD_LED,0);
  }
  else if(current > WAKEUP_CURRENT){
    digitalWrite(LOW_POWER_LED,0);
    digitalWrite(WAKEUP_LED,0);
    digitalWrite(OVER_LOAD_LED,1);
  }
}



void setup(){  
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(14,OUTPUT);
  digitalWrite(LOW_POWER_LED,1);
  digitalWrite(WAKEUP_LED,1);
  digitalWrite(OVER_LOAD_LED,1);
  Serial.begin(115200);
  if(!ads.init()){
    Serial.println("ADS1220 is not connected!");
    while(1);
  }
  ads.setAvddAvssAsVrefAndCalibrate();
  ads.setDataRate(ADS1220_DR_LVL_0);
/* The voltages to be measured need to be between negative VREF + 0.2 V and positive
 * VREF -0.2 V if PGA is enabled. For this basic example I disable PGA, to be on the 
 * safe side. */ 
 ads.bypassPGA(true);

  Serial.print("Vref = ");
  Serial.println(ads.getVRef_V());
  ads.setCompareChannels(ADS1220_MUX_1_AVSS);
  delay(1000);
  digitalWrite(8,0);
  digitalWrite(9,0);
  digitalWrite(14,0);
}
void loop(){
  long longResult = 0;
  double data = 0;
  for(unsigned char i=0;i<20;i++){
    delay(10);
    longResult = longResult + ads.getRawData();
  }

  longResult = longResult / 20;

  data = (ADC_MAX_VAL - longResult) * ADSC_CONSTANT + CURRENT_OFFSET;
  if(data > 15){
    data = data + 2;
  }
  Serial.print(data,2);
  Serial.println("uA");
  //delay(20);
  currentCheck(data);
}
