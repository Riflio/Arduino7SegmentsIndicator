#include <SegmentsIndicator.h>


/*
* Первым параметром - количество разрядов + количество сегментов. 
* Если используется стандартный индикатор, то должно стоять 12
* Дальше через запятую пины разрядов
* и пины сегментов
* кстати, A3,A5,A2 - значит аналоговое пины
*/
SegmentsIndicator segmentsIndicator(12,   13, 12 , 11, 10, 1, A5, A3, 3, 2, A2, A4, 4);



void setup() 
{

  segmentsIndicator.displayVal(-22, 1); //-- Подготавливаем значение первый параметр - значение, второй - положение точки
   
}



void loop() 
{
  
  segmentsIndicator.displayRefresh(); //-- Отображаем значение на индикаторе.
  delay(5); //-- Дадим время разгорется сегментам индикатора

}
