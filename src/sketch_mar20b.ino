/*
  UDPSendReceive.pde:
 This sketch receives UDP message strings, prints them to the serial port
 and sends an "acknowledge" string back to the sender

 A Processing sketch is included at the end of file that can be used to send
 and received messages for testing with a computer.

 created 21 Aug 2010
 by Michael Margolis

 This code is in the public domain.
 */

#include <M5Stack.h>
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet2.h>
#include <EthernetUdp2.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#define SCK 18 //M5Stack用のSPIの設定4行。これがないとM5stackでEthernetが使えない（Arduinoは無しでOK）
#define MISO 19
#define MOSI 23
#define CS 26

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { //macアドレスは自分で設定してOK。ただし被ってはいけない
  0x84, 0x0D, 0x8E, 0x3D, 0x34, 0xC3  
};
IPAddress ip(192, 168, 1, 247);  //ipアドレスは自分で設定してOK。ただし被ってはいけない
unsigned int localPort = 8888;      // local port to listen on
IPAddress remoteip(192, 168, 1, 107);  //ipアドレスは自分で設定してOK。ただし被ってはいけない
unsigned int remotePort = 54321;      // local port to listen on
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,

EthernetUDP Udp; // An EthernetUDP instance to let us send and receive packets over UDP

unsigned long int timeCount;
char buf[6] = "0,0,0";
bool btnAFlag = false, btnBFlag = false, btnCFlag = false;

void spiSetup(){ //spiのセットアップ
  // start the Ethernet and UDP:
  SPI.begin(SCK, MISO, MOSI, -1); //M5Stack用のSPIの設定。これがないとM5stackでEthernetが使えない（Arduinoは無しでOK）
  Ethernet.init(CS); //M5Stack用のSPIの設定4行。これがないとM5stackでEthernetが使えない（Arduinoは無しでOK）
}

void udpSetup(){ //udpの初期セットアップ
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);
}

void m5Setup(){ //m5stackの初期セットアップ
  M5.begin(true, false, false);
    
  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(65, 10);
  M5.Lcd.println("Button example");
  M5.Lcd.setCursor(3, 35);
  M5.Lcd.println("Press button B 700ms clear");
  M5.Lcd.setTextColor(RED);
}

void udpSend(const char* sendChar){  //udpを送る関数
  Udp.beginPacket(remoteip, remotePort);
  Udp.write(sendChar);
  Udp.endPacket(); 
}

void getBtnStatus(int m, bool* flag){ //ボタンのステータスを検知
  if(!*flag) {
    buf[m] = '1';
    *flag = true;
  } else {
    buf[m] = '0';
    *flag = false;
  }
}

void printToMonitor(const char* bufChar){ //モニター出力系をまとめた関数
  for(int i=0; i<6; i++){
    M5.Lcd.print(bufChar[i]);
    Serial.println(bufChar[i]);
  }
  M5.Lcd.println();
}

void m5MonitorClear(){
  M5.Lcd.clear(BLACK);
  M5.Lcd.setCursor(0, 0);
}

void updateBtnStatus(int m, bool* flag, const char* bufChar){
  m5MonitorClear();
  getBtnStatus(m, flag);
  printToMonitor(bufChar);
}

void setup() {  
  spiSetup();
  udpSetup();

  Serial.begin(115200);
  Serial.println(Ethernet.localIP()); //IPアドレスがちゃんと割り当てられているいるあの確認

  m5Setup();
  timeCount = millis();
}

void loop() {
  M5.update();

  if (M5.BtnA.wasReleased()) {
    updateBtnStatus(0, &btnAFlag, buf);
  } 
  if (M5.BtnB.wasReleased()){
    updateBtnStatus(2, &btnBFlag, buf);
  } 
  if (M5.BtnC.wasReleased()) {
    updateBtnStatus(4, &btnCFlag, buf);
  }

  //udpに送る関数
  if(millis() >= timeCount + 100){
    timeCount = millis();
    Serial.println(timeCount);
    udpSend(buf);
  }

    
  // if there's data available, read a packet
//  int packetSize = Udp.parsePacket();
//  Serial.println(packetSize);
//    
//  if (packetSize)
//  {
//    Serial.print("Received packet of size ");
//    Serial.println(packetSize);
//    Serial.print("From ");
//    IPAddress remote = Udp.remoteIP();
//    for (int i = 0; i < 4; i++)
//    {
//      Serial.print(remote[i], DEC);
//      if (i < 3)
//      {
//        Serial.print(".");
//      }
//    }
//    Serial.print(", port ");
//    Serial.println(Udp.remotePort());
//
//    // read the packet into packetBufffer
//    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
//    Serial.println("Contents:");
//    Serial.println(packetBuffer);
//  }
}