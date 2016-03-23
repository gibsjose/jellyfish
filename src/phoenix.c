/**********************************************************
*   Phoenix FSW - Main
*
*   25 Jan 2016
*
*   Joe Gibson 						(joseph.gibson@nasa.gov)
*	Jordi Vila Hernandez de Lorenzo	(jordi.vila.hdl@gmail.com)
*
*   License: MIT (http://gibsjose.mit-license.org)
*
*   https://github.com/gibsjose/phoenix
**********************************************************/

#include "phoenix.h"


//Don't forget `volatile`!
volatile bool sensors = false;


//gyro_t *gyro;

//Timer 1 Compare Interrupt Vector (1s CTC Timer)
ISR(TIMER1_COMPA_vect) {
	//Blink LED
	//PORTB ^= _BV(LED_PIN);

	//Initiate a sensor reading
	//sensors = true;
}

int main(void) {
	//malloc data structures
	gyro_t *gyro = (gyro_t *)malloc(sizeof(gyro_t));
	receiver = (receiver_inputs_t*)malloc(sizeof(receiver_inputs_t));
	setpoints = (setpoints_t*)malloc(sizeof(setpoints_t));

   PID_input_t *pid_input_roll = (PID_input_t *)malloc(sizeof(PID_input_t));
   PID_input_t *pid_input_pitch = (PID_input_t *)malloc(sizeof(PID_input_t));
   PID_input_t *pid_input_yaw = (PID_input_t *)malloc(sizeof(PID_input_t));

   PID_output_t *pid_output_roll = (PID_output_t *)malloc(sizeof(PID_output_t));
   PID_output_t *pid_output_pitch = (PID_output_t *)malloc(sizeof(PID_output_t));
   PID_output_t *pid_output_yaw = (PID_output_t *)malloc(sizeof(PID_output_t));

  const PID_settings_t *pid_settings_roll = (PID_settings_t *)malloc(sizeof(PID_settings_t));
  /*pid_settings_roll.KP = P_ROLL;   // Gain setting for the roll P-controller
  pid_settings_roll->KI = I_ROLL;   // Gain setting for the roll I-controller
  pid_settings_roll->KD = D_ROLL;   // Gain setting for the roll D-controller
  pid_settings_roll->upper_limit = UPPER_LIMIT; // Maximum output of the PID-controller
  pid_settings_roll->lower_limit = LOWER_LIMIT; // Minimum output of the PID-controller*/

  const PID_settings_t *pid_settings_pitch = (PID_settings_t *)malloc(sizeof(PID_settings_t));

  const PID_settings_t *pid_settings_yaw = (PID_settings_t *)malloc(sizeof(PID_settings_t));
  /*pid_settings_yaw = (struct PID_settings_t*){
      P_YAW,                 // Gain setting for the yaw P-controller
      I_YAW,              // Gain setting for the yaw I-controller
      D_YAW,                 // Gain setting for the yaw D-controller
      UPPER_LIMIT,               // Maximum output of the PID-controller
      LOWER_LIMIT,              // Minimum output of the PID-controller
  };*/

	int ret = 0;

	//Initialize UART at 9600 baud
	uart_init(UART_BAUD_SELECT(BAUD, F_CPU));

	//Initialization
	uart_puts("Initializing...\r\n");

	ret = init();
	if(ret) {
		uart_puts("<-- ERROR: Initialization failed -->\r\n");
	} else {
		uart_puts("Initialization successful\r\n");
	}

	//Initialize gyroscope
	gyro_init(gyro);

	//Calibrate gyroscope
	gyro_calibrate(gyro);

	//Main Loop
	while(true) {
		//Read and print the gyro data
		gyro_loop(gyro);

		//Calculate the pid_output to feed into the ESCs
		void calculate_pids(gyro, pid_input_roll, pid_input_pitch, pid_input_yaw,
		  pid_settings_roll, pid_settings_pitch, pid_settings_yaw, pid_output_roll
		  , pid_output_pitch, pid_output_yaw );



		//Sensor Loop
		if(sensors) {
			sensors = false;
		}
	}

	return 0;
}

int init(void) {
	int ret = 0;

	//Clear interrupts
	cli();

	//Initialize ports
	port_init();

	//Initialize peripherals
	peripheral_init();

	//Initialize devices
	ret = device_init();

	//Enable interrupts
	sei();

	return ret;
}

int port_init(void) {
	//Initialize LED as output
	//DDRB |= _BV(LED_DD);

	//Initialize battery inputs/outputs
	// rfcx_batteries_init();

	return 0;
}

int timer1_init(void) {
	//Initialize Timer 1
	TCCR1A = 0;
	TCCR1B = 0;

	//Set CTC compare value (1 second)
	OCR1A = TIMER1_COUNT;

	//Enable CTC mode
	TCCR1B |= (1 << WGM12);

	//Enable 1024 prescaler
	TCCR1B |= (1 << CS10);
	TCCR1B |= (1 << CS12);

	//Enable Timer 1 output compare interrupt
	TIMSK1 |= (1 << OCIE1A);

	return 0;
}

int peripheral_init(void) {
	//Initialize Timer 1
	timer1_init();

	//Initialize I2C (TWI) peripheral as a whole
	i2c_init();

	return 0;
}

int device_init(void) {
	int ret = 0;

	//Initialize external I2C temp sensor (LM75BD)
	// ret = rfcx_temp_init();
	// if(ret) {
	// 	uart_puts("<-- ERROR: Error initializing temp sensor -->\r\n");
	// 	return ret;
	// } else {
	// 	uart_puts("Successfully initialized temp sensor\r\n");
	// }

	//Initialize external I2C ADC (ADS1015)
	// ret = rfcx_adc_init();
	// if(ret) {
	// 	 uart_puts("<-- ERROR: Error initializing ADC -->\r\n");
	// } else {
	// 	 uart_puts("Successfully initialized ADC\r\n");
	// }

	// //Initialize external I2C humidity sensor (HIH6130)
	// ret = rfcx_humid_init();
	// if(ret) {
	// 	uart_puts("<-- ERROR: Error initializing humidity sensor -->\r\n");
	// 	return ret;
	// } else {
	// 	uart_puts("Successfully initialized humidity sensor\r\n");
	// }

	return ret;
}
