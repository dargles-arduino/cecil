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
  int  xReg;
  int  yReg;
  int  progCounter;
  bool zeroFlag;
  bool negFlag;
  bool carryFlag;
} registers;

class sim40
{
  private:
  int       memory[1023];
  registers regs;
  
  public:
  bool      running = false;

  // The constructor
  sim40(){
  }

  /**
   * loadMem
   * 
   * loadMem uploads values into the sim40 memory.
   * @param int startAddress
   * @param array int values[]
   * @return bool success
   */
   bool loadMem(int startAddress, int values[], int noOfEntries){
     bool success = true;
     // Check the parameters
     int endAddress = startAddress + noOfEntries - 1;
     Serial.printf("endAddress is: %i\n", endAddress);
     if(startAddress<0 || endAddress>1023){
      success = false;
      return success;
     }
     int arrayPtr = 0;
     for(int i=startAddress;i<=endAddress;i++){
      Serial.printf("Writing %i to memory\n", values[arrayPtr]);
      memory[i]=values[arrayPtr++];
     }
     return success;
   }

  /**
   * displayMem
   * 
   * displayMem displays values in the sim40 memory.
   * @param  int  startAddress
   * @param  int  endAddress
   * @return bool success
   */
   bool displayMem(int startAddress, int endAddress){
     bool success = true;
     // Check the parameters
     if(startAddress<0 || endAddress>1023 || startAddress>endAddress){
      Serial.printf("Start or end address is out of range for memory write");
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
     Serial.println();
     return success;
   }

 /**
  * setStartVector
  * 
  * Sets the startVector to the given value
  * @param  int  address
  * @return bool success
  */
  bool setStartVector(int address){
    bool success = true;
    if(address<0 || address>1023){
      Serial.printf("\n!!StartVector out of range: %i\n", address);
      success = false;
      return success;
    }
    memory[1023] = address;
    if(!running)regs.progCounter = address;
    return success;
  }

 /**
  * doInstruction
  * 
  * Actions the next instruction
  */
  void doInstruction(){
    int instruction = memory[regs.progCounter++];
    Serial.printf("Next instruction is %i\n", instruction);
    switch(instruction){
      case  0: //NOP - but stop for now!
        //running=false;
        break;
      case  1: //load
        regs.acc = memory[memory[regs.progCounter++]];
        Serial.printf("Setting acc to %i\n",regs.acc);
        break;
      case  2: //store
        memory[memory[regs.progCounter++]] = regs.acc;
        break;
      case  3: //add
        regs.acc = regs.acc + memory[memory[regs.progCounter++]];
        if(regs.carryFlag)regs.acc++;
        if(regs.acc>1023){
          regs.acc = regs.acc%1024;
          regs.carryFlag = true;
        }
        Serial.printf("acc is now %i\n",regs.acc);
        break;
      case  4: //sub
        regs.acc = regs.acc - memory[memory[regs.progCounter++]];
        if(!regs.carryFlag)regs.acc++;
        if(regs.acc<0){
          regs.acc = regs.acc+1024;
          regs.negFlag = true;
        }
        Serial.printf("acc is now %i\n",regs.acc);
        break;
      case  5: //and
        break;
      case  6: //or
        break;
      case 21: //print
        Serial.print(regs.acc);
        break;
      case 32: //cset
        regs.carryFlag=true;
        break;
      case 33: //cclear
        regs.carryFlag=false;
        break;
      case 38: //stop
        running=false;
        break;
      default:
        //running=false;
        break;
    }
    
    if(regs.progCounter>1023){
      Serial.println("!!Error: program counter overflow");
      running=false;
    }
    return;
  }
};
