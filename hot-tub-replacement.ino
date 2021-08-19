// Arduino pin assignments
const int therm1Pin = A0;
const int therm2Pin = A1;
const int motorPins[3] = {12, 11, 10};
const int motorRelayPins[3] = {7, 6, 5};
const int heaterPin = 9;

// Motor state tracking
int motorInput[3] = {1, 1, 1};
int motorState[3] = {0, 0, 0};
unsigned long inputLastTriggered[3] = {0, 0, 0};
int motorInputDelayTime = 250; // in ms
 
// heater / temperature variables
int tempTarget = 90;
bool heaterOn = false;
bool heaterWasOn = false;

// misc counters
unsigned long lastUpkeepCheck;


void setup() {
  // begin serial output
  Serial.begin(9600);

  // Setup pins
  for (int i; i < 3; i++) {
    pinMode(motorPins[i], INPUT_PULLUP);  
    pinMode(motorRelayPins[i], OUTPUT);
  }
  pinMode(heaterPin, OUTPUT);

  displayState(getTemp(therm1Pin), getTemp(therm2Pin));
}

void loop() {

  // Every ten seconds, adjust hot tub behavior
  if (timeSince(lastUpkeepCheck) >= 10 * 1000) {
    lastUpkeepCheck = millis();
    
    float temp1 = getTemp(therm1Pin);
    float temp2 = getTemp(therm2Pin);

    heaterLogic(temp1, temp2);

    displayState(temp1, temp2);
  }

  for (int i = 0; i < 3; i++) {
    
    // if we haven't triggered recently,
    if (timeSince(inputLastTriggered[i]) >= motorInputDelayTime) {

      // read input and determine if state has changed since last cycle  
      int motorButtonState = digitalRead(motorPins[i]);
      if (motorInput[i] != motorButtonState) {
        
        // We're currently working with a one-motor scale model, so this switch statement is temporary
        switch (i) {
          case 0:
              motorInput[i] = motorButtonState;
              // if the button went from open to pressed, we toggle the motor
              if (motorButtonState == 0) {
                toggleMotor(i);
              }
            break;
          case 1:
            if (tempTarget <= 105) {
              tempTarget++;
            }
            Serial.println("Set desired temp to " + String(tempTarget));
            break;
          case 2:
            if (tempTarget > 90) {
              tempTarget--;
            }
            Serial.println("Set desired temp to " + String(tempTarget));            
            break;
        }

        // tell the program that we've triggered
        inputLastTriggered[i] = millis();
      }
    }
  }
} // end of loop()

float getTemp(int pinID) {

  // get input from specified pin
  int thermRead = analogRead(pinID);

  // normalize analog read as a proportion of 5V
  float volts = (thermRead * 5) / 1024.0;
  
  // convert voltage to resistance; this equation is for a voltage divider setup with a 22.1-kΩ resistor on the 5V end
  float resistance = 22.1 * (5 - volts) / volts;

  // convert resistance to temp; equation derived from a scatter plot of test data points
  float temp = (-47.072 * log(resistance)) + 233.89;

  return temp;
}

void toggleMotor(int motorID) {

  if (motorState[motorID] < 2) {
    motorState[motorID]++;
    digitalWrite(motorRelayPins[motorID], HIGH); 
  } else {
    motorState[motorID] = 0;
    digitalWrite(motorRelayPins[motorID], LOW);
  }

  displayMotorState(motorID);
}

unsigned long timeSince(unsigned long checkpoint) {
  return millis() - checkpoint;
}

void heaterLogic(float temp1, float temp2) {
  // If heater is on, decide whether to turn it off
  if (heaterOn) {
    if (temp2 > 103) { // Don't heat if hot tub is too hot
      heaterOn = false;
    } else if (temp2 >= tempTarget) { // Don't heat if we've hit the target
      heaterOn = false;        
    }      
  } else {
    // heat if we're below the target; the offset constant is to prevent the heater flicking on and off
    // Currently set to 3 because we're working with a scale model tub
    if (temp2 < tempTarget - 3) { 
      heaterOn = true;
    }
  }

  // digitalWrite to heater if it needs to change
  if (heaterOn != heaterWasOn) {
    if (heaterOn) {
      digitalWrite(heaterPin, HIGH);
      Serial.println("Heater on.");
    } else {
      digitalWrite(heaterPin, LOW);
      Serial.println("Heater off.");
    }
  }
  // Then store this cycle's heater value for the next cycle to compare against
  heaterWasOn = heaterOn;
}

void displayMotorState(int motorID) {
  String motorStateLabels[3] = {"off", "low", "high"};
  Serial.println("Motor " + String(motorID) + " is " + motorStateLabels[motorState[motorID]] + ".");
}

void displayState(float temp1, float temp2) {
  String heaterState = heaterOn ? "on" : "off";
  String motorStateLabels[3] = {"off", "low", "high"};

  Serial.println("Temp1: " + String(temp1) + "; Temp2: " + String(temp2) + " Heater: " + heaterState + "; Motors: " + "1:" + motorStateLabels[motorState[0]] + " 2:" + motorStateLabels[motorState[1]] + " 3:" + motorStateLabels[motorState[2]]);
}
