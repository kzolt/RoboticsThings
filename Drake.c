#pragma config(Sensor, dgtl1,  FrontInput,     sensorSONAR_inch)
#pragma config(Sensor, dgtl3,  LeftInput,      sensorSONAR_inch)
#pragma config(Sensor, dgtl5,  RightInput,     sensorSONAR_inch)
#pragma config(Sensor, dgtl7,  BackInput,      sensorSONAR_inch)
#pragma config(Motor,  port3,           LeftMotor,     tmotorVex393_MC29, openLoop, driveLeft)
#pragma config(Motor,  port4,           RightMotor,    tmotorVex393_MC29, openLoop, driveRight)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*
TODO
Check if the new system works
*/

#define FULL_SPEED 127
#define HALF_SPEED FULL_SPEED / 2
#define THIRD_SPEED FULL_SPEED / 3
#define QUARTER_SPEED FULL_SPEED / 4

#define SENSOR_AMOUNT 4

#define LEVEL_ONE 15
#define LEVEL_TWO 12
#define LEVEL_THREE 9
#define LEVEL_FOUR 6

/**************************************************
 * SRUCTS AND ENUMS
 **************************************************/

// Enumeration for how close something is
typedef enum sevarity
{
	NORMAL = 0,
	WARNING = 1,
	MINOR_DANGER = 2,
	DANGER = 3,
	FATAL = 4
} Sevarity;

// Enumeration for the location of the sensors
typedef enum loc
{
	FRONT = 0,
	BACK = 1,
	LEFT = 2,
	RIGHT = 3
} SensorLocation;

// sensor struct, holds a bunch of sensor information
typedef struct sensors
{
	int key;
	int value;
	Sevarity sevarity_level;
	SensorLocation sensor_location;
} Sensors;

// simple 2 dimensional vector
typedef struct vector
{
	int x, y;
} Vector2i;

// struct for robot
typedef struct robot
{
	int left_speed;
	int right_speed;
	Vector2i dir_vec;
	Sensors sensors[SENSOR_AMOUNT];
} Robot;

/**************************************************
 * INITIALIZATION
 **************************************************/

// Initializes sensors inside robot struct
void init_sensor(Robot& self, int sensor, int key, SensorLocation loc)
{
	self.sensors[sensor].key = key;
	self.sensors[sensor].value = 0;
	self.sensors[sensor].sevarity_level = NORMAL;
	self.sensors[sensor].sensor_location = loc;
}

// Initializes Robot
void init_robot(Robot& self)
{
	self.left_speed = FULL_SPEED;
	self.right_speed = FULL_SPEED;
	self.dir_vec.x = 0;
	self.dir_vec.y = 0;

	init_sensor(self, 0, FrontInput, FRONT);
	init_sensor(self, 1, BackInput, BACK);
	init_sensor(self, 2, LeftInput, LEFT);
	init_sensor(self, 3, RightInput, RIGHT);
}

/**************************************************
 * FUNCTIONS
 **************************************************/

// Get's the values of the sensors and assigns sevarity according to location
void get_position_and_check_sevarity(Robot& self)
{
	// loops through sensors
	for (int i = 0; i < SENSOR_AMOUNT; i++)
	{
		// Assigns the current value of the sensor to value field
		self.sensors[i].value = SensorValue[self.sensors[i].key];

		// if the sensor is greater the "level one" then everything is a okay
		if (self.sensors[i].value > LEVEL_ONE)
		{
			self.sensors[i].sevarity_level = NORMAL;
			return;
		}

		// if the sensor is greater the "level one" then we should probably turn
		if (self.sensors[i].value <= LEVEL_ONE)
		{
			self.sensors[i].sevarity_level = WARNING;
			return;
		}
		
		// if the sensor is greater the "level two" then we should really turn
		if (self.sensors[i].value <= LEVEL_TWO)
		{
			self.sensors[i].sevarity_level = MINOR_DANGER;
			return;
		}

		// if the sensor is greater the "level three" then we should really turn
		if (self.sensors[i].value <= LEVEL_THREE)
		{
			self.sensors[i].sevarity_level = DANGER;
			return;
		}
		
		// if the sensor is greater the "level four" then we should definetly turn
		if (self.sensors[i].value <= LEVEL_FOUR)
		{
			self.sensors[i].sevarity_level = FATAL;
			return;
		}
	}
}

