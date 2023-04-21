#define USE_ARDUINO_INTERRUPTS true
#include "BluetoothSerial.h"
#include <PulseSensorPlayground.h>  
#define LED_BUILTIN 2 //pin with LED to turn on when BT connected
 
BluetoothSerial ESP_BT; // Object for Bluetooth
PulseSensorPlayground pulseSensor; //Object for pulsesensor
 
// global vars
boolean BT_cnx = false;
const int PulseWire = 39;  
int Threshold = 550;     // Determine which Signal to "count as a beat" and which to ignore.
 
void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){
  if(event == ESP_SPP_SRV_OPEN_EVT){
    Serial.println("Client Connected");
    digitalWrite(LED_BUILTIN, HIGH);
    BT_cnx = true;
  }
 
  if(event == ESP_SPP_CLOSE_EVT ){
    Serial.println("Client disconnected");
    digitalWrite(LED_BUILTIN, LOW);
    BT_cnx = false;
    ESP.restart();
  }
}
 
void setup() {
  // initialize digital pin 2 as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  // initialize the serial communication:
  Serial.begin(9600);
  Serial.println(); // blank line in serial ...
  pinMode(41, INPUT); // Setup for leads off detection LO +
  pinMode(40, INPUT); // Setup for leads off detection LO -
  // initialize the serial BT communication:
  ESP_BT.register_callback(callback);
  if(!ESP_BT.begin("ESP32_ECG")){
    Serial.println("An error occurred initializing Bluetooth");
  }else{
    Serial.println("Bluetooth initialized... Bluetooth Device is Ready to Pair...");
  }

  pulseSensor.analogInput(PulseWire);  
  pulseSensor.blinkOnPulse(LED_BUILTIN);       //blink LED with heartbeat.
  pulseSensor.setThreshold(Threshold);  // Double-check the "pulseSensor" object was created and "began" seeing a signal.
  if (pulseSensor.begin()) {
    Serial.println("We created a pulseSensor Object !");  //This prints one time at power-up, or reset. 
  }

}
 
void loop() {
  if((digitalRead(40) == 1)||(digitalRead(41) == 1)){
    Serial.println('!');
    ESP_BT.println('!');
  }
  else{
    // send the value of analog input 0 to serial:
    Serial.println(analogRead(A0));
    //Do the same for blutooth
    if(BT_cnx){
      ESP_BT.print('E'); //make the app Blutooth Graphics (https://play.google.com/store/apps/details?id=com.emrctn.BluetoothGraphics&hl=en_US) work (as specified by the app)
      ESP_BT.println(analogRead(A0));
    } 
  }
  //Wait a little to keep serial data from saturating
  delay(1);

  int myBPM = pulseSensor.getBeatsPerMinute();  // Calls function on our pulseSensor object that returns BPM as an "int".
                                               // "myBPM" hold this BPM value now.
  if (pulseSensor.sawStartOfBeat()) {            // Constantly test to see if "a beat happened".
    Serial.println("♥  A HeartBeat Happened ! "); // If test is "true", print a message "a heartbeat happened".
    Serial.print("BPM: ");                        // Print phrase "BPM: "
    Serial.println(myBPM);                        // Print the value inside of myBPM.
}
}