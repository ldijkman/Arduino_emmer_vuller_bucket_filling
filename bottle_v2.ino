

// Arduino Bottle Filler - Arduino Bucket Filler conveyor type

// http://paypal.me/LDijkman

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
  want rotary encoder for water_valve_opentime setting
  https://github.com/ldijkman/Arduino_emmer_vuller_bucket_filling
  
  add a timer/countdown output and pump or valve for nutricients / chloride / medicine / lemonade sirop ???
  
  Arduino Nano or Uno            2 to 5 euro  
              https://www.google.com/search?q=arduino+nano
              https://www.google.com/search?q=arduino+uno
  
  i2c 20x4 dot matrix LCD        5 euro
             https://www.google.com/search?q=20x4+i2c
  
  rotary encoder                 0.5 euro
            https://www.google.com/search?q=arduino+rotary+encoder+ky-040
            
  buttons                        1 euro
  
  ir optic or ultrasonic sensors for bottle bucket detection  1 to 10 euro
            https://www.google.com/search?q=arduino+HW-201  
  
  valve or pump_or_valve for water                          ??? euro
  relais engine conveyer belt beltmotor                     ??? euro
Arduino Code ??? euro    http://paypal.me/LDijkman
 */

#include <LiquidCrystal_I2C.h>                // better use i2c display??? https://www.arduinolibraries.info/libraries/liquid-crystal-i2-c
#include <EEPROM.h>                           // for saving fillingtime tru reboot / powercycle


LiquidCrystal_I2C lcd(0x27, 20, 4);          // Set the LCD address to 0x27 for a 20 chars and 4 line display
                                             // connect LCD i2c BackPack sda scl
                                             // Arduino uno A4=SDA A5=SCL 
                                             // Arduino nano A4=SDA A5=SCL

int cont = 0;

// int ???
int button_set = A0;        // button set fill timer micro seconds digit selection    could be 2,9 seconds instead of 2900 milli
int button_val_up = A1;     // button time        digit value++ up increase
int button_stop = A2;       // not used button stop    // now only start button is used for start and stop
int button_start = A3;      // button start       start and stop could be made 1 button


int set = 0;
int stop = 0, stop1;
int mode = 0;

int buz = 13;                 // I/O D13 Buzzer not yet used in code?

int timer = 0;

int pump_or_valve = 8;        // I/O D8 
int pump_or_valve_run = 1;     // 0 or 1  value depends on if valve is open on 0 low or 1 high 
int beltmotor = 9;            // I/O D9      used analog output PWM but can be changed to digital for relais
int beltmotor_run = 1;        // 0 or 1  value depends on if valve is open on 0 low or 1 high 

int ir_start = 10;    // I/O D10 IR sensor  position after filling >>> should be possible without this one if code is changed
int ir_fill = 11;     // I/O D11 IR sensor  filling position
int ir_stop = 12;     // I/O D12 IR sensor  end belt stop

// I think sensors on belt looks like next setup
// >>>>>>>>>11 filling position>>>>>>>>>>>10 position after filling>>>>>>>>>>> 12 end  belt stop




int val1 = 0, val2 = 0, val3 = 0, val4 = 1;
long fillingtime = 0;






void setup() {
  // i like to use a rotary encoder for fill time setting and 1 start/stop button
  pinMode (button_set, INPUT_PULLUP);             // input pullup ??? no resistors needed
  pinMode (button_val_up, INPUT_PULLUP);          // input pullup ??? no resistors needed
  pinMode (button_stop, INPUT_PULLUP);            // input pullup ??? no resistors needed
  pinMode (button_start, INPUT_PULLUP);           // input pullup ??? no resistors needed
  // i like to use a rotary encoder for fill time setting and 1 start/stop button
  pinMode(ir_fill, INPUT_PULLUP);
  pinMode(ir_start, INPUT_PULLUP);                // 3x input pullup ??? no resistors needed
  pinMode(ir_stop, INPUT_PULLUP);                 // think 2 product detection sensors are enough 1 fill position and 1 end conveyer position stop

  pinMode(beltmotor, OUTPUT);
  pinMode(pump_or_valve, OUTPUT);

  pinMode(buz, OUTPUT);

  lcd.begin();               // initialize the LCD

  lcd.backlight();           // Turn on the backlight

  lcd.setCursor(0, 0); lcd.print("     EmmerVuller ");
  lcd.setCursor(0, 1); lcd.print("    BucketFiller ");
  lcd.setCursor(0, 2); lcd.print("      Filling    ");
  lcd.setCursor(0, 3); lcd.print("      System     ");
  delay(5000);
  Serial.begin(9600);



  Read();                                     // read eeprom fillingtime wil be 0 at the first boot ???
  if (fillingtime <= 50) fillingtime = 2500;  // make it 2500 miliseconds 2.5 seconds



  lcd.clear();


} // end setup




