const int motorPins[3] = {12, 11, 10};
const int motorRelayPins[3] = {7, 6, 5};
int motorInput[3] = {1, 1, 1};
int motorState[3] = {0, 0, 0};
int inputDelay[3] = {0, 0, 0};

void setup() {
  Serial.begin(9600);

  for (int i; i < 3; i++) {
    pinMode(motorPins[i], INPUT_PULLUP);  
    pinMode(motorRelayPins[i], OUTPUT);
  }

  
}

void loop() {

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

void displayMotorState(int motorID) {
  String stateName[] = {"off", "low", "high"};
  Serial.println("Motor " + String(motorID) + " is " + stateName[motorState[motorID]] + ".");
}
