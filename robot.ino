// Name: Robert Martin
// Team: Orange Squad
// Project: Peel Team 6 (RF Receiver)

// libraries
#include <SPI.h>
#include <RF24.h>
#include <Servo.h>
#include <nRF24L01.h>

// radio connection
// 9 = CE pin, 10 = CSN pin
RF24 radio(9, 10);

// address
const byte address[6] = "00001";  

// weapon speed controller pin
#define esc 2

// weapon declaration
Servo weapon;

// left motor controller pins
#define motorL_Forward 3  //IN1
#define motorL_Reverse 4  //IN2
#define motorL_Speed 5    //enA

// right motor controller pins
#define motorR_Forward 8  //IN3
#define motorR_Reverse 7  //IN4
#define motorR_Speed 6    //enB

// motor speed value declarations
int motorLeft_value;
int motorRight_value;

// weapon speed value declaration
int weapon_value;

// variables used for steering
int left_speed;
int right_speed;
int steer;

// commands received from controller
struct commands
{
  // joystick 1 values
  byte js1_x;
  byte js1_y;
  byte jb1_value;

  // joystick 2 values
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

// makes variable of struct
commands data;

// time variables
// will be used to check for signal connection
unsigned long previousTime = 0;
unsigned long currentTime = 0;

// receiver setup
// applies radio connection and sets default values
void setup()
{
  // debug
  Serial.begin(9600);

  // set radio connection as receiver
  radio_receiver();

  // sets default variable values
  reset_data();

  // set up motor pins
  motor_setup();
}

// receives data from controller
// if no data is received variables are reset
// adjusts motor speeds for robot control
void loop()
{
  // check connection
  check_connection();

  // control weapon
  weapon_control();

  // control motors
  motor_control();

  // print data
  print_vardata();

  // print motor data
  print_motordata();
}

// sets up radio connection for communication
void radio_receiver()
{
  radio.begin();
  radio.setAutoAck(false);
  radio.openReadingPipe(0, address);
  radio.setDataRate(RF24_2MBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
}

// sets up motor pin settings
void motor_setup()
{
  // weapon motor pin
  pinMode(esc, OUTPUT); 

  // left motor pins
  pinMode(motorL_Forward, OUTPUT);
  pinMode(motorL_Reverse, OUTPUT);
  pinMode(motorL_Speed, OUTPUT);
  
  // right motor pins
  pinMode(motorR_Forward, OUTPUT);
  pinMode(motorL_Reverse, OUTPUT);
  pinMode(motorL_Speed, OUTPUT);

  // connect speed controllers
  // 1800 and 2000 are min and max pulses
  weapon.attach(esc, 1800, 2000);
}

// checks for radio connection
void check_connection()
{
  // checks radio connection
  if (radio.available())
  {
    // saves data being received
    radio.read(&data, sizeof(commands));

    // saves the last time of data being received
    previousTime = millis();
  }
  
  // saves current time
  currentTime = millis();

  // checks to see if connection is lost
  // if connection is missing for longer than a second connection is lost
  if (currentTime - previousTime > 1000)
  {
    // error message
    print_error();
    
    // resets variable values to defaults
    reset_data();
  }
}

// control weapon
void weapon_control()
{
  // receive weapon speed
  weapon_value = map(data.pot1_value, 0, 255, 0, 180);
  
  // update weapon speed
  weapon.write(weapon_value);
}

/**
 * IMPORTANT NOTE: This function is fairly complex, in order to simplify understanding,
 * of this function, it is advisable that you take note of the following things.
 * This function has two major components. The DIRECTION and STEERING sections.
 * The DIRECTION section determines whether the robot will move forward, reverse, or stop.
 * The STEERING section determines whether the robot will steer left or right.
 */
 
void motor_control()
{
  // DIRECTION SECTION
  // this first section deternmines the robots direction
  // this can be forward, reverse, or stop
  // steering occurs after this section
  
  // forward movement
  if (data.js2_y >= 126)
  {
    // set forward speed
    left_speed = map(data.js2_y, 126, 255, 0, 255);
    right_speed = left_speed;

    // left motor control
    digitalWrite(motorL_Forward, HIGH);
    digitalWrite(motorL_Reverse, LOW);
    analogWrite(motorL_Speed, left_speed);

    // right motor control
    digitalWrite(motorR_Forward, HIGH);
    digitalWrite(motorR_Reverse, LOW);
    analogWrite(motorR_Speed, right_speed);
  }

  // reverse movement
  else if (data.js2_y <= 122)
  {
    // set reverse speed
    left_speed = map(data.js2_y, 122, 0, 0, 255);
    right_speed = left_speed;

    // left motor control
    digitalWrite(motorL_Forward, LOW);
    digitalWrite(motorL_Reverse, HIGH);
    analogWrite(motorL_Speed, left_speed);

    // right motor control
    digitalWrite(motorR_Forward, LOW);
    digitalWrite(motorR_Reverse, HIGH);
    analogWrite(motorR_Speed, right_speed);
  }

  // stoped
  else
  {
    // set motor speeds
    left_speed = 0;
    right_speed = 0;

    // left motor control
    digitalWrite(motorL_Forward, LOW);
    digitalWrite(motorL_Reverse, LOW);
    analogWrite(motorL_Speed, left_speed);

    // right motor control
    digitalWrite(motorR_Forward, LOW);
    digitalWrite(motorR_Reverse, LOW);
    analogWrite(motorR_Speed, right_speed);
  }

  // STEERING SECTION
  // this section will determine the robots steering direction
  // this can be left or right
  
  // steer left
  if (data.js2_x >= 127)
  {
    // retrieve steering information
    steer = map(data.js2_x, 127, 255, 0, 255);

    if (left_speed - steer < 0)
    {
      left_speed = 0;
    }

    else
    {
      left_speed = left_speed - steer;
    }

    if (right_speed + steer > 255)
    {
      right_speed = 255;
    }

    else
    {
      right_speed = right_speed  + steer;
    }

    // reset steering
    steer = 0;

    // set motor speed values
    analogWrite(motorL_Speed, left_speed);
    analogWrite(motorR_Speed, right_speed);
  }

  // steer right
  else if (data.js2_x <= 125)
  {
    // retrieve steering information
    steer = map(data.js2_x, 125, 0, 0, 255);
    
    if (right_speed - steer < 0)
    {
      right_speed = 0;
    }

    else
    {
      right_speed = right_speed - steer;
    }

    if (left_speed + steer > 255)
    {
      left_speed = 255;
    }

    else
    {
      left_speed = left_speed + steer;
    }

    // reset steering
    steer = 0;

    // set motor speed values
    analogWrite(motorL_Speed, left_speed);
    analogWrite(motorR_Speed, right_speed);
  }
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

  // motor values
  left_speed = 0;
  right_speed = 0;
}

// PRINTING FUNCTIONS
// prints variable data
void print_vardata()
{
  // potentiometers
  Serial.println();
  Serial.println("DATA PACKAGE: RECIEVER");
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

// prints error message
void print_error()
{
  Serial.println();
  Serial.println("CONNECTION LOST!!!");
  Serial.println("RESET VALUES");
  Serial.println();
}

// prints motor data
void print_motordata()
{
  // LEFT MOTOR
  Serial.println();
  Serial.println("LEFT MOTOR");
  Serial.print("SPEED: ");
  Serial.println(left_speed);

  // RIGHT MOTOR
  Serial.println("RIGHT MOTOR");
  Serial.print("SPEED: ");
  Serial.println(right_speed);

  // RIGHT MOTOR
  Serial.println("STEER");
  Serial.print("SPEED: ");
  Serial.println(steer);
}
