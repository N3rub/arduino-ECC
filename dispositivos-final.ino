#include <SPI.h>
#include <Ethernet.h>

/* TIPOS */
#define uint unsigned int

/* CALIBRATION CONSTANTS */
#define CALIBRATION_PULLUP_PIN 2

/* SENSOR CONSTANTS */
#define SENSOR_OUTPUT_PIN A0
#define SENSOR_VCC 5.0f
#define SENSOR_IDEAL_V_OFFSET 2.5f
#define SENSOR_MV_I_SENSIBILITY 0.100f

/* MEASUREMENT CONSTANTS */
#define ANALOG_RESOLUTION 1023
#define NUM_SAMPLES 100

/* OTHER CONSTANTS */
#define SQRT_2 0.7071067811865475f
#define LINE_VOLTAJE 110.0f

/* MACROS */
#define ADC(x) ( (x) / ANALOG_RESOLUTION )
#define Mod(x) ( (x) > 0 ? (x) : (x)*-1 )

void Calibrate();

float MeasureCurrent ();
float MeasureVoltage ();
void SendData ( float );

volatile float voe = SENSOR_IDEAL_V_OFFSET; // Electrical offset voltage V_{OE}, deviation from Vcc/2
volatile float i_offset = 0.0f;

EthernetServer server ( 80 );

void setup () {
    pinMode( CALIBRATION_PULLUP_PIN, INPUT_PULLUP );
    byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    IPAddress ip( 192, 168, 1, 200 );
    attachInterrupt( digitalPinToInterrupt( CALIBRATION_PULLUP_PIN ), Calibrate, FALLING );
	Ethernet.begin( mac , ip );
    server.begin();
    Serial.begin( 9600 );
    Calibrate();
}

void loop () {
	float current = MeasureCurrent();
    Serial.println ( current );

    SendData( current );

	delay( 1000 );
}

float MeasureSensorVoltage ()
{
     return ADC( analogRead ( SENSOR_OUTPUT_PIN ) * SENSOR_VCC );
}

float MeasureCurrent ()
{
	float measuredI = 0;
	for ( uint i=0; i < NUM_SAMPLES; i++ )
		measuredI += ( MeasureSensorVoltage() - voe ) / SENSOR_MV_I_SENSIBILITY;
    delay( 2 );
	return measuredI / NUM_SAMPLES;
}


void Calibrate()
{
    Serial.println ( "Calibrating" );
    voe = i_offset = 0;

    for ( uint i=0; i < NUM_SAMPLES; i++ )
        voe += MeasureSensorVoltage();
    voe /= NUM_SAMPLES;
    delay(2);

    Serial.print ( "Electrical Offset Voltaje: " );
    Serial.println ( voe );
}

void SendData ( float data ) {
	EthernetClient client = server.available ();
    if ( client ) {
        bool currentLineIsBlank = true;
        while ( client.connected () ) {
            if ( client.available () ) {
                char c = client.read ();
                // Si ha terminado la petición del cliente
                if ( c == '\n' && currentLineIsBlank ) {
                    // Enviar cabecera web
                    client.println ( "HTTP/1.1 200 OK" );
                    client.println ( "Content-Type: application/json" );
                    client.println ();

                    // Crear objeto JSON
                    client.print ( "{\"current\": " );
                    client.print ( data );
                    client.println ( "}" );

                    break;
                }
                if ( c == '\n' ) {
                    currentLineIsBlank = true;
                } else if ( c != '\r' ) {
                    currentLineIsBlank = false;
                }
            }
        }
	delay ( 1 );
	client.stop ();
    }
}
