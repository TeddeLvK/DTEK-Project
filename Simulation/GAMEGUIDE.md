Tetris Terminal Simulator (DTEK Project)
========================================

Welcome to Theodor's and Jen's DTEK Project! This is a fully functional, terminal-based Tetris simulator written in C. While it is designed to eventually run on a DTEK-V RISC-V board, this simulator allows you to play and test the game directly on your computer's terminal.

Features
--------

*   **Modern Tetris Mechanics:** Includes Wall-kicks, "Hold" piece functionality, "Next" piece preview, Soft Drop, and Hard Drop.
    
*   **Coyote Time (Lock Delay):** A 500ms grace period when a block touches the bottom, allowing you to slide or rotate it before it locks. Features an anti-infinite-stall mechanic.
    
*   **Dynamic Difficulty:** The game speeds up as you clear lines and increase your level.
    
*   **Persistent High Scores:** Your highest score is automatically saved to a highscore.txt file and loads the next time you play.
    
*   **Bouncing Start Menu & Pause Screen:** An animated welcome screen and the ability to pause the game mid-run.
    

Prerequisites
-------------

To compile and run this game, you will need:

*   A Unix-like terminal (macOS Terminal, Linux, or WSL on Windows).
    
*   **GCC** (GNU Compiler Collection).
    
*   **Make** (Build automation tool).
    
*   **Terminal Size:** Make sure your terminal window is stretched to be **at least 30 lines tall**. If your terminal is too short, the screen will wrap and cause visual glitches!
    

How to Build and Run
--------------------

1.  Open your terminal and navigate to the folder containing your project files.
    
2.  make
    
3.  ./tetris\_sim
    
4.  make clean
    

Controls
--------

**Action** = Key

**Start Game** = S (At the welcome menu)

**Move Left** = Left Arrow

**Move Right** = Right Arrow

**Rotate** = Up Arrow

**Soft Drop** = Down Arrow

**Hard Drop** = Spacebar

**Hold Piece** = Z

**Pause Game** = P

**Restart Game** = R (When game over)

**Quit Game** = Q


Troubleshooting
---------------

*   **"The screen is repeating/glitching downward!"** Your terminal window is too short. Resize your terminal window vertically so it is taller, then restart the game.
    
*   **"make: gcc-O2: No such file or directory"** Ensure there is a space in your Makefile between $(CC) and $(CFLAGS).