int J1C1[600];
int J1C2[600];
int J1C3[600];
int J1C4[600];
//Initialize joystick 1 buttons
int J1C5BU[600];
int J1C5BD[600];
int J1C6BU[600];
int J1C6BD[600];
int J1C7BU[600];
int J1C7BL[600];
int J1C7BD[600];
int J1C7BR[600];
int J1C8BU[600];
int J1C8BL[600];
int J1C8BD[600];
int J1C8BR[600];
//Initialize joystick 2
int J2C7BU[600];
int J2C7BL[600];
int J2C7BD[600];
int J2C7BR[600];
int J2C8BU[600];
int J2C8BL[600];
int J2C8BD[600];
int J2C8BR[600];
//Timer - update 1 time per driver cycle, every 25 msec
int DriveTimer = 0;
int RecSwitch = 0; // Switch between playback and record
 void updateDrive() { //Update chasis to joystick control
	 motorSet(1, joystickGetAnalog(1,2));
	 motorSet(2, joystickGetAnalog(1,2));
	 motorSet(3, joystickGetAnalog(1,3));
	 motorSet(4, joystickGetAnalog(1,3));
 }
 void recordDrive() { // Record driver input while allowing driver control
	 J1C2[DriveTimer] = joystickGetAnalog(1, 2);// Get analog value of vertical axis of right stick, joystick 1
	 J1C3[DriveTimer] = joystickGetAnalog(1, 4); // Get analog value of vertical axis of left stick, joystick 1
	 J1C5BU[DriveTimer] = joystickGetDigital(1,5,JOY_UP); // Get boolean value of upper right bumper
	 J1C5BD[DriveTimer] = joystickGetDigital(1,5,JOY_DOWN); // Get boolean value of lower right bumper
   J1C6BU[DriveTimer] = joystickGetDigital(1,6,JOY_UP); // Get boolean value of upper left bumper
	 J1C6BD[DriveTimer] = joystickGetDigital(1,6,JOY_DOWN); // Get boolean value of lower left bumper
 }
task main()
{
	while (1) {
    //J1C2[DriveTimer]; // Debug function, convert array of joystick 1 channel 2, input into stringp
		if (joystickGetDigital(1,7,JOY_DOWN) == 1){// If you push the lower button, turn on recording
			RecSwitch = 1;
			DriveTimer = 0;
		}
		if (joystickGetDigital(1,7,JOY_RIGHT) == 1){// If you push the right button, turn on normal drive
			RecSwitch = 2;
			DriveTimer = 0;
		}
		if (joystickGetDigital(1,7,JOY_UP)){ // If you push the upper button, turn off recording
			RecSwitch = 0;
		}
		if (RecSwitch == 1) { // Begin Record function
			recordDrive();
			updateDrive();
		} // End Record function
	  if (RecSwitch == 2) { // Begin normal drive function
		//	updateDrive();
		}
		if (RecSwitch == 0) { // Begin playback function
			motorSet(1, J1C2[DriveTimer]); // Set motor to array values
			motorSet(2, J1C2[DriveTimer]);
			motorSet(3, J1C3[DriveTimer]);
			motorSet(4, J1C3[DriveTimer]);
		}
    DriveTimer = DriveTimer + 1; // Record time
		delay(25); // Wait for refresh
	}



}
