aalsmeer bloemen veiling emmervuller containervuller fustvuller

https://youtu.be/L9ZFgElnTGU

https://hacksterio.s3.amazonaws.com/uploads/attachments/1200705/automatic_water_bottle_filling_system_L7E1jegbDF.png

https://create.arduino.cc/projecthub/embeddedlab786/automatic-water-bottle-filling-system-e8251c
conveyer belt type water filling
want i2c lcd
want rotary encoder for watervlaveopentime setting
https://github.com/ldijkman/Arduino_emmer_vuller_bucket_filling


arduino uno 
icc 20x4 dot matrix lcd
rotary encoder
buttons
optic or ultrasonic sensors for bottle bucket detection
valve or pump for water
relais engine conveyer belt motor


#include<LiquidCrystal.h>                 // better use i2c display???
#include <EEPROM.h>                       // for saving fillingtime tru reboot / powercycle
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);      // better use i2c display???

#define echopin A4 // echo pin
#define trigpin A5 // Trigger pin

int cont = 0;

int in1 = A0;    // button set fill timer micro seconds digit selection    could be 2,9 seconds instead of 2900 milli
int in2 = A1;    // button time        digit value++ up increase
int in3 = A2;    // button stop 
int in4 = A3;    // button start       start and stop could be made 1 button

int remotePins [] = { A0, A1, A2, A3};

int set = 0;
int stop = 0, stop1;
int mode = 0;

int buz = 13;

int timer = 0;

int pump = 8;
int motor = 9;

int ir_start = 10;
int ir_fill = 11;
int ir_stop = 12;


int dist;
long duration;

int val1 = 0, val2 = 0, val3 = 0, val4 = 1;
long result = 0;

void setup() {
  for (int i = 0; i < 5; i ++) {
    pinMode (remotePins [i], INPUT);          // input pullup ???
    digitalWrite(remotePins [i], HIGH);       // i like to use a rotary encoder for fill time setting and 1 start/stop button
  }

  pinMode(ir_fill, INPUT);
  pinMode(ir_start, INPUT);             // 3x input pullup ??? no resistors needed
  pinMode(ir_stop, INPUT);              // think 2 product detection sensors are enough 1 fill position and 1 end conveyer position stop

  pinMode (trigpin, OUTPUT);
  pinMode (echopin, INPUT);

  pinMode(motor, OUTPUT);
  pinMode(pump, OUTPUT);

  pinMode(buz, OUTPUT);

  lcd.begin(16, 4);
  lcd.setCursor(0, 0); lcd.print("   WELCOME  To  ");
  lcd.setCursor(0, 1); lcd.print("Automatic  Water");
  lcd.setCursor(0, 2); lcd.print("  Filling");
  lcd.setCursor(0, 3); lcd.print("          System");
  delay(2000);
  Serial.begin(9600);
  //Write();
  Read();
  //analogWrite(motor, 100);
  for (int i = 0; i < 5; i ++) {
    dist = data1();
    data2();
    delay(100);
  }
  lcd.clear();
}

void loop() {

  if (stop == 0) {
    if (digitalRead (in1) == 0) {
      lcd.clear();
      mode = mode + 1;
      if (mode > 4) {
        mode = 0;
        Write();
        Read();
        lcd.setCursor(0, 0);
        lcd.print("  Ok Saved Data ");
        delay(1000);
      }
      delay(300);
    }

    if (digitalRead (in2) == 0) {
      if (mode == 1) {
        val1 = val1 + 1;
      }
      if (mode == 2) {
        val2 = val2 + 1;
      }
      if (mode == 3) {
        val3 = val3 + 1;
      }
      if (mode == 4) {
        val4 = val4 + 1;
      }
      if (val1 > 9) {
        val1 = 0;
      }
      if (val2 > 9) {
        val2 = 0;
      }
      if (val3 > 9) {
        val3 = 0;
      }
      if (val4 > 9) {
        val4 = 0;
      }
      delay(300);
    }
  }

  if (mode == 0) {
    if (digitalRead (in3) == 0) {
      stop = 0;
    }
    if (digitalRead (in4) == 0) {
      stop = 1;
    }

    lcd.setCursor(0, 0); lcd.print("   WELCOME  To  ");
    lcd.setCursor(0, 1); lcd.print("     A W F S    ");

    lcd.setCursor(0, 2);
    lcd.print("Tank Level=");
    lcd.print(dist);
    lcd.print("%   ");

    lcd.setCursor(0, 3);
    lcd.print("Fill Time =");
    lcd.print(result);
    lcd.print("   ");
  } else {

    lcd.setCursor(0, 0);
    lcd.print("Set Time= ");
    lcd.print(val4);
    lcd.print(val3);
    lcd.print(val2);
    lcd.print(val1);

    if (mode == 4) {
      lcd.setCursor(10, 1);
      lcd.print("-");
    }
    if (mode == 3) {
      lcd.setCursor(11, 1);
      lcd.print("-");
    }
    if (mode == 2) {
      lcd.setCursor(12, 1);
      lcd.print("-");
    }
    if (mode == 1) {
      lcd.setCursor(13, 1);
      lcd.print("-");
    }
  }




  if (stop == 1) {
    if (digitalRead (ir_stop) == 1) {
      analogWrite(motor, 200);
      if (digitalRead (ir_fill) == 0) {
        if (stop1 == 0) {
          stop1 = 1;
          analogWrite(motor, 0);
          delay(200);
          digitalWrite(pump, HIGH);
          delay(result);
          digitalWrite(pump, LOW);
          for (int i = 0; i < 10; i ++) {
            dist = data1();
            data2();
            delay(200);
          }
          analogWrite(motor, 200);
        }
      }

      if (digitalRead (ir_start) == 0) {
        stop1 = 0;
      }

    }
    else {
      analogWrite(motor, 0);
      delay(300);
    }
  } else {
    analogWrite(motor, 0);
  }

}
void Read() {
  val1 = EEPROM.read(11); val2 = EEPROM.read(12); val3 = EEPROM.read(13); val4 = EEPROM.read(14);
  result = val4 * 1000 + val3 * 100 + val2 * 10 + val1;
  Serial.println(result);
}

void Write() {
  EEPROM.write(11, val1); EEPROM.write(12, val2); EEPROM.write(13, val3); EEPROM.write(14, val4);
}


void data2() {
  dist = map(dist, 2, 30, 100, 0);
  if (dist > 100) {
    dist = 100;
  }
  if (dist < 0) {
    dist = 0;
  }
}

long data1() {
  digitalWrite(trigpin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  duration = pulseIn (echopin, HIGH);
  return duration / 29 / 2;
}
