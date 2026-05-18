#include "ssd1306h.h"
#include "MAX30102.h"
#include "Pulse.h"
#include <Wire.h>
#include <avr/pgmspace.h>

SSD1306 oled;
MAX30102 sensor;
Pulse pulseIR, pulseRed;

#define LED LED_BUILTIN
#define BUTTON 3

#define IR_UMBRAL 12000UL
#define LED_POWER 0x0F

#define COUNT_MS 12000UL
#define BPM_MIN 45
#define BPM_MAX 120
#define EMA 75
#define MAX_SALTO 5
#define SPO2_MIN 88
#define SPO2_MAX 100
#define BEATS_OK 6

const uint8_t spo2_table[184] PROGMEM = {
95,95,95,96,96,96,97,97,97,97,97,98,98,98,98,98,99,99,99,99,
99,99,99,99,100,100,100,100,100,100,100,100,100,100,100,100,
100,100,100,100,100,100,100,100,99,99,99,99,99,99,99,99,
98,98,98,98,98,98,97,97,97,97,96,96,96,96,95,95,95,94,
94,94,93,93,93,92,92,92,91,91,90,90,89,89,89,88,88,87,
87,86,86,85,85,84,84,83,82,82,81,81,80,80,79,78,78,77,
76,76,75,74,74,73,72,72,71,70,69,69,68,67,66,66,65,64,
63,62,62,61,60,59,58,57,56,56,55,54,53,52,51,50,49,48,
47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,31,30,29,
28,27,26,25,23,22,21,20,19,17,16,15,14,12,11,10,9,7,6,5,
3,2,1
};

struct {
  int bpm = 0, spo2 = 0, lastBpm = 0;
  bool counting = false, ready = false, led = false;
  long fingerT = 0, lastBeat = 0, dispT = 0;
  long Racc = 0, bpmSum = 0, spo2Sum = 0;
  uint8_t Rcnt = 0, beats = 0, avgCnt = 0;
  uint8_t sleep_counter = 0;
} S;

uint8_t viewMode = 0;
long waveDC = 0;

#define MAXW 72
class Wave {
  int w[MAXW], d[MAXW], p = 0;
public:
  void add(int v) { w[p] = v; p = (p + 1) % MAXW; }
  void scale(uint8_t top, uint8_t bot) {
    int mn = 32767, mx = -32768;
    for (int i = 0; i < MAXW; i++) {
      if (w[i] < mn) mn = w[i];
      if (w[i] > mx) mx = w[i];
    }
    if (mx - mn < 6) {
      for (int i = 0; i < MAXW; i++) d[i] = 40;
      return;
    }
    int idx = p;
    for (int i = 0; i < MAXW; i++) {
      d[i] = constrain(map(w[idx], mn, mx, top, bot), bot, top);
      idx = (idx + 1) % MAXW;
    }
  }
  void draw(uint8_t x0 = 0) {
    for (int i = 0; i < MAXW - 1; i++) {
      int y1 = d[i], y2 = d[i + 1];
      if (y2 > y1) {
        for (int y = y1; y <= y2; y++) oled.drawPixel(x0 + i, y);
      } else {
        for (int y = y2; y <= y1; y++) oled.drawPixel(x0 + i, y);
      }
    }
  }
} wave;

void hline(uint8_t y) { for (uint8_t x = 0; x < 128; x++) oled.drawPixel(x, y); }

void n3(int x, int y, int v) {
  if (v > 99) oled.drawChar(x, y, '0' + (v / 100) % 10, 2); else oled.drawChar(x, y, ' ', 2);
  oled.drawChar(x + 12, y, (v > 9) ? '0' + (v / 10) % 10 : ' ', 2);
  oled.drawChar(x + 24, y, '0' + (v % 10), 2);
}

void screen(uint8_t m) {
  oled.firstPage();
  do {
    switch (m) {
      case 0:
        oled.drawStr(8, 10, F("OXIMETRO"), 2);
      break;

      case 1:
        oled.drawStr(4, 8, F("COLOCA TU DEDO"), 1);
      break;

      case 2: {
        int s = (COUNT_MS - (millis() - S.fingerT)) / 1000;
        if (s < 0) s = 0;

        oled.drawStr(0, 0, F("MANTEN EL DEDO"), 1);

        if (!S.ready) {
          oled.drawStr(20, 18, F("CALIBRANDO"), 1);
          n3(46, 32, s);
        } else {
          oled.drawStr(18, 24, F("OBTENIENDO"), 1);
          oled.drawStr(36, 38, F("DATOS"), 1);
        }

        uint8_t bw = ((millis() - S.fingerT) * 126UL) / COUNT_MS;
        if (bw > 126) bw = 126;

        for (uint8_t bx = 1; bx <= bw; bx++)
          for (uint8_t by = 56; by <= 61; by++)
            oled.drawPixel(bx, by);

        hline(55);
        hline(62);
      } break;

      case 3:
        oled.drawStr(0, 0, F("BPM"), 1);
        if (S.bpm > 0) n3(68, 0, S.bpm);
        else oled.drawStr(68, 0, F("---"), 2);

        hline(15);

        oled.drawStr(0, 18, F("SpO2"), 1);
        if (S.spo2 > 0) {
          n3(68, 18, S.spo2);
          oled.drawChar(104, 18, '%', 2);
        } else {
          oled.drawStr(68, 18, F("--%"), 2);
        }

        hline(33);
        wave.draw();
      break;

      case 4: {
        int avgBpm = S.avgCnt ? (S.bpmSum / S.avgCnt) : 0;
        int avgSpo2 = S.avgCnt ? (S.spo2Sum / S.avgCnt) : 0;

        oled.drawStr(18, 0, F("PROMEDIO"), 1);
        hline(10);

        oled.drawStr(0, 16, F("BPM"), 1);
        if (avgBpm > 0) n3(56, 14, avgBpm);
        else oled.drawStr(56, 14, F("---"), 2);

        hline(32);

        oled.drawStr(0, 38, F("SpO2"), 1);
        if (avgSpo2 > 0) {
          n3(50, 36, avgSpo2);
          oled.drawChar(104, 36, '%', 2);
        } else {
          oled.drawStr(50, 36, F("--%"), 2);
        }
      } break;

      case 5:
        oled.drawStr(0, 0, F("PULSO"), 1);
        if (S.bpm > 0) n3(80, 0, S.bpm);
        hline(12);
        wave.draw();
      break;
    }
  } while (oled.nextPage());
}

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  oled.init();
  oled.fill(0);
  screen(0);
  delay(2500);

  if (!sensor.begin()) while (1);
  sensor.setup();

  Wire.beginTransmission(0x57); Wire.write(0x0C); Wire.write(LED_POWER); Wire.endTransmission();
  Wire.beginTransmission(0x57); Wire.write(0x0D); Wire.write(LED_POWER); Wire.endTransmission();
  Wire.beginTransmission(0x57); Wire.write(0x10); Wire.write(LED_POWER); Wire.endTransmission();
}

