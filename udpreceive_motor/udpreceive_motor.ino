#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include <Servo.h>

// MAC address and IP address
byte mac[] = {0x90, 0xA2, 0xDA, 0x10, 0xC7, 0x6A};
IPAddress ip(61,239,96,94);

unsigned int localPort = 1421;

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet
char ReplyBuffer[] = "acknowledged";        // string to send back

// EthernetUDP instance to send and receive packets over UDP
EthernetUDP Udp;

// Servos (pins and servo objects)
//const int TIME_MOVE = ;    // time (in sec) to travel 1 cm
//const int TIME_TURN = ;    // time (in sec) to turn 1 degree
byte servoLeftPin = 9;
Servo servoLeft;
byte servoRightPin = ;      // TODO: put pin numbers in
Servo servoRight;
byte servoUpPin = ;
Servo servoUp;
byte servoDownPin = ;
Servo servoDown;

// put your setup code here, to run once:
void setup() {
  // open Serial communications
  Serial.begin(9600);
//  while (!Serial) {
//    ; // wait for serial port to connect (only for native USB port)
//  }

  // start Ethernet connection (listens for incoming connections on specified port)
  Ethernet.begin(mac);
  Udp.begin(localPort);
  
  Serial.print("IP: ");
  Serial.println(Ethernet.localIP());

  // set up Servo
  servoLeft.attach(servoLeftPin);
  servoLeft.writeMicroseconds(1500); // send stop signal to ESC
  servoRight.attach(servoRightPin);
  servoRight.writeMicroseconds(1500);  // send stop signal to ESC
  servoUp.attach(servoUpPin);
  servoUp.writeMicroseconds(1500);    // send stop signal to ESC
  servoDown.attach(servoDownPin);
  servoDown.writeMicroseconds(1500);  // send stop signal to ESC
  delay(1000);    // delay to allow the ESC to recognize
}

void loop() {
  /* READ IN PACKET */
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.print("Receive packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remote = Udp.remoteIP();
    for (int i = 0; i < 4; i++) {
      Serial.print(remote[i], DEC);
      if (i < 3) {
        Serial.print(".");
      }
    }
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBuffer
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    Serial.println("Contents:");
    Serial.println(packetBuffer);
    
    // extract input (motors)
    int[] servoSign = new int[3];   // sign
    int[] servoMag = new int[3];    // magnitude
    int numOfDigits = 4;            // number of digits in input
    int divideBy = pow(10, numOfDigits);
    for (int i = 0; i < 3; i++) {
      int input = packetBuffer % divideBy;
      servoMag[i] = input % (divideBy/10);
      input /= 10;
      servoSign[i] = input;
      
      packetBuffer /= divideBy;
    }

    // extract input (claw and laser)
    boolean[] components = new boolean[2];
    for (int i = 0; i < 2; i++) {
      if (packetBuffer % 10 == 0) {
        components[i] == false;
      }
      else {
        components[i] == true;
      }
      packetBuffer /= divideBy;
    }

    // TODO: turn on/off laser (depending on boolean)
    // TODO: open/close claw (depending on boolean)

    /* WORKING WITH MOTOR INPUT (distance to travel)
      9 digits -- 4 digits each of x-, y-, z-axes
      1st digit: sign (0-negative, 1-positive)
      2nd-4th digits: magnitude (in centimeters) */

    // send information to servos
    // x distance (0-left, 1-right) + y distance (0-front, 1-back)
    // calculate shortest distance (angle and magnitude)
    if (servoMag[0] == 0) {   // no movement in x-axis
      if (servoSign[0] == 0) {
        servoLeft.write(0);
        servoRight.write(0);
      }
      else {    // servoSign[0] == 1
        servoLeft.write(180);
        servoRight.write(180);
      }
//      delay(servoMag[0] * TIME_MOVE);
    }
    else {
      int angle = atan(servoMag[0], servoMag[1]);
      int mag = sqrt(pow(servoMag[0],2) + pow(servoMag[1],2));
      
      // both positive -- turn left
      if (servoSign[0] == 1 && servoSign[1] == 1) {
        servoLeft.write(180);
        servoRight.write(0);
      }
      // +ve y-axis, -ve x-axis -- turn right
      else if (servoSign[0] == 0 && servoSign[1] == 1) {
        servoLeft.write(0);
        servoRight.write(180);
      }
      // -ve y-axis, +ve x-xis -- turn left (+degrees)
      else if (servoSign[0] == 1 && servoSign[1] == 0) {
        servoLeft.write(180);
        servoRight.write(0);
        angle = 180 - angle;
      }
      // both negative -- turn right (+degrees)
      else if (servoSign[0] == 0 && servoSign[1] == 0){
        servoLeft.write(0);
        servoRight.write(180);
        angle = 180 - angle;
      }

//      delay(angle * TIME_TURN);
    }
    
    // z distance (0-up, 1-down)
    if (servoSign[2] == 0) {
      servoUp.write(0);
      servoDown.write(0);
    }
    else {    // servoSign[2] == 1
      servoUp.write(180);
      servoDown.write(180);
    }
//    delay(servoMag[2]) * TIME_MOVE);

    // stop movement of left/right motors
    servoLeft.write(90);
    servoRight.write(90);

    // stop movement of up/down motors
    servoUp.write(90);
    servoDown.write(90);

    // send a reply to the IP address and port that sent packet
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();
  }
  delay(10);
}

//  //close connection
//  client.stop();
//  Serial.println("client disconnected");
