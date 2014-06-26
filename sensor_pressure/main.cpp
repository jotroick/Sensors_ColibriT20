//#include <windows.h> 
//#include "gpioLib.h"
//#include "I2cLib.h"
// 
//int wmain()
//{
// 
//    BYTE MS561101BA_ADDR_CSB_HIGH = 0x77;                                               ///< Device address 
//    BYTE MS561101BA_RESET = 0x1E;
//    BYTE MS561101BA_READ = 0xA0;
//
//	BYTE DATA_READ[2] = {0x00,0x00};
//
//	//BYTE eepromData[8] = {0x00, 0x00, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};   ///< First two bytes denote the address where data will be written, next 6 bytes are the data to be written
//   // BYTE eepromDataRead[8] = {0x00, 0x00, 0, 0, 0, 0, 0, 0};                 ///< Buffer for data read
//    BOOL i2cError = FALSE;
//    DWORD verMaj = 0;
//    DWORD verMin = 0;
//    DWORD verBuild = 0;
// 
//    I2CInit();                                                       ///< Initialize I2C bus
//    I2CSetSpeed(I2C_400KBPS);                                        ///< Set I2C bus speed
//    I2CGetLibVersion(&verMaj, &verMin, &verBuild);
//    printf("I2CDemo (I2CLib Version %d.%d.%d)\n", verMaj, verMin, verBuild);
// 
//   i2cError = GetI2CLock(5000);                                     ///< Wait for 5000 ms to acquire I2C lock 
//    if (i2cError == FALSE)
//    {
//        printf("cannot acquire I2C lock\n");
//        getchar();
//        return FALSE;
//    }
//    printf("empezando a escribir\n");
//	i2cError = I2CBurstWrite(MS561101BA_ADDR_CSB_HIGH, NULL, MS561101BA_RESET, 1);      ///< Writes the 6 bytes of data to the device
//    printf("empezando a escribir2\n");
//	if (i2cError == FALSE)
//    {
//        printf("cannot perform I2C write operation\n");
//        ReleaseI2CLock();
//        I2CDeInit();
//        getchar();
//        return FALSE;
//    }
//    printf("Data being written is %x\n", MS561101BA_RESET);
//
// 
//   //i2cError = I2CBurstWrite(eepromAddress, eepromData, -1, 2);      ///< Writes the first two bytes of eepromData i.e. the address to the EEPROM
//    Sleep(10);                                                       ///< delay to finish write operation to EEPROM
//    printf("empezando a leer\n");
//	i2cError = I2CBurstRead(MS561101BA_ADDR_CSB_HIGH, DATA_READ, MS561101BA_READ, 2);   ///< To read the 6 bytes of data written at the given address (0x00)
//    if (i2cError == FALSE)
//    {
//        printf("cannot perform I2C read operation\n");
//        ReleaseI2CLock();
//        I2CDeInit();
//        getchar();
//        return FALSE;
//    }
//    printf("Data read is %x %x\n", DATA_READ[0], DATA_READ[1]);
//   ReleaseI2CLock();                                                ///< Release the I2C bus for other threads
// 
//    printf("\nPress Enter to close the program\n");
//    getchar();
//    I2CDeInit();
//    return TRUE;
//}

///
/// @file         I2cLibDemo.c 
/// @copyright    Copyright (c) 2013 Toradex AG
/// $Author: gap $
/// $Revision: 1633 $
/// $Date: 2013-05-24 09:22:26 +0200 (Fr, 24 Mai 2013) $
/// @brief        Demo for I2C Library
/// @target       Colibri Modules
/// @test tested on:  Colibri PXAxxx, T20, T30

#include <windows.h>
#include <wininet.h> 
#include "I2cLib.h"

//------------------------------------------------------------------------------------
/// Try to access all Slave Addresses without data. If a slave is present, it will ACK the address

#define MS561101BA_PROM_BASE_ADDR 0xA2
#define MS561101BA_D1_CONV_ADDR 0x40
#define MS561101BA_D2_CONV_ADDR 0x50
#define OSR_CONV_TIME 3

