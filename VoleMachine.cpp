#include "VoleMachine.h"

// Memory class implementation
Memory::Memory(size_t size) {
    memory.resize(size, "00");
}

string Memory::load(int address) {
    if (address >= 0 && address < memory.size()) {
        return memory[address];
    }
    return "00";
}

void Memory::store(int address, const string &hexValue) {
    if (address >= 0 && address < memory.size()) {
        memory[address] = hexValue;
    }
}

void Memory::printMemory() {
    cout << "Memory :" << endl;
    cout << "     ";
    for (int col = 0; col < 16; ++col) {
        if (col < 10) cout << col << "  ";
        else cout << static_cast<char>('A' + col - 10) << "  ";
    }
    cout << endl;

    for (int row = 0; row < 16; ++row) {
        if (row < 10) cout << " " << row << " | ";
        else cout << " " << static_cast<char>('A' + row - 10) << " | ";

        for (int col = 0; col < 16; ++col) {
            int address = row * 16 + col;
            cout << memory[address] << " ";
        }
        cout << endl;
    }
}
const string& Memory::getScreen() { return screen; }

void Memory:: setScreen(char c){
    screen += c ;
}

// Registers class implementation
Registers::Registers(size_t count) {
    registers.resize(count, "00");
}

string Registers::load(int reg) {
    if (reg >= 0 && reg < registers.size()) {
        return registers[reg];
    }
    return "00";
}

void Registers::store(int reg, const string &hexValue) {
    if (reg >= 0 && reg < registers.size()) {
        registers[reg] = hexValue;
    }
}

void Registers::printRegisters() {
    cout << "Registers : ";
    for (int i = 0; i < registers.size(); ++i) {
        cout << "R" << i << ": " << registers[i] << " ";
    }
    cout << endl;
}
// CPU class implementation
string CPU:: hexString(int value) const {
    if(value < 0){
        value += 256 ;
    }
    value %= 256;  // Ensure within one byte (0-255)
    int highDigit = value / 16;
    int lowDigit = value % 16;

    // Convert each nibble to its hex character using static_cast
    char highChar = highDigit < 10 ? static_cast<char>('0' + highDigit) : static_cast<char>('A' + highDigit - 10);
    char lowChar = lowDigit < 10 ? static_cast<char>('0' + lowDigit) : static_cast<char>('A' + lowDigit - 10);

    return string{highChar, lowChar};
}

// Helper to convert hex string to int
int CPU :: hexToInt(const string& hexValue) const {
    int temp = stoi(hexValue, nullptr, 16);
    if (temp >= 128){
        temp -= 256 ;
    }
    return  temp ;
}
double CPU :: hexToFloatValue(const string& hexInput) {
    // Convert each hex character to binary
    string binary;
    for (char hexChar : hexInput) {
        int decimal = isdigit(hexChar) ? hexChar - '0' : toupper(hexChar) - 'A' + 10;
        binary += bitset<4>(decimal).to_string();
    }

    // Determine sign, exponent, and mantissa
    int sign = binary[0] - '0';
    string exponent = binary.substr(1, 3);
    string mantissa = binary.substr(4, 4);

    // Normalize mantissa by removing trailing zeros and adding "0."
    while (mantissa.back() == '0') mantissa.pop_back();
    mantissa = "0." + mantissa;

    // Convert normalized binary mantissa to decimal
    double decimalMantissa = 0;
    double fracWeight = 0.5;
    for (size_t i = 2; i < mantissa.size(); ++i) {
        if (mantissa[i] == '1') decimalMantissa += fracWeight;
        fracWeight /= 2;
    }

    int exponentDecimal = stoi(exponent, nullptr, 2) - 4;
    double finalDecimalValue = decimalMantissa * pow(2, exponentDecimal);

    return (sign ? -finalDecimalValue : finalDecimalValue);
}
string CPU :: FloatToHexadecimal(double number) {

    string signBit = (number < 0) ? "1" : "0";
    number = fabs(number);

    int exponent = 0;
    while (number >= 1) {
        number /= 2;
        exponent++;
    }

    while (number < 0.5 && number != 0) {
        number *= 2;
        exponent--;
    }

    int biasedExponent = exponent + 4;

    string exponentBinary = bitset<3>(biasedExponent).to_string();

    string mantissaBinary;
    for (int i = 0; i < 4; ++i) {
        number *= 2;
        if (number >= 1) {
            mantissaBinary += "1";
            number -= 1;
        } else {
            mantissaBinary += "0";
        }
    }

    string finalBinary = signBit + exponentBinary + mantissaBinary;
    stringstream hexStream;
    hexStream << uppercase << hex << stoi(finalBinary, nullptr, 2);
    return hexStream.str();
}
CPU::CPU(Memory &mem, Registers &regs, int &pc, bool &halted)
        : memory(mem), registers(regs), pc(pc), halted(halted), instReg("0000") {}

