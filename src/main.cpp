#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <PulseSensorPlayground.h>
#include "i2c.h"
#include <string.h>
#include <math.h>

#define DHTTYPE DHT11
const int DHTPin = 7;    
int buzzerPin = 4;
int pulsePin = A0;

DHT_Unified dht(DHTPin, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

PulseSensorPlayground pulseSensor;

unsigned long myTimerMillis1 = 0;
unsigned long myTimerMillis2 = 0;
unsigned long myTimerMillis3 = 0;

double med_init = 0;
double counter_init = 0;
double value_pulse_init = 0;

double med_pulse = 0;
double counter_pulse = 0;
double value_pulse = 0;

int count_lie_m1 = 0;
int count_lie_m2 = 0;

int method = 0;
bool iteration = false;

String questions[4];
int total_questions = 0;

double med_hum_init = 0;
double value_hum_init = 0;
int counter_hum_init = 0;

double med_hum = 0;
double value_hum = 0;
int counter_hum = 0;

bool questions_long[5];

// I will have a set of questions and i will say based on sensors if he lies or not
void method1(String question[4]) {
  for (int i = 0; i < total_questions; i++) {
    sensors_event_t event_t, event_h;
    dht.temperature().getEvent(&event_t);
    dht.humidity().getEvent(&event_h);
    Serial.print("Question: ");
    Serial.println(question[i]);
    if (question[i].length() > 16) {
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print(question[i]);
      for (int j = 0; j < question[i].length(); j++) {
          lcd.scrollDisplayLeft();
          delay(700);
        }
       delay(2000);
    } else {
      lcd.clear();
      for (int aux = 0; aux < 16 && aux < question[i].length(); aux++) {
        
        lcd.setCursor(aux, 0);
        lcd.print(question[i].charAt(aux));
      }
      for (int aux = 16; aux < question[i].length(); aux++) {
        lcd.setCursor(aux - 16, 1);
        lcd.print(question[i].charAt(aux));
      }
    }
    
    delay(1000);
    myTimerMillis3 = millis();

    value_pulse = 0;
    counter_pulse = 0;
    med_pulse = 0;
    value_hum = 0;
    counter_hum = 0;
    med_hum = 0;

    while(millis() - myTimerMillis3 < 40000) { 
      // lcd.clear();
      int currentBPM = pulseSensor.getBeatsPerMinute();
      pulseSensor.outputSample();
    
      // Check if a heartbeat is detected
      if (pulseSensor.sawStartOfBeat()) {
        Serial.println("♥ A HeartBeat Happened!");
        Serial.print("BPM: ");
        Serial.println(currentBPM);
        
        if (currentBPM > 160) {
          digitalWrite(10, LOW);
          digitalWrite(11, HIGH);
          digitalWrite(12, LOW);
          Serial.println("Too high BPM. Please move your finger");
        } else if (currentBPM < 30) {
          digitalWrite(10, LOW);
          digitalWrite(11, HIGH);
          digitalWrite(12, LOW);
          Serial.println("Too low BPM. Please move your finger");
        } else {
          digitalWrite(11, LOW);
          value_pulse += currentBPM;
          counter_pulse++;
        }
      }

      if (!isnan(event_h.relative_humidity) && (millis() - myTimerMillis3) % 2000 == 0){
        value_hum += event_h.relative_humidity;
        counter_hum++;
        Serial.print("Humidity: ");
        Serial.println(event_h.relative_humidity);
      }

    }
    med_pulse = value_pulse / counter_pulse;
    Serial.print("Med pulse = ");
    Serial.println(med_pulse);
    

    med_hum = value_hum / counter_hum;
    Serial.print("Med hum = ");
    Serial.println(med_hum);
    delay(2000);
    Serial.print("Med pulse_init = ");
    Serial.println(med_init);
   
    if (med_init < 85) {
      if (med_pulse - med_init > 10 || med_hum - med_hum_init > 10) {
          lcd.clear();
          lcd.setCursor(4, 0);
          lcd.print("Lie");
          digitalWrite(10, HIGH);
          digitalWrite(11, LOW);
          digitalWrite(12, LOW);
          digitalWrite(buzzerPin, HIGH);
          tone(buzzerPin, 110);
          delay(200);
          noTone(buzzerPin);
          delay(50);
          tone(buzzerPin, 110);
          delay(200);
          noTone(buzzerPin);
          digitalWrite(buzzerPin, LOW);
          count_lie_m1++;
      } else if (med_pulse - med_init <= 10 || med_hum - med_hum_init <= 10) {
        lcd.clear();
          lcd.setCursor(4, 0);
          lcd.print("Truth");
          digitalWrite(10, LOW);
          digitalWrite(11, LOW);
          digitalWrite(12, HIGH);
          digitalWrite(buzzerPin, HIGH);
          tone(buzzerPin, 900);
          delay(100);
          noTone(buzzerPin);
          delay(50);
          tone(buzzerPin, 1000);
          delay(350);
          noTone(buzzerPin);
          digitalWrite(buzzerPin, LOW);
      }
    } else {
      if (med_pulse - med_init > 5 || med_hum - med_hum_init > 10) {
          lcd.clear();
          lcd.setCursor(4, 0);
          lcd.print("Lie");
          digitalWrite(10, HIGH);
          digitalWrite(11, LOW);
          digitalWrite(12, LOW);
          digitalWrite(buzzerPin, HIGH);
          tone(buzzerPin, 110);
          delay(200);
          noTone(buzzerPin);
          delay(50);
          tone(buzzerPin, 110);
          delay(200);
          noTone(buzzerPin);
          digitalWrite(buzzerPin, LOW);
          count_lie_m1++;
      } else if (med_pulse - med_init <= 5 || med_hum - med_hum_init <= 10) {
        lcd.clear();
          lcd.setCursor(4, 0);
          lcd.print("Truth");
          digitalWrite(10, LOW);
          digitalWrite(11, LOW);
          digitalWrite(12, HIGH);
          digitalWrite(buzzerPin, HIGH);
          tone(buzzerPin, 900);
          delay(100);
          noTone(buzzerPin);
          delay(50);
          tone(buzzerPin, 1000);
          delay(350);
          noTone(buzzerPin);
          digitalWrite(buzzerPin, LOW);
      }
    }
    delay(2000);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);
    if (i < 3) {
      Serial.print("Next question is:");
      Serial.println(question[i + 1]);
    }
  }
}


