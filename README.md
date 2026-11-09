DTEK-V Tetris
=============

A bare-metal implementation of Tetris designed for the DTEK-V RISC-V development board. The project utilizes C for the core game engine and matrix logic, alongside RISC-V assembly for low-level initialization and interrupt handling.

Hardware Requirements
---------------------

*   **DTEK-V Board:** Acting as the primary microcontroller (RV32IMC architecture).
    
*   **SPI Display:** Connected via the board's SPI interface (memory-mapped to 0x40001000).
    
*   **Onboard Buttons/GPIO:** For user input (memory-mapped to 0x40000000).
    

File Structure
--------------

*   Makefile: Build automation configuration.
    
*   startup.S: RISC-V assembly boot code, stack pointer initialization, and trap handler.
    
*   main.c: Main execution loop, hardware polling, and game clock.
    
*   display.h / display.c: SPI hardware abstraction layer and frame buffer management for the screen.
    
*   tetris.h / tetris.c: Core game logic, collision detection, piece definitions, and rotation mathematics.
    
*   link.ld: Linker script defining memory regions (must be provided for your specific DTEK-V memory layout).
    

Prerequisites
-------------

You need a RISC-V cross-compiler toolchain installed on your system to build the binaries.Bash

Plain textANTLR4BashCC#CSSCoffeeScriptCMakeDartDjangoDockerEJSErlangGitGoGraphQLGroovyHTMLJavaJavaScriptJSONJSXKotlinLaTeXLessLuaMakefileMarkdownMATLABMarkupObjective-CPerlPHPPowerShell.propertiesProtocol BuffersPythonRRubySass (Sass)Sass (Scss)SchemeSQLShellSwiftSVGTSXTypeScriptWebAssemblyYAMLXML`   sudo apt install gcc-riscv64-unknown-elf   `

Building the Project
--------------------

Run the following command in the project directory to compile the source files and generate the executable ELF file:Bash

Plain textANTLR4BashCC#CSSCoffeeScriptCMakeDartDjangoDockerEJSErlangGitGoGraphQLGroovyHTMLJavaJavaScriptJSONJSXKotlinLaTeXLessLuaMakefileMarkdownMATLABMarkupObjective-CPerlPHPPowerShell.propertiesProtocol BuffersPythonRRubySass (Sass)Sass (Scss)SchemeSQLShellSwiftSVGTSXTypeScriptWebAssemblyYAMLXML`   make   `

To clean the build directory of compiled object files and binaries:Bash

Plain textANTLR4BashCC#CSSCoffeeScriptCMakeDartDjangoDockerEJSErlangGitGoGraphQLGroovyHTMLJavaJavaScriptJSONJSXKotlinLaTeXLessLuaMakefileMarkdownMATLABMarkupObjective-CPerlPHPPowerShell.propertiesProtocol BuffersPythonRRubySass (Sass)Sass (Scss)SchemeSQLShellSwiftSVGTSXTypeScriptWebAssemblyYAMLXML`   make clean   `

Flashing to the Board
---------------------

Depending on your specific DTEK-V programmer setup (e.g., OpenOCD or a custom serial bootloader), flash the compiled tetris.elf or extract the binary format to load onto the board.Bash

Plain textANTLR4BashCC#CSSCoffeeScriptCMakeDartDjangoDockerEJSErlangGitGoGraphQLGroovyHTMLJavaJavaScriptJSONJSXKotlinLaTeXLessLuaMakefileMarkdownMATLABMarkupObjective-CPerlPHPPowerShell.propertiesProtocol BuffersPythonRRubySass (Sass)Sass (Scss)SchemeSQLShellSwiftSVGTSXTypeScriptWebAssemblyYAMLXML`   riscv32-unknown-elf-objcopy -O binary tetris.elf tetris.bin   `

Controls
--------

The game relies on four hardware buttons mapped to the lowest 4 bits of the GPIO input register.

*   **Bit 0:** Move Left
    
*   **Bit 1:** Move Right
    
*   **Bit 2:** Rotate Piece
    
*   **Bit 3:** Soft Drop (Accelerate Downward)