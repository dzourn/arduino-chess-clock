
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

typedef enum {
  LEFT = 0,
  RIGHT = 1
} lr;

//pins
int left_button = 2;
int right_button = 3;
int left_led = 4;
int right_led = 5;

//hours minutes
//TODO make user set time
int hours = 1;
int minutes = 30;
int seconds = 0;
int increment = 30;

//matrix to keep track of times [3]={h,m,s}
int ltime[3] = {0, 0, 0};
int rtime[3] = {0, 0, 0};

//millis variables
unsigned long start_millis = 0;
unsigned long current_millis = 0;
unsigned long sec = 1000;

//end of time flag
lr eof_flag;

void print_greeting() {
  lcd.setCursor(0, 0);
  lcd.print("ARDUINO");
  lcd.setCursor(0, 1);
  lcd.print("CHESS CLOCK");
  delay(1500);
  lcd.clear();
}

//l_r value->where to display time (see enum)
void print_time(int ftime[3], lr l_r) {
  if (l_r) {
    lcd.setCursor(9, 1);
  } else {
    lcd.setCursor(0, 1);
  }

  lcd.print(ftime[0]);
  lcd.print(":");
  if (ftime[1] < 10) {
    lcd.print(0);
    lcd.print(ftime[1]);
  } else {
    lcd.print(ftime[1]);
  }
  lcd.print(":");
  if (ftime[2] < 10 ) {
    lcd.print(0);
    lcd.print(ftime[2]);
  } else {
    lcd.print(ftime[2]);
  }
}

void increment_time(int ftime[3], int inc) {
  if (inc == 0) {
    return;
  }
  ftime[2] += inc;
  if (ftime[2] > 59) {
    ftime[1]++;
    ftime[2] = ftime[2] - 59;
  }

  if (ftime[1] > 59) {
    ftime[0]++;
    ftime[1] = 0;
  }

}

//TODO add pause option
lr run_time(int tmp[3], lr l_r) {
  int button;
  if (l_r == LEFT) {
    button = left_button;
    lcd.setCursor(15, 0);
    lcd.print(" ");
    lcd.setCursor(0, 0);
    lcd.print("<");
  } else {
    button = right_button;
    lcd.setCursor(0, 0);
    lcd.print(" ");
    lcd.setCursor(15, 0);
    lcd.print(">");
  }

  while (digitalRead(button) != 1) {
    //check if time has ended
    if (tmp[0] == 0 && tmp[1] == 0 && tmp[2] == 0) {
      return l_r;
    }

    if (tmp[2] == 0) {
      if (tmp[1] > 0) {
        tmp[1]--;
      }
      else if (tmp[1] == 0 && tmp[0] > 0) {
        tmp[0]--;
        tmp[1] = 59;
      }
      tmp[2] = 59;
    }

    current_millis = millis();
    if (current_millis - start_millis >= sec) {
      tmp[2]--;
      print_time(tmp, l_r);
      start_millis = current_millis;
    }

  }

  if (l_r == LEFT) {
    increment_time(ltime, increment);
    print_time(ltime, LEFT);
    run_time(rtime, RIGHT);
  } else {
    increment_time(rtime, increment);
    print_time(rtime, RIGHT);
    run_time(ltime, LEFT);
  }
}

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.clear();
  pinMode(left_button, INPUT);
  pinMode(right_button, INPUT);
  pinMode(left_led, INPUT);
  pinMode(right_led, INPUT);
  print_greeting();
  ltime[0] = rtime[0] = hours;
  ltime[1] = rtime[1] = minutes;
  ltime[2] = rtime[2] = seconds;
  lcd.setCursor(7, 1);
  lcd.print("||");
  //start_millis = millis();
  print_time(ltime, LEFT);
  print_time(rtime, RIGHT);
}

//TODO add reset option
void check_eof(lr l_r) {
  if (l_r == LEFT) {
    lcd.setCursor(0, 0);
    lcd.print("Lft out of time");
    while (1) {
      lcd.setCursor(0, 1);
      lcd.print(" :  :  ");
      delay(500);
      print_time(ltime, LEFT);
      delay(500);
    }
  } else if (l_r == RIGHT) {

    lcd.setCursor(0, 0);
    lcd.print("Rght out of time");
    while (1) {
      lcd.setCursor(9, 1);
      lcd.print(" :  :  ");
      delay(500);
      print_time(rtime, RIGHT);
      delay(500);
    }
  }
}
void loop() {
  if (digitalRead(left_button)) {
    eof_flag = run_time(rtime, RIGHT);
    check_eof(eof_flag);
  } else if (digitalRead(right_button)) {
    eof_flag = run_time(ltime, LEFT);
    check_eof(eof_flag);
  }
}
