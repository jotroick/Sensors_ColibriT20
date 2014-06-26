#include <windows.h> 
#include <winbase.h> 
#include "GpioLib.h"
#include <stdlib.h>   
 
int wmain()   
{ 
    BOOL inputVal = FALSE;           ///< Variable to store input status 
	
	
    InitGPIOLib();
    printf("Demo application to show GPIO as Input\n");
    
	SetPinAltFn(101, -1, DIR_IN);    ///< Set SODIMM Pin 101 as GPIO(-1) and set to Input
    
	inputVal = GetPinLevel(101);     ///< Get the logic level of SODIMM pin 101
 	SetPinLevel(101, 1);
	Sleep(250);
	
	SetPinLevel(101, 0);
	Sleep(20);

	SetPinLevel(101, 1);
	usleep(20);



	printf("SODIMM pin 101 level: %d\n", inputVal);
 
    DeInitGPIOLib();                 ///< Release resources used by GPIO library
    printf("Press Enter to end the program\n");
    getchar();
    return(TRUE);
}