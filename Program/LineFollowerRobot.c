/**************************** Includes ****************************/

#include "inc/mega328p.h"
#include "inc/mega328p_bits.h"
#include "inc/delay.h"
#include "inc/math.h"
#include <stdbool.h>

/***************************** Defines ****************************/

#define     F_CPU                       20000000UL      // Clock frequency of quartz resonator

#define     QTY_OF_SENSORS              4                // Quantity of sensors

// Sensor order in the right -> direction
#define     ReadSensor1                 PINB & (1 << 2)
#define     ReadSensor2                 PINB & (1 << 1)
#define     ReadSensor3                 PINB & (1 << 0)
#define     ReadSensor4                 PIND & (1 << 7)
//#define     ReadSensor5                 PIND & (1 << 2)
//#define     ReadSensor6                 PIND & (1 << 4)
//#define     ReadSensor7                 PINB & (1 << 0)
//#define     ReadSensor8                 PIND & (1 << 7)
//#define     ReadSensor9                 PINx & (1 << x)
//#define     ReadSensor10                PINx & (1 << x)
//#define     ReadSensor11                PINx & (1 << x)
//#define     ReadSensor12                PINx & (1 << x)
//#define     ReadSensor13                PINx & (1 << x)
//#define     ReadSensor14                PINx & (1 << x)
//#define     ReadSensor15                PINx & (1 << x)
//#define     ReadSensor16                PINx & (1 << x)

//#define     ReadSensorIR                PINx & (1 << x)
//#define     MEncoderLeft                PINx & (1 << x)
//#define     MEncoderRight               PINx & (1 << x)

#define     LED_1_ON                      PIND |= (1 << DDD0)
#define     LED_1_OFF                     PIND &= ~(1 << DDD0)
#define     LED_2_ON                      PIND |= (1 << DDD1)
#define     LED_2_OFF                     PIND &= ~(1 << DDD1)
#define     LED_3_ON                      PIND |= (1 << DDD2)
#define     LED_3_OFF                     PIND &= ~(1 << DDD2)

/**************************** Function ****************************/

void InitSys (void) {

    // Motors
        DDRB |= (1 << DDB3);            //OC2A
        DDRD |= (1 << DDD6) |           //OC0A
                (1 << DDD5) |           //OC0B
                (1 << DDD3);            //OC2B

    // SensorLine
        DDRB &= ~((1 << DDB2) |
                (1 << DDB1) |
                (1 << DDB0));
        DDRD &= ~(1 << DDD7);

   // Infrared Sensor
          //DDRD &= ~(0 << DDD0);

   // LED
      	DDRD |= (1 << DDD2) | (1 << DDD1) | (1 << DDD0);


	// TIMER/COUNTER INITIALIZATION
		// Timer/Counter 0
		// Fast PWM Mode
        // Non-inverting mode
		// Top = 0xFF
		// Prescaler: 1:64
		TCCR0A |= (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
		TCCR0A &= ~((1 << COM0A0) | (1 << COM0B0) | (1 << 3) | (1 << 2));
		TCCR0B |= (1 << CS01) | (1 << CS00);
		TCCR0B &= ~((1 << FOC0A) | (1 << FOC0B) | (1 << 5) | (1 << 4) | (1 << WGM02) | (1 << CS02));
		TCNT0  = 0x00;
		OCR0A  = 0x00;    OCR0B  = 0x00;

		// Timer/Counter 1
        // Fast PWM 8-bit
        TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0) | (1 << COM1B1) | (1 << COM1B0) | (1 << 3) | (1 << 2) | (1 << WGM11) | (1 << WGM10));
        TCCR1B &= ~((1 << ICNC1) | (1 << ICES1) | (1 << 5) | (1 << WGM13) | (1 << WGM12) | (1 << CS12) | (1 << CS11) | (1 << CS10));
        TCCR1C = 0x00;
		TCNT1H = 0x00;    TCNT1L = 0x00;
		ICR1H  = 0x00;    ICR1L  = 0x00;
		OCR1AH = 0x00;    OCR1AL = 0x00;
		OCR1BH = 0x00;    OCR1BL = 0x00;

		// Timer/Counter 2
		// Fast PWM Mode
        // Non-inverting mode
		// Top = 0xFF
		// Prescaler: 1:64
		TCCR2A |= (1 << COM2A1) | (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
		TCCR2A &= ~((1 << COM2A0) | (1 << COM2B0) | (1 << 3) | (1 << 2));
		TCCR2B |=  (1 << CS22);
		TCCR2B &= ~((1 << FOC2A) | (1 << FOC2B) | (1 << 5) | (1 << 4) | (1 << WGM22) | (1 << CS21) | (1 << CS20));
		TCNT2  = 0x00;
		OCR2A  = 0x00;    OCR2B  = 0x00;

	// Crystal Oscillator division factor: 1
		#pragma optsize-
			CLKPR  = 0x80;
			CLKPR  = 0x00;
		#ifdef _OPTIMIZE_SIZE_
		#pragma optsize+
		#endif

	// External Interrupt(s) initialization
		// Turn OFF
		EICRA  = 0x00;
		EIMSK  = 0x00;
		PCICR  = 0x00;

	// Timer/Counter 0 Interrupt(s) initialization
		// Turn OFF
		TIMSK0 = 0x00;

	// Timer/Counter 1 Interrupt(s) initialization
		// Turn OFF
		TIMSK1 = 0x00;

	// Timer/Counter 2 Interrupt(s) initialization
		// Turn OFF
		TIMSK2 = 0x00;

	// USART initialization
		// Turn OFF
		UCSR0B = 0x00;

	// Analog Comparator initialization
		// Turn OFF
		ACSR   = 0x80;
		ADCSRB = 0x00;
		DIDR1  = 0x00;

	// ADC initialization
		// Turn OFF
		ADCSRA = 0x00;

	// SPI initialization
		// Turn OFF
		SPCR   = 0x00;

	// TWI initialization
		// Turn OFF
		TWCR   = 0x00;

}

