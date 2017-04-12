// pins 

#define RED_LED 10
#define GREEN_LED 11
#define YELLOW_LED 12

#define RED_BUTTON 7
#define GREEN_BUTTON 8
#define YELLOW_BUTTON 9

#define OPEN_LED_SERVO 6
#define OPEN_SECRET_SERVO 3

// actions

#define STARTING_CHECK 0
#define WAITING_RAIN_DROP 1
#define OPEN_LED 2
#define CREATE_LED_ARRAY 3
#define SHOW_LED_ARRAY 4
#define PRESS_LED_ARRAY 5
#define OPEN_HIDE_DOOR 6
#define TO_START_POSITION 7

// servo settings

#define SERVO_START_POSITION 0
#define SERVO_END_POSITION 180
#define SERVO_STEP 2

#define RAIN_DROP_LIMIT 500
#define LED_REPEAT 2

#define HIGH_DELAY 500
#define MEDIUM_DELAY 300
#define LOW_DELAY 10 

#include <Servo.h> 

//----------------------------------
// analog pins

const int RANDOM_ARGUMENT = 0;
const int RAIN_DROP = 1;

//------for led array creation------

const int MAX_SIZE = 5;
const int MIN_SIZE = 3;

int arraySize;                      // real array size. 
int arrayIterator;

const int MAX_VALUE = YELLOW_LED;
const int MIN_VALUE = RED_LED;

int ledArray[MAX_VALUE];

//----------------------------------

int action;

//----------------------------------

int buttonInputSignal;              // Which button was pressed - red, green or yellow
bool buttonPressed = false;         // Button press or not indicator
//int const RAIN_DROP_LIMIT = 500;    // 
int blinkIterator;

//----------------------------------

Servo openLedServo;
Servo openSecretServo;

void setup() 
{
  pinMode(RED_LED, OUTPUT); 
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);

  pinMode(RED_BUTTON, INPUT);
  pinMode(GREEN_BUTTON, INPUT);
  pinMode(YELLOW_BUTTON, INPUT);

  action = STARTING_CHECK;
  arrayIterator = 0;

  openLedServo.write(SERVO_START_POSITION);
  openSecretServo.write(SERVO_START_POSITION);
  
  openLedServo.attach(OPEN_LED_SERVO);
  openSecretServo.attach(OPEN_SECRET_SERVO);
  
  blinkIterator = 0;

  Serial.begin(9600);
}

void console(int code = 0)
{
  switch(code)
  {
    case RED_LED:
      Serial.print("R");
      Serial.print(" ");
    break;

    case GREEN_LED:
      Serial.print("G");
      Serial.print(" ");
    break;

    case YELLOW_LED:
      Serial.print("Y");
      Serial.print(" ");
    break;

    default:
      Serial.println();
  }
}

void createLedArray()
{
  randomSeed(analogRead(0));

  arraySize = random(MIN_SIZE, MAX_SIZE + 1); 

  for(int i = 0; i < MAX_SIZE; i++)
  {
    if(i < arraySize)
      ledArray[i] = random(MIN_VALUE, MAX_VALUE + 1);
    else
      ledArray[i] = 0;
  }
}

void led(int led_delay, int color = 0)
{
  switch(color)
  {
    case RED_LED:
      digitalWrite(RED_LED, HIGH);
    break;
    case GREEN_LED:
      digitalWrite(GREEN_LED, HIGH);
    break;
    case YELLOW_LED:
      digitalWrite(YELLOW_LED, HIGH);
    break;
    default:
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(YELLOW_LED, HIGH);
  }

  delay(led_delay);

  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);

  delay(led_delay);
}

