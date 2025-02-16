#include "chip8.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>

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

    // TODO: optimize type 
    uint16_t instruction_num = extractValue(opcode, 0xF000, 12); // first nibble - instruction type/no
    uint16_t x = extractValue(opcode, 0x0F00, 8); // second nibble
    uint16_t y = extractValue(opcode, 0x00F0, 4); // third nibble 
    uint16_t n = extractValue(opcode,0x000F); // last 4 bits
    uint16_t nn = extractValue(opcode,0x00FF); // last 8 bits
    uint16_t nnn = extractValue(opcode,0x0FFF); // last 12 bits

    switch(instruction_num){
        case 0: 
            switch (opcode){
                // Jump to a machine code routine at nnn.
                case(0x00E0):
                    // Clear the display.
                    break;
                case(0x00EE):
                    // Return from a subroutine.
                    break; 
            }
            break;
    
        case 1: 
            // Jump to location nnn.
            pc = nnn; 
            break;
        
        case 2: 
            //  Call subroutine at nnn.
            stack.push(pc);
            pc = nnn;
            break;

        case 3: 
            //  Skip next instruction if Vx = kk.
            if (V[x] == nn){
                pc += 2;
            }
            break;

        case 4: 
            //  Skip next instruction if Vx != kk.
            if (V[x] != nn){
                pc += 2;
            }
            break; 

        case 5: 
            //  Skip next instruction if Vx = Vy.
            if (V[x] == V[y]){
                pc += 2;
            }
            break;

        case 6:
            //  Set Vx = kk.
            V[x] = nn;
            break;

        case 7: 
            //  Set Vx = Vx + kk.
            V[x] += nn;
            break;

        case 8:
            switch(n){
                case 0:
                    //  Set Vx = Vy.
                    V[x] = V[y];
                    break;
                case 1: 
                    //  Set Vx = Vx OR Vy.
                    V[x] = (V[x] | V[y]);
                    break;
                case 2:
                    //  Set Vx = Vx AND Vy.
                    V[x] = (V[x] & V[y]);
                    break;
                case 3:
                    //  Set Vx = Vx XOR Vy.
                    V[x] = (V[x] ^ V[y]);
                    break;
                case 4:
                    //  Set Vx = Vx + Vy, set VF = carry.
                    //  The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. 
                    //Only the lowest 8 bits of the result are kept, and stored in Vx.
                    V[x] += V[y];
                    break;
                case 5:
                    //  Set Vx = Vx - Vy, set VF = NOT borrow.
                    //  If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
                    V[x] -= V[y];
                    if (V[x] > V[y]){
                        V[15] = 1;
                    }
                    else{
                        V[15] = 0;
                    }
                    break;
                case 6: 
                    //  Set Vx = Vx SHR 1.
                    //  If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
                    break;
                case 7:
                    //  Set Vx = Vy - Vx, set VF = NOT borrow. 
                    // If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx
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
            if (V[x] != V[y]){
                pc += 2;
            }
            break;

        case 0xA: // TODO: gengi - finish this
            I = nnn;
            break;  
        // Jump to location nnn + V0.
        case 0xB:
            int offset = V[2];
            pc = nnn + V[2];
        // Set Vx = random byte AND kk.
        case 0xC:
            int rand_byte = rand() % 101;
            V[x] = rand_byte  & nn;
            break;
        case 0xD: 
            //  Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
            break;
        case 0xE:
            //  Skip next instruction if key with the value of Vx is pressed.
            break;
        case 0xF:
            switch(nn){
                case 0x07:
                    //  Set Vx = delay timer value.
                    V[x] =delay_timer;
                    break;
                case 0x0A:
                    //  Wait for a key press, store the value of the key in Vx.
                case 0x15:
                    //  Set delay timer = Vx.
                    delay_timer = V[x];
                case 0x18:
                    //  Set sound timer = Vx.
                    sound_timer = V[x];
                    break;
                case 0x1E:
                    //  Set I = I + Vx.
                    I += V[x];
                case 0x29:
                    //  Set I = location of sprite for digit Vx.
                case 0x33:
                    //  Store BCD representation of Vx in memory locations I, I+1, and I+2.
                    for (int i = 0; i <= 2; i++){

                    }
                case 0x55:
                    //  Store registers V0 through Vx in memory starting at location I.
                case 0x65:
                    //  Read registers V0 through Vx from memory starting at location I.

            }
        
        
    }
}

void Chip8::storeInput(){
}

void Chip8::debug(){

}

int Chip8::extractValue(int opcode, int bitmask, int bits = 0){
    return (opcode & bitmask) >> bits;
}