void i2cScan(void)
{
	int i, count=0;
	BOOL Success;

	wprintf (L"Scanning i2c bus...\r\n");
	for (i = 1; i < 127; i++)
	{
		Success = I2CBurstWrite(i, NULL, 0, 0);
		if (Success)
        {
			wprintf(L"%s0x%02x", ((count++ == 0)? L"  " : L", "), i);
        }
        //Sleep(1);
	}
	wprintf (L"\r\n%d device(s) found.\r\n", count);
}

//------------------------------------------------------------------------------------
/// WinMain
/// @param	no parameters supported
/// @retval	application exit code
int WINAPI wmain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	BOOL Success;
	DWORD i;
	int j;
	
	DWORD VerMaj;
	DWORD VerMin;
	DWORD VerBuild;

	DWORD D1;
	DWORD D2;
	
	WORD COEF[6]; // PROM DATA CALIBRATION C1...C6
	WORD OSR; // OVERSAMPLING RATIO
	BYTE oversampling;

	INT32 dT; // Difference between actual and reference temperature 
    INT32 TEMP; // Actual temperature (-40...85°C with 0.01°C resolution)
    INT64 OFF; // Offset at actual temperature
    INT64 SENS; //sensivity at actual temperature
    INT64 P; // Temperature compensated pressure (10...1200mbar with 0.01mbar resolution) 
    INT32 T2; // Second order temperature compensation
    INT64 OFF2; // Offset second order temperature compensation
    INT64 SENS2;// Sensivity second order temperature compensation

	float temperature; // Temperature calculated (ºC)
    float pressure; // Pressure calculated (mbar)
    float altitude; // Altitude calculated (meters)
	float ref_pressure = 1013;
	
	I2cIntGetLibVersion(&VerMaj, &VerMin, &VerBuild);
	wprintf(L"Demo Application for I2cLib\r\n");
	wprintf(L"Library Version %d.%d Build %d\r\n\r\n", VerMaj, VerMin, VerBuild);

	if (I2CInit())
	{
		BYTE data[100] = {0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 
						  0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 
						  0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 
						  0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 
						  0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x0F
	
						 
		};
	
		BYTE data_read[4] = {0,0,0,0};
		I2CSetSpeed(I2C_400KBPS);

		Success = GetI2CLock(500);
		wprintf(L"write bytes...");			// write 50 bursts, 100 bytes each
		for (i = 0; i < 50; i++)
		{
			if (!I2CBurstWrite(0x68, data, 0, 100))  // slave addr + offset are already 2 bytes
            {
				break;
            }
		}
		wprintf(L"      %i bytes sent\r\nLast Error Code %d\r\n", i * 100, GetLastError());

		// BusScan
		i2cScan();

// 1. RESET COMAND

		Success = I2CBurstWrite(0x76, NULL, 0x1E, 0); /// RESET COMMAND
		if (Success)
        {
			printf("si RESET\n\n");
        }
        
/// 2. READ PROM - CALIBRATION WORDS	
		
	for ( j=0;j<6;j++) {   
		Success = I2CBurstRead( 0x76, data_read, MS561101BA_PROM_BASE_ADDR + (j * 2), 2);
		if (Success) {
			COEF[j] = data_read[0] << 8 | data_read[1];
			printf("Data read is %x %x\n", data_read[0], data_read[1]);
			printf("Data save is %x \n", COEF[j] );
				}
		else{
			printf("Error reading PROM");	
			}
  
		COEF[j] = data_read[0] << 8 | data_read[1];    
		}

/// 3. D1 CONVERSION

	OSR = 4096;

switch (OSR) {
    case 256:
      oversampling = 0x00;
      break;
    case 512:
      oversampling = 0x02;
      break;
    case 1024:
      oversampling = 0x04;
      break;
    case 2048:
      oversampling = 0x06;
      break;
    case 4096:
      oversampling = 0x08;
      break;
    default:
      oversampling = 0x06;
      OSR = 2048;
  };


Success = I2CBurstWrite(0x76, NULL, MS561101BA_D1_CONV_ADDR + oversampling, 0); /// D1 CONVERSION 
		if (Success)
        {
			printf("Comand D1 SEND \n\n");
        }

Sleep(OSR * OSR_CONV_TIME/1000); // WAIT FOR CONVERSION


Success = I2CBurstRead( 0x76, data_read, 0x00, 3);  //READ ADC
		if (Success) {
			D1 = 0;
			D1 = data_read[0] << 16 | data_read[1] << 8 | data_read[2];
			printf("Data D1 read is %x %x %x\n", data_read[0], data_read[1], data_read[2]);
			printf("Data save is %x \n", D1 );
				}

///4. D2 CONVERSION


Success = I2CBurstWrite(0x76, NULL, MS561101BA_D2_CONV_ADDR + oversampling, 0); /// D1 CONVERSION 
		if (Success)
        {
			printf("Comand D2 SEND \n\n");
        }

Sleep(OSR * OSR_CONV_TIME/1000); // WAIT FOR CONVERSION


Success = I2CBurstRead( 0x76, data_read, 0x00, 3);  //READ ADC
		if (Success) {
			D2 = 0;
			D2 = data_read[0] << 16 | data_read[1] << 8 | data_read[2];
			printf("Data D2 read is %x %x %x\n", data_read[0], data_read[1], data_read[2]);
			printf("Data save is %x \n\n", D2 );
				}

//5. CALCULATIONS

		// TEMPERATURE
		dT = D2 - COEF[4] * (2 << 7);
		printf("difference is %x \n", dT );
		TEMP = 2000 + dT * COEF[5] / (2 << 22);
		printf("TEMP 1 is %x \n", TEMP );
		
		//TEMPERATURE COMPENSATED PREASURE
		T2 = 0;
  OFF2 = 0;
  SENS2 = 0;
  if (TEMP < 2000) {
    T2 = (dT * dT) / (2 << 30);
    OFF2 = 5 * ((TEMP - 2000) * (TEMP - 2000)) / 2;
    SENS2 = 5 * ((TEMP - 2000) * (TEMP - 2000)) / 4;
  }
  if (TEMP < -1500) {
    OFF2 = OFF2 + 7 * ((TEMP + 1500) * (TEMP + 1500));
    SENS2 = SENS2 + 11 * ((TEMP + 1500) * (TEMP + 1500)) / 2;
  }
  TEMP = TEMP - T2;
      printf("TEMP 2 is %x \n", TEMP );
  // calculate temperature compensated pressure and set the public variables
  OFF = (INT64) (WORD)COEF[1] * (2 << 15) + ((WORD)COEF[3] * dT) / (2 << 6); // Offset at actual temperature
  
  SENS = COEF[0] * (2 << 14) + (COEF[2] * dT) / (2 << 7); // Sensitivity at actual temperature
  
  OFF = OFF - OFF2;
  
  SENS = SENS - SENS2;
  
  P = (D1 * SENS / (2 << 20) - OFF) / (2 << 14);  
  
  temperature = TEMP / (float)100;   
  
  pressure = P / (float)100;
    
  // calculate altitude and set the public variable
  altitude = ((1 - pow(pressure / ref_pressure, 0.19026)) * 288.15) / 0.00198122; // feet
  altitude = altitude * 0.3048;// conversion to meters
  

  printf("TEMP is %5.2f \n", temperature );
  printf("PREASURE is %5.2f\n", pressure );
  printf("ALTITUD is %5.2f \n", altitude );


		wprintf(L"Reading the RTC...\r\n");
        
		I2CBurstRead(0x68, data, 0, 8);

        wprintf(L"Resetting the RTC...\r\n");
        for (i = 0; i < 8; i++)
        {
            data[i] = 0;
        }
        I2CBurstWrite(0x68, data, 0, 8);

        wprintf(L"Reading the RTC...\r\n");
        I2CBurstRead(0x68, data, 0, 8);
		ReleaseI2CLock();
	}
	I2CDeInit();

	wprintf(L"\r\npress any key to end...");
	getchar();
	return(0);
}

