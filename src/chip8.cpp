#include "chip8.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <GLFW/glfw3.h>

Chip8::Chip8()
	: opcode(0), pc(0), I(0), sound_timer(0), delay_timer(0), drawFlag(false), memory{}, V{}, keypad{}, display{} // zero-ed initializer list 
{

}

Chip8::~Chip8() {
}

const uint8_t CHIP8_FONTSET[80] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
 
// prepare system state 
void Chip8::init() {

	memset(display, 0, sizeof(display)); // clear display 

	// clear stack
	while (!stack.empty()) {
		stack.pop();
	}

	memset(memory, 0, MEMORY_SIZE); // clear memory 

	// reset registers to zero
	for (int i = 0; i < NUM_REGISTERS; i++) {
		V[i] = 0;
	}

	pc = 0x200; // set program counter to where the application is loaded in memory

	// sp = 0; // set stack pointer to 0

	// load fontset into memory 
	for (int i = 0; i < 80; i++) {
		memory[i] = CHIP8_FONTSET[i]; // load from 0x50¨C0x9F convention
	}
}

//void Chip8::loadGame() {
//
//	// cheeky C-style, hope this doesn't cause any problems lol
//	char filename[256]; // filename buffer
//	char path[512]; // path buffer
//	const char* directory = "../games/"; // directory where game is stored
//
//	printf("Name of the game?\n"); // print input message
//	fgets(filename, sizeof(filename), stdin); // get input
//	filename[strcspn(filename, "\n")] = '\0'; // remove newline 
//
//	strcpy(path, directory); // copy directory to path buffer
//	strcat(path, filename); // append filename to path 
//
//	// open file
//	FILE* pFile;
//	pFile = fopen(path, "rb");
//
//	// check if file is open
//	if (pFile == nullptr) {
//		// Handle file open failure
//		printf("Failed to open the file!\n");
//		return;
//	}
//
//	//load the binary into memory at 0x200 (512)
//	fread(memory + 512, sizeof(uint8_t), MEMORY_SIZE - 512, pFile);
//
//	// close the binary 
//	fclose(pFile);
//
//}

void Chip8::loadGame() {
	//const char* directory = "../games/"; // directory where game is stored
	//const char* filename = "2-ibm-logo.ch8"; // hardcoded IBM Logo ROM

	//char path[512];
	//strcpy(path, directory); // copy directory
	//strcat(path, filename);  // append filename
	const char* path = "C:/Users/Admin/Documents/gengz/projects/chip8-emulator/games/flightrunner.ch8";
	// open file
	FILE* pFile = fopen(path, "rb");
	if (pFile == nullptr) {
		printf("Failed to open the ROM file: %s\n", path);
		return;
	}

	// load the binary into memory at 0x200 (512)
	size_t bytesRead = fread(memory + 512, sizeof(uint8_t), MEMORY_SIZE - 512, pFile);
	printf("ROM loaded: %zu bytes\n", bytesRead);
	printf("First few bytes: %02X %02X %02X %02X\n",
		memory[0x200], memory[0x201], memory[0x202], memory[0x203]);
	if (bytesRead == 0) {
		printf("Failed to read the ROM file: %s\n", path);
		fclose(pFile);
		return;
	}

	// close the binary 
	fclose(pFile);
}