bool ReadSensorLineData (bool lineData[]) {

    #ifdef ReadSensor1
        lineData[0] = ReadSensor1;
    #endif
    #ifdef ReadSensor2
        lineData[1] = ReadSensor2;
    #endif
    #ifdef ReadSensor3
        lineData[2] = ReadSensor3;
    #endif
    #ifdef ReadSensor4
        lineData[3] = ReadSensor4;
    #endif
    #ifdef ReadSensor5
        lineData[4] = ReadSensor5;
    #endif
    #ifdef ReadSensor6
        lineData[5] = ReadSensor6;
    #endif
    #ifdef ReadSensor7
        lineData[6] = ReadSensor7;
    #endif
    #ifdef ReadSensor8
        lineData[7] = ReadSensor8;
    #endif
    #ifdef ReadSensor9
        lineData[8] = ReadSensor9;
    #endif
    #ifdef ReadSensor10
        lineData[9] = ReadSensor10;
    #endif
    #ifdef ReadSensor11
        lineData[10] = ReadSensor11;
    #endif
    #ifdef ReadSensor12
        lineData[11] = ReadSensor12;
    #endif
    #ifdef ReadSensor13
        lineData[12] = ReadSensor13;
    #endif
    #ifdef ReadSensor14
        lineData[13] = ReadSensor14;
    #endif
    #ifdef ReadSensor15
        lineData[14] = ReadSensor15;
    #endif
    #ifdef ReadSensor16
        lineData[15] = ReadSensor16;
    #endif

    return lineData[QTY_OF_SENSORS];

}

float
RobotPosition(void) {

    int posSum = 0;
    float posMedian = QTY_OF_SENSORS / 2;
    unsigned short int sensorLineData[QTY_OF_SENSORS] = {0};
    int signalCorrected;
    unsigned char i = 0;

    ReadSensorLineData(sensorLineData);

    for (i = QTY_OF_SENSORS; i > 0; i--) {
        if (sensorLineData[i]) {
            signalCorrected = 1;
        }
        else {
            signalCorrected = 0;
        }

        posSum += signalCorrected;
    }

    return posMedian - posSum;

}

/****************************** Main ******************************/

void main (void) {

    unsigned char avgSpeed = 150; // средняя скорость моторов
    unsigned char kP = 10; // коэффициент пропорциональной обратной связи
    signed char error = 0; // Это ошибка положения
    signed short int correction = kP * error;

    signed short int leftMotorSpeed = avgSpeed * (1 + correction);
    signed short int rightMotorSpeed = avgSpeed * (1 - correction);

    InitSys ();

    error = RobotPosition();

    if (leftMotorSpeed >= 0) {
        OCR2A = leftMotorSpeed;
        OCR2B = 0;
    }
    else {
        OCR2A = 0;
        OCR2B = leftMotorSpeed;
    }

    if (rightMotorSpeed >= 0) {
        OCR0A = rightMotorSpeed;
        OCR0B = 0;
    }
    else {
        OCR0A = 0;
        OCR0B = rightMotorSpeed;
    }

	// Waiting for a signal on IR sensor
    #ifdef SensorIR
        while (SensorIR) {
            LED_ON;
            delay_ms (25);
            LED_OFF;
            delay_ms (25);
        }
    #endif

	//delay_ms (1000);

    while (true) {

    }

}
