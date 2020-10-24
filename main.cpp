#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

const int TACH_PIN = 2;

unsigned long rpmtime;
float rpmfloat;
unsigned int rpm;
bool tooslow = 1;

void calcRPM(void);
void displayRPM(int);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void RPM() 
{
  rpmtime = TCNT1;
  TCNT1 = 0;
  tooslow = 0;
}  

void setup() {
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1<<CS12);
  TIMSK1 |= (1<<TOIE1);
  pinMode(TACH_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(TACH_PIN), RPM, FALLING);
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  } 
  display.display();
  delay(2000);
  display.clearDisplay();
}

ISR(TIMER1_OVF_vect) 
{
  tooslow = 1;
}

void loop() {
  calcRPM();
  displayRPM(rpm);
}

void calcRPM(void)
{
  delay(250);
  if (tooslow == 1)
  {
    Serial.println("too slow");
    rpm = 0;
  }
  else 
  {
    rpmfloat = 120/(rpmtime/31250.00);
    rpm = round(rpmfloat);
  }
}

void displayRPM(int rpm)
{
  display.clearDisplay();
  display.setTextSize(2);      
  display.setTextColor(WHITE); 
  display.setCursor(0, 10);     
  display.cp437(true);         
  display.print("RPM:");
  display.println(rpm);
  display.display();
}
