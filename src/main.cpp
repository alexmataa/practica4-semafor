#include <Arduino.h>

const int led_green = 17; // Pin of the LED
const int led_yellow = 16; // Pin of the LED
const int led_red = 15; // Pin of the LED

bool green_to_yellow= false;
bool yellow_blink=false;
bool yellow_to_red=false;
bool red_to_green= true;

struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;

  bool pressed;
};

Button button1 = {18, 0, false};
Button button2 = {19, 0, false};


void IRAM_ATTR isr() {
button1.pressed = true;
}

void IRAM_ATTR isr2() {
button2.pressed = true;
}

void toggleGreen(void * parameter){
  for(;;){ 
  if (red_to_green==true)
  {
      digitalWrite(led_red, LOW);
      digitalWrite(led_yellow, LOW);
      digitalWrite(led_green, HIGH);
      Serial.println("GREEN LED ON");
      red_to_green=false;
  }
      vTaskDelay(100 / portTICK_PERIOD_MS);

  }
}

void toggleYellow(void * parameter){
  for(;;){ 
  if (green_to_yellow==true)
  {
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      digitalWrite(led_green, LOW);
      digitalWrite(led_yellow, HIGH);
      Serial.println("YELLOW LED ON");
      vTaskDelay(3000 / portTICK_PERIOD_MS);
      green_to_yellow=false;
      yellow_to_red=true;
  }
  else if (yellow_blink==true)
  {
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      digitalWrite(led_green, LOW);
      digitalWrite(led_yellow, HIGH);
      Serial.println("YELLOW LED ON");
      for(int i=0;i!=10;i++)
      {
        vTaskDelay(800 / portTICK_PERIOD_MS);
        digitalWrite(led_yellow, LOW);
        Serial.println("YELLOW LED OFF");
        vTaskDelay(800 / portTICK_PERIOD_MS);
        digitalWrite(led_yellow, HIGH);
        Serial.println("YELLOW LED ON");
      }
        yellow_blink=false;
        red_to_green=true;
  }
      vTaskDelay(100 / portTICK_PERIOD_MS);

  }
}

void toggleRed(void * parameter){
  for(;;){ 
  if (yellow_to_red==true)
  {
      digitalWrite(led_yellow, LOW);
      digitalWrite(led_red, HIGH);
      Serial.println("RED LED ON");
      vTaskDelay(8000 / portTICK_PERIOD_MS);
      yellow_to_red=false;
      red_to_green=true;
  }
      vTaskDelay(100 / portTICK_PERIOD_MS);

  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(led_green, OUTPUT);
  pinMode(led_yellow, OUTPUT);
  pinMode(led_red, OUTPUT);
  pinMode(button1.PIN, INPUT_PULLUP);
  attachInterrupt(button1.PIN, isr, FALLING);
  pinMode(button2.PIN, INPUT_PULLUP);
  attachInterrupt(button2.PIN, isr2, FALLING);
  xTaskCreate(
    toggleGreen, // Function that should be called
    "Toggle LED", // Name of the task (for debugging)
    1000, // Stack size (bytes)
    NULL, // Parameter to pass
    1, // Task priority
    NULL // Task handle
  );
  xTaskCreate(
    toggleYellow, // Function that should be called
    "Toggle LED", // Name of the task (for debugging)
    1000, // Stack size (bytes)
    NULL, // Parameter to pass
    1, // Task priority
    NULL // Task handle
  );
    xTaskCreate(
    toggleRed, // Function that should be called
    "Toggle LED", // Name of the task (for debugging)
    1000, // Stack size (bytes)
    NULL, // Parameter to pass
    1, // Task priority
    NULL // Task handle
  );


}

void loop() {
 if (button1.pressed) {
   if(green_to_yellow==false && yellow_to_red==false && red_to_green==false && yellow_blink==false)
   {
     green_to_yellow=true;
   }
  button1.pressed = false;
}

 if (button2.pressed) {
   if(green_to_yellow==false && yellow_to_red==false && red_to_green==false && yellow_blink==false)
   {
     yellow_blink=true;
   }
  button2.pressed = false;
}

}