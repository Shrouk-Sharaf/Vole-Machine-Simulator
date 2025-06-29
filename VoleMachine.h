#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <bitset>
#include <regex>

using namespace std;

class Memory {
private:
    vector<string> memory;
    string screen;
public:
    Memory(size_t size = 256);
    string load(int address);
    void store(int address, const string &hexValue);
    void printMemory();
    const string& getScreen();
    void setScreen(char c);
};

class Registers {
private:
    vector<string> registers;
public:
    Registers(size_t count = 16);
    string load(int reg);
    void store(int reg, const string &hexValue);
    void printRegisters();
};

class CPU {
private:
    Memory &memory;
    Registers &registers;
    int &pc;
    bool &halted;
    string instReg;
    string hexString(int value) const;
    int hexToInt(const string &hexValue) const;
    double hexToFloatValue(const string &hexInput);
    string FloatToHexadecimal(double number);
public:
    CPU(Memory &mem, Registers &regs, int &pc, bool &halted);
    void fetchInstruction();
    void executeInstruction();
    string getInstructionRegister();
    void setInstructionRegister();
};

class Simulator {
private:
    Memory memory;
    Registers registers;
    int pc;
    bool halted;
    CPU cpu;
public:
    Simulator();
    bool isValidInstruction(const string &instruction);
    void loadProgram(const string &filename);
    void run_entire_program();
    void runStepByStep();
    void displayState();
    void reset();
};

#endif