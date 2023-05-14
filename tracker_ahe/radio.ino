
// functions to control the DRA818 V modle

// Pin mapping is needed for the following pins
// PPT
// Low power
// Rx
// Tx
// wake A6(D20)

void init_radio() {

  //
  pinMode(radio_wake, OUTPUT);
  digitalWrite(radio_wake, LOW);  //sleep the radio at init

  // PPT  0: TX
  //      1: RX
  // board : D3 (external jumper)
  // set this pin to RX
  pinMode(radio_ppt, OUTPUT);
  digitalWrite(radio_ppt, LOW);

  pinMode(radio_pwr, OUTPUT);
  digitalWrite(radio_wake, LOW);
}

void radio_TX() {
  digitalWrite(radio_ppt, HIGH);
  Serial.print("T");
}

void radio_RX() {
  //pinMode(radio_ppt, OUTPUT);
  digitalWrite(radio_ppt, LOW);
    Serial.print("R");
}

void radio_on() {
  digitalWrite(radio_wake, HIGH);
}

void radio_off() {
  digitalWrite(radio_wake, LOW);
}

void set_radio_pwr(int pwr) {

  if (pwr)
    //low power
    pinMode(radio_pwr, INPUT);
  else {
    // high power
    pinMode(radio_pwr, OUTPUT);
    digitalWrite(radio_pwr, LOW);
  }
}