void loop() 
{
  switch(action)
  {
    case STARTING_CHECK:

      openLedServo.write(SERVO_START_POSITION);
      openSecretServo.write(SERVO_START_POSITION);

      blinkIterator = 0;
    
      action = WAITING_RAIN_DROP;
      
    break;
    
    case WAITING_RAIN_DROP:

      Serial.println(analogRead(RAIN_DROP));
      if(analogRead(RAIN_DROP) <= RAIN_DROP_LIMIT)
      {
        action = OPEN_LED;
      }

    break;

    case OPEN_LED:

    for(int i = SERVO_START_POSITION; i <= SERVO_END_POSITION; i += SERVO_STEP)
    {
      openLedServo.write(i);
      delay(LOW_DELAY);
    }

    led(MEDIUM_DELAY);
    led(MEDIUM_DELAY);
    led(MEDIUM_DELAY);

    action = CREATE_LED_ARRAY;
    
    break;

    case CREATE_LED_ARRAY:

      createLedArray();
      action = SHOW_LED_ARRAY;

      Serial.println(arraySize);
      
      for(int j = 0; j < MAX_SIZE; j++)
      {
        console(ledArray[j]);
      }

      console();
      
    break;

    case SHOW_LED_ARRAY:

      for(int i = 0; i < MAX_SIZE; i++)
      {
        if(ledArray[i] != 0)
        {
          led(HIGH_DELAY, ledArray[i]);
        }
      }

      buttonInputSignal = 0;
      arrayIterator = 0;
      
      action = PRESS_LED_ARRAY;
      
    break;

    case PRESS_LED_ARRAY:

      if(arrayIterator >= arraySize)
      {
        buttonPressed = false;
        blinkIterator++;
        
        if(blinkIterator < LED_REPEAT)
        {
           action = CREATE_LED_ARRAY;

           led(HIGH_DELAY);
        }
        else
        {
          blinkIterator = 0;
          action = OPEN_HIDE_DOOR;
        }
      }
      else
      {
        if(digitalRead(RED_BUTTON) == HIGH)
        {
          buttonInputSignal = RED_LED;
        }
        else if(digitalRead(RED_BUTTON) == LOW && buttonInputSignal == RED_LED)
        {
          buttonInputSignal = 0;
          buttonPressed = false;
        }
  
        if(digitalRead(GREEN_BUTTON) == HIGH)
        {
          buttonInputSignal = GREEN_LED;
        }
        else if(digitalRead(GREEN_BUTTON) == LOW && buttonInputSignal == GREEN_LED)
        {
           buttonInputSignal = 0;
           buttonPressed = false;
        }
        
        if(digitalRead(YELLOW_BUTTON) == HIGH)
        {
          buttonInputSignal = YELLOW_LED;
        }
        else if(digitalRead(YELLOW_BUTTON) == LOW && buttonInputSignal == YELLOW_LED)
        {
          buttonInputSignal = 0;
          buttonPressed = false;
        }
  
        if(buttonPressed == false)
          if(buttonInputSignal == ledArray[arrayIterator] )
          {
            Serial.println("ok");
            console(buttonInputSignal);
            console(ledArray[arrayIterator]);
            console();
      
            buttonPressed = true;
            arrayIterator++;
          }
          else if(buttonInputSignal != 0)
          {
            buttonPressed = false;
            
            action = SHOW_LED_ARRAY;
      
            Serial.println("err");
            console(buttonInputSignal);
            console(ledArray[arrayIterator]);
            console();
          }
      }

    break;

    case OPEN_HIDE_DOOR:
    
      for(int i = SERVO_START_POSITION; i <= SERVO_END_POSITION; i += SERVO_STEP)
      {
        openSecretServo.write(i);
        delay(LOW_DELAY);
      }

      led(MEDIUM_DELAY);
      led(MEDIUM_DELAY);
      led(MEDIUM_DELAY);
      
      action = TO_START_POSITION;
      
    break;

    case TO_START_POSITION:

      if(digitalRead(YELLOW_BUTTON) == HIGH && digitalRead(GREEN_BUTTON) == HIGH && digitalRead(RED_BUTTON) == HIGH)
      {
        action = STARTING_CHECK;
      }
    
    break;
  }
}
