/*
this contains the structure of the class implementation file (chip8.cpp). still a noob so bear with me if this is 
not great 
*/ 
# include <stdint.h> 
# include <stack> 

class Chip8 {
    private: 
        static const size_t MEMORY_SIZE = 4096;         // memory  size
        static const size_t KEYPAD_SIZE = 16;           // keypad size
        static const size_t NUM_REGISTERS = 16;         // number of V registers
        static const size_t DISPLAY_WIDTH = 64;         // width of display
        static const size_t DISPLAY_HEIGHT = 32;        // height of display

        uint8_t memory[MEMORY_SIZE];                    // memory 

        std::stack<uint16_t> stack;                     // stack 
        uint8_t sp;                                     // stack pointer 

        uint16_t opcode;                                // opcode
        uint16_t pc;                                    // program counter 
        uint8_t V[NUM_REGISTERS];                       // V registers
        uint16_t I;                                     // index register

        uint8_t sound_timer;                            // sound timer
        uint8_t delay_timer;                            // delay timer 

        void init();

    public: 
        Chip8(); 
        ~Chip8();

        bool drawFlag;

        void loadGame();    
        void emulateCycle();
        void storeInput();
        void debug();
        
        uint8_t keypad[KEYPAD_SIZE];                    // keypad
        uint8_t display[DISPLAY_WIDTH][DISPLAY_HEIGHT]; // display

};