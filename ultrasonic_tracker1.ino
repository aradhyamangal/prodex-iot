#include <Servo.h>

const int SENSOR_PIN = 7;        // 3-pin ultrasonic, SIG wire
const int LED_PIN = 13;
const int MAX_RANGE = 150;       // cm, ignore anything farther
const int DEADBAND = 5;          // degrees, no movement inside this
const unsigned long RECHECK = 2000;

const int PAN = 0;
const int TILT = 1;

Servo servos[2];
int angleNow[2] = {90, 90};
bool locked = false;
unsigned long lastCheck = 0;

int readCm() {
  pinMode(SENSOR_PIN, OUTPUT);
  digitalWrite(SENSOR_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(SENSOR_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(SENSOR_PIN, LOW);
  pinMode(SENSOR_PIN, INPUT);
  long t = pulseIn(SENSOR_PIN, HIGH, 25000);
  if (t == 0) return 999;        // no echo = too far
  return (int)(t * 0.034 / 2);
}

int medianDist() {               // 3 reads, take middle
  int a = readCm();
  int b = readCm();
  int c = readCm();
  int tmp;
  if (a > b) { tmp = a; a = b; b = tmp; }
  if (b > c) { tmp = b; b = c; c = tmp; }
  if (a > b) { b = a; }
  return b;
}

void moveTo(int axis, int target) {   // 1 degree at a time
  if (target < 0) target = 0;
  if (target > 180) target = 180;
  while (angleNow[axis] != target) {
    if (target > angleNow[axis]) angleNow[axis]++;
    else angleNow[axis]--;
    servos[axis].write(angleNow[axis]);
    delay(10);
  }
}

// sweep one axis, return angle where object was closest (-1 if none)
int scanAxis(int axis, int from, int to, int step) {
  int best = -1;
  int bestD = 999;
  if (from < 0) from = 0;
  if (to > 180) to = 180;
  moveTo(axis, from);
  delay(100);
  for (int a = from; a <= to; a += step) {
    moveTo(axis, a);
    delay(40);
    int d = medianDist();
    if (d <= MAX_RANGE && d < bestD) {
      bestD = d;
      best = a;
    }
  }
  return best;
}

void parkPanel() {
  moveTo(PAN, 90);
  moveTo(TILT, 90);
  digitalWrite(LED_PIN, LOW);
  locked = false;
  Serial.println("No source found - parked");
}

void acquire() {
  Serial.println("Scanning...");
  moveTo(TILT, 90);
  int p = scanAxis(PAN, 0, 180, 5);
  if (p < 0) {
    parkPanel();
    delay(1000);
    return;
  }
  moveTo(PAN, p);
  int t = scanAxis(TILT, 30, 150, 5);
  if (t < 0) {
    parkPanel();
    delay(1000);
    return;
  }
  moveTo(TILT, t);
  locked = true;
  lastCheck = millis();
  digitalWrite(LED_PIN, HIGH);
  Serial.print("LOCKED pan=");
  Serial.print(angleNow[PAN]);
  Serial.print(" tilt=");
  Serial.println(angleNow[TILT]);
}

void track() {
  int oldP = angleNow[PAN];
  int oldT = angleNow[TILT];

  int p = scanAxis(PAN, oldP - 20, oldP + 20, 2);
  if (p < 0) { parkPanel(); return; }
  if (abs(p - oldP) > DEADBAND) moveTo(PAN, p);
  else moveTo(PAN, oldP);        // small change, stay put

  int t = scanAxis(TILT, oldT - 20, oldT + 20, 2);
  if (t < 0) { parkPanel(); return; }
  if (abs(t - oldT) > DEADBAND) moveTo(TILT, t);
  else moveTo(TILT, oldT);

  Serial.print("Tracking pan=");
  Serial.print(angleNow[PAN]);
  Serial.print(" tilt=");
  Serial.println(angleNow[TILT]);
  lastCheck = millis();
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  servos[PAN].attach(9);
  servos[TILT].attach(10);
  servos[PAN].write(90);
  servos[TILT].write(90);
  delay(500);
}

void loop() {
  if (!locked) {
    acquire();
  } else if (millis() - lastCheck > RECHECK) {
    track();
  }
}
