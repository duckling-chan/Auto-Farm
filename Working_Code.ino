//####### SMALL STEPPER #######
#include <Stepper.h>

#define STEPS_PER_MOTOR_REVOLUTION 32   
Stepper small_stepper(STEPS_PER_MOTOR_REVOLUTION, 42, 44, 43, 45);
int  Steps2Take;

//####### LCD #######
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C  lcd(0x3F,16,2, LCD_5x8DOTS); // 0x27 is the I2C bus address for an unmodified backpack

//####### WATER TEMP #######
#include <DallasTemperature.h>
#include <OneWire.h>

#define ONE_WIRE_BUS 34
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//####### RGB #######
#define green_pin                 6
#define red_pin                   7
#define blue_pin                  5

int r, g, b;
int colour_count = 4; //default to purple
int brightness_count = 5; //default to brightest
//int water_test = 0; //might not need | PLS REVIEW
//int drain_refill_count = 0; //might not need | PLS REVIEW

//####### WATER LEVEL SENSOR #######
#define water_level_sensor_bottom 40 // yellow | black wire
#define water_level_sensor_top  41 // red | brown wire

int buttonState_bottom = 1; // variable for reading the pushbutton status
int buttonState_top = 1; // variable for reading the pushbutton status

//####### AIR HUM AND TEMP #######
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN  35
#define DHTTYPE DHT11

DHT_Unified dht(DHTPIN, DHTTYPE);

//####### VALVE #######
#define valve 23

//####### FAN #######
#define fan 22

//####### PUMPS #######
#define testpump 24
#define mainpump 25

//####### MOTOR #######
#define motor 26

//####### TAKE PICTURE #######
#include <Adafruit_VC0706.h>
#include <SPI.h>
#include <SD.h>

#define SD_CS 53 //define Slave Select pin
Adafruit_VC0706 cam = Adafruit_VC0706(&Serial2);

int i; //to loop for setting resolution

//####### DECODE JPEG #######
#include <JPEGDecoder.h>

int looper;
char pic_name[12];
int five_counter = 0; //to increase pic_name[5]
int six_counter = 0; //to increase pic_name[6]

//####### SD | STORE DATA #######
File myFile;

//####### XY MOVEMENT #######
#define x_step 39
#define x_dir 38  //LOW = towards button | HIGH = away from button
#define y_step 37
#define y_dir 36  //LOW = towards button | HIGH = away from button
#define xhome 49
#define yhome 48

int buttonState_xhome = 1; //read for xhome button press
int buttonState_yhome = 1; //read for yhome button press
int x_release; //to release x button
int y_release; //to release y button
int x_now; //current position for x-axis
int y_now; //current position for y-axis

//####### BUTTONS & INTERRUPTS #######
#define interrupt_a               2
#define interrupt_b               3
#define interrupt_c               18
#define interrupt_d               19

int lcd_A = 0; //colour
int lcd_B = 0; //brightness
int lcd_C = 0; //take picture + test water + decode JPEG
int lcd_D = 0; //drain and refill
volatile int interrupt_1 = 0;
volatile int interrupt_2 = 0;
volatile int interrupt_3 = 0;
volatile int interrupt_4 = 0;
unsigned long button_time_A = 0;  
unsigned long last_button_time_A = 0;
unsigned long button_time_B = 0;  
unsigned long last_button_time_B = 0;
unsigned long button_time_C = 0;  
unsigned long last_button_time_C = 0;
unsigned long button_time_D = 0;  
unsigned long last_button_time_D = 0;

//####### TIMER #######
unsigned long button_time_E = 0;    //for data logging
unsigned long last_button_time_E = 0;
unsigned long button_time_F = 0;    //for changing water
unsigned long last_button_time_F = 0;
unsigned long button_time_G = 0;    //for taking photos + water test
unsigned long last_button_time_G = 0;
unsigned long button_time_H = 0;    //for checking humidity and temperature
unsigned long last_button_time_H = 0;

