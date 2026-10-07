/*
  Producer-Consumer Conveyor System (minimal hardware version)

  Hardware:
    - Arduino Uno
    - 2x IR obstacle sensor modules (entry + exit)
    - 1x DC motor (belt) + 1x motor driver / MOSFET module
    - Onboard LED (pin 13) used as the status indicator -> no extra parts

  Wiring:
    Entry IR sensor OUT -> D2   (VCC -> 5V, GND -> GND)
    Exit  IR sensor OUT -> D3   (VCC -> 5V, GND -> GND)
    Motor driver PWM/IN -> D9   (L298N: ENA or IN1 | MOSFET module: SIG)
    Motor power         -> separate battery / supply (share GND with Arduino)

  Producer-Consumer mapping:
    Producer  = person/machine placing items at the entry sensor
    Buffer    = belt, capacity BUFFER_SIZE items
    Consumer  = person/machine removing items at the exit sensor
    count     = the "semaphore" (items currently on the belt)

  Rules:
    - Belt only runs when count > 0                (prevents underflow)
    - If count == BUFFER_SIZE, new items rejected  (prevents overflow)
      -> LED blinks fast = "producer, WAIT"
    - Belt stops while an item sits at the exit sensor;
      it resumes once the consumer removes it.
*/

const uint8_t ENTRY_PIN = 2;
const uint8_t EXIT_PIN  = 3;
const uint8_t MOTOR_PIN = 9;
const uint8_t LED_PIN   = LED_BUILTIN;   // pin 13

const uint8_t BUFFER_SIZE = 3;           // max items on the belt
const uint8_t MOTOR_SPEED = 180;         // 0-255 PWM
const bool    SENSOR_ACTIVE_LOW = true;  // most IR modules output LOW on detection
const unsigned long DEBOUNCE_MS = 40;

uint8_t count = 0;

struct Sensor {
  uint8_t pin;
  bool stable;          // debounced state (true = item detected)
  bool lastRaw;
  unsigned long changedAt;
};

Sensor entrySensor = {ENTRY_PIN, false, false, 0};
Sensor exitSensor  = {EXIT_PIN,  false, false, 0};

// Returns true on the moment the debounced state CHANGES; new state in s.stable
bool updateSensor(Sensor &s) {
  bool raw = digitalRead(s.pin);
  if (SENSOR_ACTIVE_LOW) raw = !raw;
  if (raw != s.lastRaw) { s.lastRaw = raw; s.changedAt = millis(); }
  if (raw != s.stable && millis() - s.changedAt >= DEBOUNCE_MS) {
    s.stable = raw;
    return true;
  }
  return false;
}

void setup() {
  pinMode(ENTRY_PIN, INPUT);
  pinMode(EXIT_PIN, INPUT);
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  analogWrite(MOTOR_PIN, 0);
  Serial.begin(9600);
  Serial.println(F("Conveyor ready. Buffer empty."));
}

void loop() {
  // ---- PRODUCER side: item placed at entry ----
  if (updateSensor(entrySensor) && entrySensor.stable) {
    if (count < BUFFER_SIZE) {
      count++;
      Serial.print(F("PRODUCE -> items on belt: ")); Serial.println(count);
    } else {
      Serial.println(F("OVERFLOW blocked: belt full, producer must wait"));
    }
  }

  // ---- CONSUMER side: item removed from exit ----
  if (updateSensor(exitSensor) && !exitSensor.stable) {
    if (count > 0) {
      count--;
      Serial.print(F("CONSUME -> items on belt: ")); Serial.println(count);
    }
  }

  // ---- Belt control ----
  // Run only if there is something to carry AND no item is waiting at the exit
  bool run = (count > 0) && !exitSensor.stable;
  analogWrite(MOTOR_PIN, run ? MOTOR_SPEED : 0);

  // ---- Status LED ----
  if (count >= BUFFER_SIZE) {
    digitalWrite(LED_PIN, (millis() / 100) % 2);   // fast blink = FULL
  } else if (count == 0) {
    digitalWrite(LED_PIN, LOW);                     // off = EMPTY
  } else {
    digitalWrite(LED_PIN, HIGH);                    // solid = running / has items
  }
}
