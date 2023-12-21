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

#define SCREEN_X 105
#define SCREEN_Y 59

#define TARGET_FPS 30.0f      // Lower is faster

static struct termios old_termios, new_termios;
static bool8 running = TRUE;

// ----------------------------------- Input -----------------------------------

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

// ----------------------------------- Display -----------------------------------

struct Frame_Data {
    char Frame[SCREEN_Y][COLOR_LENGTH * SCREEN_X +1];
    int ColorCodePosition[SCREEN_Y][SCREEN_X];
    float Z_Buffer[SCREEN_Y][SCREEN_X];
};

// ----------------------------------- Game -----------------------------------

struct ChPosition {
    float pos_x;
    float pos_y;
    float angle;
};

struct MapData {
    char WallData[16][16];
};


// ============================================================ INLINE FUNC ============================================================

// Clear terminal and 
void clear_Screen() {
    printf("\x1B[2J\x1B[1;1H");
}

void Set_Pixel_Color(struct Frame_Data* Frame, int X, int Y, const char* Color) {

    strncpy(Frame->Frame[Y] + ((COLOR_LENGTH-1) * X), Color, (COLOR_LENGTH-1));
}

// ============================================================ TERMINAL ============================================================

// reset Terminal to config befor game
void reset_Terminal() {

    printf("\e[m");                             // reset color changes
    printf("\e[?25h");                          // show cursor
    printf("\e[1;1H\n");    // move cursor after game board
    fflush(stdout);
    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
}

// configure Terminal to be able to play game
void configure_Terminal() {

    printf(ESC "[8;%d;%dt", SCREEN_Y, SCREEN_X*2);        // set_Terminal_Size

    tcgetattr(STDIN_FILENO, &old_termios);
	new_termios = old_termios;                  // save it to be able to reset on exit
    new_termios.c_lflag &= ~(ICANON | ECHO);    // turn off echo + non-canonical mode
    new_termios.c_cc[VMIN] = 0;                 // set input to polling mode
    new_termios.c_cc[VTIME] = 0;                // set input to polling mode
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

    printf("\e[?25l");                          // hide cursor
    atexit(reset_Terminal);


    printf("\x1B[2J\x1B[1;1H");                 // clear screen
    fflush(stdout);
}

// ============================================================ INPUT ============================================================

// handle Shutdown signal
void signal_handler(__attribute__((unused)) int signum) {
    running = FALSE;
}

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
void read_Input(Key_Info* inInputs[],const int size) {

    bool8 Input_Memory[size];
    for (int16 x = 0; x < size; x++)
        Input_Memory[x] = FALSE;
    
    // read stdin buffer for keys
    char buf;
    while (keyboard_hit()) {
        read(STDIN_FILENO, &buf, 1);

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
    for (int x = 0; x < size; x++) {
        
        if (Input_Memory[x] == FALSE) {

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
}

// ============================================================ MAIN ============================================================

int main() {

    // Seed the random number generator with the current time
    srand(time(NULL));

    // Setup
    configure_Terminal();
    signal(SIGINT, signal_handler);

    // calc FPS
   	struct timespec req = {};
	struct timespec rem = {};
    clock_t start, end;
    bool8 InTarget_FPS;

    // PLayer Info
    struct ChPosition PlayerPos = {};

    // Input Info
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
    
    // map
    struct MapData map2 = {
        "################",
        "#              #",
        "#              #",
        "#              #",
        "#              #",
        "#              #",
        "#              #",
        "#              #",
        "#              #",
        "#              #",
        "#              #",
        "#              #",
        "#              #",
        "#              #",
        "#              #",
        "################"
    };

    struct Frame_Data CurrentFrame;

    while (running) {

        start = clock();

        clear_Screen();        
        memset(&CurrentFrame, 0, sizeof(CurrentFrame));

        // do 48 times
        for (int16 x = 0; x < SCREEN_Y; x++) {
            for (int y = 0; y < SCREEN_X; y++) {
                
                strcat(CurrentFrame.Frame[x], PIXEL_GRAY_40);
            }
            strcat(CurrentFrame.Frame[x], "\0");
        }
        
        Set_Pixel_Color(&CurrentFrame, 2,5, PIXEL_RED_140);
        // Use strncpy to replace the substring
        size_t replacePosition = (COLOR_LENGTH-1) * 2;
        strncpy(CurrentFrame.Frame[1] + replacePosition, PIXEL_WHITE, COLOR_LENGTH-1);

        //CurrentFrame.Frame[1][0] = PIXEL_WHITE;

        /*
        // do 48 times
        for (int16 x = 0; x < SCREEN_Y; x++) {
            for (int y = 0; y < SCREEN_X; y++) {
                
                strcat(CurrentFrame.Frame[x], Get_Random_Gray_Pixel());
            }
            strcat(CurrentFrame.Frame[x], "\0");
            printf("%s\x1B[m\n", CurrentFrame.Frame[x]);
        }*/

        // render Frame
        for (int16 x = 0; x < SCREEN_Y; x++) {
            printf("%s\x1B[m\n", CurrentFrame.Frame[x]);
        }
        
            
    
        //update(&state);
        //render_Map();

        end = clock();
        double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
        InTarget_FPS = (time_taken > 1 / TARGET_FPS);

        read_Input(Inputs, 4);
        printf("Input:   %s\x1B[m            FPS: [%3.2f/%3.2f]\n       %s%s%s\x1B[m\n",
            (forward.state? PIXEL_GREEN : PIXEL_WHITE),
            TARGET_FPS, 1/time_taken,
            (left.state? PIXEL_GREEN : PIXEL_WHITE),
            (backwards.state? PIXEL_GREEN : PIXEL_WHITE),
            (right.state? PIXEL_GREEN : PIXEL_WHITE));
        fflush(stdin);

        // Force FPS to TARGET_FPS
        if (InTarget_FPS)
            continue;

        req.tv_sec = 0;
        req.tv_nsec = (1 / TARGET_FPS - time_taken) * 1000000000; // 0.1 seconds
        nanosleep(&req, &rem);
    }
    
    clear_Screen();
}