void setup() {
  Serial.begin(115200);

  //####### LCD #######
  lcd.begin(); // for 16 x 2 LCD module
  lcd.clear(); 
  
  //####### WATER TEMP #######
  sensors.begin();

  //####### HUM TEMP #######
  dht.begin();
  
  //####### RGB #######
  pinMode(red_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);

  //####### BUTTONS AND INTERRUPTS #######
  pinMode(interrupt_a, INPUT);
  pinMode(interrupt_b, INPUT);
  pinMode(interrupt_c, INPUT);
  pinMode(interrupt_d, INPUT);
  
  attachInterrupt(digitalPinToInterrupt(2), inc_colour_count, RISING);
  attachInterrupt(digitalPinToInterrupt(3), brightness_up, RISING);
  attachInterrupt(digitalPinToInterrupt(18), take_picture, RISING);
  attachInterrupt(digitalPinToInterrupt(19), drain_refill, RISING);
  
  //####### WATER LEVEL SENSOR #######
  pinMode(water_level_sensor_bottom, INPUT);
  pinMode(water_level_sensor_top, INPUT);

  //####### VALVE #######
  pinMode(valve, OUTPUT); 
  digitalWrite(valve, HIGH); //low - turn on relay | high - turn off;
  
  //####### FAN #######
  pinMode(fan, OUTPUT); 
  digitalWrite(fan, HIGH); //low - turn on relay | high - turn off;

  //####### PUMPS #######
  pinMode(mainpump, OUTPUT);
  digitalWrite(mainpump, HIGH); //low - turn on relay | high - turn off;
  pinMode(testpump, OUTPUT);
  digitalWrite(testpump, HIGH); //low - turn on relay | high - turn off;

  //####### MOTOR #######
  pinMode(motor, OUTPUT);
  digitalWrite(motor, HIGH); //low - turn on relay | high - turn off;
  
  //####### DECODE JPEG #######
  pinMode(13, OUTPUT); //SPI might hang if not set

  // Start the SD
  if (!SD.begin(SD_CS)) {
    // If the SD can't be started, loop forever
    Serial.println("SD failed or not present!");
    while (1);
  }
  
  //####### XY MOVEMENT #######
  pinMode(x_step, OUTPUT);
  pinMode(x_dir, OUTPUT);
  pinMode(y_step, OUTPUT);
  pinMode(y_dir, OUTPUT);
  pinMode(xhome, INPUT);
  pinMode(yhome, INPUT);

  digitalWrite(motor, LOW);
  xhome_go(); //calibrate x-axis to home position
  yhome_go(); //calibrate y-axis to home position
  digitalWrite(motor, HIGH);
  
  lcd_A = 1; //initialise to show "Colour"
}

