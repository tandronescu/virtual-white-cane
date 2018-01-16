// Group 671 Tiberiu Andronescu, Usman Khan, Brian Rojkov, Leroy Zhao

// Leroy Zhao
void swivelUltrasonic(int angle1, int angle2, int adjustGyro1 , int adjustGyro2)
{
	// calibrate gyros at start, find difference between the two gyro values
	int netAngle = (angle1 - adjustGyro1) - (angle2 - adjustGyro2);
	
	// determine the 
	int compensateAngle =  netAngle - nMotorEncoder[motorB]/10;

	if (compensateAngle > 10) 
	// ultrasonic won't move if head rotation < 10 degrees
	{
		/* Theoretically, only motorB needs to move. However, motorC ends up 
		turning as well, so power was applied to compensate. The amount of power
		applied was determined experimentally. */
		motor[motorB] = -10; 
		motor[motorC] = 5;
	}
	else if (compensateAngle < -10)
	{
		motor[motorB] = 10;
		motor[motorC] = -4;
	}
	else
	{
		motor[motorB] = 0;
		motor[motorC] = 0;
	}
}

// Usman Khan
// Did not call in main due to insufficient ports on EV3
void swivelVertical(int angle)
{
	int compensateAngle =  angle - nMotorEncoder[motorC];
	if (compensateAngle < -10) 
	// Ultrasonic won't move if head rotation < 10 degrees
	{
		// Both motors need to turn in same direction for vertical motion
		motor[motorC] = -5;	
		motor[motorB] = -5;	
	}
	else if (compensateAngle > 10)
	{
		motor[motorC] = 5;
		motor[motorB] = 5;
	}
	else
		motor[motorB] = 0;
		motor[motorC] = 0;
}

// Brian Rojkov
float verticalDistance(float angle, int distance)
{
	return distance*sinDegrees(angle);
}

// Tiberiu Andronescu
float setMountingHeight(float angle, int distance)
{
	const int ARRAYSIZE = 50;
	// create temporary array of values to use in function
	int sensorReadings[ARRAYSIZE] = {0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0,};
	
	// initalize timer for each iteration
	clearTimer(T1);

	// input values over a span of 5 seconds
	while (time[T1] < 5000)
	{
		int index = 0;
		// every tenth of a second, add a distance value to the array
		if (time[T1] % 100 == 0)
		{
			sensorReadings[index] = distance;
			count++;
		}
	}
	
	int sum = 0; 
	
	// average the distance values
	for (int count = 0; count < ARRAYSIZE; count++)
	{
		sum += sensorReadings[count];
	}
	
	sum /= ARRAYSIZE; 
	
	return sum * sinDegrees(angle);	
}

//Tiberiu Andronescu
bool distanceCheck(int sensorPort)
{
	//displayBigTextLine(8, "top %f", SensorValue[sensorPort]);
	// bool to see if function is running through the if statement, need in main
	bool isRunning = false; 
	
	if (SensorValue[sensorPort] < 250 && SensorValue[sensorPort] > 10)
	// acceptable range, excludes false readings of 255 and 0
	{
		int waitTime = 0;
		
		// frequency of wait time changes based on sensor readings
		waitTime = SensorValue[sensorPort] * 5; 
		//displayString(1, "%d", SensorValue[sensorPort]);
		playSoundFile("Click.rsf");
		wait1Msec(waitTime);
		// eraseDisplay();

		isRunning = true;
	}
	return isRunning;
}

// Brian Rojkov
void elevation(float verDist, float mountingHeight)
{
	// condition if change in elevation is upwards, exlude 255 false readings
	if(((mountingHeight - verDist) >= 30) && (SensorValue[S2] < 250))
	{
		playSoundFile("Object.rsf");
		wait1Msec(500);
	}
	
	// condition if change in elevation is downwards, exlude 255 false readings
	else if(((mountingHeight - verDist) <= -30) && (SensorValue[S2] < 250))
	{
		playSoundFile("Detected.rsf");
		wait1Msec(500);
	}
}

// Usman Khan
// Provides tactile feedback if user is in danger of walking into obstacle
void lastResort(int dist) 
{
	if (dist < 15)
	{
		motor[motorA] = 100;
		wait1Msec(3000);
		motor[motorA] = 0;
	}
}

// Leroy Zhao
void buttonPress() {
	while (!getButtonPress(buttonAny))
	{}
	while (getButtonPress(buttonAny))
	{}
}

// Everyone
task main()
{
	// initialize all values
	motor[motorC] = 0;
	motor[motorB] = 0;
	wait1Msec(50);
	setSoundVolume(100);
	SensorMode[S1] = sensorEV3_Ultrasonic;
	SensorMode[S2] = sensorEV3_Ultrasonic;
	wait1Msec(50);
	
	// wait for button press to turn on device 
	buttonPress();
	
	// initialize mounting height to how high the device is attached to the user
	float mountingHeight = mountingHeight(60, SensorValue[S2]); 
	nMotorEncoder[motorB] = 0;
	nMotorEncoder[motorC] = 0;
	wait1Msec(50);
	SensorMode[S3] = sensorEV3_Gyro;
	SensorMode[S4] = sensorEV3_Gyro;
	int adjustGyro1 = SensorValue[S3];
	int adjustGyro2 = SensorValue[S4];
	wait1Msec(50);
	
	// main loop to continually run until down button is pressed to end
	while (!getButtonPress(buttonDown))
	{
		// Mode 1 no gyro rotation to prevent accumulation of gyro drift effects
		if (getButtonPress(buttonLeft)) 
		{																
			wait1Msec(2000);
			while(!getButtonPress(buttonAny))
			{
				distanceCheck(S1);

				/* swivelUltrasonic(SensorValue[S3], SensorValue[S4], 
				adjustGyro1, adjustGyro2);
				swivelVertical(SensorValue[S4]); */
				
				lastResort(SensorValue[S1]);
				
				while(!distanceCheck(S1) && !getButtonPress(buttonAny))
				{
					/* swivelUltrasonic(SensorValue[S3], SensorValue[S4], 
					adjustGyro1, adjustGyro2);
					swivelVertical(SensorValue[S4]); */
					elevation(verticalDistance(60, SensorValue[S2]), 
					mountingHeight);
					
					lastResort(SensorValue[S1]);
				}
			}
		}
		
		// Mode 2, gyro rotation enabled
		if (getButtonPress(buttonRight))
		{
			wait1Msec(2000);
			while(!getButtonPress(buttonAny))
			{
				distanceCheck(S1);
				swivelUltrasonic(SensorValue[S3], SensorValue[S4], adjustGyro1, 
				adjustGyro2);
				
				//swivelVertical(SensorValue[S4]);
				
				lastResort(SensorValue[S1]);
				
				while(!distanceCheck(S1) && !getButtonPress(buttonAny))
				{
					swivelUltrasonic(SensorValue[S3], SensorValue[S4], 
					adjustGyro1, adjustGyro2);
					
					//swivelVertical(SensorValue[S4]);

					elevation(verticalDistance(60, SensorValue[S2]), 
					mountingHeight);
					lastResort(SensorValue[S1]);
				}
			}
		}
	}
}
