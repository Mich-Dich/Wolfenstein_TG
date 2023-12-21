#pragma once

#define COLOR_LENGTH 22

// defines for shades of gray
#define PIXEL_BLACK         "\x1B[48;2;000;000;000m  "
#define PIXEL_GRAY_20       "\x1B[48;2;020;020;020m  "
#define PIXEL_GRAY_40       "\x1B[48;2;040;040;040m  "
#define PIXEL_GRAY_60       "\x1B[48;2;060;060;060m  "
#define PIXEL_GRAY_80       "\x1B[48;2;080;080;080m  "
#define PIXEL_GRAY_100      "\x1B[48;2;100;100;100m  "
#define PIXEL_GRAY_120      "\x1B[48;2;120;120;120m  "
#define PIXEL_GRAY_140      "\x1B[48;2;140;140;140m  "
#define PIXEL_GRAY_160      "\x1B[48;2;160;160;160m  "
#define PIXEL_GRAY_180      "\x1B[48;2;180;180;180m  "
#define PIXEL_GRAY_200      "\x1B[48;2;200;200;200m  "
#define PIXEL_GRAY_220      "\x1B[48;2;220;220;220m  "
#define PIXEL_GRAY_240      "\x1B[48;2;240;240;240m  "
#define PIXEL_WHITE         "\x1B[48;2;255;255;255m  "

// Generate a random gray variant
const char* Get_Random_Gray_Pixel() {
    
    int randomIndex = rand() % 12; // Assumes 12 defines
    switch (randomIndex) {
        case 0: return PIXEL_GRAY_20; 
        case 1: return PIXEL_GRAY_40;
        case 2: return PIXEL_GRAY_60;
        case 3: return PIXEL_GRAY_80;
        case 4: return PIXEL_GRAY_100;
        case 5: return PIXEL_GRAY_120;
        case 6: return PIXEL_GRAY_140;
        case 7: return PIXEL_GRAY_160;
        case 8: return PIXEL_GRAY_180;
        case 9: return PIXEL_GRAY_200;
        case 10: return PIXEL_GRAY_220;
        case 11: return PIXEL_GRAY_240;
        default: return PIXEL_GRAY_240; // Default to the last define
    }
}

// defines for shades of red
#define PIXEL_RED_20        "\x1B[48;2;020;000;000m  "
#define PIXEL_RED_40        "\x1B[48;2;040;000;000m  "
#define PIXEL_RED_60        "\x1B[48;2;060;000;000m  "
#define PIXEL_RED_80        "\x1B[48;2;080;000;000m  "
#define PIXEL_RED_100       "\x1B[48;2;100;000;000m  "
#define PIXEL_RED_120       "\x1B[48;2;120;000;000m  "
#define PIXEL_RED_140       "\x1B[48;2;140;000;000m  "
#define PIXEL_RED_160       "\x1B[48;2;160;000;000m  "
#define PIXEL_RED_180       "\x1B[48;2;180;000;000m  "
#define PIXEL_RED_200       "\x1B[48;2;200;000;000m  "
#define PIXEL_RED_220       "\x1B[48;2;220;000;000m  "
#define PIXEL_RED_240       "\x1B[48;2;240;000;000m  "
#define PIXEL_RED           "\x1B[48;2;255;000;000m  "

// defines for shades of green
#define PIXEL_GREEN_20      "\x1B[48;2;000;020;000m  "
#define PIXEL_GREEN_40      "\x1B[48;2;000;040;000m  "
#define PIXEL_GREEN_60      "\x1B[48;2;000;060;000m  "
#define PIXEL_GREEN_80      "\x1B[48;2;000;080;000m  "
#define PIXEL_GREEN_100     "\x1B[48;2;000;100;000m  "
#define PIXEL_GREEN_120     "\x1B[48;2;000;120;000m  "
#define PIXEL_GREEN_140     "\x1B[48;2;000;140;000m  "
#define PIXEL_GREEN_160     "\x1B[48;2;000;160;000m  "
#define PIXEL_GREEN_180     "\x1B[48;2;000;180;000m  "
#define PIXEL_GREEN_200     "\x1B[48;2;000;200;000m  "
#define PIXEL_GREEN_220     "\x1B[48;2;000;220;000m  "
#define PIXEL_GREEN_240     "\x1B[48;2;000;240;000m  "
#define PIXEL_GREEN         "\x1B[48;2;000;255;000m  "