void loop() {
button_time_E = millis();
button_time_F = millis();
button_time_G = millis();
button_time_H = millis();

  if (button_time_E - last_button_time_E > 600000) //10 minutes
  {
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("DATALOG.txt", FILE_WRITE);
 
  // if the file opened okay, write to it:
  if (myFile) {
    sensors_event_t event;  
    myFile.print("Humidity;");
    dht.humidity().getEvent(&event);
    myFile.print(event.relative_humidity);
    myFile.print(";  Temp;");
    dht.temperature().getEvent(&event);
    myFile.print(event.temperature);
    myFile.print(";  Water_temperature;");
    sensors.requestTemperatures();    
    myFile.print(sensors.getTempCByIndex(0));
    myFile.print('\n');
  // close the file:
    myFile.close();
  //  Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening DATALOG.txt");
  }
last_button_time_E = button_time_E;
}

  if (button_time_F - last_button_time_F > 25920000) //3 days
  {
    buttonState_bottom = digitalRead(water_level_sensor_bottom);
    buttonState_top = digitalRead(water_level_sensor_top);
    if(buttonState_bottom == HIGH && buttonState_top == HIGH) //both sensors at bottom
    {
      lcd.setCursor(0,0);
      lcd.print("Refilling water ");
      lcd.setCursor(0,1);
      lcd.print("                ");  
      //no need to open valve since water level is at minimum
      while(buttonState_top != LOW) //keep refilling until TOP is afloat
      {
        buttonState_top = digitalRead(water_level_sensor_top);
        digitalWrite(mainpump, LOW); //turn on main pump
      }
      digitalWrite(mainpump, HIGH); //turn off main pump
    }
    else //if((buttonState_bottom == LOW && buttonState_top == HIGH) || (buttonState_bottom == LOW && buttonState_top == LOW))
    {
      //Step 1. Drain water
      lcd.setCursor(0,0);
      lcd.print("Draining water  ");
      lcd.setCursor(0,1);
      lcd.print("                ");
      while(buttonState_bottom != HIGH)
      { 
        buttonState_bottom = digitalRead(water_level_sensor_bottom);
        digitalWrite(valve, LOW); //open valve as long as there is still water
      }
      digitalWrite(valve, HIGH); //allow valve to close
     
      //Step 2. Refill water
      lcd.setCursor(0,0);
      lcd.print("Refilling water ");
      lcd.setCursor(0,1);
      lcd.print("                "); 
      buttonState_top = digitalRead(water_level_sensor_top); //need to update cuz last saved State was LOW (afloat)
      while(buttonState_top != LOW)
      {
        buttonState_top = digitalRead(water_level_sensor_top);
        digitalWrite(mainpump, LOW);
      }
      digitalWrite(mainpump, HIGH);
    }
last_button_time_F = button_time_F;
}

if (button_time_G - last_button_time_G > 3600000) //1 hour
{
  //####### TAKE PICTURE #######
        lcd.setCursor(0,0);
        lcd.print("Moving to Pot 1 ");
        lcd.setCursor(0,1);
        lcd.print("                ");  
  pot1_go();
        lcd.setCursor(0,0);
        lcd.print("Taking picture  ");
        take_pic();
        lcd.setCursor(0,1);
        lcd.print("Done!           ");
        delay(2000);
        lcd.setCursor(0,0);
        lcd.print("Moving to Pot 2 ");  
        lcd.setCursor(0,1);
        lcd.print("                ");
  pot2_go();
        lcd.setCursor(0,0);
        lcd.print("Taking picture  ");
        take_pic();
        lcd.setCursor(0,1);
        lcd.print("Done!           ");
        delay(2000);
        lcd.setCursor(0,0);
        lcd.print("Moving to Pot 3 ");
        lcd.setCursor(0,1);
        lcd.print("                ");
  pot3_go();
        lcd.setCursor(0,0);
        lcd.print("Taking picture  ");  
        take_pic();
        lcd.setCursor(0,1);
        lcd.print("Done!           ");
        delay(2000);
        lcd.setCursor(0,0);
        lcd.print("Moving to Pot 4 ");  
        lcd.setCursor(0,1);
        lcd.print("                ");
  pot4_go();
        lcd.setCursor(0,0);
        lcd.print("Taking picture  ");    
        take_pic();
        lcd.setCursor(0,1);
        lcd.print("Done!           ");
        delay(2000);
        lcd.setCursor(0,0);
        lcd.print("Moving to Pot 5 ");   
        lcd.setCursor(0,1);
        lcd.print("                ");
  pot5_go();
        lcd.setCursor(0,0);
        lcd.print("Taking picture  ");     
        take_pic();
        lcd.setCursor(0,1);
        lcd.print("Done!           ");
        delay(2000);
        lcd.setCursor(0,0);
        lcd.print("Moving to Pot 6 ");   
        lcd.setCursor(0,1);
        lcd.print("                ");
  pot6_go();
        lcd.setCursor(0,0);
        lcd.print("Taking picture  ");    
        take_pic();
        lcd.setCursor(0,1);
        lcd.print("Done!           ");
        delay(2000);
        lcd.setCursor(0,0);
        lcd.print("Moving to Test  ");  
        lcd.setCursor(0,1);
        lcd.print("                ");
  testarea_go();
        hold_test_water();
        digitalWrite(testpump, LOW);
        delay(3000);
        digitalWrite(testpump, HIGH);
        lcd.setCursor(0,0);
        lcd.print("Taking picture  ");   
        take_pic();
        lcd.setCursor(0,1);
        lcd.print("Done!           ");
        dump_test_water();
        delay(2000);
        lcd.setCursor(0,0);
        lcd.print("Sending pictures");  
        lcd.setCursor(0,1);
        lcd.print("                ");
  
  for(looper = 0; looper < 7; looper ++)
  {
  // Open the root directory
  File root = SD.open("/");
  
    strcpy(pic_name, "IMAGE00.JPG");
    if(six_counter > 9)
    {
      five_counter = five_counter + 1;
      six_counter = 0;
    }
    six_counter = six_counter + 1;
    setpic_name();
  }
  
  digitalWrite(motor, LOW);
  xhome_go();
  yhome_go();
  digitalWrite(motor, HIGH);
  last_button_time_G = button_time_G;  
  }

  if (button_time_H - last_button_time_H > 300000) // 5 minutes
  {  
    sensors_event_t event;  
    lcd.setCursor(0,0);
    lcd.print("Air temp |      ");  
    lcd.setCursor(11,0);
    dht.temperature().getEvent(&event);
    lcd.print(event.temperature);
    Serial.print(event.temperature);
    lcd.setCursor(0,1);
    lcd.print("Humidity |      ");
    lcd.setCursor(11,1);
    dht.humidity().getEvent(&event);
    lcd.print(event.relative_humidity);
    Serial.print(event.relative_humidity);

    dht.humidity().getEvent(&event);
    while(event.relative_humidity > 90)
    {
      digitalWrite(fan, LOW); //turn on the fan
      dht.humidity().getEvent(&event);
    }
    
    dht.temperature().getEvent(&event);
    while(event.temperature > 30)
    {
      digitalWrite(fan, LOW);
      dht.temperature().getEvent(&event);
    }
    digitalWrite(fan, HIGH);
    last_button_time_H = button_time_H;    
  }
  
if (lcd_A == 1)
{
  switch(colour_count)
  {
    case 1:
      lcd.setCursor(0,0);
      lcd.print("COLOUR          ");
      lcd.setCursor(0,1);
      lcd.print("    < RED >     ");
      display_colour();
      break;
    case 2:
      lcd.setCursor(0,0);
      lcd.print("COLOUR          ");
      lcd.setCursor(0,1);
      lcd.print("    < BLUE >    ");
      display_colour();
      break;
    case 3:
      lcd.setCursor(0,0);
      lcd.print("COLOUR          ");
      lcd.setCursor(0,1);
      lcd.print("    < GREEN >   ");
      display_colour();
      break;
     case 4:
      lcd.setCursor(0,0);
      lcd.print("COLOUR          ");     
      lcd.setCursor(0,1);
      lcd.print("    < PURPLE >  ");
      display_colour();
      break;
  }
  lcd_A = 0;
}
else if (lcd_B == 1)
{
  switch(brightness_count)
  {
    case 0:
      lcd.setCursor(0,0);
      lcd.print("BRIGHTNESS      ");
      lcd.setCursor(0,1);
      lcd.print("     < 0 >      ");
      display_colour();
      break;
    case 1:
      lcd.setCursor(0,0);
      lcd.print("BRIGHTNESS      ");
      lcd.setCursor(0,1);
      lcd.print("     < 1 >      ");
      display_colour();
      break;
    case 2:
      lcd.setCursor(0,0);
      lcd.print("BRIGHTNESS      ");
      lcd.setCursor(0,1);
      lcd.print("     < 2 >      ");
      display_colour();
      break;
    case 3:
      lcd.setCursor(0,0);
      lcd.print("BRIGHTNESS      ");
      lcd.setCursor(0,1);
      lcd.print("     < 3 >      ");
      display_colour();
      break;
    case 4:
      lcd.setCursor(0,0);
      lcd.print("BRIGHTNESS      ");     
      lcd.setCursor(0,1);
      lcd.print("     < 4 >      ");
      display_colour();
      break;
    case 5:
      lcd.setCursor(0,0);
      lcd.print("BRIGHTNESS      ");     
      lcd.setCursor(0,1);
      lcd.print("     < 5 >      ");
      display_colour();
      break;
  }
  lcd_B = 0;
}

else if (lcd_C == 1)
{
  //####### TAKE PICTURE #######
        lcd.setCursor(0,0);
        lcd.print("Moving to Pot 1 ");
        lcd.setCursor(0,1);
        lcd.print("                ");  
  pot1_go();
        lcd.setCursor(0,0);
        lcd.print("Taking picture  ");
        take_pic();
        lcd.setCursor(0,1);
        lcd.print("Done!           ");
        delay(2000);
        lcd.setCursor(0,0);
        lcd.print("Moving to Pot 2 ");  
        lcd.setCursor(0,1);
        lcd.print("                ");
  pot2_go();
        lcd.setCursor(0,0);
        lcd.print("Taking picture  ");
        take_pic();
        lcd.setCursor(0,1);
        lcd.print("Done!           ");
        delay(2000);
  /*      lcd.setCursor(0,0);
        lcd.print("Moving to Pot 3 ");
        lcd.setCursor(0,1);
        lcd.print("                ");
  pot3_go();
        lcd.setCursor(0,0);
        lcd.print("Taking picture  ");  
        take_pic();
        lcd.setCursor(0,1);
        lcd.print("Done!           ");
        delay(2000);
        lcd.setCursor(0,0);
        lcd.print("Moving to Pot 4 ");  
        lcd.setCursor(0,1);
        lcd.print("                ");
  pot4_go();
        lcd.setCursor(0,0);
        lcd.print("Taking picture  ");    
        take_pic();
        lcd.setCursor(0,1);
        lcd.print("Done!           ");
        delay(2000);
        lcd.setCursor(0,0);
        lcd.print("Moving to Pot 5 ");   
        lcd.setCursor(0,1);
        lcd.print("                ");
 pot5_go();
        lcd.setCursor(0,0);
        lcd.print("Taking picture  ");     
        take_pic();
        lcd.setCursor(0,1);
        lcd.print("Done!           ");
        delay(2000);
        lcd.setCursor(0,0);
        lcd.print("Moving to Pot 6 ");   
        lcd.setCursor(0,1);
        lcd.print("                ");
  pot6_go();
        lcd.setCursor(0,0);
        lcd.print("Taking picture  ");    
        take_pic();
        lcd.setCursor(0,1);
        lcd.print("Done!           ");
        delay(2000);
        lcd.setCursor(0,0);
        lcd.print("Moving to Test  ");  
        lcd.setCursor(0,1);
        lcd.print("                ");
  testarea_go();
*/        hold_test_water();
        digitalWrite(testpump, LOW);
        delay(3000);
        digitalWrite(testpump, HIGH);
        lcd.setCursor(0,0);
        lcd.print("Taking picture  ");   
//        take_pic();
        lcd.setCursor(0,1);
        lcd.print("Done!           ");
        delay(2000);
        dump_test_water();
        delay(2000);
        lcd.setCursor(0,0);
        lcd.print("Sending pictures");  
        lcd.setCursor(0,1);
        lcd.print("                ");
  
  for(looper = 0; looper < 2; looper ++)
  {
  // Wait for the PC to signal
  //while(!Serial.available());
  //  if(looper == 0)
  //    strcpy(pic_name, "IMAGE02.JPG");
  //  else 
  //    strcpy(pic_name, "IMAGE03.JPG");

    strcpy(pic_name, "IMAGE00.JPG");
    if(six_counter > 9)
    {
      five_counter = five_counter + 1;
      six_counter = 0;
    }
    setpic_name();
    
  // Open the root directory
  File root = SD.open("/");

    File jpgFile = SD.open(pic_name);

    // Decode the JPEG file
    JpegDec.decodeSdFile(pic_name);

    // Create a buffer for the packet
    char dataBuff[240];

    // Fill the buffer with zeros
    initBuff(dataBuff);

    // Create a header packet with info about the image
    String header = "$ITHDR,";
    header += JpegDec.width;
    header += ",";
    header += JpegDec.height;
    header += ",";
    header += JpegDec.MCUSPerRow;
    header += ",";
    header += JpegDec.MCUSPerCol;
    header += ",";
    header += jpgFile.name();
    header += ",";
    header.toCharArray(dataBuff, 240);

    // Send the header packet
    for(int j=0; j<240; j++) {
      Serial.write(dataBuff[j]);
    }

    // Pointer to the current pixel
    uint16_t *pImg;

    // Color of the current pixel
    uint16_t color;

    // Create a data packet with the actual pixel colors
    strcpy(dataBuff, "$ITDAT");
    uint8_t i = 6;

    // Repeat for all MCUs in the image
    while(JpegDec.read()) {
      // Save pointer the current pixel
      pImg = JpegDec.pImage;

      // Get the coordinates of the MCU we are currently processing
      int mcuXCoord = JpegDec.MCUx;
      int mcuYCoord = JpegDec.MCUy;

      // Get the number of pixels in the current MCU
      uint32_t mcuPixels = JpegDec.MCUWidth * JpegDec.MCUHeight;

      // Repeat for all pixels in the current MCU
      while(mcuPixels--) {
        // Read the color of the pixel as 16-bit integer
        color = *pImg++;
        
        // Split it into two 8-bit integers
        dataBuff[i] = color >> 8;
        dataBuff[i+1] = color;
        i += 2;

        // If the packet is full, send it
        if(i == 240) {
          for(int j=0; j<240; j++) {
            Serial.write(dataBuff[j]);
          }
          i = 6;
        }

        // If we reach the end of the image, send a packet
        if((mcuXCoord == JpegDec.MCUSPerRow - 1) && 
          (mcuYCoord == JpegDec.MCUSPerCol - 1) && 
          (mcuPixels == 1)) {
          
          // Send the pixel values
          for(int j=0; j<i; j++) {
            Serial.write(dataBuff[j]);
          }
          
          // Fill the rest of the packet with zeros
          for(int k=i; k<240; k++) {
            Serial.write(0);
          }
        }
      }
    }
  // Safely close the root directory
  root.close();
  six_counter = six_counter + 1;
  }
  
  colour_count = 4;
  lcd_C = 0; //reset lcd_C
  lcd_A = 1;
  lcd_B = 0;
  lcd_D = 0;
  digitalWrite(motor, LOW);    
  xhome_go();
  yhome_go();
  digitalWrite(motor, HIGH);
}

else if (lcd_D == 1)
{ 
    buttonState_bottom = digitalRead(water_level_sensor_bottom);
    buttonState_top = digitalRead(water_level_sensor_top);
    if(buttonState_bottom == HIGH && buttonState_top == HIGH) //both sensors at bottom
    {
      lcd.setCursor(0,0);
      lcd.print("Refilling water ");
      lcd.setCursor(0,1);
      lcd.print("                ");  
      //no need to open valve since water level is at minimum
      while(buttonState_top != LOW) //keep refilling until TOP is afloat
      {
        buttonState_top = digitalRead(water_level_sensor_top);
        digitalWrite(mainpump, LOW); //turn on main pump
      }
      digitalWrite(mainpump, HIGH); //turn off main pump
    }
    else //if((buttonState_bottom == LOW && buttonState_top == HIGH) || (buttonState_bottom == LOW && buttonState_top == LOW))
    {
      //Step 1. Drain water
      lcd.setCursor(0,0);
      lcd.print("Draining water  ");
      lcd.setCursor(0,1);
      lcd.print("                ");
      while(buttonState_bottom != HIGH)
      { 
        buttonState_bottom = digitalRead(water_level_sensor_bottom);
        digitalWrite(valve, LOW); //open valve as long as there is still water
      }
      digitalWrite(valve, HIGH); //allow valve to close
     
      //Step 2. Refill water
      lcd.setCursor(0,0);
      lcd.print("Refilling water ");
      lcd.setCursor(0,1);
      lcd.print("                "); 
      buttonState_top = digitalRead(water_level_sensor_top); //need to update cuz last saved State was LOW (afloat)
      while(buttonState_top != LOW)
      {
        buttonState_top = digitalRead(water_level_sensor_top);
        digitalWrite(mainpump, LOW);
      }
      digitalWrite(mainpump, HIGH);
    }
  colour_count = 4;  
  lcd_D = 0; //reset lcd_D
  lcd_C = 0; //reset lcd_C
  lcd_A = 1;
  lcd_B = 0;
}
} //void loop

