# korvex
A assemblage of the code work here at korvex

## Argument-based operator control

In order to simplify changes to the operator control program, I decided to switch to arguments for input, both digital and analog. This allows us to make quick changes without complication. It also allows for everybody on the team to fully understand the code, and be able to modify it if necessary.

### Control Modifiers
```c
//control modifiers
bool isReverse = false;
bool isFineControl = false;
float fineControl = 1;
```
These are the control modifiers. It allows for us to toggle a lower increment in acceleration, or reverse the drive, allowing for more precise and adjustable controls.

### Update Drive
```c
void updateDrive(int chassisControlLeft, int chassisControlRight, int liftControl) {
  //chassis control
  if (isReverse == true) { //if in reverse, invert and switch sides for normal turning
    motorSet(driveLeft, (chassisControlRight * -1));
    motorSet(driveRight, (chassisControlLeft * -1));
  }
  else if (isReverse == false) { //if in normal operator, do not invert
    motorSet(driveRight, (chassisControlRight));
    motorSet(driveLeft, (chassisControlLeft));
  }
  //lift control
  motorSet(dr4bLeft, (liftControl));
  motorSet(dr4bRight, (liftControl));
}
```
This is the first function that is argument based. It sets the motors to the speed of the input, and checks if the bot is in reverse. If so, it swaps the sides so clockwise and counterclockwise turning is not swapped, and input values negated.

### Fine Control Toggle
```c
void fineControlToggle(int fineBtn, int fineBtn2, int reverseBtn, int reverseBtn2) {
  // fine control toggle
  if (fineBtn == 1) { // toggle it on
    isFineControl = true;
    fineControl = .5;
  }
  if (fineBtn2 == 1) { // toggle it off
    isFineControl = false;
    fineControl = 1;
  }
  //reverse toggle
    if (reverseBtn == 1) { // toggle it on
    isReverse = true;
    fineControl = -1;
  }
  if (reverseBtn2 == 1) { // toggle it off
    isReverse = false;
    fineControl = 1;
  }
}
```
This toggles fine control and reverse on and off. When toggled, it sets the corresponding bool to the desired value. The input is set by an argument.

### Mobile Goal Control
```c
void mobileGoalControl (int moboLiftBtnUp, int moboLiftBtnDown, int moboTiltBtnUp, int moboTiltBtnDown) {
  // mobo lift control
  if (moboLiftBtnUp == 1) {
    motorSet(4, 127);
  }
  if (moboLiftBtnDown == 1) {
    motorSet(4, -127);
  }
  if (moboLiftBtnUp == 0 &&
      moboLiftBtnDown == 0) {
    motorSet(4, 0);
  }
}
```
This takes the digital input from the buttons provided in the arguments, and turns motors on and off accordingly.

## Cone Handler Control
```c
void coneHandlerControl(int clawBtnUp, int clawBtnDown, int chainbarBtnUp, int chainbarBtnDown) {
  motorSet(8, (joystickGetAnalog(2,2)));
  motorSet(8, (joystickGetAnalog(2,2)));
  if (chainbarBtnUp == 1) {
    // move up
    motorSet(8, 127);
  }
  if (chainbarBtnDown == 1) {
    // move down
    motorSet(8, -127);
  }
  if (chainbarBtnUp == 0 &&
      chainbarBtnDown == 0) {
    // dont move
    motorSet(8, 0);
  }
  // claw control
  if (clawBtnUp == 1) {
    // move up
    motorSet(9, 127);
  }
  if (clawBtnDown == 1) {
    // move down
    motorSet(9, -127);
  }
  if (clawBtnUp == 0 &&
      clawBtnDown == 0) {
    // dont move
    motorSet(8, 0);
  }
}
```
This takes input from the arguments and sets the motors accordingly, like the mobile goal control. However, currently the chain bar is set by an analog joystick.

## Loop
```c
void operatorControl() {
  while (isEnabled()) {
    delay(20);
    //chassisControl chassisControlLeft, chassisControlRight, liftControl
    //fineControlToggle fineBtn
    //mobileGoalControl moboLiftBtn, moboTiltBtn
    //coneHandlerControl clawBtn, chainbarBtn
    updateDrive(joystickGetAnalog(1,3), joystickGetAnalog(1,2), joystickGetAnalog(2,2));
    fineControlToggle(joystickGetDigital(1, 7, JOY_DOWN));
    mobileGoalControl(joystickGetDigital(2, 6, JOY_UP), joystickGetDigital(2, 6, JOY_DOWN), joystickGetDigital(2, 8, JOY_UP), joystickGetDigital(2, 8, JOY_DOWN));
    coneHandlerControl(joystickGetDigital(2, 5, JOY_UP), joystickGetDigital(2, 5, JOY_DOWN), joystickGetDigital(2, 7, JOY_DOWN), joystickGetDigital(2, 7, JOY_UP));
  }
}
```

## Korvex V2