void loop() {
  sensor.check();
  if (!sensor.available()) return;

  long now = millis();
  uint32_t ir = sensor.getIR();
  uint32_t red = sensor.getRed();
  sensor.nextSample();

  static uint8_t lastBtn = HIGH;
  static long btnT = 0;
  uint8_t btn = digitalRead(BUTTON);
  if (btn != lastBtn && (now - btnT) > 150) {
    btnT = now;
    lastBtn = btn;
    if (btn == LOW) viewMode = (viewMode + 1) % 3;
  }

  if (ir < IR_UMBRAL) {
    if (S.ready && S.avgCnt >= 3) {
      screen(4);
      delay(5000);
    }

    S.bpm = 0;
    S.spo2 = 0;
    S.lastBpm = 0;
    S.beats = 0;
    S.avgCnt = 0;
    S.bpmSum = 0;
    S.spo2Sum = 0;
    S.Racc = 0;
    S.Rcnt = 0;
    S.counting = false;
    S.ready = false;
    S.lastBeat = 0;
    S.led = false;

    screen(1);
    delay(150);

    if (++S.sleep_counter > 100) S.sleep_counter = 0;
    return;
  }

  S.sleep_counter = 0;

  if (!S.counting && !S.ready) {
    S.fingerT = now;
    S.counting = true;
  }

  if (S.counting && !S.ready) {
    if ((now - S.fingerT) >= COUNT_MS) {
      S.counting = false;
      S.ready = true;
    }
  }

  int16_t irf = pulseIR.dc_filter(ir);
  int16_t redf = pulseRed.dc_filter(red);

  waveDC = (waveDC * 31 + (long)ir) / 32;
  wave.add((int)(((long)ir - waveDC) * 3));

  bool beat = false;
  if (abs(irf) > 6) beat = pulseIR.isBeat(irf);

  if (beat) {
    long dt = now - S.lastBeat;
    S.lastBeat = now;

    if (dt > 500 && dt < 1400) {
      int bpm = 60000 / dt;
      if (S.lastBpm && abs(bpm - S.lastBpm) > MAX_SALTO) bpm = S.lastBpm;
      S.lastBpm = bpm;

      if (S.bpm == 0) S.bpm = bpm;
      else S.bpm = (S.bpm * EMA + bpm * (100 - EMA)) / 100;

      if (S.bpm >= BPM_MIN && S.bpm <= BPM_MAX) S.beats++;

      long num = (pulseRed.avgAC() * (long)pulseIR.avgDC()) / 256;
      long den = (pulseRed.avgDC() * (long)pulseIR.avgAC()) / 256;
      int R = (den > 0) ? (num * 100) / den : 999;

      if (R >= 0 && R < 184) {
        S.Racc += R;
        if (++S.Rcnt >= 8) {
          int avg = S.Racc / 8;
          S.Racc = 0;
          S.Rcnt = 0;
          int sp = pgm_read_byte_near(&spo2_table[avg]);
          if (sp >= SPO2_MIN && sp <= SPO2_MAX) S.spo2 = sp;
        }
      }

      if (S.ready && S.bpm > 0 && S.spo2 > 0) {
        S.bpmSum += S.bpm;
        S.spo2Sum += S.spo2;
        S.avgCnt++;
      }

      digitalWrite(LED, HIGH);
      S.led = true;
    }
  }

  if (S.led && now - S.lastBeat > 25) {
    digitalWrite(LED, LOW);
    S.led = false;
  }

  if (now - S.dispT > 45) {
    S.dispT = now;
    if (viewMode == 0) {
      wave.scale(62, 36);
      if (S.counting) screen(2);
      else screen(3);
    } else if (viewMode == 1) {
      screen(4);
    } else {
      wave.scale(62, 14);
      screen(5);
    }
  }
}