//####### SMALL STEPPER #######
void hold_test_water()
{
  //add line here to turn off backlight? | PLS REVIEW
  lcd.noBacklight();
  Steps2Take = - 700;
  small_stepper.setSpeed(500);
  small_stepper.step(Steps2Take);
}

void dump_test_water()
{
  Steps2Take  =  + 500;
  small_stepper.setSpeed(500);
  small_stepper.step(Steps2Take);
  //add line here to turn on backlight? | PLS REVIEW
  lcd.backlight();
}

//####### JPEG DECODE #######
// Function to fill the packet buffer with zeros
void initBuff(char* buff) {
  for (int i = 0; i < 240; i++) {
    buff[i] = 0;
  }
}

void setpic_name()
{
  switch (five_counter)
  {
    case 0:
      pic_name[5] = '0';
      break;
    case 1:
      pic_name[5] = '1';
      break;
    case 2:
      pic_name[5] = '2';
      break;
    case 3:
      pic_name[5] = '3';
      break;
    case 4:
      pic_name[5] = '4';
      break;
    case 5:
      pic_name[5] = '5';
      break;
    case 6:
      pic_name[5] = '6';
      break;
    case 7:
      pic_name[5] = '7';
      break;
    case 8:
      pic_name[5] = '8';
      break;
    case 9:
      pic_name[5] = '9';
      break;
  }
  switch (six_counter)
  {
    case 0:
      pic_name[6] = '0';
      break;
    case 1:
      pic_name[6] = '1';
      break;
    case 2:
      pic_name[6] = '2';
      break;
    case 3:
      pic_name[6] = '3';
      break;
    case 4:
      pic_name[6] = '4';
      break;
    case 5:
      pic_name[6] = '5';
      break;
    case 6:
      pic_name[6] = '6';
      break;
    case 7:
      pic_name[6] = '7';
      break;
    case 8:
      pic_name[6] = '8';
      break;
    case 9:
      pic_name[6] = '9';
      break;
  }
}

