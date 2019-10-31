/*
 * flow_sensor.c
 *
 * Created: 31.10.2019 
 * Author: Cemal BAYRAKTAR
 */ 

#include "flow_sensor.h"
#include <avr/io.h>
#include <avr/interrupt.h>

void flow_init(){
	//�nce timer1 ve timer2'nin kontrol ayarlar�n� s�f�rl�yoruz.
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR2A = 0;
	TCCR2B = 0;
	
	//timer1 ve timer2 ' nin �al��acaklar� modlara g�re ayarlar� yap�l�yor.
	sei();
	TIMSK1 |= (1<<TOIE1);			  //(PD5 pinindekn gelen harici darbeler i�in) say�c� ta�mas� olunca kesmeye gitsin diye bu bit setlendi.
	TCCR2A |= (1<<WGM21);			  //CTC moduna al�nd�.
	OCR2A   = 124;					  //1ms'de bir kesmeye gitmesi i�in saymas� gereken de�er.(16Mhz i�in)
	TIMSK2 |= (1<<OCIE2A);			  //TC2 e�le�me kesmesini aktif yapt�k.
	
	TCNT1 = 0;
	TCNT2 = 0;						  //saya�lar s�f�rland�.
	GTCCR |= (1<<PSRASY);			  //TC2 i�in �n derecelendirme(b�lme fakt�r�) s�f�rland�.
}

void flow_olcum_yap(int ms){
	saymaBittiMi = false;			  //sayma bitene kadar false olacak.
	olcumPeriyotu = ms;				  //ne kadar s�re boyunca �l��m yapaca�� belirlenir.
	tasmaSayisi = 0;                  //sayaclar s�f�rlan�yor.
	timerTick = 0;
	
	//TC1 ve TC2'yi ba�latmak i�in CS bitlerine yaz�yoruz;
	TCCR1B |= (1<<CS10) | (1<<CS11) | (1<<CS12);				//TC1 ba�lar.(harici saat darbelerinin y�kselen kenar�nda kesmeye gidecek)
	TCCR2B |= (1<<CS20) | (1<<CS22);							//TC2 ba�lar.(128 b�lme fakt�r� ayarland� -> 1ms de bir kesmeye gitmek i�in)
}

ISR (TIMER1_OVF_vect){			//TC1 ta�ma kesmesi alt program�
	tasmaSayisi++;				//her ta�mada saya� bir artacak.(sayac�n 1 artmas�, 65565 tane i�aret sayd� ve ta�t� demek)
}

ISR (TIMER2_COMPA_vect){		//TC2 e�le�me kesmesi alt program�. Program her 1 ms de bir buraya gelir.
	
	//kesmeye gelir gelmez TC1 saya� de�erini okuyoruz. �retici firma tavsiyesi.(Datasheet'te yaz�yor)
	unsigned int timer1SaymaDegeri;
	timer1SaymaDegeri = TCNT1;										//o an ki saya� de�erini ald�k.
	unsigned long tasmaSayisiCopy = tasmaSayisi;					//toplam tasma say�s�n� ald�k.
	
	//TC1 sayac de�erini okudukran sonra as�l i�lemleri yap�yoruz.
	if(++timerTick < olcumPeriyotu)									//olcumPeriyotu kadar s�re ge�ti mi?
	return;															//ge�mediyse alt programdan ��k.
	
	if((TIFR1 & _BV(TOV1)) && timer1SaymaDegeri <256 )				//s�re ge�ti ise buraya gelcek. ama kesmedeyken bir ta�ma ka��r�lma
	tasmaSayisiCopy++;												//ihtimalinden �t�r� bu sorgu yap�l�yor..
	
	//art�k �l��m hesab�na haz�r�z;
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR2A = 0;
	TCCR2B = 0;			//T�m Timer lar� durdurduk
	
	TIMSK1 = 0;
	TIMSK2 = 0;			//kesmeleri devre d��� b�rakt�k
	
	//�imdi �l��len toplam darbe say�s�n� hesaplayal�m;
	zamanSayaci = (tasmaSayisiCopy<<16) + timer1SaymaDegeri;		//�l�len toplam darbe say�s�=(toplam ta�ma say�s�*(65565) + son TCNT1 de�eri) dir.
	// (<<16) demek, 65565 ile �arpmak demek.
	saymaBittiMi = true;											//sayma i�lemi bitti. �l��m de�erimiz zamanSayaci'nda �u an.
}

	