#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define MOTOR_CW	0x80
#define MOTOR_CCW	0x40
#define MOTOR_STOP1	0x00
#define MOTOR_STOP2	0x11

volatile int count = 0;	// ��������(Global Variable)

//FND
unsigned char digit[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
unsigned char fnd_sel[4] = {0x01, 0x02, 0x04, 0x08}; 

// sec��ŭ ������
void delay_sec(float sec)
{
    int i;
    for(i=0; i<1000*sec; i++)
    	_delay_ms(1);
}


// count ����
SIGNAL(SIG_INTERRUPT4)
{	
   	count++;
	_delay_ms(10);
}

// ���� ���� �� count �ʱ�ȭ
SIGNAL(SIG_INTERRUPT5)
{	
	int i, length;

	// ���� ����
	PORTB = MOTOR_CW;

	// ���͸� count�ʸ�ŭ ����
	length = count*1000/4;

	for (i = 0; i < length; i++) {
		display_char();
	}

	// ���� ����
	PORTB = MOTOR_STOP1;
	
	// ������ ���� 
	PORTF = 0x01;
	delay_sec(0.5);
	PORTF = 0xfe;
	delay_sec(0.5);
	PORTF = 0x00;
	
	count=0;

}
//count ����
SIGNAL(SIG_INTERRUPT6)
{	
	if(count<0){
	 	count=0;
		return;
	}
   	count--;
	_delay_ms(10);
}
//count �ʱ�ȭ
SIGNAL(SIG_INTERRUPT7)
{	
   	count=0;
	_delay_ms(10);
}

// ^__^ ǥ�� ���
void display_char()
{
	int i,fnd[4];

	fnd[3] = 0x23;
	fnd[2] = 0x08;
	fnd[1] = 0x08;
	fnd[0] = 0x23;

	for(i=0;i<4;i++)
	{
		PORTC = fnd[i];
		PORTG = fnd_sel[i];
		_delay_ms(1);
	}
}

// ���� ���
void display_fnd(int count)
{	
	int i, fnd[4];
		
	fnd[3] = (count/1000)%10;	// õ�� �ڸ�
	fnd[2] = (count/100)%10;	// ���� �ڸ�
	fnd[1] = (count/10)%10;		// ���� �ڸ�
	fnd[0] = count%10;	// ���� �ڸ�

	for (i=0; i<4; i++)
	{
		PORTC = digit[fnd[i]];
		PORTG = fnd_sel[i];
		_delay_ms(1);
	}
}

//��Ʈ�� ������� �ʱ�ȭ
void init_module(){

	// ������ Ȯ�� ��� ��ȣ
 	DDRF = 0x00;
	// C ��Ʈ�� FND ������ ��ȣ
	DDRC = 0xff;	
	// G ��Ʈ�� FND ���� ��ȣ
	DDRG = 0x0f;
	// E��Ʈ (����ġ)�� 0000 1111 �Է����� ����
	DDRE = 0xff;
	// ������ ��Ʈ��ȣ PB7-6�� ������� ����
	DDRB = 0xf0;

}

void set_interrupt(){

	//External Interrupt Mask Register
	//INT 7~0 Bit�� set(1) ��Ű�� �ش� �ܺ� interrupt PIN�� Ȱ��ȭ��. 

	// INT4~7 interrupt enable
	EIMSK = 0xff;

	//External Interrupt Control Register A/B

	// INT0~3 = ��¿��� ���	
	EICRA = 0xff;
	// INT4~7 = ��¿��� ���
	EICRB = 0xff;

	//Status Register 
	//bit 7 = I (Interrupt Enable)
	//SREG |= 1<<7;

	// ���ͷ�Ʈ ���
	sei();

}


int main(void)
{	
	init_module();
	set_interrupt();

	while (1){
		display_fnd(count);	// FND Display
	}
	
	
}
