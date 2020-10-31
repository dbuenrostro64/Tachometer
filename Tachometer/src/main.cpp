#include <Arduino.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

const int TACH_PIN = 2;

unsigned long oneRevtime;
float rpmfloat;
unsigned int rpm;
bool tooslow = 1;

void calcRPM(void);
void displayRPM(int);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void RPM()  
{
  oneRevtime = TCNT1; // sets var equal to 16 bit timer value
  TCNT1 = 0; // resets value of timer
  tooslow = 0; // prevents timer overflow from occuring
}  

void setup() {
  TCCR1A = 0; // clears control registers
  TCCR1B = 0;
  TCCR1B |= (1<<CS12); // sets CS12 Bit(Clock Selection); Divide clock by 256 mode
  TIMSK1 |= (1<<TOIE1); // sets TOIE1 bit for Timer Int Mask; At Timer Overflow 1, jump to Interrupt Vector
  pinMode(TACH_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(TACH_PIN), RPM, FALLING); // at negative input, RPM function will run
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // don't proceed, loop forever
  } 
  display.display();
  delay(2000);
  display.clearDisplay();
}

ISR(TIMER1_OVF_vect) // runs when timer 1 overflows at value 65,535
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
    rpmfloat = 60*(62500.00/oneRevTime); // 
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
