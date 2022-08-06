//  I adapted this from the rc-switch examples (ref https://github.com/sui77/rc-switch/ )
//  All credits to the owner.

#include <RCSwitch.h>

#define USB_BAUD 9600


RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(USB_BAUD);
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
}

void loop() {
  if (mySwitch.available()) {
    
    Serial.print("Received ");
    Serial.print( mySwitch.getReceivedValue() );
    Serial.print(" / ");
    Serial.print( mySwitch.getReceivedBitlength() );
    Serial.print("bit ");
    Serial.print("Protocol: ");
    Serial.println( mySwitch.getReceivedProtocol() );

    mySwitch.resetAvailable();
  }
}
