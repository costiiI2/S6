/*
 * Author(s): KBP & CCT (Kévin BOUGNON-PEIGNE & Costantino Cecchet)
 */

#define INPUT_VOLTAGE   3.3f
#define ADC_RESOLUTION  1023.f
#define HUNDRED         100

#define S_TO_MS         1000

#define V_TO_MV         1000
#define DEGREE_25       25
#define MV_AT_25DEGREE    750
#define SCALING_MV_DEGREE 10

// Experimental measurements
#define LIGHT_MIN_VOLTAGE 0.03f
#define LIGHT_MAX_VOLTAGE (3.2f-LIGHT_MIN_VOLTAGE)

int resistiveDivPin = A0; // Input pin for the resistive divisor
int tmpSensorPin = A1;    // Input pin for the temperature sensor
int lightSensorPin = A2;  // Input pin for the light sensor

void setup() {
  Serial.begin(9600); // Prepare baudrate for Serial Monitor
}

void loop() {
  int sensorValue;
  float voltage, degree, percent;
  Serial.println("------------------------------------------------");

  Serial.print("Voltage (Res) : ");
  sensorValue = analogRead(resistiveDivPin);
  voltage = ((float)sensorValue / ADC_RESOLUTION) * INPUT_VOLTAGE;
  Serial.print(voltage);
  Serial.println("V");

  Serial.print("Temperature   : ");
  sensorValue = analogRead(tmpSensorPin);
  voltage = ((float)sensorValue / ADC_RESOLUTION) * INPUT_VOLTAGE;
  degree = (voltage*V_TO_MV - MV_AT_25DEGREE) / SCALING_MV_DEGREE + DEGREE_25;
  Serial.print(degree);
  Serial.println("°");

  Serial.print("Light ratio   : ");
  sensorValue = analogRead(lightSensorPin);
  voltage = ((float)sensorValue / ADC_RESOLUTION) * INPUT_VOLTAGE;
  percent = (voltage-LIGHT_MIN_VOLTAGE) / LIGHT_MAX_VOLTAGE * HUNDRED;
  Serial.print(percent);
  Serial.print("% (");
  if (percent < 0.75)       Serial.print("Hidden");
  else if (percent < 0.95)  Serial.print("Ambient");
  else                      Serial.print("Exposed");
  Serial.println(")");

  delay(S_TO_MS);  // Do measures every sec.
}
