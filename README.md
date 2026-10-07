# OS-Project

# Producer–Consumer Conveyor System

An automated material-handling model built on Arduino that turns the classic **producer–consumer synchronization problem** from operating systems into a physical, testable system.

> **Team:** Lalit Choudhary (Ch.sc.u4cys25046) and Mohamad Nisar (Ch.sc.u4cys25053)
> **Department:** CSE Cyber Security, Amrita Vishwa Vidyapeetham

---

## Overview

In the producer–consumer problem, a producer generates items and a consumer processes them, both sharing a fixed-size buffer. The system must stop the producer when the buffer is full (overflow) and stop the consumer when it is empty (underflow).

This project maps that idea onto a conveyor belt:

| OS Concept | Physical Equivalent |
|---|---|
| Producer | Station that places items on the belt (entry sensor) |
| Consumer | Station that removes items from the belt (exit sensor) |
| Bounded buffer | The conveyor belt (capacity of 3 items by default) |
| Semaphore / counter | The `count` variable in the Arduino sketch |
| Mutual exclusion | One item passes each sensor gate at a time |

## Objectives

- Design a working conveyor-based material handling model
- Synchronize producer and consumer stations using sensors
- Prevent buffer overflow and underflow on the conveyor
- Demonstrate a real-world application of the producer–consumer problem

## How It Works

1. **Item placed:** the producer places an item and the entry sensor (D2) detects it.
2. **Belt moves:** the Arduino increments `count` and drives the motor to advance the belt.
3. **Item arrives:** the exit sensor (D3) detects the item at the consumer end and the belt stops.
4. **Item removed:** the consumer takes the item, `count` is decremented, and the cycle is logged over Serial.
5. **Repeat:** the belt restarts if more items are still on it.

**Rules enforced by the code**

- The belt only runs when `count > 0` (prevents underflow).
- If `count == BUFFER_SIZE`, new items are rejected and the status LED blinks fast, meaning the producer must wait (prevents overflow).
- The belt stops while an item sits at the exit sensor and resumes once it is removed.

**Status LED (built-in, pin 13)**

| LED | Meaning |
|---|---|
| Off | Belt empty |
| Solid | Items on belt |
| Fast blink | Belt full, producer must wait |

## Hardware (minimal build)

| Component | Qty | Purpose |
|---|---|---|
| Arduino Uno | 1 | Central controller |
| IR obstacle sensor module | 2 | Entry and exit detection |
| DC motor | 1 | Drives the belt |
| TIP120 NPN transistor (or L298N driver) | 1 | Motor switching |
| 1N4007 diode | 1 | Flyback protection |
| 1 kΩ resistor | 1 | Transistor base resistor |
| 9 V battery | 1 | Separate motor supply |

## Wiring

| Part | Arduino Pin |
|---|---|
| Entry IR sensor OUT | D2 |
| Exit IR sensor OUT | D3 |
| Motor driver input (PWM) | D9 |
| Status LED | Built-in (pin 13) |
| Sensor VCC / GND | 5V / GND |

The motor must be powered from its own supply, with its ground shared with the Arduino. See `conveyor_wiring_diagram.svg` for the full diagram.

**Using an L298N instead:** D9 → ENA, IN1 → 5V, IN2 → GND, motor on OUT1/OUT2, battery on the 12V pin, shared GND.

## Getting Started

1. Wire the circuit as shown above.
2. Open `conveyor_producer_consumer.ino` in the Arduino IDE.
3. Select **Arduino Uno** and the correct port, then upload.
4. Open the Serial Monitor at **9600 baud** to see the log:
   ```
   Conveyor ready. Buffer empty.
   PRODUCE -> items on belt: 1
   CONSUME -> items on belt: 0
   OVERFLOW blocked: belt full, producer must wait
   ```

## Configuration

Edit these constants at the top of the sketch:

| Constant | Default | Description |
|---|---|---|
| `BUFFER_SIZE` | 3 | Max items on the belt |
| `MOTOR_SPEED` | 180 | Belt speed (0–255 PWM) |
| `SENSOR_ACTIVE_LOW` | `true` | Set to `false` if your sensors output HIGH on detection |
| `DEBOUNCE_MS` | 40 | Sensor debounce time |

## Simulating in Tinkercad

Tinkercad has no IR obstacle module, so use a pushbutton for each sensor:

- One leg to D2 (or D3), the other leg to GND, plus a 10 kΩ pull-up from the pin to 5V.
- Pressing a button counts as "item detected".
- Tap the entry button to place an item, hold the exit button to make it arrive, and release it to remove it.

## Applications

- Industrial packaging and sorting lines
- Warehouse and inventory automation
- Assembly line automation in manufacturing

## Future Scope

- IoT connectivity for remote monitoring
- Multiple producers and consumers
- Load sensing on the belt

## Repository Contents

```
├── conveyor_producer_consumer.ino   # Arduino sketch
├── conveyor_wiring_diagram.svg      # Architecture + wiring diagram
└── README.md
```

## Conclusion

The project shows that classic computing concepts translate directly into physical automation. It is a low-cost, scalable base model for real industrial systems.
