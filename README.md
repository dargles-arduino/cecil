# cecil
A simulation of an early teaching microcontroller. It has 1K of 10 bit wide memory and runs machine code.
# EH???
So, basically, CECIL ("Computer Education Centre Instruction Language" - a tribute to the 1960s ICL CESIL language) is a low-level language I developed in the 1990s as a vehicle for helping my Computing students to understand computer architecture, and how computer languages work at machine level. The language runs on a theoretical computer/microcontroller that has existed in various forms, from the original KIM10 through to the current SIM40. The hardware is a theoretical design, and yes, it has 1K of memory which is 10 bits wide. However, the design is detailed and sufficiently rigorous that it could in theory be manufactured as a real machine. It has been variously simulated in software on the Acorn Archimedes, on the IBM PC (a current version exists), and in a web browser using Java.
But it's always been my dream to produce an actual hardware version of the SIM40 running the machine code that CECIL produces. THe ESP32 is the ideal chip for doing this.
# Why?
I mean, what's the point of doing this now? Even though we're in the days of managing complexity at a high level, I believe it does no harm to remind ourselves of what has to go on at low level, even today. But also, as an exercise, this is a most informative project, involving the development of a compiler and dealing with matters such as parsing and actioning the resultant stored program codes. Once the structure is in place, there are significant other possibilities.
# How?
There are four main components of the CECIL sketch:
1) compiler.h
This provides a compiler class whose job is to take a text program and to convert it into a sequence of corresponding machine codes. These are then transfered to the:
2) sim40.h
This provides a sim40 class. This class provides methods to action the codes in the sim40 memory. The architecture of the SIM40 memory is defined within this class.
3) webServer.h
This should really be a webServer class, but isn't for pragmatic reasons (like, I'm not clever enough to make it work properly as a class, for some reason). Basically, the routines in this file handle the webpage definitions and interactions to enable the "SIM40" to be programmed and controlled from a mobile phone.
4) cecil.ino
This is the main sketch that ties everything together - or the juggler that keeps all the balls in the air!
# What...
... do I do to get it all up and running? Download the sketch, plug in an ESP32 (I'm using a Node32S), and compile ("verify") and upload the sketch to the ESP. It's using WiFiManager, so at the moment, it will look for a router to connect to. Until it's got the relevant SSID and password, it will present to your phone as "Cecil", asking for a suitable SSID and password. You'll then need to connect to it via whatever IP your router gives it, in my case 192.168.0.43. After that, play and enjoy!
