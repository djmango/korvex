
task main()
{
	while(1){
	motor[port1] = 127;
	wait1Msec(500);
	motor[port1] = 0;
	}
}
