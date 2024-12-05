#include "CPU.h"

// Default constructor
CPU::CPU() {
	PC = 0; //set PC to 0
    
    // initilizes all data memory to 0
    for (int i = 0; i < 4096; i++) {
		dmemory[i] = (0);
	}
    // initializes all registers to 0
    for (int i = 0; i < 32; i++) {
        registers[i] = 0;
    }
    
    // initializes all control signals to 0
    RegWrite = 0;
    ALUSrc = 0;
    Branch = 0;
    MemRead = 0;
    MemWrite = 0;
    MemtoReg = 0;
    ALUOp = 0;
    ALUControl = 0;
}

unsigned long CPU::readPC() {
	return PC;
}

// Updates currInstr with newly fetched instruction
void CPU::fetch(unsigned int instr) {
    currInstr = instr;
}

// Read data from register 1 (rs1) given current instruction
int CPU::readReg1() {
    int regNum = (currInstr.to_ulong() & 0x000f8000) >> 15;
    if (regNum < 0 || regNum > 31) {
        cout << "error, register number out of limits: 0x" << hex << regNum << endl;
        throw runtime_error("register number out of limits");
    }
    return registers[regNum];
}

// Read data from register 2 (rs2) given current instruction
int CPU::readReg2() {
    int regNum = (currInstr.to_ulong() & 0x01f00000) >> 20;
    if (regNum < 0 || regNum > 31) {
        cout << "error, register number out of limits: 0x" << hex << regNum << endl;
        throw runtime_error("register number out of limits");
    }
    return registers[regNum];
}

// Write data to register (rd) given current instruction
void CPU::writeReg(int data) {
    int regNum = (currInstr.to_ulong() & 0x00000f80) >> 7;
    if (regNum < 0 || regNum > 31) {
        cout << "error, register number out of limits: 0x" << hex << regNum << endl;
        throw runtime_error("register number out of limits");
    }
    registers[regNum] = data;
//    cout << "writing 0x" << hex << data << " to reg x" << dec << regNum << endl;
}

// Read data in register regNum
int CPU::getReg(int regNum) {
    if (regNum < 0 || regNum > 31) {
        cout << "error, register number out of limits: 0x" << hex << regNum << endl;
        throw runtime_error("register number out of limits");
    }
    return registers[regNum];
}

// Generates immediate given current instruction
int CPU::immGen() {
    switch (opcode()) {
        case 0b0110011: // R-type
            return -1;
            break;
        case 0b0010011: // I-type arithmetic
        case 0b0000011: // I-type load
            if (this->funct3() == 0x5)
                return (currInstr.to_ulong() & 0x01f00000) >> 20;
            return (int) (currInstr.to_ulong() & 0xfff00000) >> 20;
            break;
        case 0b0100011: // S-type store
            return ((int) (currInstr.to_ulong() & 0xfe000000) >> 20) + ((currInstr.to_ulong() & 0x00000f80) >> 7);
            break;
        case 0b1100011: // B-type branch
            return (int) (((currInstr.to_ulong() & 0x7e000000) >> 20) + ((currInstr.to_ulong() & 0x00000f00) >> 7) + ((int) (currInstr.to_ulong() & 0x80000000) >> 19) + ((currInstr.to_ulong() & 0x00000080) << 4));
            break;
        case 0b1101111: // J-type jump
            return ((currInstr.to_ulong() & 0x7fe00000) >> 20) + (currInstr.to_ulong() & 0x000ff000) + ((int) (currInstr.to_ulong() & 0x80000000) >> 11) + ((currInstr.to_ulong() & 0x00100000) >> 9);
            break;
        case 0b0110111: // U-type upper imm
            return (int) (currInstr.to_ulong() & 0xfffff000) >> 12;
            break;
        default:
            return -1;
    }
}

// Returns opcode of current instruction
int CPU::opcode() {
    return currInstr.to_ulong() & 0x7f;
}

// Returns funct3 of current instruction
int CPU::funct3() {
    return (currInstr.to_ulong() & 0x7000) >> 12;
}

// Returns funct7 of current instruction
int CPU::funct7() {
    return (currInstr.to_ulong() & 0xfe000000) >> 25;
}

