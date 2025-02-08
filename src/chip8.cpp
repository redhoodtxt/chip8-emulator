# include "chip8.h"
# include <cstring>
# include <cstdio>

Chip8::Chip8(){

}

// prepare system state 
void Chip8::init(){

    display[DISPLAY_WIDTH][DISPLAY_HEIGHT] = {0}; // clear display 
    // clear stack
    while(!stack.empty()){
        stack.pop();
    }
    memset(memory,0,MEMORY_SIZE); // clear memory 
    // reset registers to zero
    for (int i = 0; i <= NUM_REGISTERS; i++){
        V[i] = 0;
    }

    pc = 0x200; // set program counter to where the application is loaded in memory
    I = 0; // set index register to 0
    // sp = 0; // set stack pointer to 0
    opcode = 0; // reset opcode 
    
    // TODO: gengi -  load fontset into memory 
    sound_timer = 0; // reset sound timer
    delay_timer = 0; // reset delay timer 
}

void Chip8::loadGame(){
    
    // cheeky C-style, hope this doesn't cause any problems lol
    char filename[256]; // filename buffer
    char path[512]; // path buffer
    const char *directory = "../games/"; // directory where game is stored

    printf("Name of the game?\n"); // print input message
    fgets(filename,sizeof(filename),stdin); // get input
    filename[strcspn(filename, "\n")] = '\0'; // remove newline 

    strcpy(path, directory); // copy directory to path buffer
    strcat(path, filename); // append filename to path 

    // open file
    FILE * pFile;
    pFile = fopen(path, "rb");

    // check if file is open
    if (pFile == nullptr) {
        // Handle file open failure
        printf("Failed to open the file!\n");
        return;
    }

    //load the binary into memory at 0x200 (512)
    fread(memory+512,sizeof(uint8_t),MEMORY_SIZE-512,pFile);

    // close the binary 
    fclose(pFile);

}

void Chip8::emulateCycle(){
    uint8_t byte_1 = memory[pc];
    uint8_t byte_2 = memory[pc+1];

    opcode = byte_1 << 8 | byte_2; // shift byte 1 by 8 bits (adds 8 zeros) and pipe byte 2 into byte 1

    pc += 2;

    uint16_t instruction_num = extractValue(opcode, 0xF000, 12); // first nibble - instruction type/no
    uint16_t x = extractValue(opcode, 0x0F00, 8); // second nibble
    uint16_t y = extractValue(opcode, 0x00F0, 4); // third nibble 
    uint16_t n = extractValue(opcode,0x000F); // fourth nibble
    uint16_t nn = extractValue(opcode,0x00FF); // second byte, immediate number
    uint16_t nnn = extractValue(opcode,0x0FFF); // 12 bit address 

    switch(instruction_num){
        case 0: 
            switch (opcode){
                // TODO: gengi - some logic to clear screen - set pixels to 0
                case(0x00E0):
                    break;
                case(0x00EE):
                    break; 
            }
            break;
    
        case 1: // 1nnn
            pc = nnn; 
            break;
        
        case 2: // 2nnn
            stack.push(pc);
            pc = nnn;
            break;

        case 3: 
            if (V[x] == nn){
                pc += 2;
            }
            break;

        case 4: 
            if (V[x] != nn){
                pc += 2;
            }
            break; 

        case 5: 
            if (V[x] == V[y]){
                pc += 2;
            }
            break;

        case 6: // 6xnn
            V[x] = nn;
            break;

        case 7: // 7xnn
            V[x] += nn;
            break;

        case 8:
            switch(n){
                case 0:
                    V[x] = V[y];
                    break;
                case 1: 
                    V[x] = (V[x] | V[y]);
                    break;
                case 2:
                    V[x] = (V[x] & V[y]);
                    break;
                case 3:
                    V[x] = (V[x] ^ V[y]);
                    break;
                case 4:
                    V[x] += V[y];
                    break;
                case 5:
                    V[x] -= V[y];
                    if (V[x] > V[y]){
                        V[15] = 1;
                    }
                    else{
                        V[15] = 0;
                    }
                    break;
                case 6: // TODO
                    break;
                case 7:
                    V[y] -= V[x];
                    if(V[y] > V[x]){
                        V[15] = 1;
                    }
                    else{
                        V[15] = 0;
                    }
                    break;
            }
            break;

        case 9: // TODO: gengi - finish this
            break;

        case 0xA: // TODO: gengi - finish this
            I = nnn;
            break;  
        

    }
}

void Chip8::storeInput(){
}

void Chip8::debug(){

}

int Chip8::extractValue(int opcode, int bitmask, int bits = 0){
    return (opcode & bitmask) >> bits;
}