//####### XY MOVEMENT #######
void pot1_go()
{
  move_x(500);
  move_y(0);
}

void pot2_go()
{
  move_x(500);
  move_y(1500);
}

void pot3_go()
{
  move_x(2700);
  move_y(1500);
}

void pot4_go()
{
  move_x(2700);
  move_y(0);
}

void pot5_go()
{
  move_x(4900);
  move_y(0);
}

void pot6_go()
{
  move_x(4900);
  move_y(1500);
}

void testarea_go()
{
  move_x(7200);
  move_y(850);
}

void yhome_go()
{
  buttonState_yhome = digitalRead(yhome);
  while (buttonState_yhome == LOW)
  {
    digitalWrite(y_dir, LOW); //this direction is towards button
    buttonState_yhome = digitalRead(yhome);
    digitalWrite(y_step, HIGH);
    delay(10);
    digitalWrite(y_step, LOW);
    delay(10);
//    Serial.println("Y returning home");
  }
  digitalWrite(y_step, LOW);
//  Serial.println("Y reached home");
  digitalWrite(y_dir, HIGH); //to change direction
  for (y_release = 0; y_release < 40; y_release++)
  {
    digitalWrite(y_step, HIGH);
    delay(10);
    digitalWrite(y_step, LOW);
    delay(10);
  }
  y_now = 0;
}

