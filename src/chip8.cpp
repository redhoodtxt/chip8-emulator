#include "chip8.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <vector>


#include <GLFW/glfw3.h>

Chip8::Chip8()
	: opcode(0), pc(0), I(0), sound_timer(0), delay_timer(0), drawFlag(false), waitingForVBlank(false), vipMode(true), memory {
}, V{}, keypad{}, display{} // zero-ed initializer list 
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

//void Chip8::loadGame() {
//	//const char* directory = "../games/"; // directory where game is stored
//	//const char* filename = "2-ibm-logo.ch8"; // hardcoded IBM Logo ROM
//
//	//char path[512];
//	//strcpy(path, directory); // copy directory
//	//strcat(path, filename);  // append filename
//	const char* path = "C:/Users/Admin/Documents/gengz/projects/chip8-emulator/problematic/TETRIS";
//	// open file
//	FILE* pFile = fopen(path, "rb");
//	if (pFile == nullptr) {
//		printf("Failed to open the ROM file: %s\n", path);
//		return;
//	}
//
//	// load the binary into memory at 0x200 (512)
//	size_t bytesRead = fread(memory + 512, sizeof(uint8_t), MEMORY_SIZE - 512, pFile);
//	printf("ROM loaded: %zu bytes\n", bytesRead);
//	printf("First few bytes: %02X %02X %02X %02X\n",
//		memory[0x200], memory[0x201], memory[0x202], memory[0x203]);
//	if (bytesRead == 0) {
//		printf("Failed to read the ROM file: %s\n", path);
//		fclose(pFile);
//		return;
//	}
//
//	// close the binary 
//	fclose(pFile);
//}

#include <filesystem>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <algorithm>

void Chip8::loadGame() {
	// Check if CHIP8_ROOT is already set
	const char* projectRoot = std::getenv("CHIP8_ROOT");
	std::string directory;

	if (!projectRoot) {
		// Not set, so determine and set it automatically
		std::filesystem::path exePath = std::filesystem::current_path();

		// Go up directories until we find "games" folder
		std::filesystem::path searchPath = exePath;
		bool found = false;

		for (int i = 0; i < 5; i++) {  // Search up to 5 levels
			std::filesystem::path gamesPath = searchPath / "games";
			if (std::filesystem::exists(gamesPath) &&
				std::filesystem::is_directory(gamesPath)) {
				// Found it! Set the environment variable
				std::string rootPath = searchPath.string();

#ifdef _WIN32
				_putenv_s("CHIP8_ROOT", rootPath.c_str());
#else
				setenv("CHIP8_ROOT", rootPath.c_str(), 1);
#endif

				printf(" Auto-detected project root: %s\n", rootPath.c_str());
				directory = gamesPath.string() + "/";
				found = true;
				break;
			}
			searchPath = searchPath.parent_path();
		}

		if (!found) {
			printf("Error: Could not find 'games' directory\n");
			return;
		}
	}
	else {
		// Already set, use it
		directory = std::string(projectRoot) + "/games/";
		printf("Using CHIP8_ROOT: %s\n", projectRoot);
	}

	std::vector<std::string> romFiles;

	// Scan directory for all files (not just .ch8)
	try {
		for (const auto& entry : std::filesystem::directory_iterator(directory)) {
			if (entry.is_regular_file()) {
				std::string filename = entry.path().filename().string();
				// Skip hidden files and common non-ROM files
				if (filename[0] != '.' &&
					filename != "README.md" &&
					filename != "README.txt") {
					romFiles.push_back(filename);
				}
			}
		}
	}
	catch (const std::filesystem::filesystem_error& e) {
		printf("Error reading directory %s: %s\n", directory.c_str(), e.what());
		return;
	}

	if (romFiles.empty()) {
		printf("No ROM files found in %s\n", directory.c_str());
		return;
	}

	// Sort alphabetically
	std::sort(romFiles.begin(), romFiles.end());

	// Display ROMs
	printf("\nAvailable ROMs:\n");
	for (size_t i = 0; i < romFiles.size(); i++) {
		printf("  %zu. %s\n", i + 1, romFiles[i].c_str());
	}

	// Get user selection
	printf("\nEnter ROM number (1-%zu): ", romFiles.size());
	int choice;
	if (scanf("%d", &choice) != 1) {
		printf("Invalid input\n");
		return;
	}
	(void)getchar(); // consume newline

	if (choice < 1 || choice >(int)romFiles.size()) {
		printf("Invalid selection\n");
		return;
	}

	// Load selected ROM
	std::string selectedRom = romFiles[choice - 1];
	std::string fullPath = directory + selectedRom;

	FILE* pFile = fopen(fullPath.c_str(), "rb");
	if (pFile == nullptr) {
		printf("Failed to open ROM: %s\n", fullPath.c_str());
		return;
	}

	size_t bytesRead = fread(memory + 512, sizeof(uint8_t), MEMORY_SIZE - 512, pFile);
	if (bytesRead == 0) {
		printf("Failed to read ROM: %s\n", fullPath.c_str());
		fclose(pFile);
		return;
	}

	printf("Loaded %s (%zu bytes)\n\n", selectedRom.c_str(), bytesRead);
	fclose(pFile);
}


