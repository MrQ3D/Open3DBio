// **********************************************************************
// SKETCH FOR THE BIO-EXTRUDER CONTROL BOX OF THE "Open3DBio" BIOPRINTER
// by Mr.Q · https://www.mrq3d.com
// Friday April 11th, 2025
// ---------------------------------------------------------------------- 
// CREALITY STEPPER MOTOR 42-40
// DRV8825 STEPPER MOTOR DRIVER
// DISPLAY OLED 0.96" I2C SSD1306 128x64
// ----------------------------------------------------------------------
// TO-DO : TRANSLATE SPANISH VARIABLES & COMMENTS TO ENGLISH !!!
// **********************************************************************


// ***********
// OLED STUFF
// ***********

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED I2C ADDRESS. MOST COMMON ADDRESS IS 0x3C. 
#define SCREEN_ADDRESS 0x3C

// WIDTH & HEIGHT OF YOUR OLED SCREEN DEFINED IN PIXELS
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// CREATE AN OBJECT OF THE OLED SCREEN CLASS Adafruit_SSD1306.
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// TIME CONTROL
unsigned long TIEMPO_ULTIMO = 0;
unsigned long TIEMPO_INTERVALO = 10; // Actualizar cada 500ms
unsigned long TIEMPO_ACTUAL = 0;

int SPEED_TO_PRINT=0;

// ********************
// STEPPER MOTOR STUFF
// ********************

// DEFINE STEPPER MOTOR CONNECTIONS
#define dirPin 8
#define stepPin 9

// DEFINE STEPPER MOTOR STEPS PER REVOLUTION
//#define stepsPerRevolution 200
#define stepsPerRevolution 1

int VELOCIDAD_EXTRUSION=0;
int STEPPER_POSITION=0;
int VELOCIDAD_ACTUAL=0;

// *******************
// PUSH BUTTONS STUFF
// *******************

const int EXTRUSION = 2;
int EXTRUSION_VALUE = 0;

const int STOP = 3;
int STOP_VALUE = 0;

const int STEP_UP = 4;
int STEP_UP_VALUE = 0;

const int STEP_DOWN = 5;
int STEP_DOWN_VALUE = 0;

// ********************
// POTENTIOMETER STUFF
// ********************

const int VELOCIDAD_PIN = A0;

// POTENTIOMETER RAW ANALOG VALUE
int VELOCIDAD_VALUE;

// POTENTIOMETER POSITION MEASURED IN PERCENTAGE ( 0% - 100% )
int VELOCIDAD;

void setup() {

  // Inicializar comunicación serial
  Serial.begin(9600);

  // ***********
  // OLED STUFF
  // ***********

  OLED_INIT();

  delay(2000);

  // **************
  // STEPPER STUFF
  // **************

  // Declare pins as output:
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  // ******************
  // PUSH BUTTON STUFF
  // ******************
  
  pinMode(STEP_UP, INPUT);
  pinMode(STEP_DOWN, INPUT);
  pinMode(EXTRUSION, INPUT);
  pinMode(STOP, INPUT);

}

void OLED_INIT()
{
// Inicializar la pantalla OLED.
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.begin(9600);
    Serial.println(F("No se encontró una pantalla SSD1306"));
    for(;;); // No continuar, quedarse aquí.
  }

  // Limpiar el buffer de la pantalla.
  display.clearDisplay();
  
  // Configurar el tamaño del texto.
  display.setTextSize(2);
  
  // Configurar el color del texto.
  display.setTextColor(SSD1306_WHITE);
  
  // Posicionar el cursor.
  display.setCursor(0, 0);
  
  // Imprimir un mensaje en la pantalla.
  display.println(F(" Open3DBio"));
  display.println(F("   v1.0"));
  display.println(F("----------"));
  display.println(F("by MrQ3D"));
  
  // Mostrar el buffer en la pantalla.
  display.display();
}

 // Función para mostrar un número entero en la pantalla OLED.
 
