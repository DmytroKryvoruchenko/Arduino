#include <dht11.h>
dht11 DHT;
#define PIN_DATA 3


int anodPins[] = {A1, A2, A3, A4}; // Задаем пины для кажого разряда
int segmentsPins[] = {5, 6, 7, 8, 9, 10, 11, 12}; //Задаем пины для каждого сегмента (из 7 + 1(точка))

int state;                    // Переменная статуса запроса данных с датчика

unsigned long ms_mode = 0; // Счетчик для смены режима отображения
unsigned long ms_dht = 0;  // Счетчик для обновления данных с датчика

void setup() {
  for (int i = 0; i < 4; i++) pinMode(anodPins[i], OUTPUT);
  for (int i = 0; i < 8; i++) pinMode(segmentsPins[i], OUTPUT);
  //pinMode(pinIn, INPUT);
  ms_mode = millis(); // Засекаем начало работы
  ms_dht = millis();  // Засекаем начало работы

  // Первый запрос данных сделаем отсюда
  state = DHT.read(PIN_DATA);
  getDigits(); // Получаем данные для отображения и записываем их в переменную arr[]
}

// Для экономии памяти будем хранить данные в байтах
//{A, B, C, D, E, F, G,DP} - дисплей перевернутый, поэтому все цифры перевернуты!
byte seg[15] = {
  B11111100, //Цифра 0 не меняется
  B00001100, //Цифра 1
  B11011010, //Цифра 2 не меняется
  B10011110, //Цифра 3
  B00101110, //Цифра 4
  B10110110, //Цифра 5 не меняется
  B11110110, //Цифра 6 меняется с 9
  B00011100, //Цифра 7
  B11111110, //Цифра 8 не меняется
  B10111110, //Цифра 9 меняется с 6
  B11110001, //знак *C (Цельсия)
  B11100010, //знак t
  B00100100, //знак %
  B01100110, //знак h
  B00000000  //Пустой разряд
};

int err[4] = {
  B00000000, //Пусто
  B11110010, //E
  B01000010, //r
  B01000010, //r
};

static byte arr[4]; // Переменная-массив для хранения данных, которые не нужно каждый раз пересчитывать

int mode = 0;                 // Режим отображения данных 0-температура, 1-влажность
int changemodeperiod = 2000;  // Период смены режимов, в мс. 1000 - смена каждую секунду
int refreshdataperiod = 10000; // Период обновления данных с датчика, в мс. 3000 - обновление каждые 3 секунды

void loop() {
  // Блок смены режима отображения ==============================
  if ((millis() - ms_mode) > changemodeperiod) {
    ms_mode = millis(); // Обнуляем счетчик
    mode = mode == 1 ? 0 : 1; // Меняем режим

    // Блок обновления данных =====================================
    // Лучше обновлять данные в момент переключения режима
    if ((millis() - ms_dht) > refreshdataperiod) {
      ms_dht = millis(); // Обнуляем счетчик
      state = DHT.read(PIN_DATA);
    }
    getDigits(); // Получаем данные для отображения и записываем их в переменную arr[]
  }
  displayMessage(arr);
}


void getDigits () { // Получаем массив с данными для каждого индикатора
  if (state == DHTLIB_OK) {
    if (mode == 0) { //0-температура
      arr[0] = seg[11]; arr[3] = seg[10];
      arr[2] = seg[DHT.temperature % 10];
      if (DHT.temperature > 9)
        arr[1] = seg[(int)(DHT.temperature / 10)];
      else arr[1] = seg[14];
      arr[0] = seg[11]; arr[3] = seg[10];
    }
    else if (mode == 1) { // 1-влажность
      arr[0] = seg[11]; arr[3] = seg[10];
      arr[2] = seg[DHT.humidity % 10];
      if (DHT.humidity > 9)
        arr[1] = seg[(int)(DHT.humidity / 10)];
      else arr[1] = seg[14];
      arr[0] = seg[13]; arr[3] = seg[12];
    }
  }
  else {
    for (int i = 0; i < 4; i++)
      arr[i] = err[i];
  }

}

void displayMessage(byte dig[4]) {
  for (int i = 0; i < 4; i++) { // Каждый разряд по очереди
    Serial.println(dig[4], BIN);
    for (int k = 0; k < 8; k++) {// Каждый сегмент по очереди - исходя из заданной карты
      digitalWrite(segmentsPins[k], ((bitRead(dig[i], 7 - k) == 1) ? LOW : HIGH));
    }
    digitalWrite(anodPins[3 - i], HIGH); // 3-i - при перевороте инвертируем порядок пинов
    delay(1);
    digitalWrite(anodPins[3 - i], LOW); // 3-i - при перевороте инвертируем порядок пинов
  }
  Serial.println("==========");
}