// Generates control signals for current instruction
void CPU::controller() {
    switch (opcode()) {
        case 0b0110011: // R-type
            RegWrite = 1;
            ALUSrc = 0; // 0 means use rs2
            Branch = 0;
            MemRead = 0;
            MemWrite = 0;
            MemtoReg = 0; // 0 means use ALU value to write to rd
            ALUOp = 0b10;
            break;
        case 0b0010011: // I-type arithmetic
            RegWrite = 1;
            ALUSrc = 1; // 1 means use imm
            Branch = 0;
            MemRead = 0;
            MemWrite = 0;
            MemtoReg = 0; // 0 means use ALU value to write to rd
            ALUOp = 0b10;
            break;
        case 0b0000011: // I-type load
            RegWrite = 1;
            ALUSrc = 1; // 1 means use imm
            Branch = 0;
            MemRead = 1;
            MemWrite = 0;
            MemtoReg = 1; // 1 means use mem value to write to rd
            ALUOp = 0b00;
            break;
        case 0b0100011: // S-type store
            RegWrite = 0;
            ALUSrc = 1; // 1 means use imm
            Branch = 0;
            MemRead = 0;
            MemWrite = 1;
            MemtoReg = 0;
            ALUOp = 0b00;
            break;
        case 0b1100011: // B-type branch
            RegWrite = 0;
            ALUSrc = 0;
            Branch = 1;
            MemRead = 0;
            MemWrite = 0;
            MemtoReg = 0;
            ALUOp = 0b01;
            break;
        case 0b1101111: // J-type jump
            RegWrite = 1;
            ALUSrc = 0;
            Branch = 1;
            MemRead = 0;
            MemWrite = 0;
            MemtoReg = 0;
            ALUOp = 0b11; // 0b11 to signal don't use ALU/special case
            break;
        case 0b0110111: // U-type upper imm
            RegWrite = 1;
            ALUSrc = 1; // 1 means use imm
            Branch = 0;
            MemRead = 0;
            MemWrite = 0;
            MemtoReg = 0;
            ALUOp = 0b11; // 0b11 to signal don't use ALU/special case
            break;
    }
}

// Generates ALUControl for current instruction
void CPU::ALUcontroller() {
    switch (ALUOp) {
        case 0b00: // add, used by load and store
            ALUControl = 0b0010;
            break;
        case 0b01: // subtract, used by beq
            ALUControl = 0b0110;
            break;
        case 0b10: // R or I type arithmetic
            if (funct3() == 0) // add
                ALUControl = 0b0010;
            else if (funct3() == 4) // xor
                ALUControl = 0b0101; // using 0101 for xor
            else if (funct3() == 5) // srai
                ALUControl = 0b0111; // using 0111 for srai
            else if (funct3() == 6) // or
                ALUControl = 0b0001;
            else
                cout << "error, invalid funct3" << endl;
            break;
        case 0b11:
            ALUControl = 0b1111;
            break;
        default:
            cout << "error, invalid ALUOp" << endl;
    }
}

// Executes ALU operation for current ALUControl signal
void CPU::ALU() {
    int op1 = readReg1();
    int op2 = (ALUSrc) ? immGen() : readReg2();
    switch (ALUControl) {
        case 0b0010: // add
            ALUResult = op1 + op2;
            break;
        case 0b0110: // sub
            ALUResult = op1 - op2;
            break;
        case 0b0101: // xor
            ALUResult = op1 ^ op2;
            break;
        case 0b0001: // or
            ALUResult = op1 | op2;
            break;
        case 0b0111: // srai
            ALUResult = op1 >> op2;
            break;
        case 0b1111: // special case
            if (opcode() == 0b0110111) // lui
                ALUResult = op2 << 12;
            else if (opcode() == 0b1101111) // jal
                ALUResult = 0;
            break;
        default:
            ALUResult = 0x7fff;
            break;
    }
}

// Read memory if required
void CPU::readMem() {
    if (MemRead) { // for load word and load byte
        if (funct3() == 0) // lb
            // read from byte at rs1 + imm
            memReadResult = dmemory[ALUResult];
        else if (funct3() == 2) { // lw
            // read from 4 bytes starting at rs1 + imm, little endian
            memReadResult =  (static_cast<int>(dmemory[ALUResult + 3]) << 24);
            memReadResult += (static_cast<int>(dmemory[ALUResult + 2]) << 16) & static_cast<int>(0x00ff0000);
            memReadResult += (static_cast<int>(dmemory[ALUResult + 1]) << 8)  & static_cast<int>(0x0000ff00);
            memReadResult += (static_cast<int>(dmemory[ALUResult]))           & static_cast<int>(0x000000ff);
        }
        else
            cout << "error, invalid funct3" << endl;
    }
}

// Write to memory if required
void CPU::writeMem() {
    if (MemWrite) { // for store word and store byte
        if (funct3() == 0) { // sb
            // write to byte at rs1 + imm
            dmemory[ALUResult] = readReg2() & 0x00000000ff;
        }
        else if (funct3() == 2) { // sw
            // write to 4 bytes starting at rs1 + imm, little endian
            dmemory[ALUResult]     = (readReg2() & 0x000000ff);
            dmemory[ALUResult + 1] = (readReg2() & 0x0000ff00) >> 8;
            dmemory[ALUResult + 2] = (readReg2() & 0x00ff0000) >> 16;
            dmemory[ALUResult + 3] = (readReg2() & 0xff000000) >> 24;
        }
        else
            cout << "error, invalid funct3" << endl;
    }
}

// Handles write back operations
void CPU::WB() {
    if (RegWrite) {
        if (opcode() == 0b1101111) // jal special case
            writeReg(static_cast<int>(4 * PC + 4));
        else if (!MemtoReg) // write from ALU result to rd
            writeReg(ALUResult);
        else // write from memory to rd
            writeReg(memReadResult);
    }
    registers[0] = 0; // resets x0 if it has been incorrectly overidden
}

// Updates PC, considering branch and jump instructions
void CPU::updatePC() {
    if (Branch && ALUResult == 0) // Branch taken/jump
        PC += immGen() / 4;
    else
        PC++;
}
