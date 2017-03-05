#include "RF24.h" // This is the RF24 library that may need to be installed through the Manage Libraries feature in the IDE.

RF24 radio(9, 10);//Create a commuications object for talking to the NRF24L01
const uint64_t send_pipe=0xB01DFACEC11;//These are just arbitrary 64bit numbers to use as pipe identifiers
const uint64_t recv_pipe=0xDEADBEEFF12;//They must be the same on both ends of the communciations

#define RELAY1 4
#define RELAY2 5
#define RELAY3 6
#define RELAY4 7    

void setup()
{
  pinMode(RELAY1, INPUT_PULLUP);  //So they dont start high
  pinMode(RELAY2, INPUT_PULLUP);
  pinMode(RELAY3, INPUT_PULLUP);
  pinMode(RELAY4, INPUT_PULLUP);
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);

  
  Serial.begin(9600);//Set up comm with the IDE serial monitor
  Serial.println("Ready for commands");
  radio.begin();//Start up the radio object
  radio.setPALevel(RF24_PA_LOW);//This sets the power low. This will reduce the range. RF24_PA_MAX would increase the range
  radio.setRetries(15,15);//This will improve reliability of the module if it encounters interference
  radio.openWritingPipe(send_pipe);//Thses are the reverse of the transmit code.
  radio.openReadingPipe(1,recv_pipe);
  radio.startListening();//Give the module a kick
}

void loop()
{
  unsigned long message_code=0;

  if(radio.available())//Keep checking on each loop to see if any data has come in
  {
    while(radio.available())//Loop while there is incoming data. The packets are one unsigned long in total so it shoudl only loop once
    {
      radio.read(&message_code, sizeof(unsigned long));//Stuff the incoming packet into the motor_code variable
    }
    radio.stopListening();//We have heard so now we will send an ack
    radio.write(&message_code, sizeof(unsigned long));//Turn the motor code around and send it back
    radio.startListening();//Go back to listening as this is what this mostly does

    if(message_code == 1){
      Serial.println("Shift UP");
      digitalWrite(RELAY1,0);
      delay(100);
      digitalWrite(RELAY1,1);
    }
    if(message_code == 0){
      Serial.println("Shift Down");
      digitalWrite(RELAY2,0);
      delay(100);
      digitalWrite(RELAY2,1);
    }
    if(message_code == 2){
      Serial.println("SportMode ON");
      digitalWrite(RELAY3,0);
    }
    if(message_code == 3){
      Serial.println("SportMode OFF");
      digitalWrite(RELAY3,1);
    }
  }
}
