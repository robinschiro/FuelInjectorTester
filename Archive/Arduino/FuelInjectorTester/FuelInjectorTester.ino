/*********************************************************************
This is a sketch that operates a fuel injector and measures mass flow through it
*********************************************************************/

#include "Adafruit_GFX.h"
#include "Adafruit_PCD8544.h"

// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 3, 4);

int loadCell = 0;
int contrast = 1;
int startReq = 0;
int zeroReq = 0;
int spanReq = 0;
float mass;
float massFlo;
float initMass;
float finalMass;
float totMass;
float dutyCycle;
float injRating;
int injPin = 9;
int startPB = 8;
int zeroPB = 10;
int spanPB = 11;
int led = 13;
float onTime;
float offTime;
int testTime;
int purgeTime;
boolean testReq;
boolean testPurge;
boolean testPrep;
boolean testStart;
boolean testComp;
unsigned long TimerA; //this is used to start the initialization
unsigned long TimerB; //this is used to control the injector cycle time
char* dispStatus[] = {"Initializing", "Air Purge", "Preparing Test", "Testing", "Zero", "Span", "Mass Flow Of  Injector"};
float spanWeight = 1.10231;  //this is the 500 gram calibration weight converted to pounds
float loadCellSpan = 11.0231; //this is the capacity of the load cell
float newZero;
float newSpan;
float slope = 1;
float intercept = 0;
float contrastSlope = 0.09765625;

static unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

void setup()   {
  Serial.begin(9600);
  pinMode(injPin, OUTPUT);
  pinMode(led, OUTPUT); 
  pinMode(startPB, INPUT);
  pinMode(zeroPB, INPUT);
  pinMode(spanPB, INPUT);

  
  onTime = 31;
  offTime = 39;
  testTime = 30000;
  purgeTime = 5000;
  testReq = 0;
  testPrep = 0;
  testPurge = 0;
  testStart = 0;
  testComp = 0;

  display.begin();
  // init done

 

}

void loop() 
{
  
     // you can change the contrast around to adapt the display
  // for the best viewing!
  display.setContrast(analogRead(contrast)*contrastSlope);
  Serial.println(analogRead(contrast));
  startReq = digitalRead(startPB);
  zeroReq = digitalRead(zeroPB);
  spanReq = digitalRead(spanPB);
 
  if (startReq == HIGH) {         // check if the input is HIGH (button released)
     TimerA = millis();
     digitalWrite(led, HIGH);  // turn LED ON
     testReq=1;
     testInitialize();
  } else {
    digitalWrite(led, LOW);  // turn LED OFF
    displayData();
  }
  
  if (zeroReq == HIGH) {
    calcZero();
    Serial.println("zero button pressed");
  }
  
  if (spanReq == HIGH) {
    calcSpan();
    Serial.println("span button pressed");
  }
  

  }

float calcDutyCycle()
{
  return dutyCycle=(onTime/offTime)*100;
}

float massFlow()
{
  //mass = ((analogRead(loadCell))*0.003271);
  mass = (slope * (analogRead(loadCell)))+intercept;
}

float calcInjRating()
{
 return injRating = ((totMass/(testTime/1000))*3600)*((1/(100-calcDutyCycle()))+1);
}

void calcZero()
{
  newZero = (analogRead(loadCell));
  Serial.println(newZero);
}

void calcSpan()
{
 newSpan =  (analogRead(loadCell));
 Serial.println(newSpan);
 calcSlope();
 calcIntercept();
}

void calcSlope()
{
 slope = spanWeight/(newSpan - newZero); 
}

void calcIntercept()
{
 intercept = spanWeight-(newSpan*slope);
 
}

void displayData()
{
  
        //dispMass();
                    // text display tests
        display.setTextSize(1);
        display.setTextColor(BLACK);
        display.setCursor(0,0);
        display.println("Injector Test");
        display.setTextColor(WHITE, BLACK); // 'inverted' text
        display.println("current weight in pounds");
        display.setTextSize(3);
        display.setTextColor(BLACK);
        //display.println(random());
        display.println(massFlow());
        display.display();
        display.clearDisplay();
        delay(100);
}

void injectorWOT()
{
    digitalWrite(injPin,HIGH);
    delay(onTime);
    digitalWrite(injPin,LOW);
    delay(offTime);
  
}

void testInitialize()
{      Serial.println(mass);
       while(testReq=1)
       {
         displayInit(dispStatus[0]);
         if(millis() - TimerA >= 5000UL)
         {
            break;
         }
       }
        testReq=0;
        testPurge=1;
        injPurge();

}

void injPurge()
{     displayInit(dispStatus[1]);
      TimerB = millis();
      while(testStart = 1)
      {  
        injectorWOT();
        if(millis() - TimerB >= purgeTime)
           {
              break;
           }
      }
        testPurge=0;
        testPrep=1;
        testPrepare();
}

void testPrepare()
{     displayInit(dispStatus[2]);
      TimerB = millis();
      while(testPrep = 1)
      {  
        if(millis() - TimerB >= 3000UL)
           {
              break;
           }
      }
        initMass=massFlow();
        Serial.println(initMass);
        testPrep=0;
        testStart=1;
        testBegin();
}

void testBegin()
{     displayInit(dispStatus[3]);
      TimerB = millis();
      while(testStart = 1)
      {  
        injectorWOT();
        if(millis() - TimerB >= testTime)
           {
              break;
           }
      }

      finalMass=massFlow();
      Serial.println(finalMass);
      totMass = initMass-finalMass;
      testStart=0;
      testComp=1;
      testComplete();
}

void testComplete()
{     
      Serial.println(totMass);
      displayFinal(dispStatus[6]);
      TimerB = millis();
      while(testComp = 1)
      {  
        if(millis() - TimerB >= 30000UL)
           {
              break;
           }
      }
      testComp=0;
}

void displayInit(char* funDay)
{
                    // text display tests
        display.setTextSize(1);
        display.setTextColor(BLACK);
        display.setCursor(0,0);
        display.println(funDay);
        display.setTextColor(WHITE, BLACK); // 'inverted' text
        display.println("lbs");
        display.setTextSize(3);
        display.setTextColor(BLACK);
        //display.println(random());
        display.println(massFlow());
        display.display();
        display.clearDisplay();
        delay(100);
}

void displayFinal(char* funDay)
{
                    // text display tests
        display.setTextSize(2);
        display.setTextColor(WHITE, BLACK); // 'inverted' text
        display.setCursor(0,0);
        display.println("RESULTS");
        display.setTextSize(1);
        display.setTextColor(BLACK);
        display.setCursor(0,20);
        display.print("% DC= ");
        display.setCursor(40,20);
        display.println(calcDutyCycle());
        display.setCursor(0,30);
        display.print("lbs = ");
        display.setCursor(40,30);
        display.println(totMass);
        display.setCursor(0,40);
        display.print("IR=");
        display.setCursor(40,40);
        display.println(calcInjRating());
        display.display();
        display.clearDisplay();
        delay(100);
}



