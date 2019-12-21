#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Servo.h>


const char* ssid     = "Girls Gone Wireless";
const char* password = "twojastala";
int ind=1;
int lastId=0;
int lastTime=millis();
WiFiServer server(80);

// Variable to store the HTTP request
String header;
HTTPClient http;  
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

// sterowanie


Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
Servo lb3;
Servo rb3;

#define RF0 0
#define RF1 1
#define RF2 2
#define RM0 3
#define RM1 4
#define RM2 5
#define RB0 6
#define RB1 7
#define RB2 8
#define LF0 9
#define LF1 10
#define LF2 11
#define LM0 12
#define LM1 13
#define LM2 14
#define LB0 15
#define LB1 16
#define LB2 17

//  int SERVOMIN[18] ={160,158,135,320,195,150,150,190,10 ,170,135,210,245,190,175,150,170,12};
//  int SERVOMAX[18] ={600,555,510,500,595,530,580,600,170,610,510,590,482,580,570,580,570,170};
  int SERVOMIN[18] ={180,165,135,315,205,150,150,230,10 ,200,165,210,280,220,165,165,210,12};
  int SERVOMAX[18] ={630,585,570,565,630,580,620,640,170,660,580,660,530,620,640,615,630,170};


  int frontDel=120;
  int steppDel=75;
  int kalDel =120;


  int indeks=0,przycisk=0;
  String order="";
  bool awake=false;
  bool firstTime;
  bool firstRotate=true;
  bool startStepp=false,startL=false,startR=false,startForwards=false,startBackwards=false,startSteppBack=false;
  bool previosState=false;
  bool actualState=false;
  bool backFirstStep=true,frontFirstStep=true,firstStepp=true,firstSteppBack=true;
 






void setup() {
  firstTime=true;
  bool success=false;
  pinMode(15,INPUT);
  
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(50);

  // Hostname defaults to esp8266-[ChipID]
   ArduinoOTA.setHostname("edgar");

  // No authentication by default
   ArduinoOTA.setPassword("Asdasd123");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
 // ArduinoOTA.setPasswordHash("asdasd123");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  logIn();
  pwm.begin();  
  pwm.setPWMFreq(60);
  delay(10);  
}

void loop() {

  ArduinoOTA.handle();
  String order="";
  int actualTime=millis();
  
if(actualTime-lastTime>400)
  {  
  order=getCommands();
  lastTime=actualTime;
//  Serial.print("id:");
//  Serial.println(lastId);
//  Serial.println("komenda: "+komenda);
  }

        if(order=="w")
        {
          wakeWake();
        }
        else if(order=="s")
        {
          sleepSleep();
        }
        else if(order=="b1"){
          balance1();
        }
        else if(order=="b2"){
          balance2();
        }
        else if(order=="s1"){ 
         frontStep();
        }
        else if(order=="s2"){
          backStep();
        }
        else if(order=="ss"){
           stepp();   
        }
        else if(order=="sb"){
           steppBack();   
        }        
        else if(order=="k2"){
          kalibracjaL();
        }
        else if(order=="k1"){
          kalibracjaR();
        }
        else if(order=="wa"){
          wave();
        } 

}







String getCommands()
{
  bool startKomenda=false;
  bool startId=false;  
  String komenda="";
  String id="";
  String postData= "lastId="+String(lastId)+"&pass=pajakkomenda";
  http.begin("http://student.agh.edu.pl/~miera/edgarGetCommands.php");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded"); 
  int httpCode = http.POST(postData); 
  String payload = http.getString(); 
    Serial.println(); 
  Serial.println();  
  Serial.println("payload:"+payload);   
  http.end();
  
  if(payload[0]=='k'){
      for(int i=0;i<payload.length();i++){

        if(payload[i]=='k'){
          startId=true;
          i++;
         } 

         if(payload[i]=='@'){
          startKomenda=true;
          startId=false;
          i++;
         }

         if(payload[i]=='%'){
          payload="";
          break;
         }
         if(startKomenda){
          komenda+=payload[i];
         }
         if(startId){
          id+=payload[i];
         }
         }
  lastId=id.toInt();


  return komenda;
}
  else{
  Serial.println("wracam");  
  return "";
  }

}



