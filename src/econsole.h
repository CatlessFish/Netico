#include <string.h>
#include <stdlib.h>

#include "GUI_Paint.h"
#include "EPD_2in13_V3.h"

#define CONSOLE_MAX_WIDTH 300
#define CONSOLE_MAX_HEIGHT 30

#define NEXTROW(r) ((r + 1) % CONSOLE_MAX_HEIGHT)
#define PREVROW(r) ((r - 1 + CONSOLE_MAX_HEIGHT) % CONSOLE_MAX_HEIGHT)

#define ROW2YPOS(base, row, rh) (base + row * rh)
#define COL2XPOS(base, col, rw) (base + col * rw)

#define IMGSZ   (((EPD_2in13_V3_WIDTH % 8 == 0) ?   \
                      (EPD_2in13_V3_WIDTH / 8 )     \
                    : (EPD_2in13_V3_WIDTH / 8 + 1)) * EPD_2in13_V3_HEIGHT)

typedef struct {
    UBYTE *Image;
    UWORD Width, Height;
    UWORD Xpos, Ypos;
    sFONT *Font;
    char buf[CONSOLE_MAX_HEIGHT][CONSOLE_MAX_WIDTH];
    UBYTE row_consume[CONSOLE_MAX_HEIGHT]; // Keep track of how many rows a string needs
    UWORD maxCol, maxRow;
    UWORD rHead, rTail;
} Console_t;

extern Console_t console;

int console_init(UWORD width, UWORD height, UWORD xpos, UWORD ypos, sFONT *font);
// void console_display();
int console_push(char *msg);
void console_clear();
void console_destruct();