void xhome_go()
{
  buttonState_xhome = digitalRead(xhome);
  while (buttonState_xhome == LOW)
  {
    digitalWrite(x_dir, LOW); //this direction is towards button
    buttonState_xhome = digitalRead(xhome);
    digitalWrite(x_step, HIGH);
    delay(10);
    digitalWrite(x_step, LOW);
    delay(10);
//    Serial.println("X returning home");
  }
  digitalWrite(x_step, LOW);
//  Serial.println("X reached home");
  digitalWrite(x_dir, HIGH); //to change direction
  for (x_release = 0; x_release < 40; x_release++)
  {
    digitalWrite(x_step, HIGH);
    delay(10);
    digitalWrite(x_step, LOW);
    delay(10);
  }
  x_now = 0;
}

void move_x(int x_go)
{
  int steps;
  int x_distance;
  
  x_distance = x_go - x_now;
  if (x_distance <= 0)
  {
    digitalWrite(x_dir, LOW);
    x_distance = - x_distance;
  }
  else
  {
    digitalWrite(x_dir, HIGH);
  }
  digitalWrite(motor, LOW);
  for (steps = 0; steps < x_distance; steps++)
  {
    digitalWrite(x_step, HIGH);
    delay(10);
    digitalWrite(x_step, LOW);
    delay(10);
  }
  x_now = x_go; //update the current position
  digitalWrite(motor, HIGH);
}

