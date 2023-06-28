#include <CapPin.h>
#include <Adafruit_NeoPixel.h>

int touchArray[5][2]={{0,3},{1,9},{2,12},{3,1},{4,2}};

CapPin star0 = CapPin(9);
CapPin star1 = CapPin(12);
CapPin star2 = CapPin(1);
CapPin star3 = CapPin(3);
CapPin planet = CapPin(2);

Adafruit_NeoPixel internalPixel = Adafruit_NeoPixel(1, 8, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel starsPixel = Adafruit_NeoPixel(6, 10, NEO_GRB + NEO_KHZ800);

int temperaturePin = A7;
//array of temps for average
float temperatureArray[20];
int temperaturePos = 0;
float temperatureAverage = 0; 
//start with planet in temp mode
bool tempMode = HIGH;
//bool star0State = LOW;
//bool star0LastState = LOW;
//bool star0Pressed = LOW;
//{star?State,star?LastState,star?Pressed}
bool starArray[5][3]= {{LOW,LOW,LOW},{LOW,LOW,LOW},{LOW,LOW,LOW},{LOW,LOW,LOW},{LOW,LOW,LOW}};
bool starLEDs[3]={LOW,LOW,LOW};
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;


void getTempValue ()
{
  float temperatureReading = analogRead(temperaturePin);
  temperatureReading*= 3.3;
  temperatureReading /= 1024;
  float temperatureValue = (temperatureReading - 0.5)*100;
  Serial.print(temperatureValue);
  Serial.println("C");
  temperatureArray[temperaturePos]=temperatureValue;
  if (temperaturePos <19){
    temperaturePos++;
  }else{
    temperaturePos=0;
  }
}

bool checkTouchSense(int starNum, int senseValue)
{
  Serial.print("Check pin ");
  Serial.println(starNum);
  Serial.print("Arduino pin ");
  Serial.println(touchArray[starNum][1]);
  Serial.print("Value sensed: ");
  Serial.println(senseValue);
  if (senseValue > 100){
    starArray[starNum][2]=HIGH;
  } else {
    starArray[starNum][2]=LOW;
  }
  if (starArray[starNum][2] != starArray[starNum][1])
  {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay){
    if (starArray[starNum][2] != starArray[starNum][0]){
        starArray[starNum][0] = starArray[starNum][2];
    }
  }
  starArray[starNum][1] = starArray[starNum][2];
  delay(10);
  if (starArray[starNum][0] == HIGH){
    return(HIGH);
  }else{
    return(LOW);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("start");
  internalPixel.begin();
  internalPixel.setPixelColor(0, internalPixel.Color(0,0,0));
  internalPixel.show();
  starsPixel.begin();
  starsPixel.setPixelColor(0, starsPixel.Color(0,0,0));  
  starsPixel.setPixelColor(1, starsPixel.Color(0,0,0));  
  starsPixel.setPixelColor(2, starsPixel.Color(0,0,0));  
  starsPixel.setPixelColor(3, starsPixel.Color(0,0,0));  
  starsPixel.setPixelColor(4, starsPixel.Color(0,0,0));  
  starsPixel.setPixelColor(5, starsPixel.Color(0,0,0));  
  starsPixel.show();
  for (int i=0; i<20; i++){
    getTempValue();  
    delay(10);
  }
}

void loop() {
  long starValue[5];
  starValue[0]=  star0.readPin(1000);
  starValue[1]=  star1.readPin(1000);
  starValue[2]=  star2.readPin(1000);
  starValue[3]=  star3.readPin(1000);
  starValue[4]=  planet.readPin(1000);
  for (int i= 0; i<5; i++){
    if (checkTouchSense(i, starValue[i])==HIGH)
    {
      if (starArray[i][1]!=starArray[i][0]){
        Serial.println("change!!!");
        if (i<3){
          starLEDs[i]= !starLEDs[i];  
        }else if (i == 3){
          Serial.println("all change");
          if (starLEDs[0]== LOW) {
            for (int x=0;x<3;x++){
              starLEDs[x]= HIGH;
              Serial.println("All on");
            }
          }else{
            for (int x=0;x<3;x++){
              starLEDs[x]= LOW;
              Serial.println("All off");
            }
          }
        }else if (i ==4){
          Serial.println("Planet");
          if (tempMode == HIGH){
            tempMode = LOW;
            Serial.println("Turn off planet");
          }else{
            tempMode = HIGH;
            Serial.println("Turn on Planet");
          }
        }        
      }
      Serial.println("Press detected");
    }
  }

  for (int i=0; i<3; i++){
    if (starLEDs[i] == HIGH){
      starsPixel.setPixelColor(i, starsPixel.Color(125,150,0));  
    }else{
      starsPixel.setPixelColor(i, starsPixel.Color(0,0,0));  
    }
  }
  starsPixel.show();

  getTempValue();

  for (int i=0;i<20;i++){
    temperatureAverage += temperatureArray[i];
  }
  temperatureAverage /= 20;
  Serial.print("Average Temperature: ");
  Serial.println(temperatureAverage);

  if (tempMode == HIGH){
    if (temperatureAverage >18){
      int tempColour = map(temperatureAverage, 18, 25, 125, 255);
      Serial.print("R: ");
      Serial.println(tempColour);
      Serial.print("B: ");
      Serial.println(125-(tempColour-125));
      
      for (int i=3; i<6; i++){
        starsPixel.setPixelColor(i, starsPixel.Color(tempColour,0,125-(tempColour-125)));
      }
    }else{
      int tempColour = map(temperatureAverage, 0, 18, 255, 125);
      Serial.print("B; ");     
      Serial.println(tempColour);
      for (int i=3; i<6; i++){
        starsPixel.setPixelColor(i, starsPixel.Color(125-(tempColour-125),0,tempColour));
      }
    }
  }else{
    starsPixel.setPixelColor(3, starsPixel.Color(0,0,0));  
    starsPixel.setPixelColor(4, starsPixel.Color(0,0,0));  
    starsPixel.setPixelColor(5, starsPixel.Color(0,0,0)); 
  }
  starsPixel.show();
  delay(100);
}
