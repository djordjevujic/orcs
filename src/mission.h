/**
 * @file mission.h
 *
 */

#ifndef MISSION_H_
#define MISSION_H_

#include <stdint.h>

#define MAX_MISSIONS 20

/*
enum MissionList
{
	mission1,
	mission2,
	mission3,
	mission4,
	end_of_missions
};
*/
enum MissionStatus
{
	mission_never_activated,
	mission_in_progress,
	mission_done,
	mission_interrupted,
	mission_from_interrupted,
	mission_from_interrupted_wms /** wms - wait motion status
								   * go in mission_in_progress after motion status
								   * turns to MOTION_MOVING  */
};

typedef void (*mission) (void);

typedef struct t_mission
{
	enum MissionStatus status;
	mission job;
}t_mission;

void mission_forward(int distance, int speed);
void mission_go(int x, int y, int speed);
void mission_rotate_abs(int angle);
void mission_rotate_rel(int angle);

void mission_robot_stop(void);
void mission_wait(unsigned int time_ms);

void missions_init(t_mission* m);
void missions_print(t_mission* m);

#endif
