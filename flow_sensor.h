/*
 * flow_sensor.h
 *
 *  Created: 31.10.2019 
 *  Author: Cemal BAYRAKTAR
 *
 *  For water flow sensor. (I used g1/2 inch) 
 */ 

#ifndef FLOW_SENSOR_H_
#define FLOW_SENSOR_H_
#include <stdbool.h>
#include <stdint.h>

volatile unsigned long zamanSayaci;		//toplam �l��m de�erini saklar.
volatile bool saymaBittiMi;				//sayma bitene kadar bekletmek i�in boolean bir de�i�ken kulland�k.
unsigned long tasmaSayisi;				//tasma say�s�n� saklar. toplam darbe say�s�n� hesaplarken kullanaca��z.
unsigned int  timerTick;				//sayac olarak kullancaz.
unsigned int olcumPeriyotu;				//Harici darbeyi �l�me s�remiz.
char printbuff [4];						//float sonucu stringe �evirip LCD'yazd�rmak i�in kullan�lacak. ("dtostrf()" fonksiyonu i�in laz�m)

void flow_init();
void flow_olcum_yap(int ms);


#endif /* FLOW_SENSOR_H_ */