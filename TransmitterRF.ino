#include "RF24.h" // This is the RF24 library that may need to be installed through the Manage Libraries feature in the IDE.

RF24 radio(9, 10); // Establish a new RF24 object
const uint64_t send_pipe=0xB01DFACEC11;//This will be this device
const uint64_t recv_pipe=0xDEADBEEFF12;//This will be the other device

int nbts = 2; //Number of buttons(2 because shift up and shift down)
int startpin = 3; //The pin that the fist button starts on
int bts[2];
boolean btgs[2];
boolean sportbtns = false;
boolean sport = false;

void setup() {
  Serial.begin(9600); 
  for (int i = 0; i < nbts; i++) bts[i] = i + startpin;
  for (int i = 0; i < nbts; i++) btgs[i] = false;
  for (int i = 0; i < nbts; i++) pinMode(bts[i], INPUT_PULLUP);

  radio.begin();// Basically turn on communications with the device
  radio.setPALevel(RF24_PA_LOW);//RF24_PA_MAX is max power
  radio.setRetries(15,15);//This will improve reliability
  radio.openWritingPipe(recv_pipe);//Set up the two way communications with the named device
  radio.openReadingPipe(1,send_pipe);
  radio.startListening();// Start listening for data which gives the device a kick
}

void loop(){

    for(int i = 0; i < nbts; i++) {
      if (!btgs[i]) {        
          if(digitalRead(bts[i])==LOW) {
          btgs[i] = true;
          if(btgs[0] && btgs[1]) {
            sportbtns = true;
            if(!sport) {
              sport = true;
              transmit(2);
              Serial.println("SportMode ON");
            }
            else {
              sport = false;
              transmit(3);
              Serial.println("SportMode OFF");
            }
          }
        }
      }
      else {
        if(digitalRead(bts[i])==HIGH) {
          btgs[i] = false;
          if(!sportbtns) {
            if(i == 0) { 
              transmit(0);
              Serial.println("Shift DOWN"); 
            }
            else { 
              transmit(1);
              Serial.println("Shift UP");
            }
          }
          else if(!btgs[0] && !btgs[1]) {
            sportbtns = false;
          }
        }
      }
    }
  delay(15);
}

void transmit(unsigned long statusCode) 
{
  radio.stopListening();//We are sending not listening for now
  if(!radio.write(&statusCode, sizeof(unsigned long)))// Send the message_code and check to see if it comes back false
  {
    Serial.println("Failed");
  }
  
  radio.startListening();//Go back to listening and wait for the ack signal.
  
  unsigned long started_waiting_at=micros();//This notes the time 
  boolean timeout=false;//Assume for the moment that there is no timeout
  
  while(!radio.available())//Keep looping while no ack has come in
  {
    if(micros()-started_waiting_at>200000)//If the loop has been going for more than 1/5th of a second then give up
    {
      timeout=true; //Note that it has failed
      break;
    }      
  }
  if(timeout==true)//If the previous looped marked as failure then 
  {
    Serial.println("Timeout");
  }
  else// If it didn't fail to ack then read in and printout the results.
  {
    unsigned long in_data;
    radio.read(&in_data, sizeof(unsigned long));
    Serial.println("In Data: "+String(in_data));
  }
}
