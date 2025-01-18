String regime = "полив"; //поилка или полив
String state = "выкл"; //выкл или вкл
int timer = 3; //длительность полива в секундах
int period = 15; //период между поливами в секундах
unsigned long start_period = 0;
unsigned long start_timer = 0;
//задаем номера пинов ардуино
int zummer = 7;
int ttp223_base = 5;
int ttp223_down = 11;
int ttp223_up = 10;
int light_poilka = 2;
int light_poliv = 3;
int pump = 4;
int light_vkl = 8;
int pump_level = LOW;


void setup() {
  pinMode(light_poliv, OUTPUT); //светодиод полив
  pinMode(light_poilka, OUTPUT); //светодиод поилка
  pinMode(light_vkl, OUTPUT); //светодиод статуса - включен или нет
  pinMode(pump, OUTPUT); //насос
  pinMode(zummer, OUTPUT); //зуммер
  pinMode(ttp223_down, INPUT); //ttp223 нижний
  pinMode(ttp223_up, INPUT); //ttp223 верхний
  pinMode(ttp223_base, INPUT); //ttp223 базовый для проверки уровня воды в емкости-источнике воды
  Serial.begin(9600);
  digitalWrite(light_poliv, LOW);
  digitalWrite(light_poilka, LOW);
  digitalWrite(light_vkl, LOW);
}

void loop() {
  ////////////////////получение команд
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.toLowerCase();
    cmd.trim();
    
    if (cmd == "статус") {

      String status = state + "|" + regime + "|"  + period  + "|" + timer;
      Serial.println(status);

    } else if (cmd == "вкл все") {

      state = "вкл";
      digitalWrite(light_vkl, HIGH);
      if (regime == "поилка") {
        digitalWrite(light_poliv, LOW);
        digitalWrite(light_poilka, HIGH);
      } else {
        //режим полив
        digitalWrite(light_poliv, HIGH);
        digitalWrite(light_poilka, LOW);
      }

    } else if (cmd == "выкл все") {

      state = "выкл";
      digitalWrite(light_vkl, LOW);
      digitalWrite(pump, LOW);
      digitalWrite(zummer, LOW);
      pump_level = LOW;
      digitalWrite(light_poliv, LOW);
      digitalWrite(light_poilka, LOW);

    } else if (cmd == "поилка") {

      if (state == "вкл") {
        regime = "поилка";
        digitalWrite(light_poliv, LOW);
        digitalWrite(light_poilka, HIGH);
      } else {
        //система отключена - пищалку запускаем на секунду
        digitalWrite(zummer, HIGH);
        delay(1000);
        digitalWrite(zummer, LOW);
      }

    } else if (cmd == "полив") {

      if (state == "вкл") {
        regime = "полив";
        digitalWrite(light_poliv, HIGH);
        digitalWrite(light_poilka, LOW);
      }  else {
        //система отключена - пищалку запускаем на секунду
        digitalWrite(zummer, HIGH);
        delay(1000);
        digitalWrite(zummer, LOW);
      }

    } else if (cmd == "вода иди") {

      //проверка верхнего уровня воды в приемнике и уровня воды на источнике (можем и не включить воду)
      if (digitalRead(ttp223_up) == LOW && digitalRead(ttp223_base) == HIGH) {
        start_timer = millis();
        digitalWrite(pump, HIGH);
        pump_level = HIGH;
        ////////////////////////////
      } else {
        //вода переполнена - пищалку запускаем на секунду
        digitalWrite(zummer, HIGH);
        delay(1000);
        digitalWrite(zummer, LOW);
      }

    } else if (cmd == "вода стой") {

      //проверка пина насоса: если высокий уровень, то сохраняем время
      if (pump_level == HIGH) {
        start_period = millis();
        digitalWrite(pump, LOW);
        pump_level = LOW;
      } else {
        //вода не включена - пищалку запускаем на секунду
        digitalWrite(zummer, HIGH);
        delay(1000);
        digitalWrite(zummer, LOW);
      }

    } else if (cmd == "таймер") {

      while (Serial.available() <= 0) {
        //ожидаем прихода числа
        delay(1);
      }
      int n = Serial.parseInt();
      //записываем время
      if (n > 0) timer = n;

    } else if (cmd == "период") {

      while (Serial.available() <= 0) {
        //ожидаем прихода числа
        delay(1);
      }
      int n = Serial.parseInt();
      //записываем время
      if (n > 0) period = n;
      
    }

    delay(10);
  }
//-----------------------------------------------------------------------------------------//
  ////////////////////выполнение команд
  if (state == "вкл") {

    if (regime == "полив") {
      //режим полив
      if (pump_level == HIGH) {
        if (abs(millis() - start_timer) >= timer*1000 || digitalRead(ttp223_up) == HIGH) {
          //время полива вышло, выключаем насос
          start_period = millis();
          pump_level = LOW;
          digitalWrite(pump, LOW);
        }
      } else {
        //pump_level == LOW
        //ждем окончания периода для начала полива
        if (abs(millis() - start_period) >= period*1000) {
          //время ожидания полива вышло, включаем насос
          //проверка верхнего уровня воды в приемнике и уровня воды на источнике (можем и не включить воду)
          if (digitalRead(ttp223_up) == LOW && digitalRead(ttp223_base) == HIGH) {
            start_timer = millis();
            pump_level = HIGH;
            digitalWrite(pump, HIGH);
          } else {
            //не можем включить воду из-за уровня - пищалку запускаем на секунду
            digitalWrite(zummer, HIGH);
            delay(1000);
            digitalWrite(zummer, LOW);
            //откладываем попытку повторного включения насоса на 5 секунд 
            start_period = millis() - period*1000 + 5000; 
          }
        }
      }
    } else {
      //режим поилка
      //проверка нижнего уровня воды в приемнике и уровня воды на источнике
      if (digitalRead(ttp223_down) == LOW && digitalRead(ttp223_base) == HIGH) {
        pump_level = HIGH;
        digitalWrite(pump, HIGH);
      }
      //если насос включен и вода выше верхнего уровня в приемнике, то отключаем насос
      if (digitalRead(ttp223_up) == HIGH) {
        pump_level = LOW;
        digitalWrite(pump, LOW);
      }
    }

  }

  //-----------------------------------------------------------------------------------------//
}
