#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <time.h>

// Constantes del juego
#define DRAW_CONTENT "  "

#define ID_BOARD 1 
#define ID_POINT 2
#define ID_HEAD  3
#define ID_SNAKE 4
#define ID_WALL  5

#define SLEEP_MS 75

// tipos de datos del juego
enum Controls {
    c_top    = 119,
    c_left   = 97,
    c_bottom = 115,
    c_right  = 100
}; 
typedef enum Controls Controls;

enum Directions {
    top    = 1,
    bottom = 2,
    left   = 3,
    right  = 4
};
typedef enum Directions Dir;

struct Coord_XY { int x, y; };
typedef struct Coord_XY Coord;

struct Block {
    Coord coord;
    char *content;
    int  id;
};
typedef struct Block Cell;
typedef struct Block Point;

struct Snake_Body_Parts {
    Coord coord;
    struct Snake_Body_Parts *next;
};
typedef struct Snake_Body_Parts SBP;

struct Snake_Body {
    Cell head;
    struct Snake_Body_Parts *body;
};
typedef struct Snake_Body Snake;

struct Board {
    int rows, cols;
    Cell **data;
};
typedef struct Board Board;

// funciones inicializadoras
Board *initBoard(int rows, int cols) {
    Board *board = (Board *)malloc(sizeof(Board));
    if (board == NULL) {
        return NULL; 
    }
    board->rows = rows;
    board->cols = cols;

    board->data = (Cell **)malloc(rows * sizeof(Cell *));
    if (board->data == NULL) {
        free(board); 
        return NULL; 
    }

    for (int i = 0; i < rows; i++) {
        board->data[i] = (Cell *)malloc(cols * sizeof(Cell));
        if (board->data[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(board->data[j]);
            }
            free(board->data); 
            free(board); 
            return NULL;
        }
        
        for (int j = 0; j < cols; j++) {
            board->data[i][j].content = DRAW_CONTENT;
            board->data[i][j].id      = ID_BOARD;
            board->data[i][j].coord.x = j *2;
            board->data[i][j].coord.y = i;
        }
    }
    
    for (int j = 0; j < rows; j++) board->data[j][0].id = ID_WALL;
    for (int j = 0; j < cols; j++) board->data[0][j].id = ID_WALL;

    for (int j = 0; j < rows; j++) board->data[j][cols-1].id = ID_WALL;
    for (int j = 0; j < cols; j++) board->data[rows-1][j].id = ID_WALL;

    return board;
};

void initRandom(){
    srand(time(NULL));
}

// funciones de control
void initInterface() {
    initscr();

    cbreak();
    noecho();
    nodelay(stdscr, true);

    start_color();
    // (referencia, color de texto, color de fondo del texto)
    init_pair(ID_BOARD, COLOR_BLACK  , COLOR_BLACK  );
    init_pair(ID_POINT, COLOR_RED    , COLOR_RED    );
    init_pair(ID_HEAD , COLOR_MAGENTA, COLOR_MAGENTA);
    init_pair(ID_SNAKE, COLOR_CYAN   , COLOR_CYAN   );
    init_pair(ID_WALL , COLOR_GREEN  , COLOR_GREEN  );     

    curs_set(false);
}

void closeInterface() {
    curs_set(true);
    clear();
    endwin();
}

// funciones auxiliares
void printBoard(Board *board) {
    for (int i = 0; i < board->rows; i++) {
        for (int j = 0; j < board->cols; j++) {
            printf("%d ", board->data[i][j].id);
        }
        printf("\n");
    }
}

void printMessagesDev(int rows, int cols, int speed, int points){
    printf("\nBienvenido al juego de la viborita!!\n\n");

    printf(" | (%dx%d)WxH | %dms | %dp | \n", cols, rows, speed, points);
    
    printf("\nHas comido %d puntos!! \n", points);

    printf("\nGracias por jugar <3\n");
    printf("\n");
}

int randomNum(int min, int max) {
    return min + rand() % (max - min +1);
}

int kbhit(){
    int ch = getch();
    if (ch != ERR) {
        ungetch(ch);
        return 1;
    }
    return 0;
}

