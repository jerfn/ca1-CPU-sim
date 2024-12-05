#include "CPU.h"

#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

int main(int argc, char* argv[])
{
	/* This is the front end of your project.
	You need to first read the instructions that are stored in a file and load them into an instruction memory.
	*/

	/* Each cell should store 1 byte. You can define the memory either dynamically, or define it as a fixed size with size 4KB (i.e., 4096 lines). 
	Each instruction is 32 bits (i.e., 4 lines, saved in little-endian mode).
	Each line in the input file is stored as an hex and is 1 byte (each four lines are one instruction). 
	You need to read the file line by line and store it into the memory. 
	You may need a mechanism to convert these values to bits so that you can read opcodes, operands, etc.
	*/

	unsigned char instMem[4096];

	if (argc < 2) {
		//cout << "No file name entered. Exiting...";
		return -1;
	}

	ifstream infile(argv[1]); //open the file
	if (!(infile.is_open() && infile.good())) {
		cout<<"error opening file\n";
		return 0; 
	}
	string line; 
	int i = 0;
	while (infile) {
		infile>>line;
		stringstream line2(line);
		unsigned char x;

		line2>>x;
		if (x >= '0' && x <= '9') {
			x = x - '0';
		} else if (x >= 'a' && x <= 'f') {
			x = x - 'a' + 10;
		} else {
			cout << "error interpreting instruction as hex" << endl;
			return 0;
		}
		instMem[i] = x << 4;

		line2>>x;
		if (x >= '0' && x <= '9') {
			x = x - '0';
		} else if (x >= 'a' && x <= 'f') {
			x = x - 'a' + 10;
		} else {
			cout << "error interpreting instruction as hex" << endl;
			return 0;
		}
		instMem[i] += x;
		
        /*
        // Print current memory byte as binary
        cout << "Binary representation: " << bitset<8>(instMem[i]) << endl;
        cout << "Hexadecimal representation: 0x" << hex << setw(2) << setfill('0')
                  << static_cast<int>(static_cast<unsigned char>(instMem[i])) << endl;
         */
        
		i++;
	}
    int maxPC= i/4;

	/* Instantiate your CPU object here.  CPU class is the main class in this project that defines different components of the processor.
	CPU class also has different functions for each stage (e.g., fetching an instruction, decoding, etc.).
	*/

	CPU myCPU;  // call the approriate constructor here to initialize the processor...  
	// make sure to create a variable for PC and resets it to zero (e.g., unsigned int PC = 0); 
	
	bool done = true;
	while (done == true) // processor's main loop. Each iteration is equal to one clock cycle.  
	{
		// FETCH, converting little endian format into full 4 byte instruction, and storing fetched instruction into CPU
        unsigned int currInstr;
        currInstr =  (static_cast<unsigned int>(instMem[4 * myCPU.readPC() + 3]) << 24);
        currInstr += (static_cast<unsigned int>(instMem[4 * myCPU.readPC() + 2]) << 16) & static_cast<unsigned int>(0x00ff0000);
        currInstr += (static_cast<unsigned int>(instMem[4 * myCPU.readPC() + 1]) << 8)  & static_cast<unsigned int>(0x0000ff00);
        currInstr += (static_cast<unsigned int>(instMem[4 * myCPU.readPC()]))           & static_cast<unsigned int>(0x000000ff);
        myCPU.fetch(currInstr);
        
        /* Debugging print statements
         * cout << "0x" << hex << setw(8) << setfill('0') << currInstr << "\t";
         * cout << "opcode: 0x" << hex << setw(2) << setfill('0') << myCPU.opcode() << "\t";
         * cout << "generated imm: 0x" << hex << (int) myCPU.immGen() << endl;
         */

        // DECODE
        myCPU.controller();
        myCPU.ALUcontroller();
        
        // EXECUTE
        myCPU.ALU();
        
        // MEMORY
        myCPU.readMem();
        myCPU.writeMem();
        
        // WRITEBACK
        myCPU.WB();
        
        // Update PC
        myCPU.updatePC();
        
        // Check for end of instructions
		if (myCPU.readPC() + 1 > maxPC)
			break;
	}
    int a0 = myCPU.getReg(10);
    int a1 = myCPU.getReg(11);
    
    cout << "(" << a0 << "," << a1 << ")" << endl;
    
	return 0;
}
