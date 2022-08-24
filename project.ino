#include <Arduino.h>
#include <analogWrite.h>
#include "BluetoothSerial.h"
#define BUTTON_PIN 21

const int speed_for_turn =128;
int speed_move = 105;
//left
int AIN1 = 12;
int AIN2 = 14;
int PWMA = 27;

//right
int BIN1 = 33;
int BIN2 = 25;
int PWMB = 26;

int last_state = LOW; // the previous state from the input pin
int cur_state; 

// ultrasonic sensor LEFT
#define trig_pin_left 23 
#define echo_pin_left 22

// ultrasonic sensor RIGHT
#define trig_pin_right 5 
#define echo_pin_right 18

float duration_us_left, dist_cm_left;
float duration_us_right, dist_cm_right;

// BLUETOOTH
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
int received;// received value will be stored in this variable
char receivedChar;// received value will be stored as CHAR in this variable

const char c_forward_input ='f';
const char c_backward_input ='b';
const char c_left_input ='l';
const char c_right_input ='r';
const char c_stop_input ='S';
const char c_speed_up ='+';
const char c_speed_down ='-';

void setup() {
  // left motor
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  // right motor
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);

  pinMode(trig_pin_left, OUTPUT);
  pinMode(echo_pin_left, INPUT);
  
  pinMode(trig_pin_right, OUTPUT);
  pinMode(echo_pin_right, INPUT);
  
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  Serial.println("Starting program");

  // BL
  SerialBT.begin("ESP32_Cleaner"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  Serial.println("Name: ESP32_Cleaner");
  Serial.println("To turn move FORWARD send: +");//print on serial monitor  
  Serial.println("To turn STOP send: S"); //print on serial monitor
}

void loop() {

    //BL
    receivedChar =(char)SerialBT.read();
  
    if (Serial.available()) {
      SerialBT.write(Serial.read());
    
    }
    if (SerialBT.available()) {
    
    SerialBT.print("Received:");// write on BT app
    SerialBT.println(receivedChar);// write on BT app      
    Serial.print ("Received:");//print on serial monitor
    Serial.println(receivedChar);//print on serial monitor    
    //SerialBT.println(receivedChar);//print on the app    
    //SerialBT.write(receivedChar); //print on serial monitor
    if(receivedChar == c_forward_input)
    {
     SerialBT.println("FORWARD move");// write on BT app
     Serial.println("FORWARD");
     forward(speed_move);
    }
    if(receivedChar == c_backward_input)
    {
     SerialBT.println("BACKWARD move");// write on BT app
     Serial.println("BACKWARD");
     backward(speed_move);
    }
    if(receivedChar == c_left_input)
    {
     SerialBT.println("Turn LEFT");// write on BT app
     Serial.println("LEFT");
     stop_move();
     turn_left(speed_for_turn);
     delay(435);
     stop_move();
    }
    if(receivedChar == c_right_input)
    {
     SerialBT.println("Turn RIGHT");// write on BT app
     Serial.println("RIGHT");
     stop_move();
     turn_right(speed_for_turn);
     delay(435);
     stop_move();
    }
    if(receivedChar == c_stop_input)
    {
     SerialBT.println("STOP moving");// write on BT app
     Serial.println("STOP");
     stop_move();
    }
    if(receivedChar == c_speed_up)
    {
     change_speed(1);
     SerialBT.print("speed UP: ");// write on BT app
     SerialBT.print((speed_move-80)/5*10);// write on BT app
     SerialBT.print("%\n");// write on BT app
     Serial.println("speed UP");
    }
    if(receivedChar == c_speed_down)
    {
     change_speed(-1);
     SerialBT.print("speed DOWN: ");// write on BT app
     SerialBT.print((speed_move-80)/5*10);// write on BT app
     SerialBT.print("%\n");// write on BT app
     Serial.println("speed DOWN");
    }
    }
    /////////////////////////////////////////////////////////////////////////
    
    digitalWrite(trig_pin_left, LOW);
    digitalWrite(trig_pin_right, LOW);//
    delayMicroseconds(2);
    digitalWrite(trig_pin_left, HIGH);
    digitalWrite(trig_pin_right, HIGH);//
    delayMicroseconds(10);
    digitalWrite(trig_pin_left, LOW);
    digitalWrite(trig_pin_right, LOW);//
    delayMicroseconds(10);
  
    duration_us_left = pulseIn(echo_pin_left, HIGH);
    dist_cm_left = 0.017 * duration_us_left;
    duration_us_right = pulseIn(echo_pin_right, HIGH);//
    dist_cm_right = 0.017 * duration_us_right;//
  
    // print the value to Serial Monitor
    Serial.print("distance: ");
    Serial.print(dist_cm_left);
    Serial.print("   ");
    Serial.println(dist_cm_right);
    //Serial.println(" cm");
    delay(50);
}


void change_speed(int coeff) 
{
  speed_move = speed_move + coeff*5;
  if (speed_move<85)
  speed_move = 85;
  if (speed_move>130)
  speed_move = 130;
}

void forward(int spd) 
{
  runMotor(0, spd, 0);
  runMotor(1, spd, 0);
}

void turn_left(int spd)
{
  runMotor(0, spd, 0);
  runMotor(1, spd, 1);
}

void turn_right(int spd)
{
  runMotor(0, spd, 1);
  runMotor(1, spd, 0);
}

void backward(int spd) 
{
  runMotor(0, spd, 1);
  runMotor(1, spd, 1);
}

void stop_move()
{
  runMotor(0, 0, 0);
  runMotor(1, 0, 0);
}

//dir: 0 - forward, 1 - backward
void runMotor(int motor, int spd, int dir)
{
  //digitalWrite(standBy, HIGH); // Turn on Motor

  boolean dirPin1 = LOW;
  boolean dirPin2 = HIGH;

  // motor 0/1
  // 1 - left
  // 0 - right (google)
  if(dir == 1) {
    dirPin1 = HIGH;
    dirPin2 = LOW;
  }
  
  if(motor == 1) {
    digitalWrite(AIN1, dirPin1);
    digitalWrite(AIN2, dirPin2);
    analogWrite(PWMA, spd);
  } else {
    digitalWrite(BIN1, dirPin2);
    digitalWrite(BIN2, dirPin1);
    analogWrite(PWMB, spd);
  }

}
