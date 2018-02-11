/*
Metrics Server

 A simple metrics server comaptible with Prometheus,
 using an Arduino Wiznet Ethernet shield.

 See https://prometheus.io/

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)

 Based on Arduino Ethernet Shield server example by:
 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 modified 02 Sept 2015
 by Arturo Guadalupi

 Adapted to enable monitoring an alarm system with Prometheus from Arduino HTTP Server example
 2018-02-10
 Ed Simmons

 */

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(10, 100, 0, 252);


uint8_t batteryPin = 0; // analog 0, voltage divider 10k - 4.7k giving 4.796v output with a 15v input
float batteryScaling = 0.01465;
uint8_t faultPin = 6;
uint8_t alarmPin = 7;

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  pinMode(alarmPin, INPUT);
  pinMode(faultPin, INPUT);

  digitalWrite(alarmPin, HIGH);
  digitalWrite(faultPin, HIGH);

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close"); // the connection will be closed after completion of the response
          client.println();

          client.println("# HELP alarm_battery_voltage Last sampled voltage of alarm battery");
          client.println("# TYPE alarm_battery_voltage guage");
          client.print("alarm_battery_voltage ");
          client.println(batteryScaling * analogRead(batteryPin));

          client.println("# HELP alarm_alarm_relay_output Last sampled state of alarm relay output");
          client.println("# TYPE alarm_alarm_relay_output guage");
          client.print("alarm_alarm_relay_output ");
          client.println(digitalRead(alarmPin));

          client.println("# HELP alarm_fault_relay_output Last sampled state of fault relay output");
          client.println("# TYPE alarm_fault_relay_output guage");
          client.print("alarm_fault_relay_output ");
          client.println(digitalRead(faultPin));

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
    client.flush();
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

