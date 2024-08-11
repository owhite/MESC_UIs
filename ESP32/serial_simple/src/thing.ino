
#define compSerial Serial
#define TX 0
#define RX 1
HardwareSerial mescSerial(1); 

void setup() {
  compSerial.begin(115200);
  mescSerial.begin(115200, SERIAL_8N1, RX, TX); 

  compSerial.println("Setup complete");
}

void loop2() {
  char ch;

  delay(400);
}

void loop() {
  char ch;

  while (compSerial.available()) {
    ch = compSerial.read();
    if (ch == 'g') {
      mescSerial.write("get\r\n");
    }
    compSerial.write(ch);
  }

  while (mescSerial.available()) {
    ch = mescSerial.read();
    compSerial.write(ch);
  }
}
