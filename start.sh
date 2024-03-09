if [ -f "./snakegame" ]; then
    chmod +x "./snakegame" && "./snakegame"    
else
    if [ -f "./snake.c" ]; then
        if ! command -v gcc &> /dev/null; then
            sudo apt-get install gcc
            clear
        fi
        if ! dpkg -l | grep -E '^ii.*libncurses5.*$' &> /dev/null; then
            sudo apt-get install libncurses5
            clear
        fi
        gcc ./snake.c -o ./snakegame -lncurses
        chmod +x "./snakegame" && "./snakegame"
    else
        echo "Error: snake.c not found"
    fi
fi