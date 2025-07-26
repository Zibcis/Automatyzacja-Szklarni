#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <virtuabotixRTC.h>
#include <DHT.h>
#include <PCF8574.h>

#define EncoderOutputA 2
#define EncoderOutputB 4

// put function declarations here:
void encoderWork();
void setExpanderOutput();
int Counter, aState, aLastState, ctu, H1, M1, MD1, H2, M2, MD2, Selector, lastCounter, MenuCounter, LCDTimer;
bool xPump, xPorannePodlewanie, xWieczornePodlewanie, xFan, xLCDOneTime;
float duration, distance;

LiquidCrystal_I2C lcd(0x27,16,2);
virtuabotixRTC myRTC(7,6,5);
PCF8574 expander1(0x20);
DHT dht;

void setup() {
  lcd.init();
  myRTC.setDS1302Time(0, 12, 19, 4,24 , 7, 2025);
  dht.setup(12);
  pinMode(EncoderOutputA, INPUT);
  pinMode(EncoderOutputB, INPUT);
  pinMode(3,INPUT_PULLUP);
  pinMode(A1,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,INPUT);
  analogWrite(A1,255);
  expander1.pinMode(0,OUTPUT);
  expander1.pinMode(1,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(EncoderOutputA), encoderWork, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), setExpanderOutput, LOW);
  aLastState = digitalRead(EncoderOutputA);
  ctu = 0;
  xPump = 0;
  xFan = 0;
  xPorannePodlewanie = 1;
  xWieczornePodlewanie = 1;
  xLCDOneTime = 0;
  H1 = 6;
  H2 = 19;
  M1 = 30;
  M2 = 30;
  MD1 = 2;
  MD2 = 2;
  Selector = 0;
  lastCounter = 0;
  LCDTimer = 0;

}

