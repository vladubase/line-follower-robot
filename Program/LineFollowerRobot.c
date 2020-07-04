/*
*	Line Follower Robot
*	LineFollowerRobot.c
*
*	by Uladzislau 'vladubase' Dubatouka <vladubase@gmail.com>.
*	Created on 2020.05.12.
*
*/

/**************************** Includes ****************************/

#include <mega328p.h>
#include <mega328p_bits.h>
#include <delay.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

/***************************** Defines ****************************/

#define		F_CPU					20000000UL		// Quartz resonator clock frequency

#define		QTY_OF_SENSORS			4				// Quantity of sensors
#define		AVG_SPEED				155				// Average speed of robot

#define		MOTORS_NOT_PERFECT		false			// Do the motors have different real parameters (e.g. Speed, Torque, etc.)?
#if MOTORS_NOT_PERFECT
	#define	L_MOTOR_MISMATCH		1.15F			// Coefficient of motor power difference
	#define	R_MOTOR_MISMATCH		1.0F			// There is nothing perfect ;)
#endif /* MOTORS_NOT_PERFECT */

// PID
// Setup: P -> PD -> PID
#define		kP						2.0F			// Proportional	feedback coefficient	
#define		kI						0.0F			// Integral 	feedback coefficient
#define		kD						0.0F			// Differential	feedback coefficient
#define		QTY_OF_ERR				10				// Quantity of errors in memory during last (QTY_OF_ERR * MAIN_CYCLE_DELAY) ms
#define		MAIN_CYCLE_DELAY		5				// The main cycle delay (in ms) for correct work of D-regulation

// Sensor order in the right --> direction

#if QTY_OF_SENSORS >= 1
    #define	READ_SENSOR_1			PINB & (1 << DDB2)
#endif /* QTY_OF_SENSORS >= 1 */
#if QTY_OF_SENSORS >= 2
    #define	READ_SENSOR_2			PINB & (1 << DDB1)
#endif /* QTY_OF_SENSORS >= 2 */
#if QTY_OF_SENSORS >= 3
    #define	READ_SENSOR_3			PINB & (1 << DDB0)
#endif /* QTY_OF_SENSORS >= 3 */
#if QTY_OF_SENSORS >= 4
    #define	READ_SENSOR_4			PIND & (1 << DDD7)
#endif /* QTY_OF_SENSORS >= 4 */
#if QTY_OF_SENSORS >= 5
    #define	READ_SENSOR_5			PINx & (1 << DDxx)
#endif /* QTY_OF_SENSORS >= 5 */
#if QTY_OF_SENSORS >= 6
    #define	READ_SENSOR_6			PINx & (1 << DDxx)
#endif /* QTY_OF_SENSORS >= 6 */
#if QTY_OF_SENSORS >= 7
    #define	READ_SENSOR_7			PINx & (1 << DDxx)
#endif /* QTY_OF_SENSORS >= 7 */
#if QTY_OF_SENSORS >= 8
    #define	READ_SENSOR_8			PINx & (1 << DDxx)
#endif /* QTY_OF_SENSORS >= 8 */
#if QTY_OF_SENSORS >= 9
    #define	READ_SENSOR_9			PINx & (1 << DDxx)
#endif /* QTY_OF_SENSORS >= 9 */
#if QTY_OF_SENSORS >= 10
    #define	READ_SENSOR_10			PINx & (1 << DDxx)
#endif /* QTY_OF_SENSORS >= 10 */
#if QTY_OF_SENSORS >= 11
    #define	READ_SENSOR_11			PINx & (1 << DDxx)
#endif /* QTY_OF_SENSORS >= 11 */
#if QTY_OF_SENSORS >= 12
    #define	READ_SENSOR_12			PINx & (1 << DDxx)
#endif /* QTY_OF_SENSORS >= 12 */
#if QTY_OF_SENSORS >= 13
    #define	READ_SENSOR_13			PINx & (1 << DDxx)
#endif /* QTY_OF_SENSORS >= 13 */
#if QTY_OF_SENSORS >= 14
    #define	READ_SENSOR_14			PINx & (1 << DDxx)
#endif /* QTY_OF_SENSORS >= 14 */
#if QTY_OF_SENSORS >= 15
    #define	READ_SENSOR_15			PINx & (1 << DDxx)
#endif /* QTY_OF_SENSORS >= 15 */
#if QTY_OF_SENSORS >= 16
    #define	READ_SENSOR_16			PINx & (1 << DDxx)
#endif /* QTY_OF_SENSORS >= 16 */

//#define		READ_IR_SENSOR          PINx & (1 << DDxx)

/************************* Global Variables ***********************/

bool line_data[QTY_OF_SENSORS] = {0};				// Store current values from sensor line

/*********************** Function  prototypes *********************/

void InitSys (void);
void ReadSensorLineData (void);
float CurrentRobotError (void);

/****************************** Main ******************************/

