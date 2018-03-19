#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <termios.h>
#include <wiringPi.h> //for delay and millis

#include "uart.h"
#include "color.h"
#include "motioncmd.h"
#include "motion.h"
#include "packet.h"
#include "config.h"
#include "task.h"
#include "sensor.h"
#include "actuator.h"

static unsigned long int start_time = 0;

void init_main(void)
{
	int flush_success = 0;
	
	tx_packets_init();
	uart_pkt_en(TRUE);
	set_side("GREEN");
	uart0_init(B57600);
	uart1_init(B115200); //using uart1 is alternate option
	
	flush_success = uart0_input_flush();
	flush_success = uart1_input_flush();
	
	brushless_set_speed(BR_HEAD, 0);
	brushless_set_speed(BR_SECONDARY, 0);
}

int main(int argc, char* argv[])
{
	unsigned int last_check = 0;
	
	init_main();
	if(!use_input_arguments(argc, argv))
	{
		return 0;
	}
	print_side();
	
	delay(100);

	//if communication doesn't work, end program
/*
	motion_set_position(MOTION_START_X, MOTION_START_Y, MOTION_START_O);
	if(!motion_check())
		return 0;
*/
	init_task(ENTER); /** @note Start options defined in config.h */
	start_time = millis();
	//motion_soft_stop();

	delay(100);

	define_missions();
	define_obstacle_handling();
	define_sensor_obstacle_handling();
	
	brushless_set_speed(BR_HEAD, 0);
	
	while(1)
	{	
		//ask for status of motion board and sensors
		if(millis() > last_check + MOTION_REFRESH_INTERVAL)
		{
			last_check = millis();
//			motion_get_status_and_position();
//			sensor_ask_for_status();
		}

		//check end of round
		if(millis() - start_time > ROUND_TIME)
		{
			print_blue();
			printf("Round time past: %d \n", ROUND_TIME);
			return 0;
		}
	
//		update_sensor_status();
//		motion_msg_status();
		
		//task();	
		
		axpacket_prepare(1, INST_WRITE, P_GOAL_POSITION_L);
		axpacket_put_word(999);
		axpacket_end();
		
		delay(500);
/*		
		axpacket_prepare(1, INST_WRITE, P_GOAL_POSITION_L);
		axpacket_put_word(100);
		axpacket_end();
		
		delay(100);
*/
	}
	return 0;
}