void logIn(){
  Serial.println("loguje");

  String postData=  "login=edgar@pajak.com&haslo=robopajak";
  http.begin("http://student.agh.edu.pl/~miera/logowanie.php");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded"); 
  int httpCode = http.POST(postData); 
  String payload = http.getString();  
  Serial.println(payload); 
  lastId=payload.toInt(); 
  http.end();      
}

void logOut(){
    Serial.println("elo320");  
    String postData= "lastId="+String(lastId)+"&zdarzenie=pajaknara";
    http.begin("http://student.agh.edu.pl/~miera/edgarLogout.php");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded"); 
    int httpCode = http.POST(postData); 
    String payload = http.getString(); 
    Serial.println(httpCode);   
    Serial.println(payload);   
    http.end();   
}




void setServo(int nr, int pos){  
  if(pos>180){
    pos=180;
  }
  if(pos<0){
  pos=0;
  }
  
  if(nr==8)
  {
    pos=map(pos,180,0,SERVOMIN[nr], SERVOMAX[nr]);   
    lb3.attach(12);
    lb3.write(pos);
  }
  else if(nr==17){
    pos=map(pos,0,180,SERVOMIN[nr], SERVOMAX[nr]);
    rb3.attach(14);
    rb3.write(pos);    
  }
 else { 
  if(nr<8){
    if(nr==2||nr==5||nr==7){
      pos=map(pos,0,180,SERVOMIN[nr], SERVOMAX[nr]);
    }
    else{
    pos=map(pos,180,0,SERVOMIN[nr], SERVOMAX[nr]);
    }
  }
  else{
    if(nr==11||nr==14||nr==16){
       pos=map(pos,180,0,SERVOMIN[nr], SERVOMAX[nr]);
    }
    else{
    pos=map(pos,0,180,SERVOMIN[nr], SERVOMAX[nr]);
  }
  }
   if(nr<17&&nr>8){
      nr--;
     }
     
    pwm.setPWM(nr, 0, pos);
  }
}

void setServoPulse(uint8_t n, double pulse) {
  double pulselength;
  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= 60;   // 60 Hz
  pulselength /= 4096;  // 12 bits of resolution
  pulse *= 1000000;  // convert to us
  pulse /= pulselength;
  pwm.setPWM(n, 0, pulse);
}

void sleepSleep(){

Serial.println("Sleep");  
if(awake||firstTime){
  pwm.wakeup();
  awake=false;
  firstTime=false;
  for(int i=1; i<18;i=i+3){  //serwa1
  setServo(i,60);
  }
  setServo(RF1,80);
  setServo(RB1,80);
  setServo(RM1,80);
  setServo(LF1,80);
  setServo(LM1,80);
  setServo(LB1,80);
  delay(500);
  for(int j=80; j<111;j+=5){    
  setServo(RF1,j);
  setServo(RB1,j);
  setServo(RM1,j);
  setServo(LF1,j);
  setServo(LM1,j);
  setServo(LB1,j);
  delay(40);
  }
  for(int j=110; j<181;j+=5){    
  setServo(RF1,180);
  setServo(RB1,180);
  setServo(RM1,180);
  setServo(LF1,180);
  setServo(LM1,180);
  setServo(LB1,180);
  delay(1);
  }
 
  setServo(RF0,120);        //serwa 0
  setServo(LF0,120); 
  delay(200); 
  setServo(RM0,90);
  setServo(LM0,90); 
  delay(200); 
  setServo(RB0,60);
  setServo(LB0,60);
  
    for(int i=11; i<18;i=i+3){ //serwa 2
  setServo(i,0);
  setServo(i-9,0);
  delay(200);
 }
 pwm.sleep();
 lb3.detach();
 rb3.detach();
}
}
void wakeWake(){
  awake=true;
  backFirstStep=true;
  pwm.wakeup();
  Serial.println("Wake");  
  
  setServo(RF0,120);
  setServo(LF0,120); 
  delay(200); 
  setServo(RM0,90);
  setServo(LM0,90); 
  delay(200); 
  setServo(RB0,60);
  setServo(LB0,60);

  for(int i=11; i<18;i=i+3){      //serwa 2
  setServo(i,80);
  setServo(i-9,80);
  delay(200);
 }
   for(int i=1; i<18;i=i+3){      //serwa 1
  setServo(i,110);
   }
  delay(250);

  for(int i=1; i<18;i=i+3){
  setServo(i,60);
   } 
}

