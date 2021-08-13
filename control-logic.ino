int motorPins[3] = {12, 11, 10};
int motorInput[3] = {1, 1, 1};
int motorState[3] = {0, 0, 0};

void setup() {
  Serial.begin(9600);

  for (int i; i < 3; i++) {
    pinMode(motorPins[i], INPUT_PULLUP);    
  }
  
//  displayMotorState("1");
}

void loop() {

  for (int i = 0; i < 3; i++) {
    int motorButtonState = digitalRead(motorPins[i]);
    if (motorInput[i] != motorButtonState) {

      Serial.println("toggle motor" + String(i));
    }
  }

  //  Serial.println(input);
//
//  // call when input state changes
//  if (input != inputState) {
//      inputState = input;
//
//      // When button is depressed, change motor state and print
//      if (input == 0) {
//
//        if (motorState < 2) {
//          motorState++;
//        } else {
//          motorState = 0;
//        }
//
//        displayMotorState("1");
//      }
//      
//      delay(75);
//  }
}

//void displayMotorState(String motorName) {
//  String stateName[] = {"off", "low", "high"};
//  Serial.println("Motor " + motorName + " is " + stateName[motorState] + ".");
//}
