#define buzzer 12
const int alertLed=13;
char infoIn[]=new char[32];
int jobSwitch=0;
int stripCount=1;
int selectedStrip=0;
int color=0;
//power pin,R pin,G pin,B pin
int stripPins[1][4]={{2,3,5,6}};
int stripVals[1][4]={{0,0,0}};

boolean buzz=true;

void setup() {
 pinMode(alertLed,OUTPUT);
 Serial.begin(9600);
 Serial.println("READY");
 Wire.begin(1);//Start I2C as a slave w/ address 1
 Wire.onReceive(i2cIn);
 Wire.onRequest(i2cOut);
 //Pin delcarations
 for(int i=0;i<1;i++){
  for(int j=0;j<4;j++){
    pinMode(stripPins[i][j],OUTPUT);
  }
 }
}

void loop() {
 parseInput();
}

void i2cIn(int howMany){
  while(Wire.available()>1){
    infoIn=Wire.read;
  }
}

void i2cOut(){
  
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
  if(strcmp(infoIn,"SET_STRIP")==0){
    Serial.println("STR");
    jobSwitch=1;
  }else if(strcmp(infoIn,"SET_POWER")==0){
    Serial.println("POW");
    jobSwitch=2;
  }else if(strcmp(infoIn,"SET_COLOR")==0){
    Serial.println("COL");
    jobSwitch=3;
  }else if(strcmp(infoIn,"SET_BRIGHT")==0){
    Serial.println("BRI");
    jobSwitch=4;
  }else if(strcmp(infoIn,"SET_SOUND")==0){
    Serial.println("SPK");
    jobSwitch=5;
  }else if(strcmp(infoInChars,"PING")==0){
    Serial.println("PNG");
    if(buzz)alert(300,100,true);
  }else{
    Serial.println("ERR0");
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

