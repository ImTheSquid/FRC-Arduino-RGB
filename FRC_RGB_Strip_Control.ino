#define buzzer 12
const int alertLed=13;
const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data
String input="";
int jobSwitch=0;
int stripCount=1;
int selectedStrip=0;
int color=0;
//power pin,R pin,G pin,B pin
int stripPins[1][4]={{2,3,5,6}};
int stripVals[1][4]={{0,0,0}};

boolean newData = false;
boolean serCon=false;
boolean buzz=true;

void setup() {
 pinMode(alertLed,OUTPUT);
 Serial.begin(9600);
 Serial.println("READY");
 //Pin delcarations
 for(int i=0;i<1;i++){
  for(int j=0;j<4;j++){
    pinMode(stripPins[i][j],OUTPUT);
  }
 }
}

void loop() {
 recvWithEndMarker();
 parseInput();
}

void recvWithEndMarker() {
 static byte ndx = 0;
 char endMarker = '\n';
 char rc;
 while (Serial.available() > 0 && newData == false) {
   rc = Serial.read();
   if (rc != endMarker) {
    receivedChars[ndx] = rc;
    ndx++;
    if (ndx >= numChars) {
      ndx = numChars - 1;
    }
   }
   else {
    receivedChars[ndx] = '\0'; // terminate the string
    ndx = 0;
    newData = true;
   }
 }
}

void parseInput(){
  if(newData){
    parseCommand();
    newData=false;
  }
}

void parseCommand(){
  if(jobSwitch>0){
    alert(350,100,true);
    parseIntInput();
    return;
  }
  if(strcmp(receivedChars,"SET_STRIP")==0){
    Serial.println("SET_STRIP->");
    jobSwitch=1;
  }else if(strcmp(receivedChars,"SET_POWER")==0){
    Serial.println("SET_POWER->");
    jobSwitch=2;
  }else if(strcmp(receivedChars,"SET_COLOR")==0){
    Serial.println("SET_COLOR->");
    jobSwitch=3;
  }else if(strcmp(receivedChars,"SET_BRIGHT")==0){
    Serial.println("SET_BRIGHT->");
    jobSwitch=4;
  }else if(strcmp(receivedChars,"SET_SOUND")==0){
    Serial.println("SET_SOUND->");
    jobSwitch=5;
  }else if(strcmp(receivedChars,"PING")==0){
    Serial.println("PONG");
    if(buzz)alert(300,100,true);
  }else{
    Serial.println("UNKNOWN");
    alert(200,100,true);
  }
  if(jobSwitch>0)alert(300,100,true);
}

void parseIntInput(){
  switch(jobSwitch){
    case 1: setLedStrip();
    break;
    case 2: setPowerState();
    break;
    case 3: setColor();
    break;
    case 4: setBrightness();
    break;
    case 5: setSpeaker();
    break;
  }
  Serial.println("READY");
  jobSwitch=0;
}

void setLedStrip(){
  int s=atoi(receivedChars);
  if(s>0||s<0){
    Serial.println("ERROR->OOB");
  }else{
    Serial.print("STRIP->");
    Serial.println(s);
    selectedStrip=s;
  }
}

void setPowerState(){
  int s=atoi(receivedChars);
  if(s<=0){
    digitalWrite(stripPins[selectedStrip][0],LOW);
    Serial.println("POWER->LOW");
  }else{
    digitalWrite(stripPins[selectedStrip][0],HIGH);
    Serial.println("POWER->HIGH");
  }
}

void setColor(){
  int c=atoi(receivedChars);
  if(c>2||c<0){
    Serial.println("ERROR->OOB");
    return;
  }
  color=c;
  Serial.print("COLOR->");
  switch(c){
    case 0: Serial.println("RED");
    break;
    case 1: Serial.println("GREEN");
    break;
    case 2: Serial.println("BLUE");
    break;
  }
}

void setBrightness(){
  int b=atoi(receivedChars);
  if(b<0||b>255){
    Serial.println("ERROR:OOB");
    return;
  }
  analogWrite(stripPins[selectedStrip][color+1],255-b);
  Serial.print("BRIGHT->");
  Serial.println(b);
}

void setSpeaker(){
  int s=atoi(receivedChars);
  Serial.print("SOUND->");
  if(s<=0){
    buzz=false;
    Serial.println("OFF");
  }else{
    buzz=true;
    Serial.println("ON");
  }
}

void alert(int pitch,int sLength,boolean useLED){
  if(useLED)digitalWrite(alertLed,HIGH);
  if(buzz)tone(buzzer,pitch,sLength);
  delay(sLength);
  digitalWrite(alertLed,LOW);
}

