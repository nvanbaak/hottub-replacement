int pinID = A0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Read analog input
  int varLog = getTemp(pinID);

  delay(1000);
}

int getTemp(int pinID) {

  // get input from specified pin
  int thermRead = analogRead(pinID);

  // normalize analog read as a proportion of 5V
  double volts = (thermRead * 5) / 1024.0;
  
  // convert voltage to resistance; this equation is for a voltage divider setup with a 22-kΩ resistor on the 5V end
  double resistance = 22 * (5 - volts)) / volts;

  // convert resistance to temp; equation derived from a scatter plot of test data points
  double temp = (-47.072 * log(resistance)) + 233.89;

  Serial.println("thermRead = " + String(thermRead) + "; volts = " + String(volts) + "; resistance = " + String(resistance) + "; temp = " + String(temp));

  return temp;
}