void move_y(int y_go)
{
  int steps;
  int y_distance;
  
  y_distance = y_go - y_now;
  if (y_distance <= 0)
  {
    digitalWrite(y_dir, LOW);
    y_distance = - y_distance;
  }
  else
  {
    digitalWrite(y_dir, HIGH);
  }
  digitalWrite(motor, LOW);
  for (steps = 0; steps < y_distance; steps++)
  {
    digitalWrite(y_step, HIGH);
    delay(10);
    digitalWrite(y_step, LOW);
    delay(10);
  }
  y_now = y_go; //update the current position
  digitalWrite(motor, HIGH);
}

//####### TAKE PICTURE #######
void take_pic()
{
  // Start the camera
  if (!cam.begin()) {
    // If the camera can't be started, loop forever
    Serial.println("Camera failed or not present!");
    while (1);
  }
  // Set the image size to 640x480
  for(i = 0; i < 10; i++) //loop 10 times to force set resolution
    cam.setImageSize(VC0706_320x240);
    
  // Take a picture
  cam.takePicture();

  // Create a name for the new file in the format IMAGExy.JPG
  char filename[13];
  strcpy(filename, "IMAGE00.JPG");
  for (int i = 0; i < 100; i++) {
    filename[5] = '0' + i / 10;
    filename[6] = '0' + i % 10;
    if (!SD.exists(filename)) //check if file successfully created
    {
      break;
    }
  }

  // Create a file with the name we created above and open it
  File imgFile = SD.open(filename, FILE_WRITE);

  // Get the size of the image
  uint16_t jpglen = cam.frameLength();

  // Read all the image data
  while (jpglen > 0) {
    // Load the JPEG-encoded image data from the camera into a buffer
    uint8_t *buff;
    uint8_t bytesToRead = min(32, jpglen);
    buff = cam.readPicture(bytesToRead);

    // Write the image data to the file
    imgFile.write(buff, bytesToRead);
    jpglen -= bytesToRead;
  }

  // Safely close the file
  imgFile.close();
}