void method2(int nr_of_questions) {

  lcd.clear();
  
  for (int i = 0; i < nr_of_questions; i++) {
    delay(2000);
    sensors_event_t event_t, event_h;
    dht.temperature().getEvent(&event_t);
    dht.humidity().getEvent(&event_h);
  
    if (questions_long[i] == true) {
      lcd.setCursor(2, 0);
      lcd.print(questions[i]);
      for (int j = 0; j < questions[i].length(); j++) {
          lcd.scrollDisplayLeft();
          delay(700);
        }
      delay(2000);

    } else {
      for (int aux = 0; aux < 16; aux++) {
        lcd.setCursor(aux, 0);
        lcd.print(questions[i].charAt(aux));
      }
      for (int aux = 16; aux < questions[i].length(); aux++) {
        lcd.setCursor(aux - 16, 1);
        lcd.print(questions[i].charAt(aux));
      }
    }

    delay(1000);
    myTimerMillis3 = millis();

    value_pulse = 0;
    counter_pulse = 0;
    med_pulse = 0;
    value_hum = 0;
    counter_hum = 0;
    med_hum = 0;

    while(millis() - myTimerMillis3 < 40000) { 
      int currentBPM = pulseSensor.getBeatsPerMinute();
      pulseSensor.outputSample();
      if (pulseSensor.sawStartOfBeat()) {
        Serial.println("♥ A HeartBeat Happened!");
        Serial.print("BPM: ");
        Serial.println(currentBPM);
        
        if (currentBPM > 160) {
          digitalWrite(10, LOW);
          digitalWrite(11, HIGH);
          digitalWrite(12, LOW);
          Serial.println("Too high BPM. Please move your finger");
        } else if (currentBPM < 30) {
          digitalWrite(10, LOW);
          digitalWrite(11, HIGH);
          digitalWrite(12, LOW);
          Serial.println("Too low BPM. Please move your finger");
        } else {
          digitalWrite(11, LOW);
          value_pulse += currentBPM;
          counter_pulse++;
        }
      }

      if (!isnan(event_h.relative_humidity) && (millis() - myTimerMillis3) % 2000 == 0){
        value_hum += event_h.relative_humidity;
        counter_hum++;
        Serial.print("Humidity: ");
        Serial.println(event_h.relative_humidity);
      }

    }
        
    med_pulse = value_pulse / counter_pulse;
    Serial.print("Med pulse = ");
    Serial.println(med_pulse);

    med_hum = value_hum / counter_hum;
    Serial.print("Med hum = ");
    Serial.println(med_hum);
  
    delay(4000);

    if (med_init < 85) {
      if (med_pulse - med_init > 10 || med_hum - med_hum_init > 10) {
            lcd.clear();
            lcd.setCursor(4, 0);
            lcd.print("Lie");
            digitalWrite(10, HIGH);
            digitalWrite(11, LOW);
            digitalWrite(12, LOW);
            digitalWrite(buzzerPin, HIGH);
            tone(buzzerPin, 110);
            delay(200);
            noTone(buzzerPin);
            delay(50);
            tone(buzzerPin, 110);
            delay(200);
            noTone(buzzerPin);
            digitalWrite(buzzerPin, LOW);
            count_lie_m2++;
        } else if (med_pulse - med_init <= 10 || med_hum - med_hum_init <= 10) {
            lcd.clear();
            lcd.setCursor(4, 0);
            lcd.print("Truth");
            digitalWrite(10, LOW);
            digitalWrite(11, LOW);
            digitalWrite(12, HIGH);
            digitalWrite(buzzerPin, HIGH);
            tone(buzzerPin, 900);
            delay(100);
            noTone(buzzerPin);
            delay(50);
            tone(buzzerPin, 1000);
            delay(350);
            noTone(buzzerPin);
            digitalWrite(buzzerPin, LOW);
      }
    } else {
      if (med_pulse - med_init > 5 || med_hum - med_hum_init > 10) {
          lcd.clear();
          lcd.setCursor(4, 0);
          lcd.print("Lie");
          digitalWrite(10, HIGH);
          digitalWrite(11, LOW);
          digitalWrite(12, LOW);
          digitalWrite(buzzerPin, HIGH);
          tone(buzzerPin, 110);
          delay(200);
          noTone(buzzerPin);
          delay(50);
          tone(buzzerPin, 110);
          delay(200);
          noTone(buzzerPin);
          digitalWrite(buzzerPin, LOW);
          count_lie_m2++;
      } else if (med_pulse - med_init <= 5 || med_hum - med_hum_init <= 10) {
          lcd.clear();
          lcd.setCursor(4, 0);
          lcd.print("Truth");
          digitalWrite(10, LOW);
          digitalWrite(11, LOW);
          digitalWrite(12, HIGH);
          digitalWrite(buzzerPin, HIGH);
          tone(buzzerPin, 900);
          delay(100);
          noTone(buzzerPin);
          delay(50);
          tone(buzzerPin, 1000);
          delay(350);
          noTone(buzzerPin);
          digitalWrite(buzzerPin, LOW);
      }
    }
    delay(2000);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);
  }
}

