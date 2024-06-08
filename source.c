#include<reg51.h> 
#define LCD_DATA P2 
//LCD 
sbit RS=P3^2;  
sbit EN=P3^3; 
//ADC0804 
sbit RD_ADC=P3^6; //read adc 
sbit WR_ADC=P3^5; //wr adc 
sbit I_ADC=P3^4; // Interupt adc 
//Fan 
sbit FAN1 = P3^0; 
sbit FAN2 = P3^1; 
 
 
void timer(void);               
void Delay_ms(unsigned int);  
void LCD_init(void);  
void LCD_CMD(unsigned char); 
void LCD_Char_Cp(char); 
void LCD_String (char *); 
 
void timer()//tao ngat tran timer voi ck 1ms 
{ 
    TMOD = 0x01;         
    TH0=0xFC;               
    TL0=0x17; 
    TR0 = 1;               // bat timer 
    while (TF0 == 0);       
    TR0 = 0;               // dung timer 
    TF0 = 0;               // xóa flag ve 0 
} 
void Delay_ms(unsigned int count)// delay_ms 
{ 
    unsigned int i; 
    for(i=0;i<count;i++) 
    { 
        timer(); 
    } 
} 
//ghi lenh ra LCD 
void LCD_CMD(unsigned char cmd) 
{ 
    RS=0;         //Ghi lenh 
    LCD_DATA=cmd; //gui CMD ra LCD 
    EN=1;         //Cho phep 
    EN=0;          
    if(cmd<=0x02) // cho nay nhin lai tap lenh cua LCD là hieu 
    { 
        Delay_ms(2); 
    } 
    else
        Delay_ms(1); 
}
//hien thi ky tu tai vi tri con tro (Cp=current position) 
void LCD_Char_Cp(char char_dat) 
{ 
    RS=1; // ghi du lieu  
    LCD_DATA=char_dat; 
    EN=1; 
    EN=0; 
    Delay_ms(1); 
} 
// hien thi chuoi ky tu tai vi tri con tro 
void LCD_String(char *str)  
{ 
    while(*str) 
    { 
        LCD_Char_Cp(*str++); 
    } 
} 
void LCD_init (void) // ham khoi tao LCD 
{ 
    LCD_CMD(0x38); // giao tiep 8bit, 2 dong, font 5x7 
    LCD_CMD(0x0C); // bat hien thi va tat con tro 
    LCD_CMD(0x01); // xoa hien thi 
    LCD_CMD(0x80); // dua tro ve dau dong 1 
    LCD_CMD(0x06); // tu dong tang con tro 
} 
void convert_display(unsigned char value) //chuyen doi du lieu và xuat ra LCD 
{ 
    unsigned char x0,x1,x2,x3; 
    
    LCD_CMD(0xc9); 
    x0 = (value/100); 
    x0=x0+(0x30);  
    x1=((value % 100)/10);//chia lay phan nguyen roi gan vào x1 
    x1=x1+(0x30);  
    x2=value%10;  //chia lay phan du roi gan vào x2 
    x2=x2+(0x30); //chuyen kieu du lieu tu bin sang asci de hien thi ra lcd bang cách add 0x30 
    x3=0xDF;      //ma cua dau (°) 
    
    if(x0 != 0 + 0x30) 
        LCD_Char_Cp(x0); 
    if(x1 != 0 + 0x30) 
        LCD_Char_Cp(x1); // dua nhiet do ra LCD//chuc 
        LCD_Char_Cp(x2); // don vi 
        LCD_Char_Cp(x3); // dau (°) 
        LCD_Char_Cp('C');  
  
} 
void main() 
{ 
    unsigned char value; 
    LCD_init(); 
    LCD_String("MC Project"); 
    LCD_CMD(0xc0);
    LCD_String("Nhiet do: "); 
    P1=0xFF;//Port 1 là o mode input 
    I_ADC=1;     
    RD_ADC=1;   //dua chân rd len muc cao 
    WR_ADC=1;   //dua chan wr len muc cao 
    FAN1 = 1; 
    FAN2 = 1; 
    while(1)    
    {     
        WR_ADC=0;               //tao mot xung wr tu cao xuong thap de cho phep adc chuyen doi du lieu vao 
        Delay_ms(1); 
        WR_ADC=1; 

        while(I_ADC==1);        //doi cho dên khi ket thuc chuyen doi 
        RD_ADC=0;               //RD =0 doc du lieu tu ADC0804 
        value=P1;               //lay data tren port 1 luu vao bien value 
        value = value * 0.58822352941f + 0.5f; 
        if(value >= 25) 
        { 
            FAN1 = 1; 
            FAN2 = 0; 
        } 
        else 
        { 
            FAN1 = 1; 
            FAN2 = 1; 
        } 
        convert_display(value); //goi lenh chuyen doi và xuat ra LCD 
        Delay_ms(1000);         //tam nghi de den chu ki chuyen doi ke tiep 
        RD_ADC=1;               //RD=1 bat dau chu ki chuyen doi ke tiep 
    } 
} 
