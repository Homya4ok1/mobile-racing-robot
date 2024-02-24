#include <Wire.h>
//#include <TensorFlowLite.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define LCD_ADDR 0x20
#define LCD_COLUMNS 16
#define LCD_ROWS 2
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLUMNS, LCD_ROWS); 

// Піни для лівого ехо-датчика та тригера
const int echoLeft = 2;
const int trigLeft = 3;

// Піни для правого ехо-датчика та тригера
const int echoRight = 4;
const int trigRight = 5;

// Піни для мотора
const int motorDirectionPin2 = 7;
const int motorDirectionPin = 6;
const int motorSpeedPin = 9;
const int frPin = 11;
// Пін для сервопривода
const int servoPin = 10;
//const int tahoPin = 8;
const int led = 13;

// Значення кута повороту максимально вліво або вправо
const int maxTurnAngle = 45;
long durationLeft, distanceLeft, durationRight, distanceRight;
long distance = 100;// максимальна відстань до перепони для повороту
long distance2 = 8;//мінімальна відстань до перепони перед зупинкою
long distance3 = 15;
long distance4 = 60;

int speedLow = 100;
int speedHaigh = 255;

bool flag1 = 0;
bool flag2 = 0;
bool flag3 = 0;
bool flag4 = 0;

float angleDifference = 0;
Servo myServo; // Об'єкт для сервопривода

int i = 255;
void turnRight();
void turnLeft();
void moveForward();
void smoothLyStopMotor();
void moveRevers();
void movDirect(unsigned int a, unsigned int b, unsigned int c);
void handleEncoder();


void setup() {
  Serial.begin(9600);
  lcd.init();

  lcd.backlight();
  lcd.print("   my race car  ");
  delay(3000);
  myServo.attach(servoPin); // Прикріплення сервопривода до піна
  pinMode(trigLeft, OUTPUT);
  pinMode(echoLeft, INPUT);
  pinMode(frPin, INPUT_PULLUP);

  pinMode(trigRight, OUTPUT);
  pinMode(echoRight, INPUT);
  pinMode(led, OUTPUT);

  
 
  pinMode(motorDirectionPin, OUTPUT);
  pinMode(motorDirectionPin2, OUTPUT);
  pinMode(motorSpeedPin, OUTPUT);
}

void loop() {
 //функція активації
bool frVal = !digitalRead (frPin);
//перший натиск кнопки
if(frVal == 1 && flag3 == 0 && flag4 == 0){
   flag3 = 1;
//відпускання   
}else if(frVal == 0 && flag3 == 1){
  flag4 = 1;
//другий натиск кнопки  
} else if (frVal == 1 && flag4 == 1){
  flag3 = 0;
}
 //відпускання   
  if(frVal == 0 && flag4 == 1 && flag3 == 0){
    flag4 = 0;
  }
  
if(flag3 == 0){
    movDirect(0,0,0);
    myServo.write(90);
    digitalWrite (led, HIGH);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("   activated    ");
} else {
  digitalWrite (led, 0);
}
 
  
  //lcd.clear();
  // Вимірюємо відстані для лівого та правого ехо-датчиків
  distanceLeft = measureDistance(trigLeft, echoLeft);
  distanceRight = measureDistance(trigRight, echoRight);

  //Serial.print("Left Distance: ");
  //Serial.println(distanceLeft);
  //Serial.print("Right Distance: ");
  //Serial.println(distanceRight);

  //angleDifference = abs(distanceLeft - distanceRight);  // Обчислення відміни кутів
  
  //тороможення перед перепоною
  if (distanceLeft < distance3 && distanceRight < distance3) {
    smoothLyStopMotor();
    return;
  }
  
 //поворот в залежності від відстані  до перепони
  // Поворот вправо
  if ( distanceLeft < distance2 || distanceRight > distance3 && distanceRight < distance && flag3 == 1) {
    
    //lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("   turn right   ");
    angleDifference = distanceRight;
    turnRight(angleDifference);
  } 
  // Поворот вліво
  else if (distanceRight < distance2 || distanceLeft > distance4 && distanceLeft < distance && flag3 == 1)
  {
   
    //lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("   turn left    ");
    angleDifference = distanceLeft;
    turnLeft(angleDifference);
  } else if(flag3 == 1) {
    myServo.write(90);
    
  //Продовжуємо рухатися вперед
  moveForward();
 }
}

  long measureDistance(int trigPin, int echoPin) {
  long duration, distance;
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;  // Обчислення відстані у сантиметрах

    return distance;
  //}
}
  
  
void smoothLyStopMotor(){ //плавна зупинка перед перепоною
  if(flag3 == 1){
  lcd.clear();
  lcd.print("stopping the car");
  for(i; i >= 0; i--){
    analogWrite(motorSpeedPin, i);
    delay(20);
}
  //повний зупин
  movDirect(0,0,0);         
 // flag1 = 1;
 // moveRevers();
  }
}
void moveRevers(){          //хід назад та вибір напрямку руху
  lcd.clear();
  lcd.print("   revers car   ");
  if(flag2 == 0 && flag3 == 1){
  
  lcd.setCursor(0,1);
  lcd.print("   turn left    ");
  myServo.write(90 - maxTurnAngle);
  movDirect(0,1,speedLow);  //рух назад з поворотом на ліво
  delay(1500); 
  flag2 = 1;  
  moveForward(); 
    
  }else if (flag2 == 1 && flag3 == 1){
  lcd.setCursor(0,1);
  lcd.print("   turn right   ");
  myServo.write(90 + maxTurnAngle);
  movDirect(0,1,speedLow);  //рух назад з поворотом на ліво
  delay(1500); 
  flag2 = 0;  
  moveForward(); 
}
}

void movDirect(unsigned int a, unsigned int b, unsigned int c){
  digitalWrite(motorDirectionPin, a);  //рух назад з поворотом на право
  digitalWrite(motorDirectionPin2, b);
  analogWrite(motorSpeedPin, c);
}

void turnLeft(float angleDifference) {
  int turnAngle = map(angleDifference, 0, 400, 90 - maxTurnAngle, 90);// Мапування відміни кутів на діапазон кутів сервопривода
  myServo.write(turnAngle);
  // Встановлення напрямку та швидкості мотора для руху вперед
  // mov(1,0,speedHiagh);      // Вперед

}

void turnRight(float angleDifference) {
   // Мапування відміни кутів на діапазон кутів сервопривода
  int turnAngle = map(angleDifference, 0, 400, 90 + maxTurnAngle, 90);
  myServo.write(turnAngle);

  // Встановлення напрямку та швидкості мотора для руху вперед
  //mov(1,0,speedHaigh);      // Вперед
 
}

void moveForward() {
 // lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  moveForward   ");
  // Реалізуйте логіку руху вперед
  if(distanceLeft < distance2 || distanceRight < distance2 ){
       lcd.setCursor(0,1);
       lcd.print("    speedLow    ");
       movDirect(1,0,speedLow);        // Вперед
  }else {
       lcd.setCursor(0,1);
       lcd.print("   speedHaigh   ");
       movDirect(1,0,speedHaigh);      // Вперед
}
}


