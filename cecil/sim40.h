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
 
#define ANALOGUE_IN  904  // From ADC
#define ANALOGUE_OUT 905  // To DAC
#define TIMER        906
#define STACK        908
#define STACK_SIZE     9
#define STACK_PTR    1007
#define INT_V        1008
#define INT_ENABLE   1009
#define RANDOM_GEN   1010
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
  /* NOTE WELL!! Memory needs to go from 0 - 1023, but it is essential that 
   * you declare memory[1024] to say you need 1024 spaces! 
   */
  int       memory[1024]; 
  registers regs;
  int       value;
  char      item;
  char      buff[5];
  bool      simRunning = false;
  
  public:
  bool      trace = true;
  String    output = "";

  // The constructor
  sim40(){
    memory[STACK_PTR] = STACK;
  }

  /**
   * stackPush
   * 
   * stackPush pushes values onto the sim40 stack.
   * @param int value
   * @return bool success
   */
   bool stackPush(int value){
    if(memory[STACK_PTR]<(STACK+STACK_SIZE)){
      if(trace){
        Serial.printf("Pushing %i onto stack\n",value);
        //output += "Pushing " + String(value) + " onto stack\n";
        Serial.printf("Stack pointer is %i\n",memory[STACK_PTR]);
        //output += "Stack pointer is  " + String(memory[STACK_PTR]) + "\n";
      }
      memory[memory[STACK_PTR]] = value;
      memory[STACK_PTR]++;
      if(trace)Serial.printf("Stack pointer is %i\n",memory[STACK_PTR]);
    }
    else{
      Serial.println("Stack overflow\nRun terminated");
      output += "!!RUN ERROR: Stack overflow\n";
      simRunning = false;
      return false;
    }
    return true;
   }

  /**
   * stackPull
   * 
   * stackPull pulls values off the sim40 stack.
   * @return int value
   */
   int stackPull(){
    value = -1;
    if(trace)Serial.printf("Stack pointer is %i\n",memory[STACK_PTR]);
    if(memory[STACK_PTR]>(STACK)){
      value = memory[memory[STACK_PTR]-1];
      if(trace)Serial.printf("Pulling %i from stack\n",value);
      memory[STACK_PTR]--;
      if(trace)Serial.printf("Stack pointer is %i\n",memory[STACK_PTR]);
    }
    else{
      Serial.println("Stack underflow\nRun terminated");
      output += "!!RUN ERROR: Stack underflow\n";
      simRunning = false;
    }
    return value;
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
     if(trace)Serial.printf("endAddress is: %i\n", endAddress);
     if(startAddress<0 || endAddress>1023){
      success = false;
      return success;
     }
     int arrayPtr = 0;
     for(int i=startAddress;i<=endAddress;i++){
      if(trace)Serial.printf("Writing %i to memory\n", values[arrayPtr]);
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
   * @return String memory (memory contents)
   */
   String displayMem(int startAddress, int endAddress){
     //Serial.println("Entering display dump routine");
     String result = "";
     // Check the parameters
     if(startAddress<0 || endAddress>1023 || startAddress>endAddress){
      result = "Start or end address is out of range for memory access";
      return result;
     }
     // We're clear to go
     int  pointer = startAddress;
     int  counter = 0;
     int  itemsOnLine = 0;
     while(pointer<=endAddress){
       sprintf(buff," %04d", memory[pointer++]);
       //Serial.print("Adding next memory content: ");
       //Serial.println(memory[pointer]);
       //result += " ";
       result += buff;
       //for(int i=0;i<5;i++)result += buff[i];
       if(itemsOnLine++%8==7){
        result += "\n";
        //Serial.println();
       }
     }
     result += "\n";
     return result; //"[memory dump]";//
   }

  /**
   * displayRegs
   * 
   * displayRegs displays values in the sim40 registers.
   */
   String getRegs(){
     String op = "";
     sprintf(buff," %04d", regs.acc);
     op += "Accumulator:  ";
     op += buff;
     Serial.printf("\nX Register:    %04d",regs.xReg);
     sprintf(buff," %04d", regs.xReg);
     op += "\nX Register:   ";
     op += buff;
     Serial.printf("\nY Register:    %04d",regs.yReg);
     sprintf(buff," %04d", regs.yReg);
     op += "\nY Register:   ";
     op += buff;
     Serial.printf("\nProg Counter:  %04d",regs.progCounter);
     sprintf(buff," %04d", regs.progCounter);
     op += "\nProg Counter: ";
     op += buff;
     Serial.printf("\nZero Flag:     %i",regs.zeroFlag);
     sprintf(buff," %04d", regs.zeroFlag);
     op += "\nZero Flag:    ";
     op += buff;
     Serial.printf("\nNegative Flag: %i",regs.negFlag);
     sprintf(buff," %04d", regs.negFlag);
     op += "\nNegative Flag:";
     op += buff;
     Serial.printf("\nCarry Flag:    %i\n",regs.carryFlag);
     sprintf(buff," %04d", regs.carryFlag);
     op += "\nCarry Flag:   ";
     op += buff;
     Serial.println("Stack:");
     //Serial.println(displayMem(908,1007));
     return op;
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
    memory[START_V] = address;
    //output += "Setting start vector to " + String(memory[START_V]) + "\n";
    if(!simRunning)regs.progCounter = address;
    return success;
  }

  void setRunStatus(bool sRunning)
  {
    simRunning = sRunning;
    Serial.printf("simRunning now set to: %i", simRunning);
    return;
  }

  bool getRunStatus()
  {
    return simRunning;
  }

 /**
  * getStartVector
  * 
  * Sets the startVector to the given value
  * @param  int  address
  * @return bool success
  */
  int getStartVector(){
    return(memory[START_V]);
  }
  
  /**
   * beginRun()
   * 
   * Make sure everything is set up to start running the SIM code
   */
   bool beginRun(){
    regs.progCounter = memory[START_V];
    //output += "Setting progCounter to " + String(regs.progCounter)+"\n";
    //output += "Start vector is " + String(memory[START_V])+"\n";
    Serial.println("Setting progCounter to " + String(regs.progCounter));
    simRunning = true;
    return true;
  }

/**  
 *   videoOut()
 *   
 *   Deals with output to the video port
 */
void videoOut(String oput){
  output += oput;
  // Code to send to screen via I2C needs to go here:
  
  return;
}
 
 /**
  * doInstruction
  * 
  * Actions the next instruction
  */
  void doInstruction(){
    char chr;
    String tmp;

    //Serial.println("Doing next instruction...");
    int instruction = memory[regs.progCounter++];
    if(trace)Serial.printf("Next instruction is %i\n", instruction);
    switch(instruction){
      case  0: //stop
        simRunning=false;
        Serial.println("Program run concluded");
        videoOut("\n===\nProgram run concluded\n");
        // The next five! lines are trace, really
        if(trace){
          //displayRegs();
          Serial.println("Program memory: ");
          //output += "Program memory:\n";
          Serial.println(displayMem(0,23));
          //output += displayMem(0, 23) + "\n";
        }
        break;
      case  1: //load
        regs.acc = memory[memory[regs.progCounter++]];
        if(trace) Serial.printf("Setting acc to %i\n",regs.acc);
        break;
      case  2: //store
        memory[memory[regs.progCounter]] = regs.acc;
        if(trace) Serial.printf("Storing %i in %i\n",regs.acc,memory[regs.progCounter]);
        if(memory[regs.progCounter]==1015){
          chr = regs.acc;
          tmp = chr;
          videoOut(tmp); // 1015 is video out port
        }
        regs.progCounter++;
        break;
      case  3: //add
        regs.acc = regs.acc + memory[memory[regs.progCounter++]];
        if(regs.carryFlag)regs.acc++;
        if(regs.acc>1023){
          regs.acc = regs.acc%1024;
          regs.carryFlag = true;
        }
        if(regs.acc==0)regs.zeroFlag=true;
        if(trace){
          Serial.printf("acc is now %i\n",regs.acc);
          //output += "acc is now " + String(regs.acc) + "\n";
        }
        break;
      case  4: //sub
        regs.acc = regs.acc + (memory[memory[regs.progCounter++]] ^ 1023) + regs.carryFlag;
        if(regs.acc>1023){
          regs.acc = regs.acc%1024;
          regs.carryFlag = true;
          regs.negFlag = false;
        }
        else{
          regs.acc = (regs.acc ^ 1023) + 1;
          regs.carryFlag = false;
          regs.negFlag = true;
        }
        if(regs.acc==0)regs.zeroFlag = true;
        if(trace)Serial.printf("acc is now %i\n",regs.acc);
        break;
      case  5: //bitwise and (&)
        regs.acc = regs.acc & memory[memory[regs.progCounter]];
        if(trace)Serial.printf("A: %i, memory[PC]: %i, memory[memory[PC]]: %i\n", regs.acc,memory[regs.progCounter],memory[memory[regs.progCounter]]);
        regs.progCounter++;
        if(regs.acc==0)regs.zeroFlag=true;
        else regs.zeroFlag=false;
        break;
      case  6: //bitwise or (|)
        regs.acc = regs.acc | memory[memory[regs.progCounter++]];
        if(regs.acc==0)regs.zeroFlag=true;
        else regs.zeroFlag=false;
        break;
      case  7: //bitwise eor (^)
        regs.acc = regs.acc ^ memory[memory[regs.progCounter++]];
        if(regs.acc==0)regs.zeroFlag=true;
        else regs.zeroFlag=false;
        break;
      case  8: //jump
        regs.progCounter = memory[regs.progCounter];
        break;
      case  9: //comp
        value = regs.acc - memory[memory[regs.progCounter++]];
        if(value==0)regs.zeroFlag = true;
        else regs.zeroFlag = false;
        if(value<0)regs.negFlag = true;
        else regs.negFlag = false;
        break;
      case  10: //jineg
        if(regs.negFlag)regs.progCounter = memory[regs.progCounter];
        else regs.progCounter++;
        break;
      case  11: //jipos
        if(!regs.negFlag)regs.progCounter = memory[regs.progCounter];
        else regs.progCounter++;
        break;
      case  12: //jizero
        if(regs.zeroFlag)regs.progCounter = memory[regs.progCounter];
        else regs.progCounter++;
        break;
      case  13: //jmptosr
        // Get the jump address
        value = memory[regs.progCounter++];
        // Push the return address onto the stack
        stackPush(regs.progCounter);
        // point to the subroutine
        regs.progCounter = value;
        break;
      case  14: //jicarry
        if(regs.carryFlag)regs.progCounter = memory[regs.progCounter];
        else regs.progCounter++;
        break;
      case 15: //xload
        regs.xReg = memory[memory[regs.progCounter++]];
        if(trace)Serial.printf("Setting xReg to %i\n",regs.xReg);
        break;
      case 16: //xstore
        memory[memory[regs.progCounter]] = regs.xReg;
        if(trace){
          Serial.printf("Storing %i in %i\n",regs.xReg,memory[regs.progCounter]);
        }
        regs.progCounter++;
        break;
      case 17: //loadmx
        regs.acc = memory[memory[regs.progCounter++]+regs.xReg];
        if(trace)Serial.printf("Setting acc to %i\n",regs.acc);
        break;
      case 18: //xcomp
        value = regs.xReg - memory[memory[regs.progCounter++]];
        if(value==0)regs.zeroFlag = true;
        else regs.zeroFlag = false;
        if(value<0)regs.negFlag = true;
        else regs.negFlag = false;
        break;
      case 19: //yload
        regs.yReg = memory[memory[regs.progCounter++]];
        if(trace)Serial.printf("Setting yReg to %i\n",regs.yReg);
        break;
      case 20: //ystore
        memory[memory[regs.progCounter]] = regs.yReg;
        if(trace){
          Serial.printf("Storing %i in %i\n",regs.yReg,memory[regs.progCounter]);
        }
        regs.progCounter++;
        break;
      case 21: //pause
        value = memory[memory[regs.progCounter++]] * 100;
        delay(value);
        break;
      case 22: //printd
        value = memory[memory[regs.progCounter]] + (memory[memory[regs.progCounter++]+1]*1024);
        Serial.print(value);
        output += value;
        break;
      case 23: //return
        // get the return address
        regs.progCounter = stackPull();
        break;
      case 24: //push
        stackPush(regs.acc);
        break;
      case 25: //pull
        regs.acc = stackPull();
        break;
      case 26: //xpush
        stackPush(regs.xReg);
        break;
      case 27: //xpull
        regs.xReg = stackPull();
        break;
      case 28: //xinc
        regs.xReg++;
        if(regs.xReg==0)regs.zeroFlag = true;
        else regs.zeroFlag = false;
        if(regs.xReg>1023){
          regs.carryFlag = true;
          regs.xReg = regs.xReg%1024;
        }
        else regs.carryFlag = false;
        break;
      case 29: //xdec
        regs.xReg--;
        if(regs.xReg==0)regs.zeroFlag = true;
        else regs.zeroFlag = false;
        if(regs.xReg<0){
          regs.negFlag = true;
          regs.xReg = regs.xReg + 1024;
        }
        else regs.negFlag = false;
        break;
      case 30: //lshift
        regs.acc = (regs.acc * 2) + regs.carryFlag;
        if(regs.acc>1023){
          regs.acc = regs.acc%1024;
          regs.carryFlag = true;
        }
        else regs.carryFlag = false;
        break;
      case 31: //rshift
        if(regs.acc & 1){
          value = 1;
          regs.acc--;
        }
        else value = 0;     // value holds the LSB which will become the carry flag
        regs.acc = regs.acc/2;
        if(regs.carryFlag) regs.acc = regs.acc + 512; // Set the MSB
        if(value==1) regs.carryFlag = true;
        else regs.carryFlag = false;
        break;
      case 32: //cset
        regs.carryFlag=true;
        break;
      case 33: //cclear
        regs.carryFlag=false;
        if(trace) //output += "Setting carry flag to " + String(regs.carryFlag) + "\n";
        break;
      case 37: //printb
        Serial.print(regs.acc, BIN);
        for(int i=9;i>=0;i--)if(bitRead(regs.acc,i))videoOut("1");else videoOut("0");
        break;
      case 38: //print
        Serial.print(regs.acc);
        videoOut(String(regs.acc));
        break;
      case 39: //printch
        chr = regs.acc;
        if(trace)Serial.print(chr);
        videoOut(String(chr));
        break;
      case 50: //NOP - but uncomment for stop instead
        //running = false;
        break;
      default:
        tmp = "!!RUN ERROR: unknown program instruction: "+String(instruction);
        videoOut(tmp);
        simRunning=false;
        break;
    }
    
    if(regs.progCounter>1023){
      Serial.println("!!Error: program counter overflow");
      videoOut("!!RUN ERROR: program counter overflow");
      simRunning=false;
    }
    Serial.println("Instruction completed");
    return;
  }
};