void setup() { 
  // Begin serial communication at 9600 baud rate 
  Serial.begin(9600); 
  method = 2;
  iteration = true;
  total_questions = 1;
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);

  //initialize lcd screen
  lcd.begin();
  // turn on the backlight
  lcd.backlight();
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(10, OUTPUT);

  pinMode(buzzerPin, OUTPUT);
  pulseSensor.analogInput(pulsePin);
  pulseSensor.blinkOnPulse(13);
  pulseSensor.setThreshold(600);
  if (pulseSensor.begin()) 
  {
    Serial.println("PulseSensor object created successfully!");
  }
  myTimerMillis1 = millis();
  lcd.clear();
  lcd.setCursor(2, 0);
  String message = "Please wait to calibrate!";
  for (int i = 0; i < 12; i++) {
    lcd.setCursor(i + 2, 0);
    lcd.print(message.charAt(i));
  }
  for (int i = 12; i < message.length(); i++) {
    lcd.setCursor(i - 10, 1);
    lcd.print(message.charAt(i));
  }
  delay(6000);
  lcd.clear();
  message = "Is your name Caty?";
  for (int i = 0; i < 12; i++) {
    lcd.setCursor(i + 2, 0);
    lcd.print(message.charAt(i));
  }
  for (int i = 12; i < message.length(); i++) {
    lcd.setCursor(i - 10, 1);
    lcd.print(message.charAt(i));
  }
  
  while(millis() - myTimerMillis1 < 50000) {
    int BPM = pulseSensor.getBeatsPerMinute();
    pulseSensor.outputSample();
    
    // Check if a heartbeat is detected
    if (pulseSensor.sawStartOfBeat()) {
      Serial.println("♥ Init HeartBeat Happened!");
      Serial.print("BPM: ");
      Serial.println(BPM);

      if (BPM > 160) {
        Serial.println("Too high BPM INIT. Please move your finger");
        Serial.println(BPM);

      } else if (BPM < 30) {
        Serial.println("Too low BPM INIT. Please move your finger");
        Serial.println(BPM);

      } else {
        value_pulse_init += BPM;
        counter_init++;
      }
    }

    sensors_event_t event_t, event_h;
    dht.humidity().getEvent(&event_h);
    if (!isnan(event_h.relative_humidity) && (millis() - myTimerMillis1) % 2000 == 0) {
      // Serial.println(event_h.relative_humidity);
      value_hum_init += event_h.relative_humidity;
      counter_hum_init++;
    }

  }
  if (!isnan(value_pulse_init) && counter_init != 0) {
    med_init = value_pulse_init / counter_init;
    Serial.print("Value pulse med = ");
    Serial.println(med_init);
  }
 
  if (!isnan(value_hum_init) && counter_hum_init != 0) {
    med_hum_init = value_hum_init / counter_hum_init;
    Serial.print("Value hum med = ");
    Serial.println(med_hum_init);
  }
  lcd.clear();
  if (method == 1) {
    String questions_hw[4] = {
    "Is purple your favourite color?",
    "Do you like dogs?",
    "Do you like PM?",
    "Do you cheat on any exam?",
  };
    for (int i = 0; i < 4; i++) {
      questions[i] = questions_hw[i];
    }
  }
} 

