#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>

#define RED 0x1
#define GREEN 0x2

#define YELLOW 0x3 	//AND
#define BLUE 0x4	//OR
#define VIOLET 0x5	//XOR
#define TEAL 0x6	//ASCII
#define WHITE 0x7 	//display and Menu

#define DISPLAY_MODE 2
#define AND_MODE 3
#define OR_MODE 4
#define XOR_MODE 5
#define ASCII_MODE 6
#define NUMBERS_MODE 7
#define MENU_MODE 8

#define INITIAL 0x0	
#define REDRAW 	0x1
#define WAITING 0x2
#define INCORRECT 0x3
#define SUCCESS  0x4

volatile int currentMode = DISPLAY_MODE;
int currentOption = DISPLAY_MODE;
boolean optionChanged = false;

int state = INITIAL;
int num1;
int num2;
int answer;
const int colors [] = {0,0,7,3,6,5,6,3};
const String menu[] = {"", "", "DISPLAY", "AND", "OR", "XOR", "ASCII", "NUMBERS"};
const byte abacusPins[] = {2, 3, 4, 5, 6, 7, 8, 9};

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

void setup(){
  randomSeed(analogRead(0));
  
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
  lcdDisplay("   FlipBit", " by DREAM TEAM");
  delay(1500);
  randomSeed(random(0,1000) * analogRead(0));
}

void loop(){

  uint8_t buttons = lcd.readButtons();
  
  if (buttons & BUTTON_UP)
  {
    Serial.println("Pressed up.");
    lcd.clear();
    lcd.setBacklight(WHITE);
    currentMode = MENU_MODE;
    optionChanged = true;
    currentOption--;    
    if (currentOption <= 0x1)
      currentOption = 0x7;
    delay(250);
  }
  else if (buttons & BUTTON_DOWN)
  {
    Serial.println("Pressed down.");
    lcd.clear();
    lcd.setBacklight(WHITE);
    currentMode = MENU_MODE;
    optionChanged = true;
    currentOption++;
    if (currentOption >= 0x8)
      currentOption = 0x2;
    delay(250);
  }
  
  switch(currentMode)
  {
    case MENU_MODE:
      displayMenu(buttons);
      break;
    case DISPLAY_MODE: 
      displayNumbers();
      break;
    case AND_MODE:
    case OR_MODE:
    case XOR_MODE:      
    case ASCII_MODE:      
    case NUMBERS_MODE:
	  runGame(buttons);
      break;
    default:
      Serial.print("Default in main loop.  ");
      Serial.println(currentMode);
      break;    
  }
  
}

void displayMenu(uint8_t buttons)
{  
  if (optionChanged)
  {
    optionChanged = false;
    int next = currentOption+1;
    if (next > 7)
      next = 2;    
	  
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print(">> ");
    lcd.print(menu[currentOption]);
    lcd.setCursor(0,1);
    lcd.print("   ");
    lcd.print(menu[next]);
  }
  if (buttons & BUTTON_SELECT)
  {
    currentMode = currentOption;
    state = INITIAL;
    delay(250);
   }
}

void displayNumbers()
{
  if (state == INITIAL)
  {
    state = WAITING;
    lcd.setBacklight(WHITE);
    lcd.clear();
  }
  byte input = readAbacus();
  
  char line1[16];
  char line2[16];
  char tmp[9];
  
  toBinary(tmp, input);
  sprintf(line1, "0b %8s %4c", tmp, input);
  sprintf(line2, "0x %02X %10d", input, input);
  
  lcdDisplay(line1, line2);
}

void runGame(uint8_t buttons)
{
  switch (state)
  {
    case INITIAL:
      num1 = random(255);
      num2 = random(255);
      switch(currentMode)
      {
        case AND_MODE:
          answer = num1 & num2;
	  break;
	case OR_MODE:
          answer = num1 | num2;
	  break;
	case XOR_MODE:
	  answer = num1 ^ num2;
          break;
        case ASCII_MODE:
          num1 = random(65, 123);
          answer = num1;
          break;
        case NUMBERS_MODE:
          answer = num1;
          break;
      	default:
	  break;
      }
      state = REDRAW;
      break;
    case REDRAW:
      char byte1[9];
      char byte2[9];
      char line1[16];
      char line2[16];

      if (currentMode == ASCII_MODE)
      {
        sprintf(line1, "Identify binary for ");
        sprintf(line2, "ASCII:   %c", num1);
      }
      else if (currentMode == NUMBERS_MODE)
      {
        sprintf(line1, "Identify binary for ");
        sprintf(line2, "decimal: %d", num1);
      }
      else
      {
        toBinary(byte1, num1);
        toBinary(byte2, num2);
        sprintf(line1, "    %8s", byte1);
        sprintf(line2, "%3s %8s", menu[currentMode].c_str(), byte2);
      }
      lcd.setBacklight(colors[currentMode]);
      lcd.clear();
      lcdDisplay(line1, line2);
      state = WAITING;
      break;
    case WAITING:
      if (buttons & BUTTON_SELECT)
      {
        byte input = readAbacus();
	if (input == answer)
          state = SUCCESS;
        else
          state = INCORRECT;
          delay(250);
		}
		break;
	case INCORRECT:
		lcd.setBacklight(RED);
		lcd.clear();
		lcdDisplay("   Incorrect", "   Try Again");
		delay(2000);
		state = REDRAW;
		break;
	case SUCCESS:
		lcd.setBacklight(GREEN);
		lcd.clear();
		lcdDisplay("   Correct!", "");
		delay(2000);
		state = INITIAL;
		break;
	default:
          Serial.println("Stuck in default in Game");
		break;	
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

void lcdDisplay(char* line1, char* line2)
{
  lcd.setCursor(0,0);
  lcd.print(line1);
  
  lcd.setCursor(0,1);
  lcd.print(line2);   
}
