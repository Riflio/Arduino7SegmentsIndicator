#include <stdlib.h>
#include <math.h>
#include "SegmentsIndicator.h"
#include <stdlib.h>

SegmentsIndicator::SegmentsIndicator(byte digs[], byte segments[])
{
 
  	//-- Копируем переданный массивы в наши, что бы случайно не изменить.
	memcpy(rdigs, digs, sizeof(digs[0])*DIGS_COUNT); 
	memcpy(rsegs, segments, sizeof(segments[0])*SEGMENTS_COUNT);
	
	init();
}

//-- В первом параметре - количество параметров, Кэп!
SegmentsIndicator::SegmentsIndicator(int counter, ...) 
{
	va_list ap;
  	va_start(ap,counter);
  	byte i;

  	//-- Сначала заполним разряды
  	for (i=0; i<DIGS_COUNT; i++) {
  		rdigs[i] = va_arg(ap, int);
  	}
  	//-- Сегменты
  	for (i=0; i<SEGMENTS_COUNT; i++) {
  		rsegs[i] = va_arg(ap, int);
  	}
   
  	va_end (ap);
	
  	init();
}



void SegmentsIndicator::init() 
{

	byte portNum, portMask;
	byte i,j,k;

	//-- Создаём из указанных пинов и комбинаций их включения маски для каждого порта B-D 
	for (i=0; i < DIGITS_COUNT; i++) { //-- Пробегаемся по всем цифрам
		for (j=0; j < SEGMENTS_COUNT; j++) { //--Пробегаемся по каждому биту состава цифры      
      		if (digits[i] & (1 << j)) { //-- Если бит в наборе цифры установлен, то необходимо будет выставить HIGH на соответствующем сегменту индикатора пине
        		getPortMask(rsegs[j], portNum, portMask);        
        		displayDigitsPortsMask[portNum] |= portMask;//--заносим сразу в общую маску порта
        		displayDigitsMasks[ i * PORTS_COUNT + portNum] |= portMask; //-- Добавляем к существующей маске порта маску с выставленным пином
      		}  
    	}  
	}
   
	//--Пины разрядов то же преобразуем в маски для портов 
	for (i=0; i < DIGS_COUNT; i++) {
		getPortMask(rdigs[i] , portNum, portMask);	
		displayDigsPortsMask[portNum] |= portMask;	//-- заносим сразу в общую маску порта		
		displayDigsMask[i * PORTS_COUNT + portNum] |= ~portMask; //-- так как с общим катодом, то инвертируем
	}

	//-- собираем общие маски символов и разрядов в одну общую маску
	for(i=0; i < PORTS_COUNT; ++i) {
		displayPortsMask[i] |= displayDigsPortsMask[i];
		displayPortsMask[i] |= displayDigitsPortsMask[i];
	}
	

	//-- выставляем режимы пинов
	for(i=0; i < DIGS_COUNT; ++i) {
		pinMode(rdigs[i], OUTPUT);
		digitalWrite(rdigs[i], LOW); 
	}
	for(i=0; i < SEGMENTS_COUNT; ++i) {
		pinMode(rsegs[i], OUTPUT);
		digitalWrite(rsegs[i], LOW); 
	}


}


/*
* Переходим от работы с номерами пинов к портам 
* У каждого порта есть диапазон номеров пинов, например у PORTB он 8 до 13 номера цифрового пина
* Что бы установить HIGH или LOW нужно соответственно диапазону задать битовую маску
* маска состоит из 8 бит  
* т.е. например хотим мы установить пин с номером 9 и 12 в HIGH остальные в LOW
* вспоминаем, что диапазон для порта B начинается с 8
* но так как маска 8 битная нужно перейти от номера пина к ссответствующей номеру позиции в битовой маске, поэтому из номера необходимо вычесть 8
* и не забываем, что битовые маски пишутся справа налево.
* в общем маска бдет выглядеть так:  0010010
* а непосредственно установка будет вот так: PORTB = B00010010; "B" в начале записи просто означает, что это двоичная система счисления.
* Ещё нужно очень много сказать про работу с битовыми операциями... но это тема отдельного поста и 9 класса информатики =)
* Рекомендую так же прочитать http://www.arduino.cc/en/Reference/PortManipulation 
*/

