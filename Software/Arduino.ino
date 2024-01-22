#include <Adafruit_GFX.h>  // Core graphics library
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;  // hard-wired for UNO shields anyway.
#include <TouchScreen.h>
// most mcufriend shields use these pins and Portrait mode:
uint8_t YP = A2;  // must be an analog pin, use "An" notation!
uint8_t XM = A1;  // must be an analog pin, use "An" notation!
uint8_t YM = 6;   // can be a digital pin
uint8_t XP = 7;   // can be a digital pin
uint8_t SwapXY = 0;
int Horiz;
int Vert;
char TX[50];
char *name = "Unknown controller";
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint tp;

#define MINPRESSURE 20
#define MAXPRESSURE 1000
uint16_t identifier, oldcolor, currentcolor;
uint8_t Orientation = 1;  //PORTRAIT
#include "Fonts/FreeSans12pt7b.h"
#include "Fonts/FreeMono24pt7b.h"
#define BLACK 0x0000
#define NAVY 0x000F
#define DARKGREEN 0x03E0
#define DARKCYAN 0x03EF
#define MAROON 0x7800
#define PURPLE 0x780F
#define OLIVE 0x7BE0
#define LIGHTGREY 0xC618
#define DARKGREY 0x7BEF
#define BLUE 0x001F
#define GREEN 0x07E0
#define CYAN 0x07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define ORANGE 0xFD20
#define GREENYELLOW 0xAFE5
#define PINK 0xF81F
String mess;
int line = 0;
int line0 = 0;
boolean pageflag = 0;
int counter0 = 0;
int counter1 = 0;
int Door_Relay = 10;  // Relay connected to digital pin 10
unsigned int long time1 = 0;
unsigned int long time2 = 0;

void setup(void) {
  Serial.begin(115200);
  tft.reset();
  identifier = tft.readID();
  ts = TouchScreen(XP, YP, XM, YM, 300);  //call the constructor AGAIN with new values.
  tft.begin(identifier);
  // tft.invertDisplay(1);
  pinMode(Door_Relay, OUTPUT);
  digitalWrite(Door_Relay, HIGH);
  tft.setRotation(Orientation);
  tft.setTextColor(WHITE, BLACK);
  tft.fillScreen(BLACK);
  tft.setFont(&FreeSans12pt7b);
  time1 = millis();
  //***************************************************Serial Monitor at the POWER ON *************************
start:
  if (Serial.available() > 0) {
    mess = Serial.readStringUntil('\n');
    int lenght = ((mess.length() / 27) - 1);
    tft.println(mess);
    line++;
    int row = lenght + line;
    if (row == 7) {
      tft.fillRect(0, 0, 320, 240 , BLACK);
      tft.setCursor(10, 22);
      line = 0;
    }
  }
  time2 = millis();

  if ((time2 - time1) < 45000)
  {
    goto start;
  }
  //***************************************************  END of Serial Monitor  ***********************
  tft.fillScreen(BLACK);
  tft.setCursor(35, 100);
  tft.println("Please touch the screen");
  Vert = 0;
}

void loop() {

  if ((Vert < 240 && Vert > 150 && Horiz > 300 && Horiz < 415) && (pageflag == 0))  //FROM START PAGE TO MAIN PAGE
  {
    Serial.flush();

    counter0 = 0;
    tft.fillScreen(BLACK);
    tft.setTextColor(WHITE, BLACK);
    tft.setTextSize(1);
    tft.setFont(&FreeSans12pt7b);
    tft.setCursor(80, 30);
    delay(50);
    Serial.print("Start");
    pageflag = 1;
  }

  show_tft();

  if (pageflag == 1) {

    if (Serial.available() > 0) {
      char found = Serial.read();
      if (found == '!') {
        tft.fillScreen(WHITE);
        tft.fillRoundRect(30, 15, 260, 210, 20, NAVY );
        tft.fillRoundRect(45, 30, 230, 180, 20, BLUE  );
        tft.setFont(&FreeMono24pt7b);
        tft.setCursor(60, 90);
        tft.println("WELCOME ");
        tft.setFont(&FreeSans12pt7b);
        tft.setCursor(65, 130);
        tft.println("PARINAZ YOUSEFI");//***********************************ID: 0************************************
        tft.setCursor(40, 160);
        tft.println("The Door is Now Open ");
        digitalWrite(Door_Relay, LOW);
        delay(1000);
        digitalWrite(Door_Relay, HIGH);
        for (int i = 0; i <= 80; i++) {
          delay(100);
        }
        Serial.flush();
        pageflag = 0;
        Vert = 0;
        tft.fillScreen(BLACK);
        tft.setCursor(35, 100);
        tft.println("Please touch the screen");
        show_tft();

      }
      if (found == '^') {
        tft.fillScreen(RED);
        tft.setFont(&FreeMono24pt7b);
        tft.setCursor(5, 90);
        tft.println("Unauthorize");
        tft.setFont(&FreeSans12pt7b);
        delay(10000);      
      Serial.flush();
      pageflag = 0;
      Vert = 0;
      tft.fillScreen(BLACK);
      tft.setCursor(35, 100);
      tft.println("Please touch the screen");
      show_tft();
    }
    }
    counter1++;
    delay(2);
    if (counter1 == 4500) {
      counter1 = 0;
      Serial.flush();
      pageflag = 0;
      Vert = 0;
      tft.fillScreen(BLACK);
      tft.setCursor(35, 100);
      tft.println("Please touch the screen");
      show_tft();
    }
    
  }

  if (pageflag == 0) {

    if (counter0 == 0) {

      tft.setRotation(Orientation);
      tft.fillScreen(PURPLE);
      tft.setTextColor(WHITE);
      tft.setTextSize(1);
      tft.setCursor(100, 40);
      tft.setFont(&FreeSans12pt7b);
      tft.print("Get Started");
      tft.setCursor(10, 90);
      tft.print("Remove any facial coverings");
      tft.setCursor(10, 120);
      tft.print("and please face the camera");
      tft.setCursor(50, 150);
      tft.print("Press START to ");
      tft.fillRoundRect(112, 163, 90, 40, 5, YELLOW);
      tft.setCursor(120, 190);
      tft.setTextColor(BLACK);
      tft.println("START");
      counter0++;
    }
  }

}
void show_tft(void) {
  /*tft.setTextColor(WHITE, BLACK);
    tft.setCursor(10, 30);
    tft.print("Touch screen for loc");*/
  while (!pageflag) {
    tp = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    pinMode(XP, OUTPUT);
    pinMode(YM, OUTPUT);

    if ((tp.z < MINPRESSURE || tp.z > MAXPRESSURE)) continue;
    {

      //tft.setCursor(0, (tft.height() * 3) / 4);
      //tft.print("tp.x=" + String(tp.x) + " tp.y=" + String(tp.y) + "tp.z=" + String(tp.z))
      Vert = (tp.x - 140);
      Horiz = (tp.y - 927) * -1;
    }
    break;  //if(tp.x > 450 && tp.x < 570  && tp.y > 680 && tp.y < 800)
  }
}

/**********************************************************************************************/
