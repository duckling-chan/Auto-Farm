#define green_pin                 6
#define red_pin                   7
#define blue_pin                  5
#define interrupt_a               2
#define interrupt_b               3
//#define interrupt_c             some pins

int r = 0, g = 0, b = 0;
int colour_count = 4; //default to purple
int brightness_count = 6; //default to brightest
volatile int interrupt_1 = 0;
volatile int interrupt_2 = 0;
//volatile int interrupt_3 = 0;
unsigned long button_time_A = 0;  
unsigned long last_button_time_A = 0;
unsigned long button_time_B = 0;  
unsigned long last_button_time_B = 0; 

void setup() {
  Serial.begin(9600);
  pinMode(red_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);
  pinMode(interrupt_1, INPUT);
  pinMode(interrupt_2, INPUT);
//  pinMode(interrupt_3, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), inc_colour_count, RISING);
  attachInterrupt(digitalPinToInterrupt(3), brightness_up, RISING);
//  attachInterrupt(digitalPinToInterrupt(some pins), brightness_down, RISING);
}

void loop()
{
  display_colour();//call function to turn on LED
  Serial.print(r);
  Serial.print("\n");
  Serial.print(g);
  Serial.print("\n");
  Serial.print(b);
  Serial.print("\n");
  delay(1000);
}

void display_colour()
{
  if (colour_count == 1)
    red();
  else if (colour_count == 2)
    blue();
  else if (colour_count == 3)
    green();
  else if (colour_count == 4)
    purple();
  else if (colour_count == 5)
    black();
  else;
}

void black()
{
  r = 0;
  g = 0;
  b = 0;
  turn_on(r,g,b);
}

void red()
{
  r = 255;
  g = 0;
  b = 0;
  
  r = brightness_red(r);
  g = brightness_green(g);
  b = brightness_blue(b);
  turn_on(r,g,b);
}

void blue()
{
  r = 0;
  g = 0;
  b = 255;
  
  r = brightness_red(r);
  g = brightness_green(g);
  b = brightness_blue(b);
  turn_on(r,g,b);
}

void green()
{
  r = 0;
  g = 255;
  b = 0;
  
  r = brightness_red(r);
  g = brightness_green(g);
  b = brightness_blue(b);
  turn_on(r,g,b);
}

void purple()
{
  r = 255;
  g = 0;
  b = 255;
  
  r = brightness_red(r);
  g = brightness_green(g);
  b = brightness_blue(b);
  turn_on(r,g,b);
}

int brightness_red (int r)
{
  if (brightness_count == 1) // 1 being dimmest, 6 being brightest
    r = 1*r/6;
  else if (brightness_count == 2) // 1 being dimmest, 6 being brightest
    r = 2*r/6;
  else if (brightness_count == 3) // 1 being dimmest, 6 being brightest
    r = 3*r/6;
  else if (brightness_count == 4) // 1 being dimmest, 6 being brightest
    r = 4*r/6;
  else if (brightness_count == 5) // 1 being dimmest, 6 being brightest
    r = 5*r/6;
  else if (brightness_count == 6) // 1 being dimmest, 6 being brightest
    r = 6*r/6;
  else;
  return r;
}

int brightness_green (int g)
{
  if (brightness_count == 1) // 1 being dimmest, 6 being brightest
    g = 1*g/6;
  else if (brightness_count == 2) // 1 being dimmest, 6 being brightest
    g = 2*g/6;
  else if (brightness_count == 3) // 1 being dimmest, 6 being brightest
    g = 3*g/6;
  else if (brightness_count == 4) // 1 being dimmest, 6 being brightest
    g = 4*g/6;
  else if (brightness_count == 5) // 1 being dimmest, 6 being brightest
    g = 5*g/6;
  else if (brightness_count == 6) // 1 being dimmest, 6 being brightest
    g = 6*g/6;
  else;
  return g;
}

int brightness_blue (int b)
{
  if (brightness_count == 1) // 1 being dimmest, 6 being brightest
    b = 1*b/6;
  else if (brightness_count == 2) // 1 being dimmest, 6 being brightest
    b = 2*b/6;
  else if (brightness_count == 3) // 1 being dimmest, 6 being brightest
    b = 3*b/6;
  else if (brightness_count == 4) // 1 being dimmest, 6 being brightest
    b = 4*b/6;
  else if (brightness_count == 5) // 1 being dimmest, 6 being brightest
    b = 5*b/6;
  else if (brightness_count == 6) // 1 being dimmest, 6 being brightest
    b = 6*b/6;
  else;
  return b;
}

void turn_on(int r, int g, int b)
{
  analogWrite(red_pin, r);
  analogWrite(green_pin, g);
  analogWrite(blue_pin, b);
}

void inc_colour_count()
{
  button_time_A = millis();
  if (button_time_A - last_button_time_A > 250)
  {
//    lcd_A = 1;
    if(colour_count > 4)
      colour_count = 1;
    else
      colour_count = colour_count + 1;
    last_button_time_A = button_time_A;
  }
  else;  
}

void brightness_up()
{
  button_time_B = millis();
  if (button_time_B - last_button_time_B > 250)
  {
//    lcd_B = 1;
    if (brightness_count < 6)
      brightness_count = brightness_count + 1;
    else
      brightness_count = 0;
    last_button_time_B = button_time_B;
  }
  else; 
}