void loop() {

  if (stop == 0) {
    if (digitalRead (button_set) == 0) {       // set select digit button pressed
      lcd.clear();
      mode = mode + 1;
      if (mode > 4) {                          // set switch digit  select 1 of 4 digits
        mode = 0;
        Write();                               // write fillingtime to eeprom
        Read();                                // read fillingtime from eeprom
        lcd.setCursor(0, 0);
        lcd.print("  Ok Saved Data ");
        delay(1000);
      }
      delay(300);
    }

    if (digitalRead (button_val_up) == 0) {           // up button pressed fillingtime value++
      if (mode == 1) {                  // digit 1 of 4 active for change
        val1 = val1 + 1;
      }
      if (mode == 2) {                  // digit 2 of 4 active for change
        val2 = val2 + 1;
      }
      if (mode == 3) {                  // digit 3 of 4 active for change
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
      if (val3 > 9) {                  // digit3 not greater as 9 make 10 zero
        val3 = 0;
      }
      if (val4 > 9) {                  // digit4 not greater as 9 make 10 zero
        val4 = 0;
      }
      delay(300);
    }
  } // end if stop == 0






  if (mode == 0) {

    //  if (digitalRead (button_stop) == 0) {                 // stop button pressed
    //    stop = 0;
    //  }
    // if (digitalRead (button_start) == 0) {                // start button pressed   start stop could me made 1 button
    //  stop = 1;
    //  }


    // start stop could me made 1 button like next
    // toggle stopval
    if (digitalRead (button_start) == 0) {                // stop/start button pressed toggle
      while (digitalRead (button_start) == 0) {
        /*nop*/                                           // while loop as long as button is pressed
      }
      stop = !stop;                                       // !stop (stop = notstop) toggle the value in stop
      delay(500);
    }


    lcd.setCursor(0, 0); lcd.print("     EmmerVuller ");
    lcd.setCursor(0, 1); lcd.print("    BucketFiller ");

    lcd.setCursor(0, 2);
    if (stop==1)lcd.print("Running");
    if (stop==0)lcd.print("Stopped");
    lcd.print("        ");

    lcd.setCursor(0, 3);
    lcd.print("Fill Time = ");
    lcd.print(fillingtime);
    lcd.print("   ");
  } else {

    lcd.setCursor(0, 0);
    lcd.print("Set Time= ");
    lcd.print(val4);
    lcd.print(val3);
    lcd.print(val2);
    lcd.print(val1);

    if (mode == 4) {              // edit fillingtime active print a ^ for selected active for change  digit
      lcd.setCursor(10, 1);
      if ((millis() / 500) % 2) lcd.print(char(B01011110)); else  lcd.print(" ");; // blinking cursor ^
    }
    if (mode == 3) {              // edit fillingtime active print a ^ for selected active for change  digit
      lcd.setCursor(11, 1);
      if ((millis() / 500) % 2) lcd.print(char(B01011110)); else  lcd.print(" ");; // blinking cursor ^
    }
    if (mode == 2) {              // edit fillingtime active print a ^ for selected active for change  digit
      lcd.setCursor(12, 1);
      if ((millis() / 500) % 2) lcd.print(char(B01011110)); else  lcd.print(" ");; // blinking cursor ^
    }
    if (mode == 1) {              // edit fillingtime active print a ^ for selected active for change  digit
      lcd.setCursor(13, 1);
      if ((millis() / 500) % 2) lcd.print(char(B01011110)); else  lcd.print(" ");; // blinking cursor ^
    }
  }




  if (stop == 1) {
    if (digitalRead (ir_stop) == 1) {
      // analogWrite(beltmotor, 200);                 // start belt PWM
      digitalWrite(beltmotor, beltmotor_run);                     // start belt Relais
      if (digitalRead (ir_fill) == 0) {
        if (stop1 == 0) {
          stop1 = 1;
          //analogWrite(beltmotor, 0);                  // stop belt PWM
          digitalWrite(beltmotor, !beltmotor_run);                 // stop belt Relais 
          delay(200);
          lcd.setCursor(0, 2);
          lcd.print("water open");
          digitalWrite(pump_or_valve, pump_or_valve_run);          // start watering pump or open valve

          long startmillis = millis();
          while (startmillis + fillingtime > millis()) {
            lcd.setCursor(0, 2);
            lcd.print("water open ");
            if (fillingtime - (millis() - startmillis) < 1000)lcd.print(" ");   // keep position on the right
            if (fillingtime - (millis() - startmillis) < 100)lcd.print(" ");
            if (fillingtime - (millis() - startmillis) < 10)lcd.print(" ");
            lcd.print(long(fillingtime - (millis() - startmillis)));               // show countdown
            // lcd.print("    ");
            // delay(100);
          }
          lcd.setCursor(0, 2);
          lcd.print("                    ");

          digitalWrite(pump_or_valve, !pump_or_valve_run);           // !=not stop watering pump or close valve
          // analogWrite(beltmotor, 200);                // start belt PWM
          digitalWrite(beltmotor, beltmotor_run);                // start belt Relais
        }
      }

      if (digitalRead (ir_start) == 0) {
        stop1 = 0;
      }

    }
    else {
       // analogWrite(beltmotor, 0);                       // stop belt PWM
       digitalWrite(beltmotor, !beltmotor_run);                     // !=not stop belt Relais
       delay(300);
    }
  } else {
    // analogWrite(beltmotor, 0);                       // stop belt PWM
    digitalWrite(beltmotor, !beltmotor_run);                     // !=not stop belt Relais
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


// http://paypal.me/LDijkman
