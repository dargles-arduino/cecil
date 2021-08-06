/**
 * Class definition for the SIM40
 * 
 * The SIM40 class simulates the Cecil microcontroller. It has 1K of 10 bit 
 * wide memory and has an accumulator, a program counter, status flags, and 
 * x and y registers. More details can be found in the Cecil handbook.
 * 
 * @author  David Argles, d.argles@gmx.com
 * @version 06Aug2021 05:53h
 */
 
#define STACK        908
#define STACKSIZE    9
#define STACKPTR     1007
#define PITCH        1011
#define DURATION     1012
#define KEYB_IN      1013
#define VID_OUT      1015
#define SERIAL_OUT   1017
#define SERIAL_IN    1018
#define PARALLEL_OUT 1020
#define PARALLEL_IN  1021
#define START_V      1023

typedef struct{
  int  acc;
  int  xreg;
  int  yreg;
  int  progcounter;
  bool zeroflag;
  bool negflag;
  bool carryflag;
} registers;

class sim40
{
  private:
  int       memory[1023];
  registers regs;
  
  public:

  // The constructor
  sim40(){
  }

  /**
   * loadMem
   * 
   * loadMem uploads values into the sim40 memory.
   * @param int startAddress
   * @param array int values[]
   */
   bool loadMem(int startAddress, int values[]){
     bool success = true;
     // Check the parameters
     int endAddress = startAddress + (sizeof(values)/sizeof(values[0]));
     if(startAddress<0 || endAddress>1023){
      success = false;
      return success;
     }
     for(int i=startAddress;i<=endAddress;i++)memory[i]=values[i];
     return success;
   }

  /**
   * displayMem
   * 
   * displayMem displays values in the sim40 memory.
   * @param int startAddress
   * @param int endAddress
   */
   bool displayMem(int startAddress, int endAddress){
     bool success = true;
     // Check the parameters
     if(startAddress<0 || endAddress>1023 || startAddress>endAddress){
      success = false;
      return success;
     }
     // We're clear to go
     int  pointer = startAddress;
     int  counter = 0;
     char buff[5];
     int  linecount = 0;
     while(pointer<=endAddress){
       sprintf(buff," %04d", memory[pointer++]);
       if(linecount++%10!=9)Serial.print(buff);      
       else Serial.println(buff);
     }
     
     return success;
   }
};
