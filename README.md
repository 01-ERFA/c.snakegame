# SnakeGame in C (v1.0)

Snake is a classic arcade game in which you control a constantly growing snake that must eat light dots to increase its size, avoiding collisions with walls or its own tail. The goal is to survive as long as possible and achieve the highest score.

This project is designed to run on a Linux terminal. The controls are: w, s, a, d.

The project dependencies are:

  - **stdio.h**: Standard input/output header file which contains functions for input/output operations.
  - **stdlib.h**: Standard library header file which contains functions for memory allocation, process control, and other standard functions.
  - **ncurses.h**: NCurses library header file which provides functions for creating text-based user interfaces.
  - **unistd.h**: UNIX standard library header file which contains constants, types, and function prototypes for POSIX-compliant systems.
  - **time.h**: Time library header file which contains functions for manipulating time and date.

To install our project, you need to have the gcc and ncurses compiler installed on your system. If you do not have these packages, you can install them by following the following steps:

- Install the gcc compiler:
  - On Debian-based systems (such as Ubuntu), run the following command:
    > sudo apt-get install gcc
  - On Red Hat-based systems (such as Fedora), run the following command:
    > sudo dnf install gcc
    
- Install ncurses:
  - On Debian-based systems (such as Ubuntu), run the following command:
    > sudo apt-get install libncurses5-dev
  - On Red Hat-based systems (such as Fedora), run the following command:
    > sudo dnf install ncurses-devel

# Everything ready?

- Execute ```start.sh``` and begin playing!
or
- Compile the project with the command: ```gcc snake.c -o ./snakegame -lncurses``` and start playing! (run: ```./snakegame```)


