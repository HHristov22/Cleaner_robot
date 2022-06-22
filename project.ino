#include <Arduino.h>
#include <analogWrite.h>

int LED = 23;
int Button = 22;
int current_state_button;
int last_state = HIGH;
int cnt_click = 0;

void setup() {
  pinMode(LED,OUTPUT);
  pinMode(Button, INPUT_PULLUP);
  Serial.begin(115200);
  Serial.println("############################");
  turnOFF();
}

void loop() {
  current_state_button = digitalRead(Button);

  if (last_state == HIGH && current_state_button == LOW)
    Serial.println("The button is pressed");
  else if (last_state == LOW && current_state_button == HIGH)
  {
    Serial.println("The button is released");
    cnt_click = (cnt_click + 1)%2;
    Serial.println(cnt_click);
  }
  last_state = current_state_button;

  if (cnt_click)
  turnON();
  else
  turnOFF();

}

void turnON()
{
  digitalWrite(LED, HIGH);
}

void turnOFF()
{
  digitalWrite(LED, LOW);
}