void CPU::fetchInstruction() {
    instReg = memory.load(pc) + memory.load(pc + 1);
}

void CPU::executeInstruction() {
    char op_code = instReg[0];
    int R = stoi(instReg.substr(1, 1), nullptr, 16);
    int XY = stoi(instReg.substr(2, 2), nullptr, 16);

    switch (op_code) {
        case '1':
            registers.store(R, memory.load(XY));
            break;
        case '2':
            registers.store(R, hexString(XY));
            break;
        case '3':
            memory.store(XY, registers.load(R));
            if (XY == 0) {
                int decimalValue = stoi(registers.load(R), nullptr, 16);
                char asciiChar = static_cast<char>(decimalValue);
                memory.setScreen(asciiChar) ;
            }
            break;
        case '4':
            registers.store(XY % 16, registers.load(R));
            break;
        case '5': {
            int sum = hexToInt(registers.load(XY / 16)) + hexToInt(registers.load(XY % 16));
            registers.store(R, hexString(sum % 256));
            break;
        }
        case '6': {
            double sum = hexToFloatValue(registers.load(XY / 16)) + hexToFloatValue(registers.load(XY % 16));
            registers.store(R, FloatToHexadecimal(sum));
            break;
        }
        case 'B':
            if (registers.load(R) == registers.load(0)) {
                pc = XY - 2;
            }
            break;
        case 'C':
            halted = true;
            break;
        default:
            break;
    }
}

string CPU::getInstructionRegister() {
    return instReg;
}

void CPU::setInstructionRegister() {
    instReg = "0000";
}

// Simulator class implementation
Simulator::Simulator() : pc(0), halted(false), cpu(memory, registers, pc, halted) {}

bool Simulator::isValidInstruction(const string &instruction) {
    if (instruction.length() != 4) return false;
    regex instructionFormat("[1-6BC][0-9A-F]{3}");
    return regex_match(instruction, instructionFormat);
}

void Simulator::loadProgram(const string &filename) {
    ifstream file(filename);
    string instruction;
    vector<string> instructions;

    cout << "Do you want to choose a specific starting point? (y/n)" << endl;
    char choice;
    cin >> choice;
    cin.ignore();

    int address;
    if (choice == 'y' || choice == 'Y') {
        cout << "Enter the starting memory address (default is 01):\n";
        string startAddr;
        getline(cin, startAddr);
        address = stoi(startAddr, nullptr, 16);
    } else {
        address = 1;
    }
    pc = address ;
    while (file >> instruction) {
        if (isValidInstruction(instruction)) {
            instructions.push_back(instruction);
        } else {
            cout << "Skipping invalid instruction: " << instruction << endl;
        }
    }
    file.close();

    // Ensure there’s a halt instruction if not already provided.
    if (instructions.empty() || instructions.back() != "C000") {
        instructions.push_back("C000");
    }

    // Store instructions in memory starting from the specified address.
    for (const auto &instr : instructions) {
        if (instr.length() >= 4) { // Ensure instruction is long enough
            memory.store(address++, instr.substr(0, 2));
            memory.store(address++, instr.substr(2, 2));
        }
    }
}

void Simulator::run_entire_program() {
    while (!halted && pc < 256) {
        cpu.fetchInstruction();
        cpu.executeInstruction();
        pc += 2;
    }
    displayState();
}

void Simulator::runStepByStep() {
    char choice;
    while (!halted && pc < 256) {
        cpu.fetchInstruction();
        cpu.executeInstruction();
        pc += 2;

        displayState();
        if (cpu.getInstructionRegister() != "C000") {
            cout << "Step completed. Continue to next step? (y/n): ";
            cin >> choice;
            if (choice != 'y' && choice != 'Y') break;
        }
    }
}

void Simulator::displayState() {
    registers.printRegisters();
    memory.printMemory();
    cout << "PC: " << setw(2) << setfill('0') << hex << uppercase << pc << "\n";
    cout << "IR: " << cpu.getInstructionRegister() << "\n";
    cout << "Screen output: " << memory.getScreen() << "\n";
    cout << "-----------------------------\n";
}

void Simulator::reset() {
    pc = 1;
    halted = false;
    memory = Memory();
    registers = Registers();
    cpu.setInstructionRegister();
    cout << "Simulator reset.\n";
}