//INSERT YOUR CUSTOM CODE HERE
void customLoop(){
  
}


//ETHERNET USERS: CHANGE MAC, IP AND PORT HERE IF NEEDED
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte ip[] = { 192, 168, 12, 100};
int port = 80;
bool useEthernet = true;



































//Library to easily connect stepper motors
#include <Stepper.h>

//Ethernet library
#include <SPI.h>
#include <Ethernet.h>
EthernetServer server(port);

//Variables that help control pins 2 and 3 states so that they can be sent to AutoArduino as events
const byte interruptPinsNumber = 2;
const byte interruptPins[] = {2,3};
volatile bool shouldReads[] = {false,false};
unsigned long lastChanges[] = {0l,0l};
byte lastValues[] = {HIGH,HIGH};
byte lastPinMode[] = {INPUT_PULLUP,INPUT_PULLUP,OUTPUT,OUTPUT,OUTPUT,OUTPUT,OUTPUT,OUTPUT,OUTPUT,OUTPUT,OUTPUT,OUTPUT};

//Runs when pin 2 changes
void pin2Changed() {
  shouldReads[0] = true;
}
//Runs when pin 3 changes
void pin3Changed() {
  shouldReads[1] = true;
}


void setup() {

  Serial.begin(9600);
  pinMode (13, OUTPUT);
  pinMode (12, OUTPUT);
  pinMode (11, OUTPUT);
  pinMode (10, OUTPUT);
  pinMode (9, OUTPUT);
  pinMode (8, OUTPUT);
  pinMode (7, OUTPUT);
  pinMode (6, OUTPUT);
  pinMode (5, OUTPUT);
  pinMode (4, OUTPUT);
  //Pin 2 e 3 are configured for input so that AutoArduino can be alerted when they change
  pinMode (3, INPUT_PULLUP);
  pinMode (2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), pin2Changed, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), pin3Changed, CHANGE);

  if(useEthernet){
    Ethernet.begin(mac,ip);
    server.begin();
  }
  /*Serial.print("server is at ");
  Serial.println(Ethernet.localIP());*/
}
EthernetClient clientRequest;

bool connectedIpAddressSet = false;
byte connectedIpAddress[] = {0,0,0,0};
void loop() {

  handleSerial();
  handleEthernet();
  //For all pins that generate events (2 e 3) check if value changed
  for(int i = 0;i<interruptPinsNumber;i++){
    byte currentValue = hasChanged(i);
    //se o valor mudo, enviar novo valor por Serial
    if(currentValue != NULL){
      
     int pin = i + 2;
     int value = currentValue - 1;
     handleChangeSerial(pin, value);
     handleChangeEthernet(pin, value);


      
    }
  }
  customLoop();
}

