unsigned long tenSecondCheck = 0;
const int therm1 = A0;
const int therm2 = A1;
const int motorPins[3] = {12, 11, 10};
const int motorRelayPins[3] = {7, 6, 5};
int motorInput[3] = {1, 1, 1};
int motorState[3] = {0, 0, 0};
String motorStateLabels[3] = {"off", "low", "high"};
int inputDelay[3] = {0, 0, 0};


void setup() {
  Serial.begin(9600);

  for (int i; i < 3; i++) {
    pinMode(motorPins[i], INPUT_PULLUP);  
    pinMode(motorRelayPins[i], OUTPUT);
  }

}

void loop() {

  if (tenSecondsElapsed()) {
    double temp1 = getTemp(therm1);
    double temp2 = getTemp(therm2);
    displayState(temp1, temp2);
  }

  for (int i = 0; i < 3; i++) {
    // first check for a cooldown to avoid wasting time
    if (inputDelay[i] > 0) {
      inputDelay[i]--;
      
    } else {
      // read input and determine if state has changed since last cycle   
      int motorButtonState = digitalRead(motorPins[i]);
      if (motorInput[i] != motorButtonState) {
        motorInput[i] = motorButtonState;

        // if the button went from open to pressed, we toggle the motor
        if (motorButtonState == 0) {
          toggleMotor(i);
          inputDelay[i] = 250;
        }
      }
    }
  }
  
  delay(1); // otherwise the inputDelay statement goes by way too fast

}

int getTemp(int pinID) {

  // get input from specified pin
  int thermRead = analogRead(pinID);

  // normalize analog read as a proportion of 5V
  double volts = (thermRead * 5) / 1024.0;
  
  // convert voltage to resistance; this equation is for a voltage divider setup with a 22.1-kΩ resistor on the 5V end
  double resistance = 22.1 * (5 - volts) / volts;

  // convert resistance to temp; equation derived from a scatter plot of test data points
  double temp = (-47.072 * log(resistance)) + 233.89;

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
  
  if (currentTime - tenSecondCheck >= 1 * 1000) {
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

void displayState(double temp1, double temp2) {
//  Serial.println("Therm " + String(thermID) + ": thermRead = " + String(thermRead) + "; volts = " + String(volts) + "; resistance = " + String(resistance) + "; temp = " + String(temp));
    Serial.println("Therms 1: " + String(temp1) + "; Therm 2: " + String(temp2) + "; Motors: " + "1:" + motorStateLabels[motorState[0]] + " 2:" + motorStateLabels[motorState[1]] + " 3:" + motorStateLabels[motorState[2]]);
}