void main (void) {
	register float error_history[QTY_OF_ERR] = {0};	// Storing the values of recent errors
	register float error_sum = 0.0;					// Sum of errors in history
	register uint8_t i = 0;
	register float P = 0.0;
	register float I = 0.0;
	register float D = 0.0;
	register float PID_total_correction = 0.0;
	register int16_t left_motor_speed = 0;
	register int16_t right_motor_speed = 0;

	InitSys ();

	// Waiting for a signal on IR sensor
	#ifdef READ_IR_SENSOR
		while (READ_IR_SENSOR) {
			LED_1_ON;
			delay_ms (25);
			LED_1_OFF;
			delay_ms (25);
		}
	#endif /* SensorIR */

	//delay_ms (5000);								// This delay is required by the competition rules

	while (true) {
		error_sum = 0.0;

		for (i = 0; i < QTY_OF_ERR - 1; i++) {	    // Shift error values
			error_history[i] = error_history[i + 1];
		}
		error_history[QTY_OF_ERR - 1] = CurrentRobotError ();

		P = error_history[QTY_OF_ERR - 1] * kP;		// Current error * kP
		for (i = QTY_OF_ERR - 1; i >= 0; i--) {
			error_sum += error_history[i];
		}
		I = error_sum / QTY_OF_ERR * kI;			// Average error * kI
		D = (error_history[QTY_OF_ERR - 1] - error_history[QTY_OF_ERR - 1 - 1]) * kD;	// (current error - previous error) * kD

		PID_total_correction = (P + I) + D;

		left_motor_speed  = AVG_SPEED + (uint16_t)PID_total_correction;
		right_motor_speed = AVG_SPEED - (uint16_t)PID_total_correction;

		// Validating a range of variables
		if (left_motor_speed > 255)
			left_motor_speed = 255;
		else if (left_motor_speed < 0)
			left_motor_speed = 0;
		if (right_motor_speed > 255)
			right_motor_speed = 255;
		else if (right_motor_speed < 0)
			right_motor_speed = 0;

		// Motors power difference compensation
		#if MOTORS_NOT_PERFECT
			if (L_MOTOR_MISMATCH >= R_MOTOR_MISMATCH) {
				OCR2A = 0;
				OCR2B = left_motor_speed;
				OCR0A = 0;
				OCR0B = right_motor_speed / L_MOTOR_MISMATCH;
			} else {
				OCR2A = 0;
				OCR2B = left_motor_speed / R_MOTOR_MISMATCH;
				OCR0A = 0;
				OCR0B = right_motor_speed;
			}
		#else
			OCR2A = 0;
			OCR2B = left_motor_speed;
			OCR0A = 0;
			OCR0B = right_motor_speed;
		#endif /* MOTORS_NOT_PERFECT */

		delay_ms (MAIN_CYCLE_DELAY);
	}
}

/*************************** Functions ****************************/

