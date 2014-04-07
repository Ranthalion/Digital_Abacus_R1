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

#define DISPLAY 0x2
#define AND 0x3
#define OR 0x4
#define XOR 0x5
#define ASCII 0x6
#define MENU 0x7

#define INITIAL 0x0	
#define REDRAW 	0x1
#define WAITING 0x2
#define Incorrect 0x3
#define Success  0x4

byte currentMode = DISPLAY;
byte currentOption = DISPLAY;
boolean optionChanged = false;

byte state = INITIAL;
int num1;
int num2;
int answer;

const String menu[] = {"", "", "DISPLAY", "AND", "OR", "XOR", "ASCII"};
const byte abacusPins[] = {2, 3, 4, 5, 6, 7, 8, 9};

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
  lcdDisplay("     FlipBit", " by DREAM TEAM");
  delay(1500);
}

void loop(){

  uint8_t buttons = lcd.readButtons();
  
  if (buttons & BUTTON_UP)
  {
    Serial.println("Pressed up.");
    lcd.clear();
	lcd.setBacklight(WHITE)
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
	lcd.setBacklight(WHITE)
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
    case OR:
    case XOR:      
    case ASCII:      
	  runGame(buttons);
      break;
    default:
      break;    
  }
}

void displayMenu(uint8_t buttons)
{  
  if (optionChanged)
  {
	optionChanged = false;
    int next = currentOption+1;
    if (next > 6)
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
	state = 0;
  }
}

void displayNumbers()
{
  if (state == 0)
  {
	state = 1;
	lcd.setBacklight(WHITE)
	lcd.clear();
  }
  byte input = readAbacus();
  
  char line1[16];
  char line2[16];
  char tmp[9];
  Serial.println(input);

  toBinary(tmp, input);
  sprintf(line1, "0b %8s %4c", tmp, input);
  sprintf(line2, "0x %02X %10d", input, input);
  
  lcdDisplay(line1, line2);
  
}



void runGame(uint8_t buttons)
{
   /* States
	0 - Initial 	(Create 2 Random numbers and calculated expected Answer) -> REDRAW
	1 - REDRAW 	(Display the numbers and change the screen color) -> Waiting
	2 - Waiting 	(Wait for button select to be pressed) -> Wrong, Success
	3 - Incorrect	(Display Try Again message and change screen color, wait for millis or button click) -> REDRAW
	4 - Success 	(Display Success message and change screen color) -> Initial
  */
  switch (state)
  {
	case INITIAL:
		num1 = Rand(255);
		num2 = Rand(255);
		switch(currentMode)
		{
			case AND:
				answer = num1 & num2;
				break;
			case OR:
				answer = num1 | num2;
				break;
			case XOR:
				answer = num1 ^ num2;
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
		
		toBinary(byte1, num1);
		toBinary(byte2, num2);
		
		sprintf(line1, "    %8s", byte1);
		sprintf(line2, "%3s %8s", menu[currentMode], byte2);
		lcd.setBacklight(currentMode);
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
