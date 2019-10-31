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
	//önce timer1 ve timer2'nin kontrol ayarlarýný sýfýrlýyoruz.
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR2A = 0;
	TCCR2B = 0;
	
	//timer1 ve timer2 ' nin çalýþacaklarý modlara göre ayarlarý yapýlýyor.
	sei();
	TIMSK1 |= (1<<TOIE1);			  //(PD5 pinindekn gelen harici darbeler için) sayýcý taþmasý olunca kesmeye gitsin diye bu bit setlendi.
	TCCR2A |= (1<<WGM21);			  //CTC moduna alýndý.
	OCR2A   = 124;					  //1ms'de bir kesmeye gitmesi için saymasý gereken deðer.(16Mhz için)
	TIMSK2 |= (1<<OCIE2A);			  //TC2 eþleþme kesmesini aktif yaptýk.
	
	TCNT1 = 0;
	TCNT2 = 0;						  //sayaçlar sýfýrlandý.
	GTCCR |= (1<<PSRASY);			  //TC2 için ön derecelendirme(bölme faktörü) sýfýrlandý.
}

void flow_olcum_yap(int ms){
	saymaBittiMi = false;			  //sayma bitene kadar false olacak.
	olcumPeriyotu = ms;				  //ne kadar süre boyunca ölçüm yapacaðý belirlenir.
	tasmaSayisi = 0;                  //sayaclar sýfýrlanýyor.
	timerTick = 0;
	
	//TC1 ve TC2'yi baþlatmak için CS bitlerine yazýyoruz;
	TCCR1B |= (1<<CS10) | (1<<CS11) | (1<<CS12);				//TC1 baþlar.(harici saat darbelerinin yükselen kenarýnda kesmeye gidecek)
	TCCR2B |= (1<<CS20) | (1<<CS22);							//TC2 baþlar.(128 bölme faktörü ayarlandý -> 1ms de bir kesmeye gitmek için)
}

ISR (TIMER1_OVF_vect){			//TC1 taþma kesmesi alt programý
	tasmaSayisi++;				//her taþmada sayaç bir artacak.(sayacýn 1 artmasý, 65565 tane iþaret saydý ve taþtý demek)
}

ISR (TIMER2_COMPA_vect){		//TC2 eþleþme kesmesi alt programý. Program her 1 ms de bir buraya gelir.
	
	//kesmeye gelir gelmez TC1 sayaç deðerini okuyoruz. Üretici firma tavsiyesi.(Datasheet'te yazýyor)
	unsigned int timer1SaymaDegeri;
	timer1SaymaDegeri = TCNT1;										//o an ki sayaç deðerini aldýk.
	unsigned long tasmaSayisiCopy = tasmaSayisi;					//toplam tasma sayýsýný aldýk.
	
	//TC1 sayac deðerini okudukran sonra asýl iþlemleri yapýyoruz.
	if(++timerTick < olcumPeriyotu)									//olcumPeriyotu kadar süre geçti mi?
	return;															//geçmediyse alt programdan çýk.
	
	if((TIFR1 & _BV(TOV1)) && timer1SaymaDegeri <256 )				//süre geçti ise buraya gelcek. ama kesmedeyken bir taþma kaçýrýlma
	tasmaSayisiCopy++;												//ihtimalinden ötürü bu sorgu yapýlýyor..
	
	//artýk ölçüm hesabýna hazýrýz;
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR2A = 0;
	TCCR2B = 0;			//Tüm Timer larý durdurduk
	
	TIMSK1 = 0;
	TIMSK2 = 0;			//kesmeleri devre dýþý býraktýk
	
	//þimdi ölçülen toplam darbe sayýsýný hesaplayalým;
	zamanSayaci = (tasmaSayisiCopy<<16) + timer1SaymaDegeri;		//ölçlen toplam darbe sayýsý=(toplam taþma sayýsý*(65565) + son TCNT1 deðeri) dir.
	// (<<16) demek, 65565 ile çarpmak demek.
	saymaBittiMi = true;											//sayma iþlemi bitti. Ölçüm deðerimiz zamanSayaci'nda þu an.
}

	