// This #include statement was automatically added by the Particle IDE.
#include "MQTT/MQTT.h"

// This #include statement was automatically added by the Particle IDE.
#include "SparkJson/SparkJson.h"

#define LOSANT_BROKER "broker.losant.com"
#define LOSANT_DEVICE_ID "[]"
#define LOSANT_ACCESS_KEY "[]"
#define LOSANT_ACCESS_SECRET "[]"

// Topic used to subscribe to Losant commands.
String MQTT_TOPIC_COMMAND =
    String::format("losant/%s/command", LOSANT_DEVICE_ID);

// Topic used to publish state to Losant.
String MQTT_TOPIC_STATE =
    String::format("losant/%s/state", LOSANT_DEVICE_ID);

// Callback signature for MQTT subscriptions.
void callback(char* topic, byte* payload, unsigned int length);

// // MQTT client.
MQTT client(LOSANT_BROKER, 1883, callback);

//D7 LED Flash Example
int BUTTON = D2;
int btnState = LOW;
int prevBtnState;
int lastUpdate = millis();

void setup() {
     Serial.begin( 9600 );
    while( !Serial ) { }

    pinMode( BUTTON, INPUT );
}

void callback(char* topic, byte* payload, unsigned int length) {
    
    // Parse the command payload.
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& command = jsonBuffer.parseObject((char*)payload);

    Serial.println("Command received:");
    command.printTo(Serial);
    Serial.println();
    
    // If the command's name is "toggle", flip the LED.
    // if(String(command["name"].asString()).equals(String("toggle"))) {
    //     ledValue = !ledValue;
    //     digitalWrite(LED, ledValue ? HIGH : LOW);
    //     Serial.println("Toggling LED");
    // }
}

// Connects to the Losant MQTT broker.
void connect() {

    Serial.print("Connecting to Losant...");

    while(!client.isConnected()) {
        client.connect(
            LOSANT_DEVICE_ID,
            LOSANT_ACCESS_KEY,
            LOSANT_ACCESS_SECRET);

        if(client.isConnected()) {
            Serial.println("connected!");
            client.subscribe(MQTT_TOPIC_COMMAND);
        }
        else {
            Serial.print(".");
            delay(500);
        }
    }
}

void loop() {
    
    if (!client.isConnected()) {
        connect();
    }

    // Loop the MQTT client.
    client.loop();

    int now = millis();
    
    if ( Particle.connected() == false ) {
        Particle.connect();
    }

    btnState = digitalRead( BUTTON );
    
    if ( btnState != prevBtnState ) {
        if ( btnState == HIGH ) {
           tossEvent();
        }
    }
}

void tossEvent() {
    Particle.publish( "nodePDX-button-pushed", "pushed" );
    
     // Build the json payload:
        // { "data" : { "tempF" : val, "tempC" : val }}
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();
        JsonObject& state = jsonBuffer.createObject();

        // TODO: refer to your specific temperature sensor
        // on how to convert the raw voltage to a temperature.
        int tempRaw = analogRead(A0);
        state["tweet"] = "a user tweeted";
        root["data"] = state;
        
        // Get JSON string.
        char buffer[200];
        root.printTo(buffer, sizeof(buffer));

        client.publish(MQTT_TOPIC_STATE, buffer);
    
    delay( 2500 );
}