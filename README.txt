Name: Nicholas Romanchuk   
Student ID: 101214965

Program's Purpose: To create a program in C, that simulates a ghost hunt in a building. Program will initialize a map of connected rooms,
                  four hunters (each with a single device they can use to collect evidence), a ghost (wandering around and leaving evidence 
                  for hunters). Hunters can move rooms, collect evidence, or review evidence. Ghosts can move rooms or leave evidence. When 
                  a hunter is in a room with a ghost, their fear increases, if they are not with a ghost their boredom increases. If a ghost 
                  is not in a room with the hunter the ghosts boredom increases. Hunters can review their collected evidence, and if they find 
                  sufficient evidence can leave. If a hunters boredom or fear crosses the defined threshold they leave the simulation, if a ghosts
                  bored crosses the defined threshold they leave the simulation.  Simulation ends when the ghost and all hunters have left the 
                  house. If the hunters collected enough evidence to guess the correct ghost type they win the simulation!

Files Included:
      i) README.txt - authorship details and instructions for compiling and running program
     ii) defs.h - function signatures, constant definitions, structure definitions
    iii) main.c - main control flow of C program
     iv) ghosts.c - C functions to manage the functionality of the ghost data (initalizing, adding, cleaning memory etc.)
     vi) house.c - C functions to manage the house data (initalizing, adding, cleaning memory etc.)
    vii) evidence.c - C functions to manage the evidence data (initalizing, adding, cleaning memory etc.)
   viii) rooms.c - C functions to manage the room data (initalizing, adding, cleaning memory etc.)
     ix) utils.c - helper functions to assist with simulation
      x) hunters.c - C functions to manage the hunters data (initalizing, adding, cleaning memory etc.)
     xi) logger.c - C functions to log all activities performed by hunters and ghosts in the simulation
    xii) data.txt - data to initialize hunters that can be piped into executable
   xiii) makefile - make file that can be used to compile and link program into a 'fp' executable
    
Compiling Program:   
      i) Extract contents of COMP2401_ROMANCHUK_NICHOLAS_101214965_FP.tar
     ii) Open terminal
     ii) In the terminal navigate to folder where files were unzipped
    iii) In the terminal run the command "make"

Running Program:
      i) open terminal
     ii) navigate to folder containing the "fp" program
    iii) run "./fp" in the terminal
     iv) To pipe in data to intialize hunters run "./fp < data.txt" in terminal

How to Use the Program:
      i) Run the program (see above)
     ii) If data.txt was piped in to executable ignore steps iii) to x)
    iii) Enter the name of hunter #1
     iv) Enter the what type of data hunter #1 should collect
      v) Enter the name of hunter #2
     vi) Enter the what type of data hunter #2 should collect
    vii) Enter the name of hunter #3
   viii) Enter the what type of data hunter #3 should collect
     ix) Enter the name of hunter #4
      x) Enter the what type of data hunter #4 should collect
     xi) Observe results at the end of simulation
