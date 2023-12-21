#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <signal.h>
#include <string.h>

#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>
#include <poll.h>

#include "defines.h"
#include "colors.h"

// ANSI escape codes for controlling the terminal
#define ESC "\x1B"

#define MAX_X 106
#define MAX_Y 30
#define SPEED 0.02      // Lower is faster

static struct termios old_termios, new_termios;
static bool8 running = TRUE;

typedef enum {
    released = 0,
    pressed,
    hold,
} key_State;

typedef struct {
    char key;
    key_State state;
    struct timeval last_changed;
} Key_Info;

struct screenData {
    int width;
    int height;
    char screen_Pixel[MAX_Y][MAX_X];
    float Z_Buffer[MAX_Y][MAX_X];
};

struct ChPosition {
    float pos_x;
    float pos_y;
    float angle;
};

typedef struct {
    int key;
    int pos_x;
    int pos_y;
    char old_screen[MAX_Y][MAX_X];
    char screen[MAX_Y][MAX_X];
} GameState;

// ============================================================ TERMINAL ============================================================

void set_Terminal_Size(int cols, int rows) {
    printf(ESC "[8;%d;%dt", rows, cols);
}

// reset Terminal to config befor game
void reset_Terminal() {

    printf("\e[m");                             // reset color changes
    printf("\e[?25h");                          // show cursor
    printf("\e[%d;%dH\n", MAX_Y + 3, MAX_X);    // move cursor after game board
    fflush(stdout);
    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
}

// configure Terminal to be able to play game
void configure_Terminal() {

    set_Terminal_Size(MAX_X,MAX_Y);

    tcgetattr(STDIN_FILENO, &old_termios);
	new_termios = old_termios; // save it to be able to reset on exit
    new_termios.c_lflag &= ~(ICANON | ECHO);    // turn off echo + non-canonical mode
    new_termios.c_cc[VMIN] = 0;                 // set input to polling mode
    new_termios.c_cc[VTIME] = 0;                // set input to polling mode
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

    printf("\e[?25l");                          // hide cursor
    atexit(reset_Terminal);

    clear_Screen();
    fflush(stdout);
}


void signal_handler(__attribute__((unused)) int signum) {
    running = FALSE;
}

void render(GameState* state) {


    char screen_Pixel[MAX_Y][MAX_X];

    for (int x = 0; x < MAX_Y; x++) {
        
        printf("");
    }

    fflush(stdout);
}

// ============================================================ INPUT ============================================================

// Check if any key is pressed
int keyboard_hit() {

    struct pollfd fds[1];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    return poll(fds, 1, 0);
}

#define MAX_KEY_RELEASE_TIME 0.5
#define MIN_KEY_HOLD_TIME 0.15

// Read all input from the terminal
read_Input(Key_Info* inInputs[],const int size) {

    bool8 Input_Memory[size];
    for (int16 x = 0; x < size; x++)
        Input_Memory[x] = FALSE;
    
    printf("    pressed: ");
    // read stdin buffer for keys
    char buf;
    while (keyboard_hit()) {
        read(STDIN_FILENO, &buf, 1);
        printf("%c", buf);

        for (int x = 0; x < size; x++) {
            
            if (inInputs[x]->key == buf) {
                
                Input_Memory[x] = TRUE;
                switch (inInputs[x]->state)
                {
                    case released:
                        inInputs[x]->state = pressed;
                        gettimeofday(&inInputs[x]->last_changed, NULL);
                        break;
                    
                    case pressed:
                        inInputs[x]->state = hold;
                        break;

                    default:
                        break;
                }
            }
        }
    }

    // all key that are not pressed 
    printf("\nNot pressed: ");
    for (int x = 0; x < size; x++) {
        
        if (Input_Memory[x] == FALSE) {

            printf("%d", x);
            switch (inInputs[x]->state) {

                case pressed:{

                    struct timeval time_Now;
                    gettimeofday(&time_Now, NULL);
                    double elapsed_time = (time_Now.tv_sec - inInputs[x]->last_changed.tv_sec) +
                                        (time_Now.tv_usec - inInputs[x]->last_changed.tv_usec) / 1.0e6;

                    if (elapsed_time > MAX_KEY_RELEASE_TIME)    // if last changed is older than 0.6
                        inInputs[x]->state = released;
                        
                    break;
                }

                case hold:{

                    struct timeval time_Now;
                    gettimeofday(&time_Now, NULL);
                    double elapsed_time = (time_Now.tv_sec - inInputs[x]->last_changed.tv_sec) +
                                        (time_Now.tv_usec - inInputs[x]->last_changed.tv_usec) / 1.0e6;

                    if (elapsed_time > MIN_KEY_HOLD_TIME)    // if last changed is older than 0.6
                        inInputs[x]->state = released;
                        
                    break;
                }

                default:
                    break;
            }
        }
    }

    printf("\n");
}

// ============================================================ INLINE FUNC ============================================================

// Clear terminal and 
inline void clear_Screen(void) { printf("\e[2J\e[1;1H");}

// ============================================================ MAIN ============================================================

int main() {

    // Setup
    configure_Terminal();
    signal(SIGINT, signal_handler);

   	struct timespec req = {};
	struct timespec rem = {};
    clock_t start, end;

    struct screenData Frame = {};
    struct ChPosition PlayerPos = {};

    Key_Info forward = {'w', released, 0};
    Key_Info backwards = {'s', released, 0};
    Key_Info right = {'d', released, 0};
    Key_Info left = {'a', released, 0};

    Key_Info* Inputs[4] = {
        &forward,
        &backwards,
        &right,
        &left
    };

    char key;
    
    while (running) {

        start = clock();

        clear_Screen();

        printf("\e[1;1H");
        printf("%s%s\x1B[m\n", PIXEL_WHITE, PIXEL_GREEN);
        printf("%s\x1B[m\n\n", PIXEL_GREEN);
    
        read_Input(Inputs, 4);
        printf("Input: %d|%d|%d|%d\n", forward.state, backwards.state, right.state, left.state);
    
        //update(&state);
        //render_Map();

        end = clock();

        double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf(" \n\n FPS: [%3.2f/%3.2f]\n",1/SPEED, 1/time_taken);
        
        if (time_taken > SPEED)
            continue;

        req.tv_sec = 0;
        req.tv_nsec = (SPEED - time_taken) * 1000000000; // 0.1 seconds
        printf(" sleep time: [%3.2f]\n", ((SPEED - time_taken) * 1000000000));
        fflush(stdin);
        nanosleep(&req, &rem);

    }
}
