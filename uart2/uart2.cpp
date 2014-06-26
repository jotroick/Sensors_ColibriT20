#include <windows.h>
#include <stdio.h>
#include <string.h>


#include "gpioLib.h"


/// Switch cases
#define READ                                1
#define WRITE                               2
#define EXIT                                3
 
//******************************************************************************
/// Configures COM port for specified baudrate
/// @param[in]    port         COM port
/// @param[in]    baudRate     Baudrate for communication
/// @retval       TRUE         Success
///               FALSE        Failure
BOOL PortOpen(HANDLE *port, DWORD baudRate)
{
    DCB portDCB;                                              ///< COM port configuration structure
    BOOL returnValue = FALSE;
    COMMTIMEOUTS comTimeOut;
    /// Open interface to reader
    *port = CreateFile(TEXT("COM2:"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_WRITE_THROUGH, NULL);
    if (*port == INVALID_HANDLE_VALUE)
    {
        printf("Error Opening COM Port\n");
        return FALSE;
    }
 
    /// COM Port Configuration
    portDCB.DCBlength = sizeof (DCB);                         ///< Initialize the DCBlength member
    GetCommState (*port, &portDCB);                           ///< Get the default port setting information.
    /// Change the DCB structure settings
    portDCB.BaudRate = baudRate;                              ///< Current baud 
    portDCB.fBinary = TRUE;                                   ///< Binary mode; no EOF check 
    portDCB.fParity = FALSE;                                  ///< Disable parity checking 
    portDCB.fOutxCtsFlow = FALSE;                             ///< No CTS output flow control 
    portDCB.fOutxDsrFlow = FALSE;                             ///< No DSR output flow control 
    portDCB.fDtrControl = DTR_CONTROL_DISABLE;                ///< Disable DTR flow control type 
    portDCB.fDsrSensitivity = FALSE;                          ///< DSR sensitivity 
    portDCB.fTXContinueOnXoff = TRUE;                         ///< XOFF continues Tx 
    portDCB.fOutX = FALSE;                                    ///< No XON/XOFF out flow control 
    portDCB.fInX = FALSE;                                     ///< No XON/XOFF in flow control 
    portDCB.fErrorChar = FALSE;                               ///< Disable error replacement 
    portDCB.fNull = FALSE;                                    ///< Disable null stripping 
    portDCB.fRtsControl = RTS_CONTROL_DISABLE;                ///< Disable RTS flow control 
    portDCB.fAbortOnError = FALSE;                            ///< Do not abort reads/writes on error
    portDCB.ByteSize = 8;                                     ///< Number of bits/byte, 4-8 
    portDCB.Parity = NOPARITY;                                ///< 0-4 = no, odd, even, mark, space 
    portDCB.StopBits = ONESTOPBIT;                            ///< 0, 1, 2 = 1, 1.5, 2 
 
    /// Configure the port according to the specifications of the DCB structure
    if (!SetCommState (*port, &portDCB))
    {
      printf("Error Configuring COM Port\n");                 ///< Could not configure the serial port
      return FALSE;
    }
 
    /// Get communication time out values
    returnValue = GetCommTimeouts(*port, &comTimeOut);
    comTimeOut.ReadIntervalTimeout = 100;
    comTimeOut.ReadTotalTimeoutMultiplier = 1;
    comTimeOut.ReadTotalTimeoutConstant = 1;
    /// Set communication time out values
    returnValue = SetCommTimeouts(*port, &comTimeOut);
 
    return TRUE;
}
 
//******************************************************************************
/// Close UART port
/// @param[in]    port     COM port
/// @retval       TRUE     Success
///               FALSE    Failure
BOOL PortClose(HANDLE *port)
{
    if (*port == NULL)
    {
        return FALSE;
    }
    CloseHandle(*port);
    *port = NULL;
    return TRUE;
}
 
//******************************************************************************
int wmain(void)
{
	
	
	InitGPIOLib();
	SetPinLevel(102, FALSE);
	SetPinLevel(104, FALSE);

	HANDLE hFile;
	HANDLE hAppend;
	HANDLE portHandle;
    
	char DataBuffer[] = "\r\n";
    DWORD dwBytesToWrite = (DWORD)strlen(DataBuffer);
    DWORD dwBytesWritten = 0;
	DWORD dwPos;
    DWORD dwBytesWritten_1;
	DWORD noOfBytesRead = 0;
    DWORD bytesTransmitted = 0;  
	DWORD dwBytesRead = 512;
    DWORD firstChoice = 0;
    BOOL retVal = FALSE;
    char transmitBuffer[128] = "Hello, this is UART test sisisvvi s isis code\n";
    char receiveBuffer[512] = {0};
	
 
    retVal = PortOpen(&portHandle, 9600);
    if (!retVal)
    {
        printf("Could not open COM port");
        getchar();
        return FALSE; 
    }   
    else
    {
        retVal = FALSE;
        while (TRUE)
        {
            memset(receiveBuffer, 0, 512); 
            printf("\n\n\n*****************************************\n");
            printf("Toradex AG\n");
            printf("*****************************************\n");
            printf("[1] Read\n");
            printf("[2] Write\n");
            printf("[3] Exit\n");
            printf("Enter your choice:\t");
            scanf_s("%d", &firstChoice);
 
            switch(firstChoice)
            {
                case READ:
                    
					ReadFile(portHandle, receiveBuffer, 512, &noOfBytesRead, NULL); // READING THE PORT
					
					 hFile = CreateFile (TEXT("\\GPS.txt"),      // Open GPS.txt
                      GENERIC_READ | GENERIC_WRITE,           // Open for reading
                      0,                      // Do not share
                      NULL,                   // No security
                      OPEN_ALWAYS,          // Existing file only
                      FILE_ATTRIBUTE_NORMAL,  // Normal file
                      NULL);                  // No template file

					  if (hFile == INVALID_HANDLE_VALUE)
						{
							// Your error-handling code goes here.
							printf ("Could not open GPS.txt");
							getchar();
							return FALSE;
						}
					  else 
						{
						 dwPos = SetFilePointer (hFile, 0, NULL, FILE_END); //write at the end of the file
					
                         WriteFile (hFile, DataBuffer, dwBytesToWrite,&dwBytesWritten, NULL); // new line
						 WriteFile (hFile, receiveBuffer, noOfBytesRead, &dwBytesWritten_1, NULL);
						 printf("%s\n", receiveBuffer);						
						        	
                        CloseHandle (hFile);
						PurgeComm(portHandle, PURGE_RXCLEAR);///< Clear receive buffer
						break;

						}


                   
              
				case WRITE:
                    WriteFile(portHandle, transmitBuffer, strlen(transmitBuffer), &bytesTransmitted, NULL);
                    break;
                case EXIT:
                    return TRUE;
                    break;
                default:
                    printf("Please enter your choice between 1-3..\n");
                    break;
            }
        }
    }
}