# project-1

## Project Overview
This project consisted of two components:

* Multi-Threading implementation with four phases that simulate a banking system to concurrently perform deposits, withdrawals, and fund transfers on bank accounts.
  * Phase 1: Basic Thread Operations
  * Phase 2: Resource Protection with mutex locks
  * Phase 3: Deadlock Creation
  * Phase 4: Deadlock Resoution

* Inter-Process Communication with pipes (|). One process being the producer that sends data through a pipe, while the other process, the consumer, reading it.

## Build Instructions

1. Environment Setup
 - A Linux based system (Ubuntu)
 - A C++ compiler (g++)
 - A code editor or IDE of your choice
 - Open terminal, run commands:
   - sudo apt update
   - sudo apt install build-essential

2. Download Project Source Code
 - "Bank Simulation.cpp", producer.cpp, consumer.cpp
 - Place all .cpp files into an easily accessible directory

3. Building the Multi-Threaded Bank Simulation
 - In "Bank Simulation.cpp" there are various phases being called in the main function
 - Edit "Bank Simulation.cpp" with a code editor or IDE to uncomment a phase to test it
 - Compile the program with terminal commands:
   - g++ -std=c++20 -pthread -o "Bank Simulation" "Bank Simulation.cpp"
 - Run the program with terminal commands:
   - ./"Bank Simulation"
 - Comment or delete the tested phase, then repeat the steps above for the other phases

4. Building the IPC program
 - Compile both producer and consumer program with terminal commands:
   - g++ -std=c++20 -o producer producer.cpp
   - g++ -std=c++20 -o consumer consumer.cpp
 - Run both programs through a pipe (|) to one another with terminal commands:
   - ./producer | ./consumer
