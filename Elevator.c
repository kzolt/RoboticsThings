#pragma config(Sensor, dgtl1,  FloorOneButton, sensorDigitalIn)
#pragma config(Sensor, dgtl2,  FloorTwoButton, sensorDigitalIn)
#pragma config(Sensor, dgtl3,  FloorThreeButton, sensorDigitalIn)
#pragma config(Sensor, dgtl4,  distanceThing,  sensorSONAR_cm)
#pragma config(Sensor, dgtl10, FOLight,        sensorLEDtoVCC)
#pragma config(Sensor, dgtl11, FTLight,        sensorLEDtoVCC)
#pragma config(Sensor, dgtl12, FTHLight,       sensorLEDtoVCC)
#pragma config(Motor,  port8,           RightMotor,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port9,           LeftMotor,     tmotorVex393_MC29, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define QUEUE_SIZE 10			// Elevator Queue Size

/*************************************
STRUCTURE AND ENUMERATION DEFINITIONS
*************************************/

// Enumeration for the direction of the elevator
typedef enum dir
{
	DIR_NONE = 0,
	DIR_DOWN = 1,
	DIR_UP   = 2
} Direction;

// Enumeration for the floor, just helps make code more readable
typedef enum flr
{
	FLOOR_NIL   = -1,
	FLOOR_ONE   =  0,
	FLOOR_TWO   =  1,
	FLOOR_THREE =  2
} Floor;

// Circular Queue Structure For Elevator
typedef struct queue
{
	Floor data[QUEUE_SIZE];
	int front;
	int rear;
	int count;
} ElevatorQueue;

// Elevator structure
typedef struct elevator
{
	Floor current_floor;
	Direction dir;
	int speed;
	bool running;
	ElevatorQueue q;
} Elevator;

// Global Elevator Definition
static Elevator elv;

// Global Array for Floor Distances
static int floorDistance[3] =
{
	 7,		// FLOOR_ONE Location
	20,		// FLOOR_TWO Location
	32		// FLOOR_THREE Location
};

/*************************************
FUNCTION DEFINITIONS
*************************************/

// Checks Wether our queue is emtpy
int isEmpty(ElevatorQueue& self)
{
	return !(self.count);
}

// Gets the element in the front of the queue
Floor front(ElevatorQueue& self)
{
	if (isEmpty(self))
	{
		writeDebugStream("[ERROR]: Queue Empty!");
		return FLOOR_NIL;
	}

	return self.data[self.front];
}

// Adds Element to rear of queue
void enqueue(ElevatorQueue& self, Floor floor)
{
	// Checks if we exceed size, if we do, stop
	if (self.count >= QUEUE_SIZE)
	{
		writeDebugStream("[ERROR]: Overflow Error!");
		return;
	}

	// Adds the element and updates rear value to be correct, same goes for the count
	self.rear = (self.rear + 1) % QUEUE_SIZE;
	self.data[self.rear] = floor;
	self.count++;
}

// Removes the element at the front by changing the location of the front
void dequeue(ElevatorQueue& self)
{
	// Checks if we run out of space, i.e. we can't make the queue smaller
	if (isEmpty(self))
	{
		writeDebugStream("[ERROR]: Underflow Error!\n");
		return;
	}

	// Change location of the front, and decrease count
	self.front = (self.front + 1) % QUEUE_SIZE;
	self.count--;
}

// Moves Elevator Either Up or Down
void move_elevator()
{
	// Checks if the elevator should stop
	if (elv.dir == DIR_NONE)
	{
		motor[LeftMotor] = 0;
		motor[RightMotor] = 0;
		return;
	}

	// Moves Elevator in the desired direction
	motor[LeftMotor] = (elv.dir == DIR_UP ? -elv.speed : elv.speed);
	motor[RightMotor] = motor[LeftMotor];
}