void handleChangeSerial(int pin, int value){
  Serial.print("i");
  Serial.print(pin);
  Serial.print(",");
  Serial.print(value);
  Serial.print(";");
}
void handleChangeEthernet(int pin, int value){
  if(!useEthernet){
    return;
  }
  if (connectedIpAddressSet && clientRequest.connect(connectedIpAddress, 8080)) {
    //Serial.println("connected");
    clientRequest.print("GET /?aacomm=");
  
    clientRequest.print("i");
    clientRequest.print(pin);
    clientRequest.print(",");
    clientRequest.print(value);
    clientRequest.print(";");
    clientRequest.println(" HTTP/1.0");
    clientRequest.println();
    clientRequest.stop();
  } else {
    //Serial.println("connection failed");
  }
  //Serial.println();
  //Serial.println("disconnecting.");
  clientRequest.stop();
}
bool writeEthernet = false;
EthernetClient client;
void handleEthernet(){
  if(!useEthernet){
    return;
  }
  client = server.available();
  if (client) {
    String readStringEthernet = "";
    boolean currentLineIsBlank = true;
    //Serial.println("new client");
    while (client.connected()) {
      if (client.available()) {      
        char c = client.read();
        readStringEthernet += c;
        if(readStringEthernet.indexOf("GET /?aacomm=")==0){
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println();
          while(true){
            char mode = client.read();
            if(mode == 'i'){
              connectedIpAddress[0] = client.parseInt();
              connectedIpAddress[1] = client.parseInt();
              connectedIpAddress[2] = client.parseInt();
              connectedIpAddress[3] = client.parseInt();/*
              Serial.print("Got address: ");
              Serial.print(connectedIpAddress[0]);
              Serial.print(".");
              Serial.print(connectedIpAddress[1]);
              Serial.print(".");
              Serial.print(connectedIpAddress[2]);
              Serial.print(".");
              Serial.println(connectedIpAddress[3]);*/
              connectedIpAddressSet = true;
            }
            //Serial.print(mode);
            //check how many values this mode expects
            int modeValues = getModeValues(mode);
            int values[modeValues];
            if(modeValues != NULL){
              //assign all values
              for(int i = 0;i < modeValues;i++){
                values[i] = client.parseInt();
                //Serial.print(values[i]);
              }            
            }else{
              //client.println("Done wrong command!");
              //client.println("Host: https://autoremotejoaomgcd.appspot.com/sendnotification?key=APA91bHQi071DSkIHbDHqP4t2Y1MhhpiUNPyrfBn3WpUkKZHhYPdFdJb6UoEYOieiEpH9PWRitOrfqqSQXzk0xESpwK0DJXHgbUB85eaUq3kf6ANz850OkCWj_CUFGAb9LUUuUy3F-9F_HY-AgIu_amJNubDIpHfXV5APMLvWaGUwhRP_YmTWNU&title=aaa&text=ccc");
              //client.print("OK");
              //Serial.println("OK");
              break;
            }
            //read command delimiter
            char endChar = client.read();
            if(endChar != ';'){
             //client.println("Done wrong end char!");
              //client.println();      

              break;
            }else{              
              //run command
              /*client.print("Running command: ");
              client.print(mode);
              client.print(" - ");
              client.print(modeValues);
              client.println(" values");*/
              writeEthernet = true;
              handleCommand(mode,values);
            }
          }
          break;
          //Serial.println(endChar);
        }        
        if (c == '\n' && currentLineIsBlank) {
          //client.print("KO");
          //Serial.println("KO");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    //Serial.println("client disconnected");
    Ethernet.maintain();
  }
}
void handleSerial(){
  while (Serial.available() > 0) {
    //get mode
    char mode = Serial.read();
    //check how many values this mode expects
    int modeValues = getModeValues(mode);
    int values[modeValues];
    if(modeValues != NULL){
      //assign all values
      for(int i = 0;i < modeValues;i++){
        values[i] = Serial.parseInt();
      }
    }
    //read command delimiter
    char endChar = Serial.read();
    //run command
    writeEthernet = false;
    handleCommand(mode,values);
  }
}

//returns the number of values each mode expects
int getModeValues(char mode){
  //digital and analog write is 2
  if(mode == 'd' || mode == 'a'){
    return 2;
  }else 
  //digital and analog read is 1
  if(mode == 'r' || mode == 'e'){
    return 1;
  }else
  //motor is 7
  if(mode == 'm'){
    return 7;
  }
  return NULL;
}
void handleCommand(char mode, int values[]){
  if(mode == 'd'){
    handleDigitalWrite(values);
  }else if(mode == 'a'){
    handleAnalogWrite(values);
  }else if(mode == 'r'){
    handleReadDigital(values);
  }else if(mode == 'e'){
    handleReadAnalog(values);
  }else if(mode == 'm'){
    handleStepper(values);
  }
}
void handleDigitalWrite(int values[]){
  int pin = values[0];
  int on = values[1];
  changePinMode (pin, OUTPUT);
  //Serial.println("digital");
  /*Serial.println(pin);
  Serial.println(on);*/
  //se for 1, mete HIGH, senão mete LOW
  if(on == 1){
    digitalWrite(pin, HIGH);
  }else{
    digitalWrite(pin, LOW);
  }
}
void handleAnalogWrite(int values[]){
  int pin = values[0];
  int value = values[1];
  changePinMode (pin, OUTPUT);
  //Serial.print("analog");
  analogWrite (pin, value);
}
void handleReadDigital(int values[]){
  int pin = values[0];
  changePinMode(pin, INPUT_PULLUP);
  if(!writeEthernet){
    Serial.print(digitalRead(pin));
    Serial.print(","); 
  }else{
    client.print(digitalRead(pin));
    client.print(","); 
  }
}
void handleReadAnalog(int values[]){
  int pin = values[0];
  changePinMode(pin, INPUT_PULLUP);
  if(!writeEthernet){
    Serial.print(analogRead(pin));
    Serial.print(","); 
  }else{
    client.print(analogRead(pin));
    client.print(","); 
  }
}
void handleStepper(int values[]){
  int pin1 = values[0];
  int pin2 = values[1];
  int pin3 = values[2];
  int pin4 = values[3];
  int motorSteps = values[4];
  int motorSpeed = values[5];
  int stepsToTake = values[6];
  changePinMode(pin1, OUTPUT);
  changePinMode(pin2, OUTPUT);
  changePinMode(pin3, OUTPUT);
  changePinMode(pin4, OUTPUT);
  Stepper motor(motorSteps, pin1, pin2, pin3, pin4);      
  motor.setSpeed(motorSpeed);
  motor.step(stepsToTake);
}
//Changes pin mode (INPUT to OUTPUT or vice-versa)
void changePinMode(int pin, byte mode){
   if(lastPinMode[pin-2] != mode){
        //Serial.print("Changing mode");
        pinMode (pin, mode);
        //Delay exists because when you change from OUTPUT to INPUT and you read its value right away you get an old value. With the delay the read value is correct
        delay(100);
        lastPinMode[pin-2] = mode;
    }
}

//Determins if an input pin has changed
byte hasChanged(int i){
  bool shouldRead = shouldReads[i];
  if(shouldRead){
    unsigned long currentTime = millis();
    if(currentTime - lastChanges[i] > 16){
      shouldReads[i] = false;
      lastChanges[i] = currentTime;
      int currentValue = digitalRead(interruptPins[i]);
      if(currentValue != lastValues[i]){
        lastValues[i] = currentValue;
        return currentValue + 1;
      }
    }
  }
  return NULL;
}


