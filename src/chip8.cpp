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
    sp = 0; // set stack pointer to 0
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

}

void Chip8::storeInput(){

}

void Chip8::debug(){

}