void loop() {
if (ctu >= 100){
  myRTC.updateTime();
  lcd.clear();

  // Menu 
  switch(Counter){
    case(2):                  //Pompa Man
      lcd.setCursor(0,0);
      lcd.print("Stan Pompy:");
      lcd.setCursor(6,1);
      if (xPump == 0)
      {
        lcd.print("Wylaczona");
      }else{
        lcd.print("Zalaczona");
      }
    break;

    case(4):                  //Ustaw czas porannego podlewania
      lcd.setCursor(0,0);
      lcd.print("Poranne Podlewanie");
      lcd.setCursor(1,1);
      if(xPorannePodlewanie == 0){
        lcd.print("Nie");
      }else{
        lcd.print("Tak");
        lcd.print(" ");  
        lcd.print(H1);
        lcd.print(":");
        lcd.print(M1);
        lcd.print(" ");  
        lcd.print(MD1);
      }
      lcd.setCursor(13,1);
      lcd.print(Selector);
    break;

    case(6):                  //Ustaw czas wieczornego podlewania
      lcd.setCursor(0,0);
      lcd.print("Wieczorn Podlewanie");
      lcd.setCursor(1,1);
      if(xWieczornePodlewanie == 0){
        lcd.print("Nie");
      }else{
        lcd.print("Tak");
        lcd.print(" ");  
        lcd.print(H2);
        lcd.print(":");
        lcd.print(M2);
        lcd.print(" ");  
        lcd.print(MD2);
      }
      lcd.setCursor(13,1);
      lcd.print(Selector);
    break;

    default:                  //Basic screen
      lcd.setCursor(1,0);
      lcd.print(dht.getTemperature());
      lcd.print("*C");
      lcd.setCursor(10,0);
      lcd.print(myRTC.hours);
      lcd.print(":");
      lcd.print(myRTC.minutes);
      lcd.setCursor(1,1);
      lcd.print(dht.getHumidity());
      lcd.print("% ");
      lcd.print(distance);
      lcd.setCursor(11,1);
      //lcd.print(myRTC.dayofmonth);
      //lcd.print(".");
      //lcd.print(myRTC.month);
    break;
  }
  lcd.setCursor(15,1);
  lcd.print(Counter);
   ctu=0;
}


// Logika

if (Counter == 4)           //Usatawianie czasu porannego podlewania
{
  switch (Selector)
  {
  case 1:
    if (MenuCounter > lastCounter)
    {
      xPorannePodlewanie = 1;
    }else if(MenuCounter < lastCounter){
      xPorannePodlewanie = 0;
    } 
  case 2:
      if (MenuCounter > lastCounter)
    {
      H1++;
    }else if(MenuCounter < lastCounter){
      H1--;
    } 
    if (H1 > 24) H1 = 0;
    if (H1 < 0) H1 = 23;
  break;

  case 3:
      if (MenuCounter > lastCounter)
    {
      M1++;
    }else if(MenuCounter < lastCounter){
      M1--;
    } 
    if (M1 > 60) M1 = 0;
    if (M1 < 0) M1 = 59;
  break;

  case 4:
    if (MenuCounter > lastCounter)
    {
      MD1++;
    }else if(MenuCounter < lastCounter){
      MD1--;
    } 
    if (MD1 > 10) MD1 = 0;
    if (MD1 < 0) MD1 = 10;
  break;
  
  default:
    break;
  }
}

if (Counter == 6)         //Ustawianie wieczornego podlewania
{
  switch (Selector)
  {
  case 1:
    if (MenuCounter > lastCounter)
    {
      xWieczornePodlewanie = 1;
    }else if(MenuCounter < lastCounter){
      xWieczornePodlewanie = 0;
    } 
  case 2:
      if (MenuCounter > lastCounter)
    {
      H2++;
    }else if(MenuCounter < lastCounter){
      H2--;
    } 
    if (H2 > 24) H2 = 0;
    if (H2 < 0) H2 = 23;
  break;

  case 3:
      if (MenuCounter > lastCounter)
    {
      M2++;
    }else if(MenuCounter < lastCounter){
      M2--;
    } 
    if (M2 > 60) M2 = 0;
    if (M2 < 0) M2 = 59;
  break;

  case 4:
    if (MenuCounter > lastCounter)
    {
      MD2++;
    }else if(MenuCounter < lastCounter){
      MD2--;
    } 
    if (MD2 > 10) MD2 = 0;
    if (MD2 < 0) MD2 = 10;
  break;
  
  default:
    break;
  }
}

if(xPorannePodlewanie)   //Porannego podlewanie logika
  if(myRTC.hours == H1)
    if(myRTC.minutes > M1 && myRTC.minutes < M1 + MD1){
      xPump = 1;}
      else{
      xPump = 0;}

if(xWieczornePodlewanie)   //Porannego podlewanie logika
  if(myRTC.hours == H2)
    if(myRTC.minutes > M2 && myRTC.minutes < M1 + MD2){
      xPump = 1;}
      else{
      xPump = 0;}

if ((analogRead(A0)*(4.70/1024.0)*100) > 40.0){
  xFan = 1;
}
if((analogRead(A0)*(4.70/1024.0)*100) < 35.0){
  xFan = 0;
}

//Sterowanie podświetleniem ekranu
if (LCDTimer < 1510)
{
if (xLCDOneTime == 0)
{
lcd.backlight();
xLCDOneTime = 1;
}
}else{
 if (xLCDOneTime == 1)
 {
 lcd.noBacklight();
 xLCDOneTime = 0;
 }
  
}


//Uruchomienie Urządzeń wykonawnczych
expander1.digitalWrite(1, xPump);
expander1.digitalWrite(0, xFan);
//digitalWrite(9,LOW);
//delayMicroseconds(2);
//digitalWrite(9,HIGH);
//delayMicroseconds(10);
//digitalWrite(9,LOW);
//duration = pulseIn(8, HIGH);
//distance = (duration*0.0343)/2; 
delay(20);
ctu++;
if (LCDTimer < 3500)
{
 LCDTimer++;
}


lastCounter = MenuCounter;
}

    // put your main code here, to run repeatedly:
    
  //Here after clearing the LCD we take the time from the module and print it on the screen with usual LCD functions
 /*
 lcd.setCursor(0,0);
 lcd.print(analogRead(A0)*(4.70/1024.0)*100);
 lcd.print(" ");
 lcd.print(dht.getHumidity());
 lcd.print(" ");
 lcd.print(Counter);
 lcd.print(" ");
 lcd.print(xPump);
 lcd.setCursor(0,1);
 lcd.print(myRTC.hours);
 lcd.print(":");
 lcd.print(myRTC.minutes);
 lcd.print(":");
 lcd.print(myRTC.seconds);
 lcd.print(" ");
 lcd.print(dht.getTemperature());
 ctu=0;
    }
 expander1.digitalWrite(0,xPump);
 analogWrite(A1,255);
 delay(5);
 analogWrite(A1,0);
delay(15);
ctu++;

}
*/

void encoderWork(){
  aState = digitalRead(EncoderOutputA);
  if (aState != aLastState){
    if (digitalRead(EncoderOutputB) != aState){
      if(Selector == 0 ){
        Counter++;}else{
         MenuCounter++;}
    }else{
      if(Selector == 0 ){
        Counter--;}else{
        MenuCounter--;}
    }
  }
  if (Counter <0) {
    Counter = 6;
  }
  if (Counter > 6 ){
    Counter = 0;
  }
  aLastState = aState;
  LCDTimer = 0;
}

void setExpanderOutput(){
switch(Counter){
  case(2):
    if (xPump == 1){
      xPump = 0; 
    }else{
      xPump = 1;
    }
  break;

  case(4):
    Selector++;
    if (Selector > 4)
    Selector = 0;
    {
    }
    
  break;
}

}
/*
#include <Wire.h>

int lu;
byte error, address;

void setup() {
  Wire.begin();
  Serial.begin(115200);
  while (!Serial); // Czeka na otwarcie monitora portu szeregowego
  Serial.println("\nI2C Scanner startuje...");
 
}

void loop() {
  lu = 0;

  Serial.println("Skanuję...");

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Znaleziono urządzenie I2C na adresie 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println();
      lu++;
    }
  }

  if (lu == 0) {
    Serial.println("Nie znaleziono żadnych urządzeń I2C.");
  } else {
    Serial.println("Skanowanie zakończone.");
  }

  delay(5000); // Czekaj 5 sekund przed kolejnym skanowaniem
}
*/