// Displays the current floor number using the leds
void display_floor_number()
{
	// checks to see which floor we're on and "displays it"
	switch (elv.current_floor)
	{
		case FLOOR_ONE:
			SensorValue[FOLight]  = 1;
			SensorValue[FTLight]  = 0;
			SensorValue[FTHLight] = 0;
			break;
		case FLOOR_TWO:
			SensorValue[FOLight]  = 0;
			SensorValue[FTLight]  = 1;
			SensorValue[FTHLight] = 0;
			break;
		case FLOOR_THREE:
			SensorValue[FOLight]  = 0;
			SensorValue[FTLight]  = 0;
			SensorValue[FTHLight] = 1;
			break;
		default:
			SensorValue[FOLight]  = 0;
			SensorValue[FTLight]  = 0;
			SensorValue[FTHLight] = 0;
			break;
	}
}

// Checks the position of the elevator floor using Ultra Sonic Range Finder and returns the appropriate floor
Floor get_floor_position()
{
	if (SensorValue[distanceThing] < floorDistance[(int)FLOOR_TWO])
		return FLOOR_ONE;
	else if (SensorValue[distanceThing] >= floorDistance[(int)FLOOR_ONE] && SensorValue[distanceThing] < floorDistance[(int)FLOOR_THREE])
		return FLOOR_TWO;
	else if (SensorValue[distanceThing] >= floorDistance[(int)FLOOR_THREE] && SensorValue[distanceThing] <= floorDistance[(int)FLOOR_THREE] + 3)
		return FLOOR_THREE;

	return FLOOR_NIL;
}

/*************************************
TASK DEFINITIONS
*************************************/

// Uses queue to figure out which floor to go to and goes there
task go_to_floor()
{
	// Set the elevator to be running so we can't start this thread again by accident
	elv.running = true;

	// Loops through entire queue until it is empty (count = 0)
	while (!isEmpty(elv.q))
	{
		// Create Local variable to the floor that we want to go to
		Floor ftg = front(elv.q);

		// Checks if the Floor we're trying to go to is our current floor and just breaks
		// Also checks if the floor value is nil, if it is, then it also just dequeues and breaks
		if (ftg == elv.current_floor || ftg == FLOOR_NIL)
		{
			dequeue(elv.q);
			break;
		}

		// Set the direction of which way we want the elevator to move
		if (ftg > elv.current_floor)
			elv.dir = DIR_UP;
		else
			elv.dir = DIR_DOWN;

		// Moves to appropriate floor using sensor value
		while (SensorValue[distanceThing] != floorDistance[(int)ftg])
			move_elevator();

		// Stops the elevator and sets current floor to be the floor we just traveled to, also removes the floor from our queue
		elv.dir = DIR_NONE;
		move_elevator();
		elv.current_floor = ftg;
		dequeue(elv.q);

		// Simulate Wait Time
		delay(500);
	}

	// We're done moving the elevator, so set running to false for the next "batch" of commands
	elv.running = false;
	return;
}

task main()
{
	// Create main elevator structure
	elv.current_floor = get_floor_position();
	elv.speed = 127;
	elv.dir = DIR_NONE;
	elv.running = false;

	// Setting up the queue
	elv.q.front = 0;
	elv.q.rear = -1;
	elv.q.count = 0;

	// Sets queue to be "nil"
	for (int i = 0; i < QUEUE_SIZE; i++)
		elv.q.data[i] = FLOOR_NIL;

	// main loop
	while (true)
	{
		// If the first floor button is pushed then add it to the queue
		if (SensorValue[FloorOneButton] == 0)
		{
			enqueue(elv.q, FLOOR_ONE);
			delay(200);
		}

		// If the second floor button is pushed then add it to the queue
		if (SensorValue[FloorTwoButton] == 0)
		{
			enqueue(elv.q, FLOOR_TWO);
			delay(200);
		}

		// If the third floor button is pushed then add it to the queue
		if (SensorValue[FloorThreeButton] == 0)
		{
			enqueue(elv.q, FLOOR_THREE);
			delay(200);
		}

		// Display the floor number
		display_floor_number();

		// Starts moving the elevator on seperate thread
		if (!elv.running)
			startTask(go_to_floor);

	}
}