void Chip8::emulateCycle() {
	uint8_t byte_1 = memory[pc];
	uint8_t byte_2 = memory[pc + 1];

	opcode = byte_1 << 8 | byte_2; // shift byte 1 by 8 bits (adds 8 zeros) and pipe byte 2 into byte 1

	pc += 2;

	// TODO: optimize type 
	uint16_t instruction_num = extractValue(opcode, 0xF000, 12); // first nibble - instruction type/no
	uint16_t x = extractValue(opcode, 0x0F00, 8); // second nibble
	uint16_t y = extractValue(opcode, 0x00F0, 4); // third nibble 
	uint16_t n = extractValue(opcode, 0x000F); // last 4 bits
	uint16_t nn = extractValue(opcode, 0x00FF); // last 8 bits
	uint16_t nnn = extractValue(opcode, 0x0FFF); // last 12 bits

	// fallthrough warning comes cuz i haven't finish writing the cases lol https://learn.microsoft.com/en-us/cpp/code-quality/c26819?view=msvc-170&f1url=%3FappId%3DDev17IDEF1%26l%3DEN-US%26k%3Dk(C26819)%26rd%3Dtrue
	switch (instruction_num) {
	case 0:
		switch (opcode) {
			// Jump to a machine code routine at nnn.
		case(0x00E0):
			// Clear the display.
			memset(display, 0, sizeof(display));
			break;	
		case(0x00EE): // TODO
			// Return from a subroutine
			pc = stack.top(); // retrieves the value since pop(0) returns void
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
		if (V[x] == nn) {
			pc += 2;
		}
		break;
	case 4:
		//  Skip next instruction if Vx != kk.
		if (V[x] != nn) {
			pc += 2;
		}
		break;
	case 5:
		//  Skip next instruction if Vx = Vy.
		if (V[x] == V[y]) {
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
		switch (n) {
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
		case 4: {
			//  Set Vx = Vx + Vy, set VF = carry.
			//  The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. 
			//Only the lowest 8 bits of the result are kept, and stored in Vx.
			uint16_t sum = V[x] + V[y];
			V[0xF] = (sum > 0xFF); // 255
			V[x] = sum & 0xFF; // mask to get lower 8 
			break;
		}
		case 5:
			//  Set Vx = Vx - Vy.
			//  If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
			V[x] -= V[y];
			V[0xF] = (V[x] > V[y]);
			break;
		case 6: {
			//  Set Vx = Vx SHR 1.
			//  If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
			// older versions assigned VY to VX
			V[x] = V[y];
			V[0xF] = V[x] & 1; // set the bool lsb to the VF flag 
			V[x] >>= 1; // divide by 2 and reassign to Vx
			break;
		}
		case 7:
			//  Set Vx = Vy - Vx, set VF = NOT borrow. 
			// If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx
			V[y] -= V[x];
			V[0xF] = (V[y] > V[x]);
			break;
		case 0xe:
			// Set Vx = Vx SHL 1.
			// If the most - significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
			V[0xF] = (V[x] & 0x80) >> 7; // 0x80 is binary 1000 0000. shift by 15 bits to lsb for bool value and set VF based on that
			V[x] <<= 1;
			break;
		}
		break;
	case 9:
		// Skip next instruction if Vx != Vy.
		// The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.
		if (V[x] != V[y]) {
			pc += 2;
		}
		break;
	case 0xa:
		// Set I = nnn.
		// The value of register I is set to nnn.
		I = nnn;
		break;
	case 0xb: {
		// Jump to location nnn + V0.
		// The program counter is set to nnn plus the value of V0.
		int offset = V[0];
		pc = nnn + offset;
		break;
	}
	case 0xc: {
		// Set Vx = random byte AND kk.
		int rand_byte = rand() % 101;
		V[x] = rand_byte & nn;
		break;
	}
	case 0xd: {
		// Draw n-byte sprite at (Vx, Vy)
		int vx = V[x] % 64;
		int vy = V[y] % 32;
		V[0xF] = 0; // collision flag

		//printf("DRAW: opcode=%04X, I=%03X, Vx=%d, Vy=%d, n=%d\n",
		//	opcode, I, V[x], V[y], n);

		for (int row = 0; row < n; row++) {
			uint8_t sprite_byte = memory[I + row];
			for (int col = 0; col < 8; col++) {
				bool sprite_pixel = (sprite_byte >> (7 - col)) & 1;
				if (!sprite_pixel) continue;

				int x_coord = (vx + col) % 64;
				int y_coord = (vy + row) % 32;

				uint8_t& screen_pixel = display[x_coord][y_coord];
				if (screen_pixel) V[0xF] = 1;  // collision detection
				screen_pixel ^= 1;             // XOR draw
			}
		}
		drawFlag = true;
		break;
	}
	case 0xe:
		switch (nn) {
		case 0x9E: {
			//  Skip next instruction if key with the value of Vx is pressed.
			printf("EX9E: Checking if key %X is pressed. State: %d\n", V[x], keypad[V[x]]);
			if (keypad[V[x]]) {
				pc += 2;
			}
			break;
		}
		case 0xA1: {
			printf("EXA1: Checking if key %X is NOT pressed. State: %d\n", V[x], keypad[V[x]]);
			if (!keypad[V[x]]) {
				pc += 2;
			}
			break;
		}
		}
		break;
	case 0xf:
		switch (nn) {
		case 0x07:
			//  Set Vx = delay timer value.
			V[x] = delay_timer;
			break;
		case 0x0A: {
			// Wait for a key press, store the value in Vx
			bool keyPressed = false;
			for (int i = 0; i < 16; i++) {
				if (keypad[i]) {
					printf("FX0A: Key %X pressed, storing in V[%X]\n", i, x);
					V[x] = i;
					keyPressed = true;
					break;
				}
			}

			if (!keyPressed) {
				pc -= 2;  // Repeat this instruction until a key is pressed
			}
			break;
		}
		case 0x15:
			//  Set delay timer = Vx.
			delay_timer = V[x];
			break;
		case 0x18:
			//  Set sound timer = Vx.
			sound_timer = V[x];
			break;
		case 0x1E:
			//  Set I = I + Vx.
			I += V[x];
			break;
		case 0x29:
			//  Set I = location of sprite for digit Vx.
			I = V[x] * 5; // each digit sprite is 5 bytes long
			break;
		case 0x33:
			//  Store BCD representation of Vx in memory locations I, I+1, and I+2.
			//  The interpreter takes the decimal value of Vx, and places the hundreds digit in memory at location in I, 
			//  the tens digit at location I+1, and the ones digit at location I+2.
			memory[I] = floor(V[x] / float(100));
			memory[I + 1] = floor(V[x] / float(10));
			memory[I + 2] = V[x] % 10;
			break;
		case 0x55: 
			//  Store registers V0 through Vx in memory starting at location I.
			for (int i = 0; i <= x; i++) {
				memory[I + i] = V[i];
			}
			break;
		case 0x65:
			//  Read registers V0 through Vx from memory starting at location I.
			//  The interpreter reads values from memory starting at location I into registers V0 through Vx.
			for (int i = 0; i <= x; i++) {
				V[i] = memory[I + i];
			}
			break;
		}
	}
}

int Chip8::extractValue(int opcode, int bitmask, int bits) { // default argument for bits is 0
	return (opcode & bitmask) >> bits;
}