void InitSys (void) {
	// Motors
	    // Output mode
		DDRB |= (1 << DDB3);			            // OC2A
		DDRD |= (1 << DDD6) |			            // OC0A
				(1 << DDD5) |			            // OC0B
				(1 << DDD3);			            // OC2B

	// SensorLine
	    // Input mode
		DDRB &= ~((1 << DDB2) | (1 << DDB1) | (1 << DDB0));
		DDRD &= ~(1 << DDD7);

	// Infrared Sensor
	    // Input mode
		#ifdef READ_SENSOR_IR
			DDRx &= ~(1 << DDxx);
		#endif

	// Timer/Counter(s) initialization
		// Timer/Counter 0
		// Fast PWM Mode
		// Clear OC0A on Compare Match, set OC0A at BOTTOM (non-inverting mode)
		// TOP = 0xFF
		// Prescaler: 1:64
		TCCR0A |= (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
		TCCR0A &= ~((1 << COM0A0) | (1 << COM0B0) | (1 << 3) | (1 << 2));
		TCCR0B |= (1 << CS01) | (1 << CS00);
		TCCR0B &= ~((1 << FOC0A) | (1 << FOC0B) | (1 << 5) | (1 << 4) | (1 << WGM02) | (1 << CS02));
		TCNT0  = 0x00;
		TIMSK0 = 0x00;
		OCR0A  = 0x00;	OCR0B  = 0x00;

		// Timer/Counter 1
		// Fast PWM 10-bit Mode
		// Clear OC1A/OC1B on Compare Match, set OC1A/OC1B at BOTTOM (non-inverting mode)
		// TOP = 0x03FF
		// Prescaler: 1:64
		TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11) | (1 << WGM10);
		TCCR1A &= ~((1 << COM1A0) | (1 << COM1B0) | (1 << 3) | (1 << 2));
		TCCR1B |= (1 << WGM12) | (1 << CS11) | (1 << CS10);
		TCCR1B &= ~((1 << ICNC1) | (1 << ICES1) | (1 << 5) | (1 << WGM13) | (1 << CS12));
		TCCR1C = 0x00;
		TCNT1H = 0x00;	TCNT1L = 0x00;
		TIMSK1 = 0x00;
		ICR1H  = 0x00;	ICR1L  = 0x00;
		OCR1AH = 0x00;	OCR1AL = 0x00;
		OCR1BH = 0x00;	OCR1BL = 0x00;

		// Timer/Counter 2
		// Fast PWM Mode
		// Clear OC0A on Compare Match, set OC0A at BOTTOM (non-inverting mode)
		// TOP = 0xFF
		// Prescaler: 1:64
		TCCR2A |= (1 << COM2A1) | (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
		TCCR2A &= ~((1 << COM2A0) | (1 << COM2B0) | (1 << 3) | (1 << 2));
		TCCR2B |= (1 << CS22);
		TCCR2B &= ~((1 << FOC2A) | (1 << FOC2B) | (1 << 5) | (1 << 4) | (1 << WGM22) | (1 << CS21) | (1 << CS20));
		TCNT2  = 0x00;
		TIMSK2 = 0x00;
		OCR2A  = 0x00;	OCR2B  = 0x00;

	// Crystal Oscillator division factor: 1
		#pragma optsize-
			CLKPR = 0x80;
			CLKPR = 0x00;
		#ifdef _OPTIMIZE_SIZE_
		#pragma optsize+
		#endif

	// External Interrupt(s) initialization
		// Turn OFF
		EICRA  = 0x00;
		EIMSK  = 0x00;
		PCICR  = 0x00;
	
	// Analog Comparator initialization
		// Turn OFF
		ACSR   = 0x80;
		ADCSRB = 0x00;
		DIDR1  = 0x00;

	// USART initialization
		// Turn OFF
		UCSR0B = 0x00;

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

void ReadSensorLineData (void) {
	#ifdef READ_SENSOR_1
		line_data[0] = READ_SENSOR_1;
	#endif /* READ_SENSOR_1 */
	#ifdef READ_SENSOR_2
		line_data[1] = READ_SENSOR_2;
	#endif /* READ_SENSOR_2 */
	#ifdef READ_SENSOR_3
		line_data[2] = READ_SENSOR_3;
	#endif /* READ_SENSOR_3 */
	#ifdef READ_SENSOR_4
		line_data[3] = READ_SENSOR_4;
	#endif /* READ_SENSOR_4 */
	#ifdef READ_SENSOR_5
		line_data[4] = READ_SENSOR_5;
	#endif /* READ_SENSOR_5 */
	#ifdef READ_SENSOR_6
		line_data[5] = READ_SENSOR_6;
	#endif /* READ_SENSOR_6 */
	#ifdef READ_SENSOR_7
		line_data[6] = READ_SENSOR_7;
	#endif /* READ_SENSOR_7 */
	#ifdef READ_SENSOR_8
		line_data[7] = READ_SENSOR_8;
	#endif /* READ_SENSOR_8 */
	#ifdef READ_SENSOR_9
		line_data[8] = READ_SENSOR_9;
	#endif /* READ_SENSOR_9 */
	#ifdef READ_SENSOR_10
		line_data[9] = READ_SENSOR_10;
	#endif /* READ_SENSOR_10 */
	#ifdef READ_SENSOR_11
		line_data[10] = READ_SENSOR_11;
	#endif /* READ_SENSOR_11 */
	#ifdef READ_SENSOR_12
		line_data[11] = READ_SENSOR_12;
	#endif /* READ_SENSOR_12 */
	#ifdef READ_SENSOR_13
		line_data[12] = READ_SENSOR_13;
	#endif /* READ_SENSOR_13 */
	#ifdef READ_SENSOR_14
		line_data[13] = READ_SENSOR_14;
	#endif /* READ_SENSOR_14 */
	#ifdef READ_SENSOR_15
		line_data[14] = READ_SENSOR_15;
	#endif /* READ_SENSOR_15 */
	#ifdef READ_SENSOR_16
		line_data[15] = READ_SENSOR_16;
	#endif /* READ_SENSOR_16 */
}

float CurrentRobotError (void) {
	register uint8_t i = 0;
	register float current_error = 0.0;

	ReadSensorLineData ();

	for (i = QTY_OF_SENSORS - 1; i >= 0; i--) {
		if (line_data[i] != 0)						// If no data on [i] sensor skip counting the error
			current_error += pow (QTY_OF_SENSORS / 2 - i, 3);	// Odd degree to preserve the sign '-'
	}

	return current_error;
}


//  Literature:
//      http://we.easyelectronics.ru/Theory/pid-regulyatory--dlya-chaynikov-praktikov.html
//      https://wiki.roboforum.ru/index.php?title=%d0%9f%d0%b5%d1%80%d0%b5%d0%b2%d0%be%d0%b4_%d1%81%d1%82%d0%b0%d1%82%d1%8c%d0%b8_%22%d0%9f%d1%80%d0%be%d1%81%d1%82%d0%be_%d0%be_%d0%9f%d0%98%d0%94-%d0%b0%d0%bb%d0%b3%d0%be%d1%80%d0%b8%d1%82%d0%bc%d0%b0%d1%85%22#.D0.9D.D0.B0.D1.81.D1.82.D1.80.D0.BE.D0.B9.D0.BA.D0.B0_.D0.9F.D0.98.D0.94-.D1.80.D0.B5.D0.B3.D1.83.D0.BB.D1.8F.D1.82.D0.BE.D1.80.D0.B0