int cnt = 0, canCall = 0, nr_of_questions = 0;

void loop() {

  if (method == 2) {
    while(Serial.available() > 0 && cnt < total_questions) {
      char c = Serial.read();
      if (c != '\r') {
        questions[cnt] += c;
      }
      if (c == '?') {
        cnt++;
        Serial.println(cnt);
      }
    }
    delay(3000);
    for (int i = 0; i < total_questions && canCall <= total_questions; i++) {
      if (questions[i].length() > 16) {
        questions_long[i] = true;
      } else {
        questions_long[i] = false;
      }
      Serial.println(questions[i]);
      delay(6000);
    }
    for (int i = 0; i < total_questions && cnt == total_questions && canCall <=total_questions; i++) {
      if (questions[i] != "") {
        canCall++;
      }
    }
    if (iteration == true) {
      Serial.print("Can call: ");
      Serial.println(canCall);
    }

    if (iteration == true) {
      if (canCall == total_questions) {
        method2(total_questions);
        iteration = false;
        lcd.clear();
        lcd.setCursor(2, 0);
        int score = 100 - (count_lie_m2 * (100 / total_questions));
        char buffer[3] = {0};
        itoa(score, buffer, 10);
        char buffer2[11] = "You are ";
        strcat(buffer2, buffer);
        strcat(buffer2, "%");
        lcd.print(buffer2);
        lcd.setCursor(5, 1);
        lcd.print("honest");
        iteration = false;
      }
    }
  }

  else if (method == 1) {
    String questions_hw[total_questions];
    if (iteration == true) {
      for (int i = 0; i < total_questions; i++) {
        Serial.println(questions[i]);
        // memccpy((void*)questions_hw[i], questions[i], 0, questions[i].length());
        questions_hw[i] = questions[i];
        canCall++;
        // questions_hw[i] = questions[i];
      }
      delay(10000);
      if (canCall == total_questions) {

        method1(questions_hw);
        if (count_lie_m1 == 0) {
          lcd.clear();
          lcd.setCursor(2, 0);
          lcd.print("You are 100%");
          lcd.setCursor(5, 1);
          lcd.print("honest");
        } else if (count_lie_m1 == 1) {
          lcd.clear();
          lcd.setCursor(2, 0);
          lcd.print("You are 75%");
          lcd.setCursor(5, 1);
          lcd.print("honest");
        } else if (count_lie_m1 == 2) {
          lcd.clear();
          lcd.setCursor(2, 0);
          lcd.print("You are 50%");
          lcd.setCursor(5, 1);
          lcd.print("honest");
        } else if (count_lie_m1 == 3) {
          lcd.clear();
          lcd.setCursor(2, 0);
          lcd.print("You are 25%");
          lcd.setCursor(5, 1);
          lcd.print("honest");
        } else if (count_lie_m1 == 4) {
          lcd.clear();
          lcd.setCursor(2, 0);
          lcd.print("You are 0%");
          lcd.setCursor(5, 1);
          lcd.print("honest");
        }
        iteration = false;
      }
      // iteration = true;
     
    }
  }
}