void balance1(){
  Serial.println("Balance 1");  
if(awake){
  setServo(RF1,90);
  setServo(RB1,90);
  setServo(LM1,90);
  delay(500);
  setServo(RF2,180);
  setServo(RB2,180);
  setServo(LM2,180);
delay(1000);
  
  for(int j=0; j<91;j+=5){    
  setServo(RF1,j+90);
  setServo(RF2,180-2*j);
  setServo(RB1,j+90);
  setServo(RB2,180-2*j);
  setServo(LM1,j+90);
  setServo(LM2,180-2*j);
  delay(10);
  }
  for(int j=0; j<91;j+=5){    
  setServo(RF1,180-j);
  setServo(RF2,2*j);
  setServo(RB1,180-j);
  setServo(RB2,2*j);
  setServo(LM1,180-j);
  setServo(LM2,2*j);
  delay(10);
  }
  for(int j=0; j<91;j+=5){    
  setServo(RF1,j+90);
  setServo(RF2,180-2*j);
  setServo(RB1,j+90);
  setServo(RB2,180-2*j);
  setServo(LM1,j+90);
  setServo(LM2,180-2*j);
  delay(10);
  }
  for(int j=0; j<91;j+=5){    
  setServo(RF1,180-j);
  setServo(RF2,2*j);
  setServo(RB1,180-j);
  setServo(RB2,2*j);
  setServo(LM1,180-j);
  setServo(LM2,2*j);
  delay(10);
  }
  delay(1000);
  setServo(RF2,80);
  setServo(RB2,80);
  setServo(LM2,80);
  delay(500);
  setServo(RF1,60);
  setServo(RB1,60);
  setServo(LM1,60); 
} 
}


void balance2(){
  Serial.println("Balance 2");  
if(awake){
  setServo(LF1,90);
  setServo(LB1,90);
  setServo(RM1,90);
  delay(500);
  setServo(LF2,180);
  setServo(LB2,180);
  setServo(RM2,180);
delay(1000);
  
  for(int j=0; j<91;j+=5){    
  setServo(LF1,j+90);
  setServo(LF2,180-2*j);
  setServo(LB1,j+90);
  setServo(LB2,180-2*j);
  setServo(RM1,j+90);
  setServo(RM2,180-2*j);
  delay(10);
  }
  for(int j=0; j<91;j+=5){    
  setServo(LF1,180-j);
  setServo(LF2,2*j);
  setServo(LB1,180-j);
  setServo(LB2,2*j);
  setServo(RM1,180-j);
  setServo(RM2,2*j);
  delay(10);
  }
  for(int j=0; j<91;j+=5){    
  setServo(LF1,j+90);
  setServo(LF2,180-2*j);
  setServo(LB1,j+90);
  setServo(LB2,180-2*j);
  setServo(RM1,j+90);
  setServo(RM2,180-2*j);
  delay(10);
  }
  for(int j=0; j<91;j+=5){    
  setServo(LF1,180-j);
  setServo(LF2,2*j);
  setServo(LB1,180-j);
  setServo(LB2,2*j);
  setServo(RM1,180-j);
  setServo(RM2,2*j);
  delay(10);
  }
  delay(1000);
  setServo(LF2,80);
  setServo(LB2,80);
  setServo(RM2,80);
  delay(500);
  setServo(LF1,60);
  setServo(LB1,60);
  setServo(RM1,60); 
} 
}

  void wave(){
    Serial.println("Wave");
  if(awake){
  setServo(RF1,160); 
  setServo(RF2,140); 
  delay(500);
  setServo(RF0,130); 
  delay(500);
  setServo(RF0,50); 
  delay(500);
  setServo(RF0,130); 
  delay(500);
  setServo(RF0,50); 
  delay(500);
  setServo(RF0,120); 
  delay(400);
  setServo(RF2,80);
  delay(400); 
  setServo(RF1,60); 
  }
  }

