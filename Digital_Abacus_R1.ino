#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>

// These #defines make it easy to set the backlight color
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

#define DISPLAY 0x2
#define AND 0x3
#define OR 0x4
#define XOR 0x5
#define ASCII 0x6
#define MENU 0x7

byte currentMode = DISPLAY;
boolean optionChanged = false;
byte currentOption = DISPLAY;

const String menu[] = {"", "",
                    "DISPLAY",
                    "AND",
                    "OR",
                    "XOR",
                    "ASCII"};

//An array to receive a binary 8-bit number on not necessarily consecutive pins, ordered by LSb to MSb
const byte abacusPins[] = {2, 3, 4, 5, 6, 7, 8, 9};
// Size of the dipPins[] array in number of bytes. (Remember, this is one of the few built in functions that doesn't use camel case...)
const byte abacusSize = sizeof(abacusPins);  

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

void setup(){
  // Set each pin connected to the Abacus switches as an input
  for(int i = 0; i < 8; i++)
  {
    pinMode(abacusPins[i], INPUT);
  }

  //Serial for debugging
  Serial.begin(9600);
  
  //Initiate LCD
  lcd.begin(16,2);
  lcd.setBacklight(WHITE);
  lcd.print("     FlipBit");  
  lcd.setCursor(0,1);
  lcd.print(" by DREAM TEAM");
  delay(1500);
}

void loop(){

  /*
    Modes - Numeric Display (Binary, Hex, Dec, ASCII
          - Game
            - AND
            - OR
            - XOR
            -
  */
  
  uint8_t buttons = lcd.readButtons();
  
  if (buttons & BUTTON_UP)
  {
    Serial.println("Pressed up.");
    lcd.clear();
    currentMode = MENU;
    optionChanged = true;
    currentOption--;    
    if (currentOption <= 0x1)
      currentOption = 0x6;
    delay(250);
  }
  else if (buttons & BUTTON_DOWN)
  {
    Serial.println("Pressed down.");
    lcd.clear();
    currentMode = MENU;
    optionChanged = true;
    currentOption++;
    if (currentOption >= 0x7)
      currentOption = 0x2;
    delay(250);
  }
  
  switch(currentMode)
  {
    case MENU:
      displayMenu(buttons);
      break;
    case DISPLAY: 
      displayNumbers();
      break;
    case AND:
      displayAND(buttons);
      break;
    case OR:
      displayOR(buttons);
      break;
    case XOR:
      displayXOR(buttons);
      break;
    case ASCII:
      displayASCII(buttons);
      break;
    default:
      break;    
  }
}

void displayNumbers()
{
  byte input = readAbacus();
  
  char line1[16];
  char line2[16];
  char tmp[9];
  Serial.println(input);

  toBinary(tmp, input);
  sprintf(line1, "0b %8s %4c", tmp, input);
  sprintf(line2, "0x %02X %10d", input, input);
  //lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(line1);
  
  lcd.setCursor(0,1);
  lcd.print(line2);   
}

void displayAND(uint8_t buttons)
{
  lcd.setCursor(0,0);
  lcd.print("AND");
  lcd.setCursor(0,1);
  lcd.print("Not Implemented.");
}

void displayOR(uint8_t buttons)
{
  lcd.setCursor(0,0);
  lcd.print("OR");
  lcd.setCursor(0,1);
  lcd.print("Not Implemented.");
}
void displayXOR(uint8_t buttons)
{
  lcd.setCursor(0,0);
  lcd.print("XOR");
  lcd.setCursor(0,1);
  lcd.print("Not Implemented.");
}
void displayASCII(uint8_t buttons)
{
  lcd.setCursor(0,0);
  lcd.print("XOR");
  lcd.setCursor(0,1);
  lcd.print("Not Implemented.");
}
void displayMenu(uint8_t buttons)
{  
  if (optionChanged)
  {
    char line1[16];
    char line2[16];
    
    optionChanged = false;
    int next = currentOption+1;
    if (next > 6)
      next = 2;    
    
    //sprintf(line1, ">> %s", menu[currentOption]);
    //sprintf(line2, "   %s", menu[next]);
    
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print(">> ");
    lcd.print(menu[currentOption]);
    lcd.setCursor(0,1);
    lcd.print("   ");
    lcd.print(menu[next]);
  }
}

void toBinary(char* buf, byte val)
{
  sprintf(buf, 
            "%c%c%c%c%c%c%c%c", 
            (val & 0x80) ? '1':'0', 
            (val & 0x40) ? '1':'0', 
            (val & 0x20) ? '1':'0', 
            (val & 0x10) ? '1':'0', 
            (val & 0x08) ? '1':'0', 
            (val & 0x04) ? '1':'0', 
            (val & 0x02) ? '1':'0', 
            (val & 0x01) ? '1':'0');
  
  buf[8] = '\0';
}




byte readAbacus()
{
  byte val = 0;
  for(int i = 0; i < 8; i++)
  {
    val <<= 1;
    val |= digitalRead(abacusPins[i]);
  }
  return val;
}
