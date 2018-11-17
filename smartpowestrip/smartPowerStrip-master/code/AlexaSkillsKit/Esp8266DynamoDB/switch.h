#ifndef SWITCH_H
#define SWITCH_H

typedef struct attribute{
	int 	val;
	char*	dbName;	
}attrib;

typedef struct switchInfo{
	int			outPin;
	attrib 		devID;
	attrib		onStatus;
	attrib		brightLvl;
}espSwitch;


#endif // end of SWITCH_H
