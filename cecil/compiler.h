/**
 * Class definition for the SIM40 compiler
 * 
 * The SIM40 compiler holds a current CECIL program which may or may not
 * be compiled. If it has been compiled, it will also hold the resulting 
 * machine code  together with the starting address at which it should 
 * reside.
 * 
 * @author  David Argles, d.argles@gmx.com
 * @version 23Aug2021 15:02h
 */
 
class compiler
{
  private:
  
  public:

  String  program;
  int     code[903];  // Analogue port is at 904, so can't be bigger
  int     pointer;    // Points to next free location in the code
  bool    compiled = false;
  int     errors;
  String  input;
  String  output = "";
  String  instructions[51]={"stop","load","store","add","sub","and","or","eor","jump","comp",
    "jineg","jipos","jizero","jmptosr","jicarry","xload","xstore","loadmx","xcomp","yload",
    "ystore","pause","printd","return","push","pull","xpush","xpull","xinc","xdec",
    "lshift","rshift","cset","cclear","getkey","wait","retfint","printb","print","printch",
    "ypush","ypull","yinc","ydec","swapax","swapay","swapxy","swapas","intenable","intdisable",
    "nop"};
  int     takesData[51] = {0,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,
    1,1,1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0};
  String  labelNames[];
  int     labelLocs[];
  int     labelPtr;

  // The constructor
  compiler(){
    program =  "program example\n";
    program += "author  David Argles\n";
    program += "date    20.08.01\n";
    program += ";---program starts here---\n";
    program += ".start  load    data1\n";
    program += "        cclear\n";
    program += "        add     data2\n";
    program += "        print\n";
    program += "        stop\n";
    program += ";---data starts here---\n";
    program += ".data1  insert  12\n";
    program += ".data2  insert  11\n";
    program += ";---end of code---";
    return;
  }

  void compileError(String message){
    output += message;
    errors++;
    return;
  }

  String getWord(){
    input.trim();
    String  nextWord;
    String  terminator;
    String  ignore;
    while(input.startsWith(";")){
      ignore = getRestOfLine();
      input.trim();
    }
    if(input.indexOf(" ")<input.indexOf("\n"))terminator = " ";
    else terminator = "\n";  
    nextWord = input.substring(0,input.indexOf(terminator));
    input = input.substring(input.indexOf(terminator),input.length());      
    return nextWord;
  }

  String getRestOfLine(){
    input.trim();
    String line = input.substring(0,input.indexOf("\n"));
    input = input.substring(input.indexOf("\n"),input.length());
    return line;
  }

  String parse(){
    input = program;
    String stuff = "Starting parser:\n";
    while(input.length()>0){
      stuff += getWord();
      stuff += "+";
    }
    stuff += "Parser finished\n";
    return stuff;
  }

  int decypher(String keyword){
    int value = -1;
    int i = 0;
    while(value==-1 && i<40){
      if(keyword==instructions[i])value = i;
      i++;
    }
    return value;
  }

  int lookupLabel(String label){
    int value = -1;
    int i = 0;
    while(value==-1 && i<labelPtr){
      if(labelNames[i]==label)value=labelLocs[i];
    }
    return value;
  }

  int getData(){
    int location = -1;
    String nextWord;
    nextWord = getWord();
    output += "Data found: "+nextWord+"\n";
    location = lookupLabel(nextWord);
    if(location==-1)compileError("Label not found");
    return location;
  }

  bool compile(){
    input = program;
    labelPtr = 0;   // i.e. reset the label table
    output = "Starting compiler...\n";
    String next;
    bool success = true;
    compiled = false;
    errors = 0;
    pointer = 0;
    int   instruction;

    // First, replace all (other) whitespace chars with spaces
    input.replace("\t"," "); // tabs
    
    // ---Check the headers---
    String keywords[] = {"program","author","date"};
    for(int ptr=0;ptr<3;ptr++){
      next = getWord();
      if(next!=keywords[ptr])compileError("Missing"+keywords[ptr]+"declaration\n");
      else{
        next = getRestOfLine();
        output += "Found "+keywords[ptr]+": " + next + "\n";
      }
    }

    // ---Now compile the program---
    while(input.length()>0){
      // While there's text left, compile next command
      next = getWord();
      // Check for a label
      if(next.startsWith(".")){
        output+="Label found: "+next+"\n";
        // Enter the label into the table
        labelNames[labelPtr] = next;
        labelLocs[labelPtr++] = pointer;
        next = getWord();
      }
      // We should now have a command
      instruction = decypher(next);
      if(instruction==-1){
        compileError("Unknown instruction: "+next+"\n");
      }
      else{
        output+="Instruction found: "+next+"\n";
        code[pointer++]=instruction;
      }
      // Now check for a data field
      if(instruction!=-1 && takesData[instruction]){
        // Current command takes a data field, deal with it
        code[pointer++]=getData();
      }
    }
    // ---Finish off---
    if(errors>0) success = false;
    if(success)compiled = true;
    //int val = pointer-1;
    output += "There are "+String(pointer)+" memory locations of code\n";
    output += "Code block is:\n";
    for(int i=0;i<pointer;i++)output += String(code[i])+" ";
    return(success);
  }
};