void SegmentsIndicator::getPortMask(byte pinNum, byte & portNum, byte & portMask)
{
	byte p; //-- Номер порта 
	byte n; //-- Позиция бита в маске
  
	//-- Берём нужный пин для сегмента и проверяем какому порту он принадлежит
	if (pinNum<=7) {  //-- PORTD  от 0 до 7 включительно      
		p=2; n=pinNum; 
	} else if (pinNum<=13) { //--PORTB от 8 до 13 включительно
		p=0; n=pinNum-8; 
	} else if (pinNum<=19) { //--PORTC от 14 до 19 включительно
		p=1; n=pinNum-14; 
	} 

	portNum   = p;
	portMask  = (1 << n);  
}


/**
* Подготавливаем значение для дисплея 
* val - целочисленное значение
* dot - положение точки 
* digit - если установлен, то выводится указанный символ из набора digits
*/
void SegmentsIndicator::displayVal(long int val, int dot) 
{
	long int displayDig = 1; //-- Для перевода из порядкого номера разряда в 10, 100, 1000 и тд 
	int digit;
	
	for (byte dig=0; dig < DIGS_COUNT; dig++, displayDig*=10) { //-- Пробежимся по всем разрядам    	
		digit = -1; 
		
		if ((val/displayDig==0 )) { //-- Если разряды в числе закончились и число не ноль
			if (( dig > dot ) ) { //-- Пока текущий разряд не станет большего разряда, в котором должна быть точка. 
				
				if (val<0 && ( val/(displayDig/10)!=0 || dig==dot+1  ) ) { //-- Число отрицательно, в предыдущем разряде что-то есть или на предыдущем шаге был поставлена точка, то самое время отобразить минус
					digit=12;         
				} else			
				
				if (dig==0 && val==0) {
					digit = 1; //-- Если число 0, то просто выведем 0 в первом разряде и всё.
				} else //-- Устанавливаем пустой символ, т.к. разряды установлены, минус и точка установлены.
					digit = 0; 	
				
			} else { //-- Дополняем слева нулями
				digit=1;
			}    
			
			
		}
		digit =  (digit>=0)? digit : ((abs(val) / displayDig % 10)+ 1); //-- Если мы заранее не установили какой символ сейчас выводить, то получаем цифру по разряду из числа

	     
		//-- Запоминаем значения для портов. Цифра и её разряд 	   
		byte pp =  digit * PORTS_COUNT;
		byte pd =  dig * PORTS_COUNT;
	    
		byte portb = 0;
		byte portc = 0;
		byte portd = 0;

		//-- 
		makeDisplayResultPortMask(portb, displayDigitsPortsMask[0], displayDigitsMasks[pp + 0]);
		makeDisplayResultPortMask(portc, displayDigitsPortsMask[1], displayDigitsMasks[pp + 1]);
		makeDisplayResultPortMask(portd, displayDigitsPortsMask[2], displayDigitsMasks[pp + 2]);
		//--
		makeDisplayResultPortMask(portb, displayDigsPortsMask[0], displayDigsMask[pd + 0]);
		makeDisplayResultPortMask(portc, displayDigsPortsMask[1], displayDigsMask[pd + 1]);
		makeDisplayResultPortMask(portd, displayDigsPortsMask[2], displayDigsMask[pd + 2]);
      
		if (dig==dot) { //-- Есть точка в этом разряде добавим её маску к маске цифры
			pp = 11 * PORTS_COUNT;
			portb |= displayDigitsMasks[pp + 0]; 
			portc |= displayDigitsMasks[pp + 1];
			portd |= displayDigitsMasks[pp + 2];
		} 
	        
		displayData[pd + 0] = portb;
		displayData[pd + 1] = portc;
		displayData[pd + 2] = portd;
     
	}
  
	
}

/**
* Отображаем указанные символы из таблицы digits для каждого разряда 
* counter - количество  
* а дальше непосредственно номера символов
*/
void SegmentsIndicator::displaySymbol2(int counter, ...)
{
	va_list ap;
  	va_start(ap, counter);
  	byte i;
  	
  	for (i=counter; i>0; i--) {  		
		
		byte pp =  va_arg(ap, int) * PORTS_COUNT;
		byte pd =  (i-1) * PORTS_COUNT;
		
		byte portb = 0;
		byte portc = 0;
		byte portd = 0;

		//-- 
		makeDisplayResultPortMask(portb, displayDigitsPortsMask[0], displayDigitsMasks[pp + 0]);
		makeDisplayResultPortMask(portc, displayDigitsPortsMask[1], displayDigitsMasks[pp + 1]);
		makeDisplayResultPortMask(portd, displayDigitsPortsMask[2], displayDigitsMasks[pp + 2]);
		//--
		makeDisplayResultPortMask(portb, displayDigsPortsMask[0], displayDigsMask[pd + 0]);
		makeDisplayResultPortMask(portc, displayDigsPortsMask[1], displayDigsMask[pd + 1]);
		makeDisplayResultPortMask(portd, displayDigsPortsMask[2], displayDigsMask[pd + 2]);
		
		displayData[pd + 0] = portb;
		displayData[pd + 1] = portc;
		displayData[pd + 2] = portd;
		
  	}
  	   
  	va_end (ap);	
}