void stepp(){
Serial.println("stepp");        //80 ms   spoko
if(firstStepp){
firstStepp=false;

  setServo(LF1,70);
  setServo(RB1,70);
  setServo(LF0,90);
  setServo(RB0,40);
  delay(150);
  setServo(LF1,60);
  setServo(RB1,60);
  delay(150);
  setServo(LM1,70);
  setServo(RM1,70);
  setServo(LM0,50);
  setServo(RM0,50);
  delay(steppDel);
}
  

  setServo(RF1,70);
  setServo(RF0,90);

  delay(steppDel);
  setServo(LF0,140);
  setServo(RB0,90);
  delay(steppDel);
  setServo(LB1,70);
  setServo(LB0,40);
  delay(steppDel);
  setServo(LF1,70);
  setServo(LF0,90); 
  setServo(LM1,60);
  setServo(RM1,60); 
 
  delay(steppDel);
  setServo(LM0,150);
  setServo(RM0,150); 
  delay(steppDel);
  setServo(RB1,70);
  setServo(RB0,40);
  delay(steppDel);
  setServo(RF1,60);
  setServo(LB1,60);
  setServo(LM1,70); 
  setServo(LM0,50);
  delay(steppDel);
  setServo(RF0,140);
  setServo(LB0,90);  
  delay(steppDel);
  setServo(RM1,70);    
  setServo(RM0,50);  
  delay(steppDel);
  setServo(LF1,60);
  setServo(RB1,60);
  delay(steppDel);  
}


void steppBack(){
Serial.println("stepp back");        //80 ms   spoko
if(firstStepp){
firstSteppBack=false;

  setServo(RF1,70);
  setServo(LB1,70);
  setServo(RF0,140);
  setServo(LB0,90);
  delay(150);
  setServo(RF1,60);                                          //f90-140       b 40-90    150-50    
  setServo(LB1,60);
  delay(150);
  setServo(RM1,70);
  setServo(LM1,70);
  setServo(RM0,150);
  setServo(LM0,150);
  delay(steppDel);
}
  

  setServo(LF1,70);
  setServo(LF0,140);

  delay(steppDel);
  setServo(RF0,90);
  setServo(RB0,40);
  delay(steppDel);
  setServo(RB1,70);
  setServo(RB0,90);
  delay(steppDel);
  setServo(RF1,70);
  setServo(RF0,140); 
  setServo(RM1,60);
  setServo(LM1,60); 
 
  delay(steppDel);
  setServo(RM0,50);
  setServo(LM0,50); 
  delay(steppDel);
  setServo(LB1,70);
  setServo(LB0,90);
  delay(steppDel);
  setServo(LF1,60);
  setServo(RB1,60);
  setServo(RM1,70); 
  setServo(RM0,150);
  delay(steppDel);
  setServo(LF0,90);
  setServo(RB0,40);  
  delay(steppDel);
  setServo(LM1,70);    
  setServo(LM0,150);  
  delay(steppDel);
  setServo(RF1,60);
  setServo(LB1,60);
  delay(steppDel);  
}
  
void backStep(){
  Serial.println("back");
  frontFirstStep=true;
  if(backFirstStep){
    backFirstStep=false;
  setServo(RB1,80);
  setServo(RF1,80);
  setServo(LM1,80);  
  setServo(RB0,90);
  setServo(LM0,150);
  setServo(RF0,140);
  
  delay(frontDel);
  setServo(RB1,60);
  setServo(RF1,60);
  setServo(LM1,60);  
  delay(frontDel); 
  }
  
  setServo(LB1,80);
  setServo(RM1,80);  
  setServo(LF1,80);

  setServo(LB0,90);
  setServo(RM0,150); 
  setServo(LF0,140);
     
  delay(80);
  setServo(RB0,40);
  setServo(LM0,50);
  setServo(RF0,90);
  delay(frontDel);
  setServo(LB1,60);
  setServo(RM1,60); 
  setServo(LF1,60);
  
  delay(frontDel);
  setServo(RB1,80);
  setServo(RF1,80);
  setServo(LM1,80);

  setServo(RB0,90);
  setServo(LM0,150); 
  setServo(RF0,140);

  delay(frontDel);
  setServo(LB0,40);
  setServo(RM0,50);
  setServo(LF0,90);

  delay(frontDel);  
  setServo(RB1,60);
  setServo(RF1,60);
  setServo(LM1,60); 
  delay(frontDel); 
}


