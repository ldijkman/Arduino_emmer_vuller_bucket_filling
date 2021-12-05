




/*
  started with the code from
      https://youtu.be/L9ZFgElnTGU
      https://create.arduino.cc/projecthub/embeddedlab786/automatic-water-bottle-filling-system-e8251c
      https://github.com/embeddedlab786/Automatic_Water_Bottle_Filling_System
  but will change a lot and add some comments to the code
  https://github.com/ldijkman/Arduino_emmer_vuller_bucket_filling
  should have a screen for testing all inputs / outputs to check working
  aalsmeer bloemen veiling emmervuller containervuller fustvuller
  low cost water filling timer conveyer belt automation controller +/-10 euro
  https://youtu.be/L9ZFgElnTGU
  https://hacksterio.s3.amazonaws.com/uploads/attachments/1200705/automatic_water_bottle_filling_system_L7E1jegbDF.png
  https://create.arduino.cc/projecthub/embeddedlab786/automatic-water-bottle-filling-system-e8251c
  conveyer belt type water filling
  want i2c lcd
  want rotary encoder for water_valve_opentime setting
  https://github.com/ldijkman/Arduino_emmer_vuller_bucket_filling
  arduino uno
  icc 20x4 dot matrix lcd
  rotary encoder
  buttons
  optic or ultrasonic sensors for bottle bucket detection
  valve or pump_or_valve for water
  relais engine conveyer belt beltmotor
*/

#include <LiquidCrystal_I2C.h>                // better use i2c display??? https://www.arduinolibraries.info/libraries/liquid-crystal-i2-c
#include <EEPROM.h>                           // for saving fillingtime tru reboot / powercycle


LiquidCrystal_I2C lcd(0x27, 20, 4);          // Set the LCD address to 0x27 for a 20 chars and 4 line display

int cont = 0;

// int ???
int button_set = A0;        // button set fill timer micro seconds digit selection    could be 2,9 seconds instead of 2900 milli
int button_val_up = A1;     // button time        digit value++ up increase
int button_stop = A2;       // button stop
int button_start = A3;      // button start       start and stop could be made 1 button


int set = 0;
int stop = 0, stop1;
int mode = 0;

int buz = 13;

int timer = 0;

int pump_or_valve = 8;
int beltmotor = 9;

int ir_start = 10;
int ir_fill = 11;
int ir_stop = 12;



int val1 = 0, val2 = 0, val3 = 0, val4 = 1;
long fillingtime = 0;






void setup() {
                                                  // i like to use a rotary encoder for fill time setting and 1 start/stop button
  pinMode (button_set, INPUT_PULLUP);             // input pullup ???
  pinMode (button_val_up, INPUT_PULLUP);          // input pullup ???
  pinMode (button_stop, INPUT_PULLUP);           // input pullup ???
  pinMode (button_start, INPUT_PULLUP);           // input pullup ???
                                                  // i like to use a rotary encoder for fill time setting and 1 start/stop button
  pinMode(ir_fill, INPUT_PULLUP);
  pinMode(ir_start, INPUT_PULLUP);                // 3x input pullup ??? no resistors needed
  pinMode(ir_stop, INPUT_PULLUP);                 // think 2 product detection sensors are enough 1 fill position and 1 end conveyer position stop

  pinMode(beltmotor, OUTPUT);
  pinMode(pump_or_valve, OUTPUT);

  pinMode(buz, OUTPUT);

  lcd.begin();               // initialize the LCD

  lcd.backlight();           // Turn on the backlight 
  
  lcd.setCursor(0, 0); lcd.print("   WELCOME  To  ");
  lcd.setCursor(0, 1); lcd.print("Automatic  Water");
  lcd.setCursor(0, 2); lcd.print("  Filling");
  lcd.setCursor(0, 3); lcd.print("          System");
  delay(2000);
  Serial.begin(9600);



  Read();                                   // read eeprom fillingtime wil be 0 at the first boot ???
  if (fillingtime == 0) fillingtime = 2500; // make it 2500 miliseconds 2.5 seconds



  lcd.clear();


} // end setup