/**
* Отображаем указанные символы из таблицы digits для каждого разряда 
* symbol - Символ из таблицы digits
* digit - в каком разряде
* metod - по какому методу ставим:  0 - заменяем символ, 1- дополняем новым
* а дальше непосредственно номера символов
*/
void  SegmentsIndicator::displaySymbol(byte symbol, byte digit, byte metod)
{
	byte pp =  symbol * PORTS_COUNT;
	byte pd =  digit * PORTS_COUNT;
		
	byte portb = 0;
	byte portc = 0;
	byte portd = 0;

	//-- 
	makeDisplayResultPortMask(portb, displayDigitsPortsMask[0], displayDigitsMasks[pp + 0]);
	makeDisplayResultPortMask(portc, displayDigitsPortsMask[1], displayDigitsMasks[pp + 1]);
	makeDisplayResultPortMask(portd, displayDigitsPortsMask[2], displayDigitsMasks[pp + 2]);
	//--
	makeDisplayResultPortMask(portb, displayDigsPortsMask[0], displayDigsMask[pd + 0]);
	makeDisplayResultPortMask(portc, displayDigsPortsMask[1], displayDigsMask[pd + 1]);
	makeDisplayResultPortMask(portd, displayDigsPortsMask[2], displayDigsMask[pd + 2]);
	
	if (metod == 1) { //-- Дополняем существующий символ заданным
		portb |= displayData[pd + 0];
		portc |= displayData[pd + 1];
		portd |= displayData[pd + 2];
	}
	
	//-- Иначе оставляем как есть, т.е. заменяем
	
	
	displayData[pd + 0] = portb;
	displayData[pd + 1] = portc;
	displayData[pd + 2] = portd;
}


/**
* Делаем итоговые данные по маске и самих данных
*/
void SegmentsIndicator::makeDisplayResultPortMask(byte & portData, byte portMask, byte data)
{
	byte a = portData;
	byte b = portMask;
	byte c = data;
	byte result = 0;

	/*
	* Если вкратце, то нам нужно установить биты 0 или 1 только в тех местах, 
	* которые соответствуют нашим пинам по заданной маске, что бы не затронуть остальные, которые уже могли прийти
	* Для этого сначала установим на нужные места нули,
	* затем на нужные места установим еденицы	
	* В общем, магия, бля...
	*/

	//result =  a & ~ (b & ~ c) | (b & c);	
	
	//--упростим слегонца
	result =  a & ~ b | (b & c);		


	portData = result;
}

/**
* Обёртка над makeDisplayResultPortMask(byte & portData, byte portMask, byte data)
*/
byte SegmentsIndicator::makeDisplayResultPortMask(volatile int8_t portData, byte portMask, byte data)
{
	byte a = portData;
	makeDisplayResultPortMask(a, portMask, data);
	return a;
}

/*
* Отрисовываем значение на индикаторе
*
*/
void SegmentsIndicator::displayRefresh() 
{
	//-- В один момент должен быть включён только один разряд индикатора  
	static byte dig =0;  
  
	if (dig >= DIGS_COUNT) dig=0;
  
	//-- Записываем в порты
	byte pd = dig * PORTS_COUNT;

  	PORTB = makeDisplayResultPortMask(PORTB, displayPortsMask[0], displayData[pd + 0]);
	PORTC = makeDisplayResultPortMask(PORTC, displayPortsMask[1], displayData[pd + 1]);
	PORTD = makeDisplayResultPortMask(PORTD, displayPortsMask[2], displayData[pd + 2]);

	dig++;
  
}

//-- Очищаем индикатор, т.е. выводим ничего =) 
void SegmentsIndicator::clearDisplay()
{
	displaySymbol2(4, 0,0,0,0);	
}


//-- Балл-маскарад закончен феерично.
