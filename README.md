# PRÀCTICA 4: SISTEMES OPERATIUS EN TEMPS REAL

En aquest pràctica he vist i après el funcionament de tenir varies tasques realitzant-se al mateix moment.

## Pregunta inicial

Abans de mostrar què he programat en aquesta pràctica, respondré a la pregunta que se'ns ha fet inicilament: <b>Què passa si s'està utilitzant una
pantalla de tinta electrònica que triga uns segons a actualitzar-se?</b>

Bàsicament, si estem utilitzant aquesta pantalla sense fer ús de la multi-tasca, per tant, un procés rere procés, les dades que volem veure per la pantalla ens aparaixeran amb delay i, a més a més, amb molts pocs fotogrames-per-segon (FPS).

## Exercici pràctic 1

Podem observear que, utilitzant el següent codi:

```scala
void setup()
{
Serial.begin(112500);
/* we create a new task here */
xTaskCreate(
anotherTask, /* Task function. */
"another Task", /* name of task. */
10000, /* Stack size of task */
NULL, /* parameter of the task */
1, /* priority of the task */
NULL); /* Task handle to keep track of created task */
}
 
/* the forever loop() function is invoked by Arduino ESP32 loopTask */
void loop()
{
Serial.println("this is ESP32 Task");
delay(1000);
}
 
/* this function will be invoked when additionalTask was created */
void anotherTask( void * parameter )
{
/* loop forever */
for(;;)
{
Serial.println("this is another Task");
delay(1000);
}
/* delete a task when finish,
this will never happen because this is infinity loop */
vTaskDelete( NULL );
}

```

Per un costat, tenim la tasca "anotherTask" i per l'altre "loop", la tasca per defecte d'Arduino. Ambdues estan programades per mostrar un text cada 1000ms. Quan passa aquest període de temps, ens apareix al monitor el següent:

```
this is ESP32 Task
this is another task
```
Per tant, s'estan executant simultàniament.

## Exercici pràctic 2

He volgut anar una mica més enllà, i a part de crear un programa que utilitzi dues tasques, una que encengui un led i una altra que l'apagui, també he volgut fer un semàfor que funciona amb 4 tasques.

### LED Canviant

El codi del primer exercici que se'ns demanava és el següent:

```scala
#include <Arduino.h>

const int led1 = 15;
bool led1_ = false;

void enciendeLed( void * parameter )
{
  /* loop forever */
  for(;;)
  {
    while(led1_==true)
    {
      vTaskDelay(50 / portTICK_PERIOD_MS);
    }
  // Turn the LED on
  digitalWrite(led1, HIGH);
  Serial.println("LED ENCENDIDO");
  // Pause the task for 500ms
    vTaskDelay(500 / portTICK_PERIOD_MS);
      led1_=true;

  }
}

void apagaLed( void * parameter )
{
  /* loop forever */
   for(;;)
  {
    while(led1_==false)
    {
      vTaskDelay(50 / portTICK_PERIOD_MS);
    }
  // Turn the LED on
  digitalWrite(led1, LOW);
  Serial.println("LED APAGADO");
  // Pause the task for 500ms
    vTaskDelay(500 / portTICK_PERIOD_MS);
      led1_=false;

  }
}

void setup()
{
  Serial.begin(112500);
  pinMode(led1, OUTPUT);

  /* we create a new task here */
  xTaskCreate(
    enciendeLed, /* Task function. */
    "Enciende LED", /* name of task. */
    1000, /* Stack size of task */
    NULL, /* parameter of the task */
    1, /* priority of the task */
  NULL); /* Task handle to keep track of created task */

  xTaskCreate(
    apagaLed, /* Task function. */
    "Apaga LED", /* name of task. */
    1000, /* Stack size of task */
    NULL, /* parameter of the task */
    1, /* priority of the task */
  NULL); /* Task handle to keep track of created task */
}

/* the forever loop() function is invoked by Arduino ESP32 loopTask */
void loop()
{

}
```

Cada 500ms, el LED canvia d'estat.

### Semàfor

El funcionament física és el següent:

* BOTÓ 1: Quan es pressiona, el semàfor passa del color verd al vermell durant uns segons (per descomptat, sempre passant pel groc). Després, torna al verd.
* BOTÓ 2: Quan es pressiona, el semàfor passa del color verd al groc, el qual estarà parpadejant durant uns segons. Després, torna al verd.

[Vídeo del funcionament](https://drive.google.com/file/d/1U_v7O8Qjv87FIQh41pdIkNZGWInZMi_Y/view?usp=sharing) (s'ha de tenir iniciada la sessió amb un usuari acabat en "upc.edu").

El codi està inclòs dins del directori, però l'adjunto a continuació:

```scala
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
```