### Motor Port Map
```c
port 1 = **scrubbed**
port 2 = drive left side (y-cable to power expander)
port 3 = drive right side (y-cable to power expander)
port 4 = mobile goal lift (y-cable)
port 5 = chain bar 1
port 6 = dr4b left (y-cable)
port 7 = dr4b right (y-cable)
port 8 = second dof on lift (y-cable)
port 9 = claw (direct)
port 10 = **scrubbed**
```
Current port map, use whatever is applicable
### Control Map
```c
drive left side = mainLeftJoy(1,3)
drive right side = mainRightJoy(1,2)
mobile goal lift = seccondaryRightBumper(2,5)
lift = seccondaryRightJoy(2,2)
second dof on lift = seccondaryLeftJoy(2,3)
claw = seccondaryLeftBumper(2,6)
```
The first number in the sequence refers to the controler, the second refers to channel

## ReRun Documentation

At the end of the 2016-17 season, VEX Starstruck, we reviewed what had we done right and what we needed to improve on for the 2017-18 season. One of the aspects we decided on improving upon was our autonomous mode. In the previous season, our autonomous was both difficult to change and inaccurate. We decided that we would implement two new methods into our autonomous. A ‘rerun’ function, and a sensor based self-correcting mode. The rerun function would feature the ability to record a user’s actions, store them, and play them back accurately. To do this, I decided to record the inputs and store them in a two-dimensional array, a vector. 

### cache
```c
int cache[6][600]
```
The cache is the two-dimensional array. The [6] represents how many inputs it takes in. The [600] is where I store the input values. It is 600 because the values update 4 times a seccond and the autonomous is 15 secconds long.

### record function
```c
void recordDrive() { // Record driver input while allowing driver control
   delay(1);
	 cache[1][DriveTimer] = joystickGetAnalog(1, 2);// Get analog value of vertical axis of right stick, joystick 1
	 cache[2][DriveTimer] = joystickGetAnalog(1, 4); // Get analog value of vertical axis of left stick, joystick 1
	 cache[3][DriveTimer] = joystickGetDigital(1,5,JOY_UP); // Get boolean value of upper right bumper
	 cache[4][DriveTimer] = joystickGetDigital(1,5,JOY_DOWN); // Get boolean value of lower right bumper
   	 cache[5][DriveTimer] = joystickGetDigital(1,6,JOY_UP); // Get boolean value of upper left bumper
	 cache[6][DriveTimer] = joystickGetDigital(1,6,JOY_DOWN); // Get boolean value of lower left bumper
}
```
The recordDrive function is how the data is stored. It allows the data to be stored while also allowing driver control over the robot. The delay is for redundancy. Cache[1] specifies that I am recording the analog Y axis input of the right stick on the first controller. The [DriveTimer] is to store what time the data is recorded, so that it can be replayed in the same order and frequency.

### playback function
```c
void playbackDrive() { // Set motor to array values
   delay(1);
   motorSet(2, -cache[1][DriveTimer]);
   motorSet(3, -cache[1][DriveTimer]);
   motorSet(4, cache[2][DriveTimer]);
   motorSet(5, cache[2][DriveTimer]);
}
```
This function takes the stored input values and sets them to the corresponding motors. The delay is for redundancy.

### set mode function
```c
void setMode() { // Check buttons to set mode
   delay(1);
   if (joystickGetDigital(1,7,JOY_DOWN) == 1){// If you push the lower button, turn on recording
     RecSwitch = 1;
     DriveTimer = 0;
   }
   if (joystickGetDigital(1,7,JOY_RIGHT) == 1){// If you push the right button, turn on normal drive
     RecSwitch = 2;
     DriveTimer = 0;
   }
   if (joystickGetDigital(1,7,JOY_LEFT) == 1){// If you push the left button, turn on playback from cache
     RecSwitch = 3;
     DriveTimer = 0;
     setCache();
   }
   if (joystickGetDigital(1,7,JOY_UP)){ // If you push the upper button, turn off recording NOT USABLE
     RecSwitch = 0;
   }
   if (RecSwitch == 1) { // Begin Record function
     recordDrive();
     updateDrive();
   } // End Record function
   if (RecSwitch == 2) { // Begin normal drive function
     updateDrive();
   }
   if (RecSwitch == 3) { // Begin normal drive function
     playbackDrive();
   }
   if (RecSwitch == 0) { // Begin playback function
     playbackDrive();
   }
}
```
(the function was too large to fit onto the page, had to compress it)
This function sets the mode of the program. The modes include record, playback, user control, and a placeholder for future functions. If a button is pushed on the controller, the RecSwitch value is changed to an integer (0,1,2,3).

### op control loop
```c
void operatorControl() { //Motor documentation, 1-2 left joystick, 3-4 right joystick, Left Claw 8 Right Claw 9
	while (1) {
    setMode();
    clawSync();
    updateSensors();
    //printf("Value is %d, time is %d \n", J1C2[DriveTimer], DriveTimer);
    //printf("J1C2[%d] = %d \n", DriveTimer, J1C2[DriveTimer]); //Print statements formatted so that copy-paste will work
    //printf("J1C3[%d] = %d \n", DriveTimer, J1C3[DriveTimer]);
    printf("left value %d \n", leftencodervalue);
    printf("right value %d \n", rightencodervalue);
    DriveTimer =+ 1; // Record time
		delay(50); // Wait for refresh
	}
}
```
This loop brings everything together. It runs all the necessary functions, in a much cleaner way than in my previous seasons. Debug code placeholders are also included.
