# CHIP-8 Emulator

A modern CHIP-8 interpreter built with C++ and OpenGL, featuring automatic ROM detection, support for different keyboard formats and a modern rendering pipeline.

## Description

This CHIP-8 emulator accurately interprets and executes CHIP-8 programs, supporting the full instruction set with modern quirks mode. Built with OpenGL for hardware-accelerated rendering and GLFW for cross-platform windowing, the emulator features dynamic ROM loading, configurable quirks, and passes the comprehensive Timendus test suite. The project includes automatic project root detection, making it easy to run from any directory without manual path configuration.

## Getting Started

### Dependencies

* **C++17 or higher**
* **CMake 3.10.0 or higher**
* **OpenGL 3.3 or higher**
* **Operating System:** Windows 10/11, macOS, or Linux
* **Libraries (included in project):**
  * GLFW 3.4
  * GLAD
  * GLM (for matrix operations)

### Installing

1. Clone the repository:
```bash
git clone https://github.com/redhoodtxt/chip8-emulator.git
cd chip8-emulator
```

2. Create a build directory:
```bash
mkdir build
cd build
```

3. Generate build files with CMake:
```bash
cmake ..
```

4. Build the project:
```bash
# On Windows with Visual Studio
cmake --build . --config Release

# On macOS/Linux
make
```

5. Place your CHIP-8 ROM files in the `games/` directory at the project root

### Executing Program

1. Run the emulator executable:
```bash
# Windows
.\Release\chip8.exe

# macOS/Linux
./chip8
```

2. When prompted, select a ROM from the list:
```
Available ROMs:
  1. ibm-logo.ch8
  2. pong.ch8
  3. tetris.ch8
  4. ....

Enter ROM number (1-3): 
```

3. **Keyboard Mapping:**
```
CHIP-8 Keypad:     Your Keyboard (QWERTY):
©°©¤©¤©¤©Ð©¤©¤©¤©Ð©¤©¤©¤©Ð©¤©¤©¤©´  ©°©¤©¤©¤©Ð©¤©¤©¤©Ð©¤©¤©¤©Ð©¤©¤©¤©´
©¦ 1 ©¦ 2 ©¦ 3 ©¦ C ©¦  ©¦ 1 ©¦ 2 ©¦ 3 ©¦ 4 ©¦
©À©¤©¤©¤©à©¤©¤©¤©à©¤©¤©¤©à©¤©¤©¤©È  ©À©¤©¤©¤©à©¤©¤©¤©à©¤©¤©¤©à©¤©¤©¤©È
©¦ 4 ©¦ 5 ©¦ 6 ©¦ D ©¦  ©¦ Q ©¦ W ©¦ E ©¦ R ©¦
©À©¤©¤©¤©à©¤©¤©¤©à©¤©¤©¤©à©¤©¤©¤©È  ©À©¤©¤©¤©à©¤©¤©¤©à©¤©¤©¤©à©¤©¤©¤©È
©¦ 7 ©¦ 8 ©¦ 9 ©¦ E ©¦  ©¦ A ©¦ S ©¦ D ©¦ F ©¦
©À©¤©¤©¤©à©¤©¤©¤©à©¤©¤©¤©à©¤©¤©¤©È  ©À©¤©¤©¤©à©¤©¤©¤©à©¤©¤©¤©à©¤©¤©¤©È
©¦ A ©¦ 0 ©¦ B ©¦ F ©¦  ©¦ Z ©¦ X ©¦ C ©¦ V ©¦
©¸©¤©¤©¤©Ø©¤©¤©¤©Ø©¤©¤©¤©Ø©¤©¤©¤©¼  ©¸©¤©¤©¤©Ø©¤©¤©¤©Ø©¤©¤©¤©Ø©¤©¤©¤©¼
```

4. Press `ESC` to exit the emulator

## Help

### Common Issues

**"Could not find 'games' directory"**
```bash
# Ensure you're running from the correct directory or that
# the games folder exists in your project root
```

**"No ROM files found"**
```bash
# Add .ch8 ROM files to the games/ directory
# ROMs can be found at: https://github.com/kripod/chip8-roms
```

**Black screen / Nothing displays**
```bash
# Check console output for ROM loading confirmation
# Verify your OpenGL drivers are up to date
# Try running a known-working ROM like ibm-logo.ch8
```

**Keys not responding**
```bash
# Ensure the window has focus
# Check that you're using the correct keyboard layout (QWERTY)
```

## Authors

[Roshan]  
[@redhoodtxt](https://github.com/redhoodtxt)

## Version History

* 1.0
    * Full CHIP-8 instruction set implementation
    * Passes Timendus flags and quirks tests
    * Dynamic ROM loading with automatic path detection
    * Modern quirks mode support
    * Hardware-accelerated OpenGL rendering
    
* 0.5
    * Added keyboard input handling
    * Implemented all arithmetic and logic opcodes
    * Fixed VF flag behavior
    
* 0.1
    * Initial Release
    * Basic opcode implementation
    * IBM logo rendering

## License

This project is licensed under the MIT License - see the LICENSE.md file for details

## Acknowledgments

Big thanks to:
* [Cowgod's Chip-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM) - The definitive CHIP-8 specification
* [Tobias V. Langhoff's Guide to Making a CHIP-8 Emulator](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/) - Excellent explanations on concepts without giving everything away. The display opcode explanation was particularly helpful
* [Laurence Muller's CHIP-8 Tutorial](https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/) - Great resource for understanding graphics and timing
* [Timendus CHIP-8 Test Suite](https://github.com/Timendus/chip8-test-suite) - Comprehensive test ROMs for validation
* [GLFW Documentation](https://www.glfw.org/documentation.html) - Cross-platform windowing library
* [LearnOpenGL](https://learnopengl.com/) - OpenGL tutorials and best practices