//####### RGB #######
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
  else;
}

void red()
{
  r = 255;
  g = 0;
  b = 0;

  r = brightness_red(r);
  g = brightness_green(g);
  b = brightness_blue(b);
  turn_on(r, g, b);
}

void blue()
{
  r = 0;
  g = 0;
  b = 255;

  r = brightness_red(r);
  g = brightness_green(g);
  b = brightness_blue(b);
  turn_on(r, g, b);
}

void green()
{
  r = 0;
  g = 255;
  b = 0;

  r = brightness_red(r);
  g = brightness_green(g);
  b = brightness_blue(b);
  turn_on(r, g, b);
}

void purple()
{
  r = 255;
  g = 0;
  b = 255;

  r = brightness_red(r);
  g = brightness_green(g);
  b = brightness_blue(b);
  turn_on(r, g, b);
}

int brightness_red (int r)
{
  if (brightness_count == 0) // 1 being dimmest, 5 being brightest
    r = 0; // 0 being OFF;
  else if (brightness_count == 1) // 1 being dimmest, 5 being brightest
    r = 1 * r / 5;
  else if (brightness_count == 2) // 1 being dimmest, 5 being brightest
    r = 2 * r / 5;
  else if (brightness_count == 3) // 1 being dimmest, 5 being brightest
    r = 3 * r / 5;
  else if (brightness_count == 4) // 1 being dimmest, 5 being brightest
    r = 4 * r / 5;
  else if (brightness_count == 5) // 1 being dimmest, 5 being brightest
    r = 5 * r / 5;
  else;
  return r;
}

int brightness_green (int g)
{
  if (brightness_count == 0) // 1 being dimmest, 5 being brightest
    g = 0; // 0 being OFF;
  else if (brightness_count == 1) // 1 being dimmest, 5 being brightest
    g = 1 * g / 5;
  else if (brightness_count == 2) // 1 being dimmest, 5 being brightest
    g = 2 * g / 5;
  else if (brightness_count == 3) // 1 being dimmest, 5 being brightest
    g = 3 * g / 5;
  else if (brightness_count == 4) // 1 being dimmest, 5 being brightest
    g = 4 * g / 5;
  else if (brightness_count == 5) // 1 being dimmest, 5 being brightest
    g = 5 * g / 5;
  else;
  return g;
}

int brightness_blue (int b)
{
  if (brightness_count == 0) // 1 being dimmest, 5 being brightest
    b = 0; // 0 being OFF;
  else if (brightness_count == 1) // 1 being dimmest, 5 being brightest
    b = 1 * b / 5;
  else if (brightness_count == 2) // 1 being dimmest, 5 being brightest
    b = 2 * b / 5;
  else if (brightness_count == 3) // 1 being dimmest, 5 being brightest
    b = 3 * b / 5;
  else if (brightness_count == 4) // 1 being dimmest, 5 being brightest
    b = 4 * b / 5;
  else if (brightness_count == 5) // 1 being dimmest, 5 being brightest
    b = 5 * b / 5;
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
    lcd_A = 1;
    if(colour_count > 3)
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
    lcd_B = 1;
    if (brightness_count < 6)
      brightness_count = brightness_count + 1;
    else
      brightness_count = 0;
    last_button_time_B = button_time_B;
  }
  else; 
}

void take_picture()
{
  button_time_C = millis();
  if (button_time_C - last_button_time_C > 250)
  {
    lcd_C = 1;
    last_button_time_C = button_time_C;
  }
  else; 
}

void drain_refill()
{
  button_time_D = millis();
  if (button_time_D - last_button_time_D > 250)
  {  
    lcd_D = 1;
    last_button_time_D = button_time_D;
  }
  else; 
}

