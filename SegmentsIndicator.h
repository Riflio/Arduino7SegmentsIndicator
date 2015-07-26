/**
* SegmentsIndicator 
* ver 1.5
* Библиотека для вывода значений на сегментные индикаторы
* без жёсткой привязки к количеству сегментов, их разрядов и комбинаций возможных символов из сегментов
* Created by PavelK.ru 10.02.15
* Updated 26.07.15
*/

#ifndef SegmentsIndicator_h
#define SegmentsIndicator_h

#include <Arduino.h>

class SegmentsIndicator
{
  public:
	
	static const byte DIGS_COUNT = 4;//-- Количество разрядов
	static const byte SEGMENTS_COUNT = 8; //-- Количество сегментов для одного символа
	static const byte DIGITS_COUNT = 16; //-- Количество закодированных символов для каждого разряда из сегментов
	static const byte PORTS_COUNT = 3; //-- Количество портов ввода/вывода Ардуинки

	SegmentsIndicator(byte digs[], byte segments[]);
	SegmentsIndicator(int counter, ...);


	void displayVal(long int val, int dot);	//-- отображаем конкретное значение
	void displaySymbol(byte symbol, byte digit, byte metod=0); //-- отображаем символ в заданном разряде (metod 0 - просто добавляет к текущему, например полезно для точки)
	void displaySymbol2(int counter, ...); //-- отображаем за раз сразу несоколько символов counter - сколько именно, кэп
	void displayRefresh(); //-- перерисовываем для отображения следующего разряда
	void clearDisplay(); //-- действительно тут нужен коммент? =)
	

  private:
	
	byte rdigs[DIGS_COUNT]  = {0}; //--разряды  (4  3  2  1)
	byte rsegs[SEGMENTS_COUNT]  = {0}; //-- сегменты (A  B  C  D  E  F  G  H-точка) 

	byte digits[DIGITS_COUNT] = { //-- В какой комбинации устанавливать HIGH на пины, что бы получился символ.
	 0b00000000, //-- "" (none)
	 0b00111111, //-- 0
	 0b00000110,  
	 0b01011011, 
	 0b01001111, 
	 0b01100110, 
	 0b01101101, 
	 0b01111101, 
	 0b00000111, 
	 0b01111111, 
	 0b01101111, //-- 9
	 0b10000000, //-- "."
	 0b01000000, //-- "-"	 
	 0b00111001, //-- "C"
	 0b01111001,  //-- "E"
     0b00110111 //-- "П"
	}; 

	//-- Что бы не использовать двумерные массивы приведём к одномерному
	byte displayDigitsMasks[ DIGITS_COUNT * PORTS_COUNT] = {0}; //-- Распихиваем маски для каждого символа по портам A-D. 
	byte displayDigsMask[ DIGS_COUNT * PORTS_COUNT ]  = {0}; //-- Маски для казждого разряда
	byte displayData[ DIGS_COUNT * PORTS_COUNT ]  = {0}; //-- Запоминаем что сейчас отображается (итоговые маски для портов)
	

	byte displayPortsMask [ PORTS_COUNT ] = {0}; //-- Какие биты в портах должны быть затронуты итоговыми масками, что бы не заменить значения других пинов
	byte displayDigsPortsMask [ PORTS_COUNT ] = {0}; //-- аналогично общая маска только для разрядов
	byte displayDigitsPortsMask [ PORTS_COUNT ] = {0}; //-- аналогично общая маска только для символов

	void getPortMask(byte pinNum, byte & portNum, byte & portMask);

	void makeDisplayResultPortMask(byte & portData, byte portMask, byte data);
	byte makeDisplayResultPortMask(volatile int8_t portData, byte portMask, byte data);

	void init();
	
};


#endif
