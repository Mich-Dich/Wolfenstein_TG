#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <signal.h>
#include <string.h>

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define MAX_X 106
#define MAX_Y 30

// ANSI escape codes for controlling the terminal
#define ESC "\x1B"

void setConsoleSize(int cols, int rows) {
    printf(ESC "[8;%d;%dt", rows, cols);
}

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

// Create a char array to store the Unicode character
char FullBlockChar[4] = "\u2591"; // UTF-8 encoding for U+2591 (░)

typedef struct {
    int key;
    int pos_x;
    int pos_y;
    char old_screen[MAX_Y][MAX_X];
    char screen[MAX_Y][MAX_X];
} GameState;

static struct termios old_termios, new_termios;

// reset Terminal to config befor game
void reset_terminal() {

    printf("\e[m");                             // reset color changes
    printf("\e[?25h");                          // show cursor
    printf("\e[%d;%dH\n", MAX_Y + 3, MAX_X);    // move cursor after game board
    fflush(stdout);
    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
}

// configure Terminal to be able to play game
void configure_Terminal() {

    setConsoleSize(MAX_X,MAX_Y);

    tcgetattr(STDIN_FILENO, &old_termios);
	new_termios = old_termios; // save it to be able to reset on exit

    new_termios.c_lflag &= ~(ICANON | ECHO);    // turn off echo + non-canonical mode
    new_termios.c_cc[VMIN] = 0;                 // set input to polling mode
    new_termios.c_cc[VTIME] = 0;                // set input to polling mode
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

    printf("\e[?25l");                          // hide cursor
    atexit(reset_terminal);
}

static int exit_loop;

void signal_handler(__attribute__((unused)) int signum) {
    exit_loop = 1;
}

int read_key(char* buf, int k) {
    if (buf[k] == '\033' && buf[k + 1] == '[') {
		switch (buf[k + 2]) {
			case 'A': return 1; // UP
			case 'B': return 2; // DOWN
			case 'C': return 3; // RIGHT
			case 'D': return 4; // LEFT
		}
	}
	return 0;
}

void read_input(GameState* state) {

   	char buf[4096]; // maximum input buffer
	int n = read(STDIN_FILENO, buf, sizeof(buf));
	int final_key = 0;
    // it's okay if we miss some keys
    // we will correct it on next frame
	for (int k = 0; k <= n - 3; k += 3) {

		int key = read_key(buf, k);
		if (key == 0) continue;
		final_key = key;
	
    }
    state->key = final_key;
}

void handle_player(GameState* state) {
	switch (state->key) {
	case 1:
        if (state->pos_y > 1) {
            state->screen[state->pos_y][state->pos_x] = ' ';
            state->screen[state->pos_y - 1][state->pos_x] = '@';
            --state->pos_y;
        }
        break;
 	case 2:
        if (state->pos_y < MAX_Y - 2) {
            state->screen[state->pos_y][state->pos_x] = ' ';
            state->screen[state->pos_y + 1][state->pos_x] = '@';
            ++state->pos_y;
        }
		break;
	case 3:
        if (state->pos_x < MAX_X - 3) {
            state->screen[state->pos_y][state->pos_x] = ' ';
            state->screen[state->pos_y][state->pos_x + 1] = '@';
            ++state->pos_x;
        }
		break;
	case 4:
        if (state->pos_x > 1) {
            state->screen[state->pos_y][state->pos_x] = ' ';
            state->screen[state->pos_y][state->pos_x - 1] = '@';
            --state->pos_x;
        }
		break;
	default:
		break;
	}
}

void update(GameState* state) {
    memcpy(state->screen, state->old_screen, sizeof(state->screen));
    handle_player(state);
}

void render(GameState* state) {
    for (int j = 0; j < MAX_Y; ++j) {
        for (int i = 0; i < MAX_X; ++i) {
            if (state->old_screen[j][i] != state->screen[j][i]) {
                printf("\e[%d;%dH", j + 1, i + 1); // move cursor
                printf("%c", state->screen[j][i]);
            }
        }
    }
    fflush(stdout);
}


inline void Clear_Screen(void) { printf("\e[2J"); }

// Lower is faster
#define SPEED 0.1

int main() {


    // Setup
    configure_Terminal();
    signal(SIGINT, signal_handler);
   	struct timespec req = {};
	struct timespec rem = {};

    struct screenData Frame = {};
    struct ChPosition PlayerPos = {};

    while (1) {


    }

    Clear_Screen();

    clock_t start, end;

    while (!exit_loop) {
        start = clock();

        read_input(&state);
        update(&state);
        render(&state);

        end = clock();

        double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
        if (time_taken > SPEED) continue;

        req.tv_sec = 0;
        req.tv_nsec = (SPEED - time_taken) * 1000000000; // 0.1 seconds
        nanosleep(&req, &rem);

    }


}



