void frontStep(){
  Serial.println("front");
  backFirstStep=true;
  if(frontFirstStep){
    frontFirstStep=false;
  setServo(RB1,80);
  setServo(RF1,80);
  setServo(LM1,80);  
  setServo(RB0,40);
  setServo(LM0,50);
  setServo(RF0,90);
  
  delay(frontDel);
  setServo(RB1,60);
  setServo(RF1,60);
  setServo(LM1,60);  
  delay(frontDel); 
  }
  setServo(LB1,80);
  setServo(RM1,80);  
  setServo(LF1,80);

  setServo(LB0,40);
  setServo(RM0,50); 
  setServo(LF0,90);

   
  delay(frontDel);
  setServo(RB0,90);
  setServo(LM0,150);
  setServo(RF0,140);
  delay(frontDel);

  setServo(LB1,60);
  setServo(RM1,60); 
  setServo(LF1,60);
  
  delay(frontDel);
  setServo(RB1,80);
  setServo(RF1,80);
  setServo(LM1,80);

  setServo(RB0,40);
  setServo(LM0,50); 
  setServo(RF0,90);

  delay(frontDel);
  setServo(LB0,90);
  setServo(RM0,150);
  setServo(LF0,140);

  delay(frontDel);  
  setServo(RB1,60);
  setServo(RF1,60);
  setServo(LM1,60); 
  delay(frontDel); 
}

void kalibracjaL(){
  Serial.println("KL");
  setServo(LB1,80);
  setServo(RM1,80); 
  setServo(LF1,80);
  
  setServo(LB0,40);
  setServo(RM0,50); 
  setServo(LF0,90);
  delay(kalDel);
  setServo(RB0,90);
  setServo(RF0,140);
  delay(kalDel);

  setServo(LB1,60);
  setServo(RM1,60); 
  setServo(LF1,60);

  delay(kalDel);
  setServo(RB1,80);
  setServo(LM1,80);
  setServo(RF1,80);


  setServo(RB0,40);
  setServo(LM0,50); 
  setServo(RF0,90);
  
  delay(kalDel);  
  setServo(RB1,60);
  setServo(RF1,60);
  setServo(LM1,60); 
  delay(kalDel);  
}

void kalibracjaR(){
  Serial.println("KR");

  
  setServo(RB1,80);
  setServo(LM1,80);  
  setServo(RF1,80);
  
    setServo(RB0,40);
  setServo(LM0,50); 
  setServo(RF0,90);

  delay(kalDel);
  setServo(LB0,90);
  setServo(LF0,140);
  delay(kalDel);

  setServo(RB1,60);
  setServo(LM1,60); 
  setServo(RF1,60);

  delay(kalDel);
  setServo(LB1,80);
  setServo(RM1,80);
  setServo(LF1,80);


  setServo(LB0,40);
  setServo(RM0,50); 
  setServo(LF0,90);
  
  delay(kalDel);  
  setServo(LB1,60);
  setServo(RM1,60); 
  setServo(LF1,60);
  delay(kalDel);  
}

 



  
void step1long(){
  if(backFirstStep){
    backFirstStep=false;
  setServo(RB1,80);
  setServo(RF1,80);
  setServo(LM1,80);  
  setServo(RB0,30);
  setServo(RF0,90);
  setServo(LM0,50);  
  delay(200);
  setServo(RB1,60);
  setServo(RF1,60);
  setServo(LM1,60);  
  delay(200); 
  }
  setServo(LB1,80);
  setServo(RM1,80);   
  setServo(LF1,80); 

  setServo(LB0,40);
  setServo(RM0,50); 
  setServo(LF0,90);

  delay(150);
  setServo(RB0,90);
  setServo(LM0,150);
  setServo(RF0,140);
  setServo(RB1,45);
  setServo(LM1,45);
  setServo(RF1,45);
  setServo(RB2,112);
  setServo(LM2,112);
  setServo(RF2,112); 
  delay(180);
 
  setServo(LB1,60);
  setServo(RM1,60); 
  setServo(LF1,60);
  delay(250);
  setServo(RB1,80);
  setServo(RF1,80);
  setServo(LM1,80);
  delay(50);
  setServo(RB2,80);
  setServo(LM2,80);
  setServo(RF2,80); 

  setServo(RB0,40);
  setServo(LM0,50); 
  setServo(RF0,90);

  delay(150);
  setServo(LB0,90);
  setServo(RM0,150);
  setServo(LF0,140);
  setServo(LB1,45);
  setServo(RM1,45);
  setServo(LF1,45);
  setServo(LB2,112);
  setServo(RM2,112);
  setServo(LF2,112);
 
  delay(180);  
  setServo(RB1,60);
  setServo(LM1,60); 
  setServo(RF1,60);
  delay(250);
  setServo(LB1,80);
  setServo(RM1,80);
  setServo(LF1,80);
  delay(50);
  setServo(LB2,80);
  setServo(RM2,80);
  setServo(LF2,80);
  delay(200); 
      
}

