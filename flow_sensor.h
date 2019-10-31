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

volatile unsigned long zamanSayaci;		//toplam ölçüm deðerini saklar.
volatile bool saymaBittiMi;				//sayma bitene kadar bekletmek için boolean bir deðiþken kullandýk.
unsigned long tasmaSayisi;				//tasma sayýsýný saklar. toplam darbe sayýsýný hesaplarken kullanacaðýz.
unsigned int  timerTick;				//sayac olarak kullancaz.
unsigned int olcumPeriyotu;				//Harici darbeyi ölçme süremiz.
char printbuff [4];						//float sonucu stringe çevirip LCD'yazdýrmak için kullanýlacak. ("dtostrf()" fonksiyonu için lazým)

void flow_init();
void flow_olcum_yap(int ms);


#endif /* FLOW_SENSOR_H_ */