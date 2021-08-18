// Arduino pin assignments
const int therm1Pin = A0;
const int therm2Pin = A1;
const int motorPins[3] = {12, 11, 10};
const int motorRelayPins[3] = {7, 6, 5};

// Motor state tracking
int motorInput[3] = {1, 1, 1};
int motorState[3] = {0, 0, 0};
int inputDelay[3] = {0, 0, 0};
String motorStateLabels[3] = {"off", "low", "high"};

// heater / temperature variables
int tempTarget = 90;
bool heaterOn = false;
bool heaterWasOn = false;

// misc counters
unsigned long tenSecondCheck = 0;


void setup() {
  Serial.begin(9600);

  for (int i; i < 3; i++) {
    pinMode(motorPins[i], INPUT_PULLUP);  
    pinMode(motorRelayPins[i], OUTPUT);
  }
}

void loop() {

  // Every ten seconds, adjust hot tub behavior
  if (tenSecondsElapsed()) {
    float temp1 = getTemp(therm1Pin);
    float temp2 = getTemp(therm2Pin);

    // If heater is on, decide whether to turn it off
    if (heaterOn) {
      if (temp2 > 105) { // Don't heat if hot tub is too hot
        heaterOn = false;
      } else if (temp2 >= tempTarget) { // Don't heat if we've hit the target
        heaterOn = false;        
      }      
    } else {
      if (temp2 < tempTarget - 1) { // heat if we're below the target
        heaterOn = true;
      }      
    }

    // digitalWrite(heaterPin, something conditional)
    heaterWasOn = heaterOn;

    displayState(temp1, temp2);
  }

  for (int i = 0; i < 3; i++) {
    // first check for a cooldown to avoid wasting time
    if (inputDelay[i] > 0) {
      inputDelay[i]--;
      
    } else {
      // read input and determine if state has changed since last cycle  
      // We're currently working with a one-motor scale model, so this switch statement is temporary
      switch (i) {
        case 0:
          int motorButtonState = digitalRead(motorPins[i]);
          if (motorInput[i] != motorButtonState) {
            motorInput[i] = motorButtonState;
    
            // if the button went from open to pressed, we toggle the motor
            if (motorButtonState == 0) {
              toggleMotor(i);
              inputDelay[i] = 250;
            }
          }
          break;
        case 1:
          if (tempTarget < 105) {
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
    }
  }
  
  delay(1); // otherwise the inputDelay statement goes by way too fast

}

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


bool tenSecondsElapsed() {
  
  unsigned long currentTime = millis();
  
  if (currentTime - tenSecondCheck >= 10 * 1000) {
    tenSecondCheck = currentTime;
    return true;
  } else {
    return false;
  }
}

void displayMotorState(int motorID) {
  String stateName[] = {"off", "low", "high"};
  Serial.println("Motor " + String(motorID) + " is " + stateName[motorState[motorID]] + ".");
}

void displayState(float temp1, float temp2) {
//  Serial.println("Therm " + String(thermID) + ": thermRead = " + String(thermRead) + "; volts = " + String(volts) + "; resistance = " + String(resistance) + "; temp = " + String(temp));
    Serial.println("Temp 1: " + String(temp1) + "; Temp 2: " + String(temp2) + "; Motors: " + "1:" + motorStateLabels[motorState[0]] + " 2:" + motorStateLabels[motorState[1]] + " 3:" + motorStateLabels[motorState[2]]);
}