void rotateL(){
  if(firstRotate)
  {
    firstRotate=false;
  setServo(RF1,80);
  setServo(LB1,80);
  setServo(RF0,90);
  setServo(LB0,90);
  delay(200);
  setServo(RF1,60);
  setServo(LB1,60);  
  delay(200);
  setServo(LF1,80);
  setServo(RB1,80);  
  setServo(LF0,90);
  setServo(RB0,90);
  delay(200);
  setServo(LF1,60);
  setServo(RB1,60);  
  delay(200);
  }


  
}

void step1test(){
  if(backFirstStep){
    backFirstStep=false;
  setServo(RB1,80);
  setServo(RF1,80);
  setServo(LM1,80);  
  setServo(RB0,30);
  setServo(RF0,90);
  setServo(LM0,50);  
  delay(200);
  setServo(RB1,60);
  setServo(RF1,60);
  setServo(LM1,60);  
  delay(200); 
  }
  if(indeks==0){
    indeks++;
  setServo(LB1,80);
  setServo(RM1,80);   
  setServo(LF1,80); 
  }
   else if(indeks==1){
    indeks++;
  setServo(LB0,40);
  setServo(RM0,50); 
  setServo(LF0,90);
   }
      else if(indeks==2){
    indeks++; 
  delay(50);
  setServo(RB0,90);
  setServo(LM0,150);
  setServo(RF0,140);
  setServo(RB1,50);
  setServo(LM1,50);
  setServo(RF1,50);
  setServo(RB2,112);
  setServo(LM2,112);
  setServo(RF2,112); 
  delay(50);
  }
      else if(indeks==3){
    indeks++; 
  setServo(LB1,60);
  setServo(RM1,60); 
  setServo(LF1,60);
  }
      else if(indeks==4){
    indeks++; 
  setServo(RB1,80);
  setServo(RF1,80);
  setServo(LM1,80);
  delay(50);
  setServo(RB2,80);
  setServo(LM2,80);
  setServo(RF2,80); 
  
  }
      else if(indeks==5){
    indeks++; 

  setServo(RB0,40);
  setServo(LM0,50); 
  setServo(RF0,90);
  }
      else if(indeks==6){
    indeks++; 
  delay(50);
  setServo(LB0,90);
  setServo(RM0,150);
  setServo(LF0,140);
  setServo(LB1,50);
  setServo(RM1,50);
  setServo(LF1,50);
  setServo(LB2,112);
  setServo(RM2,112);
  setServo(LF2,112);
  }
      else if(indeks==7){
    indeks=0; 
  delay(50);  
  setServo(RB1,60);
  setServo(LM1,60); 
  setServo(RF1,60);
  setServo(LB1,80);
  setServo(RM1,80);
  setServo(LF1,80);
  delay(50);
  setServo(LB2,80);
  setServo(RM2,80);
  setServo(LF2,80);
  delay(200); 
      }
}
