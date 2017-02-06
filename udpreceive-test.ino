#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008

// MAC address and IP address
byte mac[] = {0x90, 0xA2, 0xDA, 0x10, 0xC7, 0x6A};
//IPAddress ip(61,239,96,94);

unsigned int localPort = 1421;

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet
char ReplyBuffer[] = "acknowledged";        // string to send back

// EthernetUDP instance to send and receive packets over UDP
EthernetUDP Udp;

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
}

void loop() {
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
