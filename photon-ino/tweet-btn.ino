
//D7 LED Flash Example
int BUTTON = D2;
int btnState = LOW;
int prevBtnState;

void setup() {
    pinMode( BUTTON, INPUT );
}

void loop() {
    
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
    delay( 2500 );
}