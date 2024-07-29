#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Alamat I2C mungkin berbeda, sesuaikan jika perlu

const int pirPin = 2;
const int lampPin = 3;
const int acPin = 4;
const int projectorPin = 5;

unsigned long lastMotionTime = 0;
bool roomOccupied = false;
const unsigned long usageTimeout = 60000; // 60 detik delay sebelum mulai proses menonaktifkan
const unsigned long shutdownInterval = 5000; // 5 detik antara menonaktifkan setiap perangkat

unsigned long shutdownStartTime = 0;
bool shutdownInProgress = false;

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(lampPin, OUTPUT);
  pinMode(acPin, OUTPUT);
  pinMode(projectorPin, OUTPUT);
  
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ruang Meeting");
  lcd.setCursor(0, 1);
  lcd.print("Kosong");
}

void loop() {
  if (digitalRead(pirPin) == HIGH) {
    lastMotionTime = millis();
    if (!roomOccupied) {
      roomOccupied = true;
      shutdownInProgress = false;
      turnOnDevices();
    }
  } else if (roomOccupied) {
    unsigned long timeSinceLastMotion = millis() - lastMotionTime;
    
    if (timeSinceLastMotion > usageTimeout && !shutdownInProgress) {
      shutdownInProgress = true;
      shutdownStartTime = millis();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Menonaktifkan");
      lcd.setCursor(0, 1);
      lcd.print("semua perangkat");
      delay(3000); // Tampilkan pesan ini selama 3 detik
    }
    
    if (shutdownInProgress) {
      turnOffDevicesGradually();
    }
  }
}

void turnOnDevices() {
  digitalWrite(lampPin, HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Menyalakan");
  lcd.setCursor(0, 1);
  lcd.print("Lampu");
  
  delay(10000);  // Delay 10 detik
  
  digitalWrite(acPin, HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Menyalakan AC");
  
  delay(10000);  // Delay 10 detik lagi
  
  digitalWrite(projectorPin, HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Menyalakan");
  lcd.setCursor(0, 1);
  lcd.print("Proyektor");
  
  delay(2000);  // Tunda sebentar sebelum menampilkan status akhir
  
  updateLCDStatus();
}

void turnOffDevicesGradually() {
  unsigned long timeSinceShutdownStart = millis() - shutdownStartTime;

  if (timeSinceShutdownStart >= 0 && timeSinceShutdownStart < shutdownInterval) {
    if (digitalRead(projectorPin) == HIGH) {
      digitalWrite(projectorPin, LOW);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Menonaktifkan");
      lcd.setCursor(0, 1);
      lcd.print("Proyektor");
    }
  } else if (timeSinceShutdownStart >= shutdownInterval && timeSinceShutdownStart < 2 * shutdownInterval) {
    if (digitalRead(acPin) == HIGH) {
      digitalWrite(acPin, LOW);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Menonaktifkan");
      lcd.setCursor(0, 1);
      lcd.print("AC");
    }
  } else if (timeSinceShutdownStart >= 2 * shutdownInterval && timeSinceShutdownStart < 3 * shutdownInterval) {
    if (digitalRead(lampPin) == HIGH) {
      digitalWrite(lampPin, LOW);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Menonaktifkan");
      lcd.setCursor(0, 1);
      lcd.print("Lampu");
    }
  } else if (timeSinceShutdownStart >= 3 * shutdownInterval) {
    roomOccupied = false;
    shutdownInProgress = false;
    updateLCDStatus();
  }
}

void updateLCDStatus() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ruang Meeting");
  lcd.setCursor(0, 1);
  if (roomOccupied) {
    lcd.print("Sedang Digunakan");
  } else {
    lcd.print("Kosong");
  }
}