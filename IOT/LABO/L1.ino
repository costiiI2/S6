
// Define the LED pin
const int ledPin = 9;
// Define the RGB pins
const int redPin = 2;
const int greenPin = 3;
const int bluePin = 4;

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // Initialize the LED pin as an output
  pinMode(ledPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
    pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}

void loop() {
  // Turn on the LED
  digitalWrite(ledPin, HIGH);
  digitalWrite(LED_BUILTIN, LOW);
  // Print a message to the serial monitor
  Serial.println("LED on!");
  // Wait for a second
  delay(1000);

  // Turn off the LED
  digitalWrite(ledPin, LOW);
  digitalWrite(LED_BUILTIN, HIGH);
  // Print a message to the serial monitor
  Serial.println("LED off");
  // Wait for a second
  delay(1000);
   // Set RGB to red
   Serial.println("RED")
  setColor(255, 0, 0);
  delay(1000);
  // Set RGB to green
  setColor(0, 255, 0);
     Serial.println("GREEN")

  delay(1000);
  // Set RGB to blue
  setColor(0, 0, 255);
     Serial.println("BLUE")

  delay(1000);
}

