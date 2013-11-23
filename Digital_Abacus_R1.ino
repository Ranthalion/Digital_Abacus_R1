//An array to receive a binary 8-bit number on not necessarily consecutive pins, ordered by LSb to MSb
const byte abacusPins[] = {22, 24, 26, 28, 30, 32, 34, 36};

// Size of the dipPins[] array in number of bytes. (Remember, this is one of the few built in functions that doesn't use camel case...)
const byte abacusSize = sizeof(abacusPins);  

#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);


void setup(){
  // Set each pin connected to the Abacus switches as an input
  for(int i = 0; i < 8; i++)
  {
    pinMode(abacusPins[i], INPUT);
  }

  display.begin();
  display.clearDisplay();
  // you can change the contrast around to adapt the display
  // for the best viewing!
  display.setContrast(20);
  display.display();

}

void loop(){

  int binaryNumber = parallelToByte();

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(10,0);
  display.setTextColor(WHITE, BLACK); // 'inverted' text
  display.print("DREAM TEAM");

  display.setTextColor(BLACK);
  display.setCursor(0,10);
  display.print(binaryNumber, BIN); 
  display.setCursor(65, 10);
  display.print("BIN");
  // Erase the leading 1 (msb 9th digit) and leave 8 leading zeroes
  display.setTextColor(WHITE);
  display.setCursor(0,10); display.print("1");
  display.setTextColor(BLACK);
  
  display.setCursor(37, 20);
  display.print(binaryNumber, HEX);
  // Erase the leading 1 (msb 9th digit) and leave 8 leading zeroes
  display.setTextColor(WHITE);
  display.setCursor(37,20); display.print("1");
  display.setTextColor(BLACK);

  display.setCursor(65, 20);
  display.print("HEX");
  
  // I subtract that extra 256 from here so I can get a normal decimal
  display.setCursor(37, 40);
  display.print(binaryNumber - 256); 
  display.setCursor(65, 40);
  display.print("DEC");

  delay(250);

  //Generate printable ASCII characters in the empty space on screen
  binaryNumber = binaryNumber - 256;
  if ((binaryNumber > 32) && (binaryNumber < 127)){
  char binaryString = binaryNumber;  
  // itoa(binaryNumber, binaryString, 1);  
    
    display.setTextSize(2);
//      display.setTextColor(WHITE, BLACK); // 'inverted' text
      display.setCursor(10, 20);
      display.print(binaryString);
    
  }
  
  display.display();
  
}



/*
 * parallelToByte() - converts a binary value on 8 digital inputs to an integer.
 */
int parallelToByte()
{
  int ret = 0;  // Initialize the variable to return
  int bitValue[] = {1,2,4,8,16,32,64,128};  // bit position decimal equivalents
  for(int i = 0; i < 8; i++)  // cycle through all the pins
  {
    if(digitalRead(abacusPins[i]) == HIGH)  // because all the bits are pre-loaded with zeros, only need to do something when we need to flip a bit high
    {
      ret = ret + bitValue[i];  // adding the bit position decimal equivalent flips that bit position
    }
  }
  // I'm adding 256 to the final value to b/c I can't print leading zeros
  // so I add a 9th digit which is always '1' and then overwriting it
  // with a '1' in WHITE so it disappears from the screen
  return ret + 256;
}