void sleep_ms(int milliseconds){
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

// funciones principales del juego
void generatePoint(Board *board) {
    Coord pos;
    pos.y = randomNum(1, board->rows - 1);
    pos.x = randomNum(1, board->cols - 1);
    while (board->data[pos.y][pos.x].id > ID_POINT) {
        pos.y = randomNum(1, board->rows-1);
        pos.x = randomNum(1, board->rows-1);
    }
    board->data[pos.y][pos.x].id = ID_POINT;
}

void moveSnake(Board *board, Snake *body, Dir dir) {

    SBP *queue = body->body;
    Coord old_pos;
    
    if(body == NULL) {
        return;
    } 
    old_pos.x = body->head.coord.x;
    old_pos.y = body->head.coord.y;

    switch (dir) {
        case top:
            body->head.coord.y = body->head.coord.y -1;
            break;
        case bottom:
            body->head.coord.y = body->head.coord.y +1;
            break;
        case left:
            body->head.coord.x = body->head.coord.x -1;
            break;
        case right:
            body->head.coord.x = body->head.coord.x +1;
            break;
        default:
            break;
    }
    
    if (body->body == NULL && board->data[body->head.coord.y][body->head.coord.x].id == ID_POINT) {
        body->body = (SBP *)malloc(sizeof(SBP));
        body->body->coord.x = old_pos.x;
        body->body->coord.y = old_pos.y;
        body->body->next = NULL;
        return;
    }

    if (body->body == NULL) {
        return;
    }

    if (board->data[body->head.coord.y][body->head.coord.x].id == ID_POINT) {

        SBP *new_part = (SBP *)malloc(sizeof(SBP));
        
        new_part->next = body->body;
        new_part->coord.x = old_pos.x;
        new_part->coord.y = old_pos.y;

        body->body = new_part;
        return;
    }

    SBP *queue2 = NULL;
    while (queue->next != NULL) {
        queue2 = queue;
        queue = queue->next;
    }

    queue->coord.x = old_pos.x;
    queue->coord.y = old_pos.y;

    if (queue2 != NULL) {
        queue->next = body->body;
        body->body = queue;
        queue2->next = NULL;
    } else {
        queue->next = NULL;
    }
}

void update_snake(Board *board, Snake *new, Snake *old, Dir dir, bool *game_over, int *points) {

    if (old == NULL || new == NULL) {
        return;
    }
    SBP *current = old->body;

    moveSnake(board, new, dir);

    if (board->data[new->head.coord.y][new->head.coord.x].id > ID_HEAD) {
        *game_over = true;
    }

    if (board->data[new->head.coord.y][new->head.coord.x].id == ID_POINT) {
        *points = *points +1;
        generatePoint(board);
        moveSnake(board, old, dir);
        
    } else {
        board->data[old->head.coord.y][old->head.coord.x].id = ID_BOARD;
        while (current != NULL) {
            board->data[current->coord.y][current->coord.x].id = ID_BOARD;
            current = current->next;
        }
        moveSnake(board, old, dir);
    }

    board->data[new->head.coord.y][new->head.coord.x].id = ID_HEAD;
    current = new->body;
    while (current != NULL) {
        board->data[current->coord.y][current->coord.x].id = ID_SNAKE;
        current = current->next;
    }
}

// funciones de dibujo
void draw(int color, char *content, Coord coord) {
    attron(COLOR_PAIR(color));
    mvprintw(coord.y, coord.x, "%s", content);
    attroff(COLOR_PAIR(color));
}

void draw_board(Board *board) {
    for (int i = 0; i < board->rows; i++) {
        for (int j = 0; j < board->cols; j ++) {
            draw(board->data[i][j].id, "  ", board->data[i][j].coord);
        }
    }
}

int main() {
    initInterface();
    initRandom(); 

    // Variables principales
    int rows, cols, user_points = 0, key = c_top; bool game_over = false; Coord init_snake_position;
    
    // Inicializando las variables
    getmaxyx(stdscr, rows, cols);

    cols = cols / 2; // en cada columna se imprime un dibujo de 2 de largo

    Board *board = initBoard(rows, cols);
    Snake *snake_old = (Snake *)malloc(sizeof(Snake)), *snake_current = (Snake *)malloc(sizeof(Snake));


    // Manejo de errores por asignacion de memoria 
    if (board == NULL) {
        closeInterface();
        printf("Error: No se pudo inicializar el tablero.\n");
        return 1;
    }
    if (snake_current == NULL || snake_old == NULL) {
        closeInterface();
        printf("Error: No se pudo inicializar el cuerpo de la serpiente.\n");
        return 1;
    }

    init_snake_position.x = cols-(cols/2);
    init_snake_position.y = rows-(rows/2);
    
    snake_old->head.id        = ID_HEAD;
    snake_old->head.coord     = init_snake_position;
    snake_old->body           = NULL;
    snake_current->head.id    = ID_HEAD;
    snake_current->head.coord = init_snake_position;
    snake_current->body       = NULL;

    generatePoint(board);


    // bucle principal
    do {
        draw_board(board);
        refresh();

        if (kbhit()) {
            int n_key = getch();
            if (n_key == c_top && key != c_bottom) { key = n_key; } else 
            if (n_key == c_bottom && key != c_top) { key = n_key; } else 
            if (n_key == c_left && key != c_right) { key = n_key; } else
            if (n_key == c_right && key != c_left) { key = n_key; } 
        }

        switch (key) {
            case c_top:
                update_snake(board, snake_current, snake_old, top   , &game_over, &user_points);
                break;
            case c_bottom:
                update_snake(board, snake_current, snake_old, bottom, &game_over, &user_points);
                break;
            case c_left:
                update_snake(board, snake_current, snake_old, left  , &game_over, &user_points);
                break;
            case c_right:
                update_snake(board, snake_current, snake_old, right , &game_over, &user_points);
                break;
            default:
                break;
        }
        
        
        sleep_ms(SLEEP_MS);
    } while (!game_over);
    
    // Liberar memoria
    for (int i = 0; i < board->rows; i++) {
        free(board->data[i]);
    }
    struct Snake_Body_Parts *current = snake_old->body;
    while (current != NULL) {
        SBP *temp = current;
        current = current->next;
        free(temp);
    }
    current = snake_current->body;
    while (current != NULL){
        SBP *temp = current;
        current = current->next;
        free(temp);
    }

    closeInterface();
    printMessagesDev(rows, cols, SLEEP_MS, user_points);
    return 0;
}