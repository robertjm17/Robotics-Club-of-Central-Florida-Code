// Name: Robert Martin
// Team: Orange Squad
// Project: Peel Team 6 (RF Controller)

// libraries
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

// radio connection
RF24 radio(2, 3); // 2 = CE pin, 3 = CSN pin
const byte address[6] = "00001";  // addres

// digital inputs
#define ts1 7 // toggle switch 1
#define ts2 6 // toggle switch 2
#define ts3 4 // toggle switch 3
#define ts4 5 // toggle switch 4
#define jb1 8 // joystick button 1
#define jb2 9 // joystick button 2

// analog inputs
#define pot1 7  // potentiometer 1
#define pot2 6  // potentiometer 2
#define js1x 5  // joystick button 1 x-input
#define js1y 4  // joystick button 1 y-input
#define js2x 1  // joystick button 2 x-input
#define js2y 0  // joystick button 2 y-input

/**
 * IMPORTANT NOTE: To adjust inputs, change the position value.
 * FORMAT: #define name position
 * EXAMPLE: #define ts1 1 ===> #define ts1 2
 * In the example above the input postion of the digital input, 
 * ts1 was changed from 1 to 2.
 */

// commands sent from controller
struct commands
{
  // joystick button 1 values
  byte js1_x;
  byte js1_y;
  byte jb1_value;

  // joystick button 2 values
  byte js2_x;
  byte js2_y;
  byte jb2_value;

  // potentiometer values
  byte pot1_value;
  byte pot2_value;

  // toggle switch values
  byte ts1_value;
  byte ts2_value;
  byte ts3_value;
  byte ts4_value;
};

/**
 * IMPORTANT NOTE: In order to change what data is being transmitted from the controller 
 * you may need to add, remove, or change parts of the commands struct. If this is done,
 * you will need to adjust other parts of this code where struct variable values are set,
 * read, or reset.
 */

// makes variable of struct
commands data;

// controller setup
// applies radio connection
// sets default values
void setup()
{
  // debug
  Serial.begin(9600);

  // radio connection transmitter
  radio_transmitter();

  // set default pin setttings
  pin_settings();

  // set default values
  reset_data();
}

// runs controller
// will read all values as long as controller is on
void loop()
{ 
  // reads data
  read_data();

  // prints data
  print_data();

  // transmit data
  radio.write(&data, sizeof(commands));
}

// sets up radio connection for communication
void radio_transmitter()
{
  radio.begin();
  radio.setAutoAck(false);
  radio.openWritingPipe(address);
  radio.setDataRate(RF24_2MBPS);  
  radio.setPALevel(RF24_PA_LOW);
}

// sets up various pin settings
void pin_settings()
{
  // make use of pull-up resistors
  // stops false values from occuring
  // is used for digital inputs
  pinMode(ts1, INPUT_PULLUP);
  pinMode(ts2, INPUT_PULLUP);
  pinMode(ts3, INPUT_PULLUP);
  pinMode(ts4, INPUT_PULLUP);
  pinMode(jb1, INPUT_PULLUP);
  pinMode(jb2, INPUT_PULLUP);
}

// reads data from inputs
void read_data()
{
  // read joystick 1 values
  data.js1_x = map(analogRead(js1x), 0, 1023, 0, 255);
  data.js1_y = map(analogRead(js1y), 0, 1023, 0, 255);
  data.jb1_value = digitalRead(jb1);

  // read joystick 2 values
  data.js2_x = map(analogRead(js2x), 0, 1023, 0, 255);
  data.js2_y = map(analogRead(js2y), 0, 1023, 0, 255);
  data.jb2_value = digitalRead(jb2);

  // read potentiometer values
  // after testing potentiometer values before conversion I found highest value was 906
  // converting from (0, 906) to (0, 255)
  data.pot1_value = map(analogRead(pot1), 0, 906, 0, 255);
  data.pot2_value = map(analogRead(pot2), 0, 906, 0, 255);

  // read switch values
  data.ts1_value = digitalRead(ts1);
  data.ts2_value = digitalRead(ts2);
  data.ts3_value = digitalRead(ts3);
  data.ts4_value = digitalRead(ts4); 
}

// resets the values of the input variables
void reset_data()
{
  // default values
  // joystick 1
  data.js1_x = 127;
  data.js1_y = 127;
  data.jb1_value = 1;

  // joystick 2
  data.js2_x = 127;
  data.js2_y = 127;
  data.jb2_value = 1;

  // potentiometers
  data.pot1_value = 0;
  data.pot2_value = 0;
  
  // switches
  data.ts1_value = 1;
  data.ts2_value = 1;
  data.ts3_value = 1;
  data.ts4_value = 1;
}

// prints input data
void print_data()
{
  // potentiometers
  Serial.println();
  Serial.println("DATA PACKAGE: TRANSMITTER");
  Serial.print("potentiometer 1: ");
  Serial.println(data.pot1_value);
  Serial.print("potentiometer 2: ");
  Serial.println(data.pot2_value);

  // toggle switches
  Serial.print("Toggle Switch 1: ");
  Serial.println(data.ts1_value);
  Serial.print("Toggle Switch 2: ");
  Serial.println(data.ts2_value);
  Serial.print("Toggle Switch 3: ");
  Serial.println(data.ts3_value);
  Serial.print("Toggle Switch 4: ");
  Serial.println(data.ts4_value);

  // joystic buttons
  Serial.print("Joystick Button 1: ");
  Serial.println(data.jb1_value);
  Serial.print("Joystick Button 2: ");
  Serial.println(data.jb2_value);
  
  // joystick values
  Serial.print("Joystic 1X: ");
  Serial.println(data.js1_x);
  Serial.print("Joystic 1Y: ");
  Serial.println(data.js1_y);
  Serial.print("Joystic 2X: ");
  Serial.println(data.js2_x);
  Serial.print("Joystic 2Y: ");
  Serial.println(data.js2_y);
  Serial.println();
}
