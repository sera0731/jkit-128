#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define MOTOR_CW	0x80
#define MOTOR_CCW	0x40
#define MOTOR_STOP1	0x00
#define MOTOR_STOP2	0x11

volatile int count = 0;	// 전역변수(Global Variable)

//FND
unsigned char digit[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
unsigned char fnd_sel[4] = {0x01, 0x02, 0x04, 0x08}; 

// sec만큼 딜레이
void delay_sec(float sec)
{
    int i;
    for(i=0; i<1000*sec; i++)
    	_delay_ms(1);
}


// count 증가
SIGNAL(SIG_INTERRUPT4)
{	
   	count++;
	_delay_ms(10);
}

// 모터 동작 및 count 초기화
SIGNAL(SIG_INTERRUPT5)
{	
	int i, length;

	// 모터 시작
	PORTB = MOTOR_CW;

	// 모터를 count초만큼 동작
	length = count*1000/4;

	for (i = 0; i < length; i++) {
		display_char();
	}

	// 모터 정지
	PORTB = MOTOR_STOP1;
	
	// 기어모터 제어 
	PORTF = 0x01;
	delay_sec(0.5);
	PORTF = 0xfe;
	delay_sec(0.5);
	PORTF = 0x00;
	
	count=0;

}
//count 감소
SIGNAL(SIG_INTERRUPT6)
{	
	if(count<0){
	 	count=0;
		return;
	}
   	count--;
	_delay_ms(10);
}
//count 초기화
SIGNAL(SIG_INTERRUPT7)
{	
   	count=0;
	_delay_ms(10);
}

// ^__^ 표정 출력
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

// 숫자 출력
void display_fnd(int count)
{	
	int i, fnd[4];
		
	fnd[3] = (count/1000)%10;	// 천의 자리
	fnd[2] = (count/100)%10;	// 백의 자리
	fnd[1] = (count/10)%10;		// 십의 자리
	fnd[0] = count%10;	// 일의 자리

	for (i=0; i<4; i++)
	{
		PORTC = digit[fnd[i]];
		PORTG = fnd_sel[i];
		_delay_ms(1);
	}
}

//포트의 입출력을 초기화
void init_module(){

	// 기어모터 확장 모듈 신호
 	DDRF = 0x00;
	// C 포트는 FND 데이터 신호
	DDRC = 0xff;	
	// G 포트는 FND 선택 신호
	DDRG = 0x0f;
	// E포트 (스위치)를 0000 1111 입력으로 설정
	DDRE = 0xff;
	// 모터의 포트번호 PB7-6를 출력으로 설정
	DDRB = 0xf0;

}

void set_interrupt(){

	//External Interrupt Mask Register
	//INT 7~0 Bit를 set(1) 시키면 해당 외부 interrupt PIN이 활성화됨. 

	// INT4~7 interrupt enable
	EIMSK = 0xff;

	//External Interrupt Control Register A/B

	// INT0~3 = 상승엣지 사용	
	EICRA = 0xff;
	// INT4~7 = 상승엣지 사용
	EICRB = 0xff;

	//Status Register 
	//bit 7 = I (Interrupt Enable)
	//SREG |= 1<<7;

	// 인터럽트 사용
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
