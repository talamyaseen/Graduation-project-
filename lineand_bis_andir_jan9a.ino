#include <Servo.h>

// تعريف السيرفو
Servo myServoUP;    // سيرفو لحركة رفع البسكويت
Servo myServoDown;  // سيرفو لتنزيل البسكويت

// تعريف دبابيس مستشعرات IR
#define IR_SENSOR_1_PIN 16 // مستشعر IR الأول
#define IR_SENSOR_2_PIN 8  // مستشعر IR الثاني
#define RELAY_PIN 22       // دبوس التحكم بالموتور

// تعريف أعلام التحكم
bool creamFlag = true;       // علم خاص بالكريمة
bool chocolateFlag = true;   // علم خاص بالشوكولاتة
bool biscuitDropped = false; // علم لتنزيل البسكويت مرة واحدة

// إعداد النظام
void setup() {
  // إعداد السيرفو
  myServoUP.attach(37);    // توصيل السيرفو الأول على الدبوس 37
  myServoDown.attach(36);  // توصيل السيرفو الثاني على الدبوس 36
  myServoUP.write(72);     // تعيين الوضع الابتدائي للسيرفو الأول
  myServoDown.write(100);  // تعيين الوضع الابتدائي للسيرفو الثاني

  // إعداد المستشعرات
  pinMode(IR_SENSOR_1_PIN, INPUT);
  pinMode(IR_SENSOR_2_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);  // تشغيل الموتور في البداية

  // بدء الاتصال التسلسلي
  Serial.begin(9600);
  Serial.println("System Initialized");
}

// وظيفة لتنزيل البسكويت باستخدام السيرفو
void dropBiscuit() {
  if (!biscuitDropped) {  // إذا لم يتم تنزيل البسكويت من قبل
    // حركة السيرفو لتنزيل البسكويت
    myServoUP.write(0);      // تحريك السيرفو الأول لرفع البسكويت
    delay(100);              // انتظار
    myServoUP.write(72);     // إعادة السيرفو الأول للوضع الافتراضي
    delay(2000);             // انتظار بسيط

    myServoDown.write(0);    // تحريك السيرفو الثاني لتنزيل البسكويت
    delay(500);              // انتظار
    myServoDown.write(100);  // إعادة السيرفو الثاني للوضع الافتراضي
    delay(800);              // انتظار بسيط

    Serial.println("Biscuit dropped.");
    biscuitDropped = true;   // تعيين العلم إلى true
  }
}

// الحلقة الرئيسية
void loop() {
  // تنزيل البسكويت مرة واحدة
  dropBiscuit();

  // التحقق من مستشعر IR الأول
  if (digitalRead(IR_SENSOR_1_PIN) == LOW) {
    digitalWrite(RELAY_PIN, LOW);  // إيقاف خط الإنتاج
    Serial.println("Biscuit detected at IR sensor 1. Waiting for cream...");

    if (creamFlag) {  // إذا تم إضافة الكريمة
      Serial.println("Cream added. Resuming line...");
      creamFlag = false; // إعادة تعيين العلم إلى false بعد إضافة الكريمة
      delay(500); // تأخير بسيط
      digitalWrite(RELAY_PIN, HIGH); // إعادة تشغيل خط الإنتاج
      delay(3000);
    }
  }

  // التحقق من مستشعر IR الثاني
  if (digitalRead(IR_SENSOR_2_PIN) == LOW) {
    digitalWrite(RELAY_PIN, LOW);  // إيقاف خط الإنتاج
    Serial.println("Biscuit detected at IR sensor 2. Waiting for chocolate...");

    if (chocolateFlag) {  // إذا تم إضافة الشوكولاتة
      Serial.println("Chocolate added. Resuming line...");
      chocolateFlag = false; // إعادة تعيين العلم إلى false بعد إضافة الشوكولاتة
      delay(500); // تأخير بسيط
      digitalWrite(RELAY_PIN, HIGH); // إعادة تشغيل خط الإنتاج
      delay(3000);

    }
  }

  // إعادة تعيين العلم بعد اكتمال العملية
  if (digitalRead(IR_SENSOR_1_PIN) == HIGH && !creamFlag) {
    creamFlag = true; // إعادة العلم إلى true بعد إضافة الكريمة
    Serial.println("Cream flag reset.");
  }

  if (digitalRead(IR_SENSOR_2_PIN) == HIGH && !chocolateFlag) {
    chocolateFlag = true; // إعادة العلم إلى true بعد إضافة الشوكولاتة
    Serial.println("Chocolate flag reset.");
  }

  delay(100); // تأخير بسيط لتخفيف الحمل على المعالج
}
