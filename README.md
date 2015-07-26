# Arduino7SegmentsIndicator
Вывод на 7 сегментный 4х разрядный индикатор с помощью Ардуинки

Распаковать папку Arduino7SegmentsIndicator в папку libraries у установленной Arduino IDE

Пример использования:
```
#include <SegmentsIndicator.h>

/*
* Первым параметром - количество разрядов + количество сегментов. 
* Если используется стандартный индикатор, то должно стоять 12
* Дальше через запятую пины разрядов
* и пины сегментов
* кстати, A3,A5,A2 - значит аналоговое пины
*                                          D4  D3  D2  D1    A   B   C  D   E   F   G   H
*/
SegmentsIndicator segmentsIndicator(12,   13, 12 , 11, 10,   1, A5, A3, 3,  2,  A2, A4, 4);



void setup() 
{
  segmentsIndicator.displayVal(-22, 1); //-- Подготавливаем значение первый параметр - значение, второй - положение точки
}



void loop() 
{
 
  segmentsIndicator.displayRefresh(); //-- Отображаем значение на индикаторе.
  delay(5); //-- Дадим время разгорется сегментам индикатора

}
```

Подробнее: http://pavelk.ru/arduino-ispolzovanie-7-ili-8-segmentnyx-4x-razryadnyx-indikatorov