void Chip8::emulateCycle() {

	if (waitingForVBlank) {
		return;  // Skip this cycle
	}

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
			stack.pop();
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
	case 8: // a note to always set the VF register after the arithmetic operation, if not VF will not hold the right value at calculation time
		switch (n) {
		case 0:
			//  Set Vx = Vy.
			V[x] = V[y];
			break;
		case 1:
			//  Set Vx = Vx OR Vy.
			V[x] = (V[x] | V[y]);
			V[0xF] = 0;
			break;
		case 2:
			//  Set Vx = Vx AND Vy.
			V[x] = (V[x] & V[y]);
			V[0xF] = 0;
			break;
		case 3:
			//  Set Vx = Vx XOR Vy.
			V[x] = (V[x] ^ V[y]);
			V[0xF] = 0;
			break;
		case 4: {
			//  Set Vx = Vx + Vy, set VF = carry.
			//  The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. 
			//  Only the lowest 8 bits of the result are kept, and stored in Vx.
			uint16_t sum = V[x] + V[y];
			V[x] = sum & 0xFF; // mask to get lower 8
			V[0xF] = (sum > 0xFF); // 255			 
			break;
		}
		case 5: {
			//  Set Vx = Vx - Vy.
			//  If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx
			uint8_t vx = V[x];
			uint8_t vy = V[y];
			V[x] = vx - vy;
			V[0xF] = (vx >= vy);
			break;
		}
		case 6: {
			//  Set Vx = Vx SHR 1.
			// If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
			// older versions assigned VY to VX

			// VIP mode?
			if (vipMode) {
				V[y] = V[x];
				uint8_t vy = V[y];
				V[x] = vy >> 1; // divide by 2 and reassign to Vx
				V[0xF] = vy & 1; // set the bool lsb to the VF 
				break;
			}

			uint8_t vx = V[x];
			V[x] >>= 1; // divide by 2 and reassign to Vx
			V[0xF] = vx & 1; // set the bool lsb to the VF 
			break;
		}
		case 7:
			//  Set Vx = Vy - Vx, set VF = NOT borrow. 
			// If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx
			V[x] = V[y] - V[x];
			V[0xF] = (V[y] >= V[x]);
			break;
		case 0xe:
			// Set Vx = Vx SHL 1.
			// If the most - significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.

			// VIP mode?
			if (vipMode) {
				V[y] = V[x];
				uint16_t vy = V[y];
				V[x] = vy << 1; // divide by 2 and reassign to Vx
				V[0xF] = (vy & 0x80) >> 7; // set the bool lsb to the VF 
				break;
			}

			uint16_t vx = V[x];
			V[x] <<= 1;
			V[0xF] = (vx & 0x80) >> 7; // 0x80 is binary 1000 0000. shift by 15 bits to lsb for bool value and set VF based on that
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

				int x_coord = (vx + col);
				int y_coord = (vy + row);
				// clipping
				if (x_coord >= 64) break;
				if (y_coord >= 32) break;

				uint8_t& screen_pixel = display[x_coord][y_coord];
				if (screen_pixel) V[0xF] = 1;  // collision detection
				screen_pixel ^= 1;             // XOR draw
			}
		}
		drawFlag = true;
		waitingForVBlank = true;
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
					V[x] = keypad[i];
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
			memory[I + 1] = floor((V[x] % 100) / 10);
			memory[I + 2] = V[x] % 10;
			break;
		case 0x55: 
			//  Store registers V0 through Vx in memory starting at location I.
			for (int i = 0; i <= x; i++) {
				memory[I + i] = V[i];
			}
			I += x + 1; // increment register quirk
			break;
		case 0x65:
			//  Read registers V0 through Vx from memory starting at location I.
			//  The interpreter reads values from memory starting at location I into registers V0 through Vx.
			for (int i = 0; i <= x; i++) {
				V[i] = memory[I + i];
			}
			I += x + 1;
			break;
		}
	}
}

int Chip8::extractValue(int opcode, int bitmask, int bits) { // default argument for bits is 0
	return (opcode & bitmask) >> bits;
}
