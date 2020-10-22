// This #include statement was automatically added by the Particle IDE.
// This #include statement was automatically added by the Particle IDE.
#include <MQTT.h>

int btnPin = A4;                                                    // tell the program where the button is connected to              
int ledPin = D7;                                                    // tell the program where the on-board LED is connected to           
int currentBtnState = -1;                                           // LOW = idle, HIGH = pressed
int prevBtnState = -1;
char deviceName[32] = "";
char msg[50] = "Hello ! ";
char mainTopic[20] = "Mercaptan/";
char fullTopic[60] = "";
unsigned long lastTime = 0;


//MQTT client("test.mosquitto.org", 1883, MQTTcallback);             // we will be using the public Mosquitto MQTT server
// MQTT client("mqtt.eclipse.org",1883,MQTTcallback);                // we will be using the public Eclipse MQTT server
MQTT client("207.253.185.55", 1883, MQTTcallback);                   // we will be using the public Jstrategia MQTT server
void MQTTcallback(char* topic, byte* payload, unsigned int length)   // will be called if a message is posted to a MQTT topic we subscribed to
{
   
}

void handler(const char *topic, const char *data)                    // This is called when the Cloud makes the current device name available
{
  strncpy(deviceName, data, sizeof(deviceName)-1);
  strcat(fullTopic,mainTopic);
  strcat(fullTopic,deviceName);
}

void setup()
{
  Particle.subscribe("particle/device/name", handler);               // prepare to retrieve current device public name
  Particle.publish("particle/device/name");                          // ask the cloud for the name to be sent to you
  pinMode(ledPin, OUTPUT);                                           // configure the on-board LED
  pinMode(btnPin, INPUT_PULLDOWN);                                   // tell the program we will read the button state soon
}

void loop()
{
    // connect to MQTT broker
    if (!client.isConnected())
    {
       if (!client.connect("MyPhoton"))
       {
           digitalWrite(ledPin, HIGH);                                 // turn Photon LED on, indicating we are unable to connect
           delay(1000);
           return;
       }
    }
    digitalWrite(ledPin, LOW);                                         // turn Photon LED off
    unsigned long now = millis();                                      
    currentBtnState = digitalRead(btnPin);                             // read the emergency button state 
    if (currentBtnState == HIGH && prevBtnState != currentBtnState)    // the button has been pressed
    {
      if ((now - lastTime) >= 1000)                                    // we don't want to send more than one message per second
      {
        digitalWrite(ledPin, HIGH);                                    // turn Photon LED on, indicating we are sending a message
        client.publish(fullTopic, msg);                                // send a message to the MQTT server
        delay(100);                                                    // to give some time to see the LED on
        lastTime = now;                                                               
        digitalWrite(ledPin, LOW);                                     // turn Photon LED off, confirming the message has been sent
      }
    }
    prevBtnState = currentBtnState;                                    // to keep track off button state for next iteration
    client.loop();                                                     // this is required to ensure the MQTT stuff works
}