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
#include <math.h>

#include "defines.h"
#include "colors.h"

#define SCREEN_X 105
#define SCREEN_Y 59

#define TARGET_FPS 30.0f      // Lower is faster
#define MAX_KEY_RELEASE_TIME 0.5
#define MIN_KEY_HOLD_TIME 0.15

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
    float Z_Buffer[SCREEN_Y][SCREEN_X];
};

// ----------------------------------- Game -----------------------------------

struct ChPosition {
    float pos_x;
    float pos_y;
    float angle;
};

#define MAP_WIDTH 16
#define MAP_HEIGHT 16

struct MapData {
    char WallData[MAP_WIDTH][MAP_HEIGHT];
};

typedef enum {
	Nearest = 0,
	DepthLevel_1,
	DepthLevel_2,
	DepthLevel_3,
	DepthLevel_4,
	Furthest,
} PixelShadeLevel;

// ============================================================ INLINE FUNC ============================================================

// Clear terminal and 
static inline void clear_Screen()      { printf("\x1B[2J\x1B[1;1H"); }

static inline void Set_Pixel_Color(struct Frame_Data* Frame, int X, int Y, const char* Color)       { strncpy(Frame->Frame[Y] + ((COLOR_LENGTH-1) * X), Color, (COLOR_LENGTH-1)); }

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

    printf("\1xB[8;%d;%dt", SCREEN_Y, SCREEN_X*2);        // set_Terminal_Size

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

