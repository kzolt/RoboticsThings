#pragma config(Sensor, dgtl2,  DownButton,     sensorDigitalIn)
#pragma config(Sensor, dgtl3,  UpButton,       sensorDigitalIn)
#pragma config(Motor,  port2,           TwackerMotor,  tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           UppyDownyMotor, tmotorVex393_MC29, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define TIME_TO_WAIT 1500
#define TIME_ADD 100
#define TIME_REMOVE 0
#define MOTOR_SPEED 127

typedef enum dir
{
	NONE = 0,
	UP = 1,
	DOWN = 2
} Direction;

void MoveTheThing(int mot, int time, Direction dir)
{
	motor[mot] = dir == UP ? MOTOR_SPEED : -MOTOR_SPEED;
	wait1Msec(time);
	motor[mot] = 0;
}

void MoveMotor(Direction dir)
{
	if (dir == UP)
	{
		MoveTheThing(UppyDownyMotor, TIME_TO_WAIT + TIME_ADD, dir);
		MoveTheThing(TwackerMotor, 0, dir);
	}

	if (dir == DOWN)
	{
		MoveTheThing(UppyDownyMotor, TIME_TO_WAIT + TIME_ADD, dir);
		MoveTheThing(TwackerMotor, 1000, dir);
	}

}

task main()
{

	while (true)
	{

		if (SensorValue[UpButton] == 0)
			MoveMotor(UP);

		if (SensorValue[DownButton] == 0)
			MoveMotor(DOWN);


	}

}
