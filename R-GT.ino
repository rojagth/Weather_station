#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <EduIntro.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MQ135.h>
#include <LiquidCrystal_I2C.h>

#define OLED_RESET 5
#define PIN_MQ135 A0
#define BUTTON_PIN 4

//první button
int QState=0;
int QPin=2;
int B1Pin=7;
int buttonNew1;
int buttonOld1=1;

//druhý button
int LState=0;
int LPin=9;
int B2Pin=8;
int buttonNew2;
int buttonOld2=1;

LiquidCrystal_I2C lcd(0x27,16,2);
Adafruit_SSD1306 display(OLED_RESET);
MQ135 mq135_sensor(PIN_MQ135);

DHT11 dht11(D13);
int C;  
int H;

void setup() {
  Serial.begin(9600);
  while (!Serial) ; // wait for serial
  delay(200);
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  lcd.init();
  pinMode(BUTTON_PIN, INPUT);
  
  //první button
  pinMode(QPin, OUTPUT);
  pinMode(B1Pin, INPUT);

  //druhý button
  pinMode(LPin, OUTPUT);
  pinMode(B2Pin, INPUT);
  }

  
void loop() {
  tmElements_t tm;
  if (RTC.read(tm)) {
    Serial.print("| Time = ");
    print2digits(tm.Hour);
    Serial.write(':');
    print2digits(tm.Minute);
    Serial.write(':');
    print2digits(tm.Second);
    Serial.print(", Date (D/M/Y) = ");
    Serial.print(tm.Day);
    Serial.write('/');
    Serial.print(tm.Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.println();
  } else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
  }
  
  dht11.update();
  C = dht11.readCelsius();       
  H = dht11.readHumidity();  
  float ppm = mq135_sensor.getCorrectedPPM(C, H);

  // Print the collected data in a row on the Serial Monitor
  Serial.print("ppm: ");
  Serial.println(ppm);
  Serial.print("Vlhkost: ");
  Serial.print(H);
  Serial.print("%");
  Serial.print("\tTeplota: ");
  Serial.print(C);
  Serial.println("°C");

    // Clear the display
    display.clearDisplay();
    //Set the color - always use white despite actual display color
    display.setTextColor(WHITE);
    //Set the font size
    display.setTextSize(1);
    //Set the cursor coordinates
    display.setCursor(0,0);
    display.print("ppm: ");
    display.print(ppm);
    display.print("   Q:");
    if (ppm<500){
      display.print(" A");
    } else {
      if (500<ppm<1000) {
        display.print(" B");
      }
      else {
        display.print(" C");
      }
    }
    display.setCursor(0,10);
    display.print("Cas: "); 
    print2digits(tm.Hour);
    display.print(':');
    print2digits(tm.Minute);
    display.print(':');
    print2digits(tm.Second);
    display.setCursor(0,20);
    display.print("Datum: ");
    display.print(tm.Day);
    display.print('/');
    display.print(tm.Month);
    display.print('/');
    display.print(tmYearToCalendar(tm.Year));
    display.display();

    lcd.setCursor(0,0);
    lcd.print("Teplota: ");
    lcd.print(C);
    lcd.print((char)223);
    lcd.print("C");
    lcd.setCursor(0,1);
    lcd.print("Vlhkost: ");
    lcd.print(H);
    lcd.print("%");  

    if (digitalRead(BUTTON_PIN) == HIGH) {
      lcd.backlight();
    }
    else {
    lcd.noBacklight();
    }

      //první button
buttonNew1=digitalRead(B1Pin);
if(buttonOld1==0 && buttonNew1==1){
  if (QState==0){
    digitalWrite(QPin,HIGH);
    QState=1;
  }
  else{
    digitalWrite(QPin, LOW);
    QState=0;
  }
}
buttonOld1=buttonNew1;

  //druhý button
buttonNew2=digitalRead(B2Pin);
if(buttonOld2==0 && buttonNew2==1){
  if (LState==0){
    digitalWrite(LPin,HIGH);
    LState=1;
  }
  else{
    digitalWrite(LPin, LOW);
    LState=0;
  }
}
buttonOld2=buttonNew2;
 }
 
void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
    display.print('0');
  }
  Serial.print(number);
  display.print(number);
}
