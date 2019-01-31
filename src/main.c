#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
#include <stdlib.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"


//D portu kontrol pinleri olarak kullanılan RS(register select) ve E(enable) pinleri içindir.
void init_port_D(){

	volatile unsigned long tmp;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOD; // Port D’yi aktiflestirir.
	tmp = SYSCTL_RCGC2_R;  	// zaman gecirmek icin.

	GPIO_PORTD_DIR_R |= 0x03;	 // PortD 1,0 pinlerini cikis yapar.
	GPIO_PORTD_AFSEL_R &= ~0x03; // PortD 1,0 pinlerinin alternatif fonksiyonlarını kapatır.
	GPIO_PORTD_DEN_R |= 0x03;	 // PortD 1,0 pinlerini dijital olarak aktifleştirir.
}

//B portu Lcd 'nin 4 bitlik veri yolu içindir.
void init_PORT_B(){

	volatile unsigned long tmp;
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; // Port B’yi aktiflestirir
	tmp = SYSCTL_RCGC2_R;  	// zaman gecirmek icin.

	GPIO_PORTB_DIR_R |= 0x0F;    // PortB 3,2,1,0 pinlerini cikis yapar.
    GPIO_PORTB_AFSEL_R &= ~0x0F; // PortB 3,2,1,0 pinlerininin alternatif fonksiyonlarını kapatır.
	GPIO_PORTB_DEN_R |= 0x0F;	 // PortB 3,2,1,0 pinlerini dijital olarak aktifleştirir.
}

//E portu kırmızı, mavi ,yesil, beyaz ledler içindir.
void init_PORT_E(){

	volatile unsigned long tmp;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE; // Port E’yi aktiflestirir.
	tmp = SYSCTL_RCGC2_R;  	// zaman gecirmek icin

	GPIO_PORTE_DIR_R |= 0x0F;    // PortE 3,2,1,0 pinlerini cikis yapar.
	GPIO_PORTE_AFSEL_R &= ~0x0F; // PortE 3,2,1,0 pinlerininin alternatif fonksiyonlarını kapatır.
    GPIO_PORTE_DEN_R |= 0x0F;	 // PortE 3,2,1,0 pinlerini dijital olarak aktifleştirir.
}


//Bu fonksyon Lcd ile ilgili ayarları yapar.
void init_LCD()
{

	GPIO_PORTD_DATA_R=0x00; //Lcd 'nin RS ve E bağlantılarının bulunduğu portun değerlerini sıfır yapar.
	GPIO_PORTB_DATA_R=0x00; //Lcd 'nin veri bağlantılarının bulunduğu portun değerlerini sıfır yapar.

     SysCtlDelay(50000); // İlk komuttan önce biraz beklenir.

     Lcd_komut(0x03); //Lcd yi aktifeştirmek için 3 kez Lcd ye 0x03 değeri gönderilir.
     Lcd_komut(0x03);
     Lcd_komut(0x03);
     Lcd_komut(0x02); //Bir kez 0x02 degeri gönderilir.

     SysCtlDelay(50000); //bekleme

     Lcd_komut(0x28); // Lcd yi 4 bitlik 2 satırlık ve 5*7 li matris olarak ayarlar.
     Lcd_komut(0x06); //Entry Mode
     Lcd_komut(0x0C); //Display on cursor off
     Lcd_komut(0x01); //Display clear

}


//Lcd nin imlecini verilen satir ve sutuna götürür.
void Lcd_imlec(int satir, int sutun){

        if(satir==1)
                Lcd_komut(0x80+sutun);
        else
                Lcd_komut(0xC0+sutun);
}

//Gönderilen komutu Lcd nin 4 bitlik veri yoluna gönderir.
void Lcd_komut(unsigned char c)
{
    SysCtlDelay(50000);

	GPIO_PORTD_DATA_R &= ~(0x01);	 //Komut gönderimi içiçn RS pini 0 olarak ayarlanır.
	GPIO_PORTB_DATA_R=(c & 0xf0)>>4; //Önce en yüksek değerlikli 4 bit lcd nin 4 bitlik veri yoluna gönderilir.
	GPIO_PORTD_DATA_R |=0x02;        //Komutun yazılması için gereken negatif kenar tetiklemesi yapılır. (E=1 yapılır)
	GPIO_PORTD_DATA_R &=~(0x02);	 //(E=0 yapılır)

	SysCtlDelay(50000);

	GPIO_PORTD_DATA_R &= ~(0x01);
	GPIO_PORTB_DATA_R=c & 0x0f;      // Sonra en düşük değerlikli 4 bit veri yoluna gönderilir.
	GPIO_PORTD_DATA_R |=0x02;
	GPIO_PORTD_DATA_R &=~(0x02);




}