void OLED_PRINT()
{

  TIEMPO_ACTUAL = millis();
 
  display.clearDisplay();

  // Configurar el tamaño del texto.
  display.setTextSize(7);

  // Configurar el color del texto.
  display.setTextColor(SSD1306_WHITE);
  
  // Posicionar el cursor.
  display.setCursor(0, 0);
  
    
  // REVERSE SPEED !!! 
  SPEED_TO_PRINT = 100-VELOCIDAD;

  if (SPEED_TO_PRINT<10)
  {
    display.print(F("  "));
    display.println(SPEED_TO_PRINT);
  }
  else if (SPEED_TO_PRINT<100)
  {
    display.print(F(" "));
    display.println(SPEED_TO_PRINT);
  }
  else
  {
    display.println(SPEED_TO_PRINT);
  }
  
  
  // Mostrar el buffer en la pantalla.
  display.display();
 

}

void UP()
{
 // Set the spinning direction clockwise:
  digitalWrite(dirPin, HIGH);

  // Spin the stepper motor 1 revolution slowly:
  
  for (int i = 0; i < stepsPerRevolution; i++) {

    STEPPER_POSITION=i;
     
    // OLED_PRINT(i);

    // These four lines result in 1 step:
 
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(VELOCIDAD_EXTRUSION);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(VELOCIDAD_EXTRUSION);
  }
}

void DOWN()
{
// Set the spinning direction counterclockwise:
  digitalWrite(dirPin, LOW);

  // Spin the stepper motor 1 revolution quickly:
  for (int i = 0; i < stepsPerRevolution; i++) {

    STEPPER_POSITION=i;

    //OLED_PRINT(stepsPerRevolution-i);

    // These four lines result in 1 step:
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(VELOCIDAD_EXTRUSION);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(VELOCIDAD_EXTRUSION);
  }
}

void EXTRUDE()
{
// Set the spinning direction counterclockwise:
  digitalWrite(dirPin, LOW);

  // Spin the stepper motor 1 revolution quickly:
  for (int i = 0; i < stepsPerRevolution; i++) {

    STEPPER_POSITION=i;

    //OLED_PRINT(stepsPerRevolution-i);

    // These four lines result in 1 step:
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(VELOCIDAD_EXTRUSION);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(VELOCIDAD_EXTRUSION);
  }
}

void READ_SPEED()
{
  VELOCIDAD_VALUE = analogRead(VELOCIDAD_PIN);       // realizar la lectura analógica raw
  VELOCIDAD = map(VELOCIDAD_VALUE, 0, 1023, 0, 100);  // convertir a porcentaje
  if(VELOCIDAD != VELOCIDAD_ACTUAL)
  {
     // VELOCIDAD_EXTRUSION = VELOCIDAD*100+200; // TO TUNE !!!
    VELOCIDAD_EXTRUSION = VELOCIDAD*50+100;
    OLED_PRINT();
    VELOCIDAD_ACTUAL=VELOCIDAD;
  }
}

void PRINT_SPEED()
{
  if(VELOCIDAD != VELOCIDAD_ACTUAL)
  {
     // VELOCIDAD_EXTRUSION = VELOCIDAD*100+200; // TO TUNE !!!
    VELOCIDAD_EXTRUSION = VELOCIDAD*50+100;
    OLED_PRINT();
    VELOCIDAD_ACTUAL=VELOCIDAD;
  }
}

void loop() {
  
  STEP_UP_VALUE = digitalRead(STEP_UP);  //lectura digital de pin
  STEP_DOWN_VALUE = digitalRead(STEP_DOWN);  //lectura digital de pin
  EXTRUSION_VALUE = digitalRead(EXTRUSION);  //lectura digital de pin
  // STOP_VALUE = digitalRead(STOP);  //lectura digital de pin

 
  //mandar mensaje a puerto serie en función del valor leido
  if (STEP_UP_VALUE == HIGH) {
      Serial.println("UP");
      UP();
      //OLED_PRINT();
  }

  if (STEP_DOWN_VALUE == HIGH) {
      Serial.println("DOWN");
      DOWN();
      //OLED_PRINT();
  }

  if (EXTRUSION_VALUE == HIGH) {
      Serial.println("EXTRUSION");

      STOP_VALUE = digitalRead(STOP);  //lectura digital de pin

      while(STOP_VALUE!=HIGH) {
      EXTRUDE();
      READ_SPEED();
      //PRINT_SPEED();
      STOP_VALUE = digitalRead(STOP);
      }

      Serial.println("STOP EXTRUSION !!!");

  }
   

  READ_SPEED();
 
}