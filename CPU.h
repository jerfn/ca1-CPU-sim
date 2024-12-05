#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

class CPU {
private:
    unsigned char dmemory[4096]; // data memory byte addressable in little endian fashion;
	unsigned long PC; // pc
    int registers[32]; // unsigned int is 32-bits
    bitset<32> currInstr;
    
    // Control signals
    bool Branch;                        // 1 means branch/jump instruction
    bool MemRead;                       // 1 means read from mem
    bool MemtoReg;                      // 0 means use ALU value to write to rd, 1 means use mem value to write to rd
    unsigned char ALUOp, ALUControl;    // 2-bit ALUOp and 4-bit ALUControl signals
    bool MemWrite;                      // 1 means write to mem
    bool ALUSrc;                        // 0 means use rs2, 1 means use imm
    bool RegWrite;                      // 1 means write to reg (rd)
    
    // Intermediate values
    int ALUResult;
    int memReadResult;

public:
	CPU();
	unsigned long readPC();
    
    // Datapath and controller functions
    void fetch(unsigned int instr); // Updates currInstr with newly fetched instruction
    void controller(); // Generates control signals for current instruction
    void ALUcontroller(); // Generates ALUControl for current instruction
    void ALU(); // Executes ALU operation for current ALUControl signal
    void readMem(); // Read memory if required
    void writeMem(); // Write to memory if required
    void WB(); // Handles write back operations
    void updatePC(); // Updates PC, considering branch and jump instructions
    
    // Helper functions
    int opcode(); // Returns opcode of current instruction
    int funct3(); // Returns funct3 of current instruction
    int funct7(); // Returns funct7 of current instruction
    int immGen(); // Generates immediate for current instruction
    int readReg1(); // Read data from register 1 (rs1) for current instruction
    int readReg2(); // Read data from register 2 (rs2) for current instruction
    void writeReg(int data); // Write data to register (rd) for current instruction
    
    int getReg(int regNum); // Read data in register regNum
};