//Bu fonksiyon gelen veriyi lcd ye göndererek ekrana çıkmasını sağlar.
void Lcd_harf_yaz(unsigned char d)
{
    SysCtlDelay(50000);

	GPIO_PORTD_DATA_R |=(0x01); 	 // Veri gönderildiği için RS ucu 1 yapılır.
	GPIO_PORTB_DATA_R=(d & 0xf0)>>4;
	GPIO_PORTD_DATA_R |=0x02;
	GPIO_PORTD_DATA_R &=~(0x02);

	SysCtlDelay(50000);

	GPIO_PORTD_DATA_R |= (0x01);
	GPIO_PORTB_DATA_R=d & 0x0f;
	GPIO_PORTD_DATA_R |=0x02;
	GPIO_PORTD_DATA_R &=~(0x02);




}

//Gönderilen stringin tüm karakterleri \0 karakterine kadar tek tek ekrana yazdırılır.
void kelime_yaz(char * s)
{
	while((*s)!='\0')
	{
		Lcd_harf_yaz(*s);
		s++;
	}


}




void sagdan_sola_kayan_yazi(char * yazi,int satir){


	Lcd_imlec(satir,15);//Yazi verilen satirda 15. sütuna yazılır.
	kelime_yaz(yazi);

    int i=0;
	char * temp=yazi;

    while(*temp!='\0') //Bu döngü yazının kaç karakterden oluştuğunu hesaplar.
    {
		i++;
		temp++;
	}

	int kaydir=0;

	while(kaydir!=15+i) //karakter sayısı kadar lcd ekran kaydırılır.
	{
		SysCtlDelay(1500000);
		Lcd_komut(0x18);//bu komut lcd ekranı sagdan sola dogru kaydırır.
		kaydir++;
	}

}

void soldan_saga_kayan_yazi(char * yazi,int satir)
{

	Lcd_komut(0x08);//ekran kapatılır
	Lcd_imlec(satir,0);
	kelime_yaz(yazi);

	int i=0;
	char * temp=yazi;

	while(*temp!='\0')//bu döngüde yazının boyutu hesaplanır ve yazı önce sola dogru kaydırılır.
	{
	   i++;
	   temp++;
	   Lcd_komut(0x18);
	}


	int kaydir=0;
	Lcd_komut(0x0C);//ekran acılır

	while(kaydir!=15+i)//bu döngüde sola kaydıralarak gizlenen yazı saga dogru kaydırılır.
	{
		Lcd_komut(0x1C);//bu komut lcd ekranı soldan saga dogru kaydırır.
		SysCtlDelay(1500000);
		kaydir++;
	}



}


int main(void){

	init_port_D();
	init_PORT_B();
	init_PORT_E();
	init_LCD();

	int ledsayisi;

    srand(rand());

	while(1){


     ledsayisi =rand()%4;

     if(ledsayisi==0)//kırmızı
     {
    	 GPIO_PORTE_DATA_R |=0b00000001;

    	  Lcd_imlec(1,0);
    	 kelime_yaz("Kocaeli Uni");
    	 SysCtlDelay(SysCtlClockGet()/3*2);

    	 Lcd_imlec(2,0);
    	 kelime_yaz("Kubra Koksal");
    	 SysCtlDelay(SysCtlClockGet()/3*3) ;
    	 Lcd_komut(0x01);

    	 GPIO_PORTE_DATA_R &=~(0b00000001);


     }
     if(ledsayisi==1)//beyaz
     {
    	 GPIO_PORTE_DATA_R |=0b00000010;

    	 Lcd_imlec(2,0);
    	 kelime_yaz("Kubra Koksal");
    	 SysCtlDelay(SysCtlClockGet()/3*2);

    	 Lcd_imlec(1,0);
       	 kelime_yaz("Kocaeli Uni");
    	 SysCtlDelay(SysCtlClockGet()/(3)*3) ;
       	 Lcd_komut(0x01);

    	 GPIO_PORTE_DATA_R &=~(0b00000010);

     }
     if(ledsayisi==2)//yesil
     {
    	 GPIO_PORTE_DATA_R |=0b00000100;

       	 soldan_saga_kayan_yazi("Kocaeli Uni",1);
    	 Lcd_komut(0x01);
    	 SysCtlDelay(SysCtlClockGet()/3*2);

       	 soldan_saga_kayan_yazi("Kubra Koksal",2);
      	 Lcd_komut(0x01);
    	 SysCtlDelay(SysCtlClockGet()/(3)*3) ;

 	  	 GPIO_PORTE_DATA_R &=~(0b00000100);
     }
     if(ledsayisi==3)//mavi
     {
    	 GPIO_PORTE_DATA_R |=0b00001000;

    	 sagdan_sola_kayan_yazi("Kocaeli Uni",1);
    	 Lcd_komut(0x01);
    	 SysCtlDelay(SysCtlClockGet()/3*2);

      	 sagdan_sola_kayan_yazi("Kubra Koksal",2);
      	 Lcd_komut(0x01);
    	 SysCtlDelay(SysCtlClockGet()/(3)*3) ;

       	 GPIO_PORTE_DATA_R &=~(0b00001000);

     }


   }

}