void loop() {

  if (stop == 0) {
    if (digitalRead (button_set) == 0) {       // set select digit button pressed
      lcd.clear();
      mode = mode + 1;
      if (mode > 4) {                  // set switch digit  select 1 of 4 digits
        mode = 0;
        Write();                       // write fillingtime to eeprom
        Read();                        // read fillingtime from eeprom
        lcd.setCursor(0, 0);
        lcd.print("  Ok Saved Data ");
        delay(1000);
      }
      delay(300);
    }

    if (digitalRead (button_val_up) == 0) {           // up button pressed fillingtime value++
      if (mode == 1) {                      // digit 1 of 4 active for change
        val1 = val1 + 1;
      }
      if (mode == 2) {                     // digit 2 of 4 active for change
        val2 = val2 + 1;
      }
      if (mode == 3) {                   // digit 3 of 4 active for change
        val3 = val3 + 1;
      }
      if (mode == 4) {                  // digit 5 of 4 active for change
        val4 = val4 + 1;
      }
      if (val1 > 9) {                   // digit1 not greater as 9 make 10 zero
        val1 = 0;
      }
      if (val2 > 9) {                  // digit2 not greater as 9 make 10 zero
        val2 = 0;
      }
      if (val3 > 9) {                // digit3 not greater as 9 make 10 zero
        val3 = 0;
      }
      if (val4 > 9) {                // digit4 not greater as 9 make 10 zero
        val4 = 0;
      }
      delay(300);
    }
  } // end if stop == 0






  if (mode == 0) {
    if (digitalRead (button_stop) == 0) {                 // stop button pressed
      stop = 0;
    }
    if (digitalRead (button_start) == 0) {                // start button pressed   start stop could me made 1 button
      stop = 1;
    }

 /*   
    // start stop could me made 1 button like next
    // toggle stopval
    if (digitalRead (button_start) == 0) {                // stop/start button pressed toggle
      stop = !stop;                                      // !stop (stop = notstop) toggle the value in stop
    }
 */   
    
    lcd.setCursor(0, 0); lcd.print("   WELCOME  To  ");
    lcd.setCursor(0, 1); lcd.print("     A W F S    ");

    lcd.setCursor(0, 2);
   // lcd.print("Tank Level=");
   // lcd.print(dist);
   // lcd.print("%   ");

    lcd.setCursor(0, 3);
    lcd.print("Fill Time =");
    lcd.print(fillingtime);
    lcd.print("   ");
  } else {

    lcd.setCursor(0, 0);
    lcd.print("Set Time= ");
    lcd.print(val4);
    lcd.print(val3);
    lcd.print(val2);
    lcd.print(val1);

    if (mode == 4) {              // edit fillingtime active print a - for selected active for change  digit
      lcd.setCursor(10, 1);
      lcd.print("-");
    }
    if (mode == 3) {              // edit fillingtime active print a - for selected active for change  digit
      lcd.setCursor(11, 1);
      lcd.print("-");
    }
    if (mode == 2) {               // edit fillingtime active print a - for selected active for change  digit
      lcd.setCursor(12, 1);
      lcd.print("-");
    }
    if (mode == 1) {              // edit fillingtime active print a - for selected active for change  digit
      lcd.setCursor(13, 1);
      lcd.print("-");
    }
  }




  if (stop == 1) {
    if (digitalRead (ir_stop) == 1) {
      analogWrite(beltmotor, 200);                    // start belt
      if (digitalRead (ir_fill) == 0) {
        if (stop1 == 0) {
          stop1 = 1;
          analogWrite(beltmotor, 0);                  // stop belt
          delay(200);
          digitalWrite(pump_or_valve, HIGH);          // start watering
          delay(fillingtime);                         // delay fillingtiime
          digitalWrite(pump_or_valve, LOW);           // stop wattering
          analogWrite(beltmotor, 200);                // start belt
        }
      }

      if (digitalRead (ir_start) == 0) {
        stop1 = 0;
      }

    }
    else {
      analogWrite(beltmotor, 0);   // stop belt
      delay(300);
    }
  } else {
    analogWrite(beltmotor, 0);    // stop belt
  }

}

// read and store filling timer value setting for reboot / powercycle
void Read() {
  val1 = EEPROM.read(11);
  val2 = EEPROM.read(12);
  val3 = EEPROM.read(13);
  val4 = EEPROM.read(14);
  fillingtime = val4 * 1000 + val3 * 100 + val2 * 10 + val1;
  Serial.println(fillingtime);
}
// read and store filling timer value setting for reboot / powercycle
void Write() {
  EEPROM.write(11, val1);
  EEPROM.write(12, val2);
  EEPROM.write(13, val3);
  EEPROM.write(14, val4);
}





// https://github.com/ldijkman/Arduino_emmer_vuller_bucket_filling