float g_FOV = 3.14159f / 4.0f;
float g_MaxRayCastDepth = 16.0f;

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
    struct ChPosition PlayerPos = { 2.0f, 2.0f, 0.0f };

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
    char map[MAP_WIDTH * MAP_HEIGHT];
    strcat(map, "################");
    strcat(map, "#              #");
    strcat(map, "#              #");
    strcat(map, "#              #");
    strcat(map, "#              #");
    strcat(map, "#              #");
    strcat(map, "#              #");
    strcat(map, "#              #");
    strcat(map, "#              #");
    strcat(map, "#              #");
    strcat(map, "#              #");
    strcat(map, "#              #");
    strcat(map, "#              #");
    strcat(map, "#              #");
    strcat(map, "#              #");
    strcat(map, "################");

    struct Frame_Data CurrentFrame;

    while (running) {

        start = clock();

        clear_Screen();

        // Print help
        read_Input(Inputs, 4);


        if (right.state != released) {
            PlayerPos.angle += 0.05f;
        }

        if (left.state != released) {
            PlayerPos.angle -= 0.05f;
        }
        

        // reset all pixel to black
        memset(&CurrentFrame, 0, sizeof(CurrentFrame));
        for (int16 x = 0; x < SCREEN_Y; x++) {
            for (int y = 0; y < SCREEN_X; y++) {
                
                strcat(CurrentFrame.Frame[x], PIXEL_BLACK);
            }
            strcat(CurrentFrame.Frame[x], "\0");
        }
        
        Set_Pixel_Color(&CurrentFrame, 2,5, PIXEL_RED_140);




        // Ray Tracer
        for (int16 x = 0; x < SCREEN_X; x++) {

            // calculate projected ray angle into world space
            float RayAngle = (PlayerPos.angle - g_FOV / 2.0f) + ((float)x / (float)SCREEN_X) * g_FOV;

            // UnitVector of Player Eye
            float LookDirX = sin(RayAngle);
            float LookDirY = cos(RayAngle);

            // OPTIMIZE RayTracer
            float DistanceToWall = 0;
            bool8 HitWall = FALSE;
            bool8 Boundary = FALSE;		// Set when ray hits boundary between two wall blocks

            // Incrementally cast ray from player, along ray angle, testing for 
            // intersection with a block
            while (!HitWall && DistanceToWall < g_MaxRayCastDepth) {
            
                DistanceToWall += 0.1f;
                int nTestX = (int)(PlayerPos.pos_x + LookDirX * DistanceToWall);
                int nTestY = (int)(PlayerPos.pos_y + LookDirY * DistanceToWall);

                // Test if ray is out of bounds
                if (nTestX < 0 || nTestX >= MAP_WIDTH || nTestY < 0 || nTestY >= MAP_HEIGHT) {
                    HitWall = TRUE;			// Just set distance to maximum depth
                    DistanceToWall = g_MaxRayCastDepth;
                }

                else {
                    // Ray is inbounds so test to see if the ray cell is a wall block
                    if (map[nTestX * MAP_WIDTH + nTestY] == '#') {

                        // Ray has hit wall
                        HitWall = TRUE;
                        /*
                        // To highlight tile boundaries, cast a ray from each corner
                        // of the tile, to the player. The more coincident this ray
                        // is to the rendering ray, the closer we are to a tile
                        // boundary, which we'll shade to add detail to the walls
                        std::vector<std::pair<float, float>> Block_Corner;

                        // Test each corner of hit tile, storing the distance from
                        // the player, and the calculated dot product of the two rays
                        for (int tx = 0; tx < 2; tx++)
                            for (int ty = 0; ty < 2; ty++) {

                                // Angle of corner to eye
                                float vx = (float)nTestX + tx - PlayerPos.posX;
                                float vy = (float)nTestY + ty - PlayerPos.posY;
                                float d = sqrt(vx * vx + vy * vy);
                                float dot = (LookDirX * vx / d) + (LookDirY * vy / d);
                                Block_Corner.push_back(std::make_pair(d, dot));
                            }

                        // Sort Pairs from closest to farthest
                        std::sort(Block_Corner.begin(), Block_Corner.end(),
                            [](const std::pair<float, float>& left, const std::pair<float, float>& right) {return left.first < right.first; }
                        );

                        // First two/three are closest (we will never see all four)
                        float fBound = 0.005;
                        if (acos(Block_Corner.at(0).second) < fBound) Boundary = true;
                        if (acos(Block_Corner.at(1).second) < fBound) Boundary = true;
                        //if (acos(Block_Corner.at(2).second) < fBound) Boundary = true;*/
                    }
                }
            }

            int Ceiling = ((float)SCREEN_Y / 2.0) - SCREEN_Y / ((float)(DistanceToWall));
            int Floor = SCREEN_Y - Ceiling;

            short Shade = ' ';
            PixelShadeLevel PS_Level;

            for (int y = 0; y < SCREEN_Y; y++) {

                // Draw Ceiling
                if (y < Ceiling)
                    Set_Pixel_Color(&CurrentFrame, x, y, PIXEL_BLACK);

                // Draw Wall
                else if (y > Ceiling && y <= Floor) {

                    int shade_Brightness = (int)DistanceToWall;
                    switch (shade_Brightness) {
                        case 1:     Set_Pixel_Color(&CurrentFrame, x, y, PIXEL_GRAY_240); break;
                        case 2:     Set_Pixel_Color(&CurrentFrame, x, y, PIXEL_GRAY_220); break;
                        case 3:     Set_Pixel_Color(&CurrentFrame, x, y, PIXEL_GRAY_200); break;
                        case 4:     Set_Pixel_Color(&CurrentFrame, x, y, PIXEL_GRAY_180); break;
                        case 5:     Set_Pixel_Color(&CurrentFrame, x, y, PIXEL_GRAY_160); break;
                        case 6:     Set_Pixel_Color(&CurrentFrame, x, y, PIXEL_GRAY_140); break;
                        case 7:     Set_Pixel_Color(&CurrentFrame, x, y, PIXEL_GRAY_120); break;
                        case 8:     Set_Pixel_Color(&CurrentFrame, x, y, PIXEL_GRAY_100); break;
                        case 9:     Set_Pixel_Color(&CurrentFrame, x, y, PIXEL_GRAY_80); break;
                        case 10:    Set_Pixel_Color(&CurrentFrame, x, y, PIXEL_GRAY_60); break;
                        case 11:    Set_Pixel_Color(&CurrentFrame, x, y, PIXEL_GRAY_40); break;
                        default:    Set_Pixel_Color(&CurrentFrame, x, y, PIXEL_GRAY_20); break;
                    }
                }

                // Draw Floor
                else {
                    float b = 1.0f - (((float)y - SCREEN_Y / 2.0f) / (float)SCREEN_Y / 2.0f);
                    if (b < 0.25)		Set_Pixel_Color(&CurrentFrame, x, y, PIXEL_BLACK);
                    else if (b < 0.5)	Set_Pixel_Color(&CurrentFrame, x, y, PIXEL_GRAY_220);
                    else if (b < 0.75)	Set_Pixel_Color(&CurrentFrame, x, y, PIXEL_GRAY_180);
                    else if (b < 0.9)	Set_Pixel_Color(&CurrentFrame, x, y, PIXEL_GRAY_140);
                    else				Set_Pixel_Color(&CurrentFrame, x, y, PIXEL_GRAY_100);

                    
                }
            }

        }









        // render Frame
        for (int16 x = 0; x < SCREEN_Y; x++) {
            printf("%s\x1B[m\n", CurrentFrame.Frame[x]);
        }

        // Calc FPS
        end = clock();
        double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
        InTarget_FPS = (time_taken > 1 / TARGET_FPS);

        printf("Input:   %s\x1B[m            FPS: [%3.2f/%3.2f]\n       %s%s%s\x1B[m\n",
            (forward.state? PIXEL_GREEN : PIXEL_WHITE),
            TARGET_FPS, 1/time_taken,
            (left.state? PIXEL_GREEN : PIXEL_WHITE),
            (backwards.state? PIXEL_GREEN : PIXEL_WHITE),
            (right.state? PIXEL_GREEN : PIXEL_WHITE));
        fflush(stdin);

        // Force FPS <= TARGET_FPS
        if (InTarget_FPS)
            continue;

        req.tv_sec = 0;
        req.tv_nsec = (1 / TARGET_FPS - time_taken) * 1000000000; // 0.1 seconds
        nanosleep(&req, &rem);
    }
    
    clear_Screen();
}