// defines for shades of blue
#define PIXEL_BLUE_20       "\x1B[48;2;000;000;020m  "
#define PIXEL_BLUE_40       "\x1B[48;2;000;000;040m  "
#define PIXEL_BLUE_60       "\x1B[48;2;000;000;060m  "
#define PIXEL_BLUE_80       "\x1B[48;2;000;000;080m  "
#define PIXEL_BLUE_100      "\x1B[48;2;000;000;100m  "
#define PIXEL_BLUE_120      "\x1B[48;2;000;000;120m  "
#define PIXEL_BLUE_140      "\x1B[48;2;000;000;140m  "
#define PIXEL_BLUE_160      "\x1B[48;2;000;000;160m  "
#define PIXEL_BLUE_180      "\x1B[48;2;000;000;180m  "
#define PIXEL_BLUE_200      "\x1B[48;2;000;000;200m  "
#define PIXEL_BLUE_220      "\x1B[48;2;000;000;220m  "
#define PIXEL_BLUE_240      "\x1B[48;2;000;000;240m  "
#define PIXEL_BLUE          "\x1B[48;2;000;000;255m  "

// defines for shades of yellow
#define PIXEL_YELLOW_20     "\x1B[48;2;020;020;000m  "
#define PIXEL_YELLOW_40     "\x1B[48;2;040;040;000m  "
#define PIXEL_YELLOW_60     "\x1B[48;2;060;060;000m  "
#define PIXEL_YELLOW_80     "\x1B[48;2;080;080;000m  "
#define PIXEL_YELLOW_100    "\x1B[48;2;100;100;000m  "
#define PIXEL_YELLOW_120    "\x1B[48;2;120;120;000m  "
#define PIXEL_YELLOW_140    "\x1B[48;2;140;140;000m  "
#define PIXEL_YELLOW_160    "\x1B[48;2;160;160;000m  "
#define PIXEL_YELLOW_180    "\x1B[48;2;180;180;000m  "
#define PIXEL_YELLOW_200    "\x1B[48;2;200;200;000m  "
#define PIXEL_YELLOW_220    "\x1B[48;2;220;220;000m  "
#define PIXEL_YELLOW_240    "\x1B[48;2;240;240;000m  "
#define PIXEL_YELLOW        "\x1B[48;2;255;255;000m  "

// defines for shades of magenta
#define PIXEL_MAGENTA_20    "\x1B[48;2;020;000;020m  "
#define PIXEL_MAGENTA_40    "\x1B[48;2;040;000;040m  "
#define PIXEL_MAGENTA_60    "\x1B[48;2;060;000;060m  "
#define PIXEL_MAGENTA_80    "\x1B[48;2;080;000;080m  "
#define PIXEL_MAGENTA_100   "\x1B[48;2;100;000;100m  "
#define PIXEL_MAGENTA_120   "\x1B[48;2;120;000;120m  "
#define PIXEL_MAGENTA_140   "\x1B[48;2;140;000;140m  "
#define PIXEL_MAGENTA_160   "\x1B[48;2;160;000;160m  "
#define PIXEL_MAGENTA_180   "\x1B[48;2;180;000;180m  "
#define PIXEL_MAGENTA_200   "\x1B[48;2;200;000;200m  "
#define PIXEL_MAGENTA_220   "\x1B[48;2;220;000;220m  "
#define PIXEL_MAGENTA_240   "\x1B[48;2;240;000;240m  "
#define PIXEL_MAGENTA       "\x1B[48;2;255;000;255m  "

// defines for shades of cyan
#define PIXEL_CYAN_20       "\x1B[48;2;000;020;020m  "
#define PIXEL_CYAN_40       "\x1B[48;2;000;040;040m  "
#define PIXEL_CYAN_60       "\x1B[48;2;000;060;060m  "
#define PIXEL_CYAN_80       "\x1B[48;2;000;080;080m  "
#define PIXEL_CYAN_100      "\x1B[48;2;000;100;100m  "
#define PIXEL_CYAN_120      "\x1B[48;2;000;120;120m  "
#define PIXEL_CYAN_140      "\x1B[48;2;000;140;140m  "
#define PIXEL_CYAN_160      "\x1B[48;2;000;160;160m  "
#define PIXEL_CYAN_180      "\x1B[48;2;000;180;180m  "
#define PIXEL_CYAN_200      "\x1B[48;2;000;200;200m  "
#define PIXEL_CYAN_220      "\x1B[48;2;000;220;220m  "
#define PIXEL_CYAN_240      "\x1B[48;2;000;240;240m  "
#define PIXEL_CYAN          "\x1B[48;2;000;255;255m  "
