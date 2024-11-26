#include <SPI.h>
#include <Ethernet.h>

/* TIPOS */
#define uint unsigned int

/* SENSOR CONSTANTS */
#define SENSOR_OUTPUT_PIN A0
#define SENSOR_VCC 5.0f
#define SENSOR_V_OFFSET 2.5f
#define SENSOR_MV_I_SENSIBILITY 0.100f

/* MEASUREMENT CONSTANTS */
#define ANALOG_RESOLUTION 1023.0f
#define NUM_SAMPLES 100

/* OTHER CONSTANTS */
#define SQRT_2 0.7071067811865475f
#define LINE_VOLTAJE 110.0f

/* MACROS */
#define MeasureVoltaje ( analogRead(SENSOR_OUTPUT_PIN) * SENSOR_VCC / ANALOG_RESOLUTION )

const int ledPin = 13;

void setup() {
	pinMode(ledPin, OUTPUT);
}

void loop() {

	digitalWrite(ledPin, HIGH);
	delay(1000);
	digitalWrite(ledPin, LOW);
	delay(1000);
}

float MeasureCurrent()
{
	float measuredI = 0;
	for ( uint i=0; i < NUM_SAMPLES; i++ )
		measuredI += ( MeasureVoltaje - SENSOR_V_OFFSET ) / SENSOR_MV_I_SENSIBILITY;
	return measuredI / NUM_SAMPLES;
}
