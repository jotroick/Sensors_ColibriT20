#ifndef MS561101BA_h
#define MS561101BA_h

//#include <unistd.h>

//#include "stdint.h"
#include <math.h>
#include <windows.h> 
//#include "I2Cdev/I2Cdev.h"

// device addresses 
#define MS561101BA_ADDR_CSB_HIGH  0x76   //CBR=1 0x76 I2C address when CSB is connected to HIGH (VCC)
#define MS561101BA_ADDR_CSB_LOW   0x77   //CBR=0 0x77 I2C address when CSB is connected to LOW (GND, Default)

// device registers 
#define MS561101BA_RESET 0x1E // reset
#define MS561101BA_D1 0x40 //D1 Base register (256 OSR)
#define MS561101BA_D2 0x50 //D2 Base register (256 OSR)
#define MS561101BA_PROM_BASE_ADDR 0xA2 // address for the first of 6 PROM registers (2 bytes each)

// others defines
#define OSR_CONV_TIME 3// OSR conversion time (us x sample)



class MS561101BA {
  public:
    MS561101BA();
    BYTE init(BYTE addr = MS561101BA_ADDR_CSB_LOW);    
    BYTE readData(WORD OSR = 2048);        
    void setRefPressure(float ref_press);
    float getRefPressure();
    bool dataAvailable();
    
    float temperature; // Temperature calculated (ºC)
    float pressure; // Pressure calculated (mbar)
    float altitude; // Altitude calculated (meters)
    
    /*function to check formulas
    void test();*/
    
private:
    BYTE _addr; // device address
    WORD _C[6]; // PROM registers    
    DWORD D1; // raw pressure value
    DWORD D2; // raw temperature value
    INT32 dT; // Difference between actual and reference temperature 
    INT32 TEMP; // Actual temperature (-40...85°C with 0.01°C resolution)
    INT32 OFF; // Offset at actual temperature
    INT64 SENS; //sensivity at actual temperature
    INT32 P; // Temperature compensated pressure (10...1200mbar with 0.01mbar resolution) 
    INT32 T2; // Second order temperature compensation
    INT64 OFF2; // Offset second order temperature compensation
    INT64 SENS2;// Sensivity second order temperature compensation
    BYTE oversampling; // Oversampling register increase value
    float ref_pressure; // Reference pressure (standar = 1013 mbar);
    bool data_available; // TRUE = new data available, get it reading public variables
    
    BYTE reset();
    BYTE readPROM();
};

#endif // MS561101BA_h