// Moves robot based off of what is closer
void adjust_robot(Robot& self)
{
	// check sevarity level of sensor and get the one with the highest sevarity
	int priority = 0;
	for (int i = 0; i < SENSOR_AMOUNT; i++)
	{
		if (self.sensors[i].sevarity_level > self.sensors[priority].sevarity_level)
			priority = i;
	}

	// adjust vector accordingly depending on sevarity
	switch (self.sensors[priority].sevarity_level)
	{
	case WARNING:
		// slow down if we are approaching something
		if (self.sensors[priority].sensor_location == FRONT)
		{
			self.dir_vec.y -= QUARTER_SPEED;
			break;
		}

		// slow down if we are approaching something
		if (self.sensors[priority].sensor_location == BACK)
		{
			self.dir_vec.y += QUARTER_SPEED;
			break;
		}
		
		// start turning if we are about to hit something
		if (self.sensors[priority].sensor_location == LEFT)
		{
			self.dir_vec.x -= QUARTER_SPEED;
			break;
		}

		// start turning if we are about to hit something
		if (self.sensors[priority].sensor_location == RIGHT)
			self.dir_vec.x += QUARTER_SPEED;
		
		break;
	case MINOR_DANGER:
		// really slow down if we are approaching something
		if (self.sensors[priority].sensor_location == FRONT)
		{
			self.dir_vec.y -= THIRD_SPEED;
			break;
		}

		// really slow down if we are approaching something
		if (self.sensors[priority].sensor_location == BACK)
		{
			self.dir_vec.y += THIRD_SPEED;
			break;
		}

		// really turn if we are going to hit something
		if (self.sensors[priority].sensor_location == LEFT)
		{
			self.dir_vec.x -= THIRD_SPEED;
			break;
		}

		// really turn if we are going to hit something
		if (self.sensors[priority].sensor_location == RIGHT)
				self.dir_vec.x += THIRD_SPEED;
		break;
	case DANGER:
		// genuinly slow down you made man
		if (self.sensors[priority].sensor_location == FRONT)
		{
			self.dir_vec.y -= HALF_SPEED;
			break;
		}

		// genuinly slow down you made man
		if (self.sensors[priority].sensor_location == BACK)
		{
			self.dir_vec.y += HALF_SPEED;
			break;
		}

		// omg turn
		if (self.sensors[priority].sensor_location == LEFT)
		{
			self.dir_vec.x -= HALF_SPEED;
			break;
		}

		// omg turn
		if (self.sensors[priority].sensor_location == RIGHT)
				self.dir_vec.x += HALF_SPEED;
		break;
	case FATAL:
		// jesus christ hit the breaks
		if (self.sensors[priority].sensor_location == FRONT)
		{
			self.dir_vec.y = -FULL_SPEED;
			break;
		}

		// jesus christ hit the breaks
		if (self.sensors[priority].sensor_location == BACK)
		{
			self.dir_vec.y = FULL_SPEED;
			break;
		}

		// swerve
		if (self.sensors[priority].sensor_location == LEFT)
		{
			self.dir_vec.x = -FULL_SPEED;
			break;
		}

		// swerve
		if (self.sensors[priority].sensor_location == RIGHT)
				self.dir_vec.x = FULL_SPEED;
		break;
	default:
		// Normal settings if everything is normal
		self.dir_vec.y = FULL_SPEED;
		self.dir_vec.x = 0;
		break;
	}

	add_vectors(self, priority);
}

void add_vectors(Robot& self, int priority)
{
	// set both motors to be the same value
	if (self.sensors[priority].sensor_location == FRONT || self.sensors[priority].sensor_location == BACK)
	{
		self.left_speed = self.dir_vec.y;
		self.right_speed = self.dir_vec.y;
	}

	// slow down left motor because we want to turn left, keep right motor at same speed
	if (self.sensors[priority].sensor_location == LEFT)
	{
		self.left_speed = self.dir_vec.y - self.dir_vec.x;
		self.right_speed = self.dir_vec.y;
	}

	// slow down right motor because we want to turn right, keep left motor at same speed
	if (self.sensors[priority].sensor_location == RIGHT)
	{
		self.left_speed = self.dir_vec.y;
		self.right_speed = self.dir_vec.y - self.dir_vec.x;
	}
}

void move_robot(Robot& self)
{
	// gets location of everything, sets sevarity
	get_position_and_check_sevarity(self);
	// adds vectors based off of sevarity and location
	adjust_robot(self);

	// sets actual motors to it's respected values
	motor[LeftMotor] = self.left_speed;
	motor[RightMotor] = self.right_speed;
}

task main()
{
	Robot rbot;
	init_robot(rbot);

	while (true)
	{
		move_robot(rbot);
	}
}
