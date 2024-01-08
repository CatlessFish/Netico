#include "econsole.h"

Console_t console;

static void debuginfo() {
    printf("Console params:\r\n");
    printf("W=%d, H=%d, X=%d, Y=%d\r\n", console.Width, console.Height, console.Xpos, console.Ypos);
    printf("mC=%d, mR=%d, hd=%d, tl=%d\r\n", console.maxCol, console.maxRow, console.rHead, console.rTail);
    printf("Console Buffer: \r\n");
    for (int r = console.rHead; r != console.rTail; r = NEXTROW(r)) {
        printf("%s ## %d\r\n", console.buf[r], console.row_consume[r]);
    }
}

static void dumpimg() {
    if (!console.Image) return;
    printf("IMAGE: -----\r\n");
    for (int i = 0; i < IMGSZ; i++) {
        printf("%02x", console.Image[i]);
    }
    printf("-----IMAGE END-----\r\n");
}

int console_init(UWORD width, UWORD height, UWORD xpos, UWORD ypos, sFONT *font) {
    if (width <= 0 || height <= 0) {
        return -1;
    }
    // Rotate 90, height <-> width
    if (xpos + width > EPD_2in13_V3_HEIGHT || ypos + height > EPD_2in13_V3_WIDTH) {
        return -1;
    }
    console.Image = (UBYTE *)malloc(IMGSZ);
    if (!console.Image) {
        return -1;
    }
    Paint_NewImage(console.Image, EPD_2in13_V3_WIDTH, EPD_2in13_V3_HEIGHT, ROTATE_90, WHITE);
    Paint_SelectImage(console.Image);
    Paint_Clear(WHITE);

    console.Width = width;
    console.Height = height;
    console.Xpos = xpos;
    console.Ypos = ypos;
    console.Font = font;
    memset(console.buf, 0, sizeof(console.buf));
    console.maxCol = width / font->Width;
    console.maxRow = height / font->Height;
    console.rHead = console.rTail = 0;
    return 0;
}

void console_display() {
    Paint_SelectImage(console.Image);
    int r_avail = console.maxRow;
    for (int r = console.rHead; r != console.rTail; r = NEXTROW(r)) {
        // Update rHead and rTail for proper display
        r_avail -= console.row_consume[r];
        while (r_avail < 0) {
            // Flush a message at rHead
            r_avail += console.row_consume[console.rHead];
            console.rHead = NEXTROW(console.rHead);
        }
    }
    // debuginfo();
    // Now print message between [Head, tail)
    Paint_Clear(WHITE);
    char tmp[64];
    int row_no = 0;
    for (int r = console.rHead; r != console.rTail; r = NEXTROW(r)) {
        int pos = 0;
        while (pos < strlen(console.buf[r])) {
            // print maxcol characters
            strncpy(tmp, &console.buf[r][pos], console.maxCol);
            tmp[console.maxCol] = '\0';
            Paint_DrawString_EN(console.Xpos, ROW2YPOS(console.Ypos, row_no, console.Font->Height), 
                tmp, console.Font, BLACK, WHITE);
            pos += console.maxCol;
            row_no++;
        }
    }
    // dumpimg();
    EPD_2in13_V3_Display(console.Image);
}

int console_push(char *msg) {
    if (strlen(msg) >= CONSOLE_MAX_WIDTH) return -1;
    // Update buffer
    strcpy(console.buf[console.rTail], msg);
    // Update row_consume
    console.row_consume[console.rTail] = strlen(msg) / console.maxCol;
    if ((strlen(msg) % console.maxCol) != 0) console.row_consume[console.rTail]++;
    // Update tail and head
    console.rTail = NEXTROW(console.rTail);
    if (console.rHead == console.rTail) console.rHead = NEXTROW(console.rHead);
    // Display
    console_display();
    return 0;
}

void console_clear() {
    console.rHead = console.rTail = 0;
    Paint_SelectImage(console.Image);
    Paint_Clear(WHITE);
    EPD_2in13_V3_Display(console.Image);
}

void console_destruct() {
    if (console.Image) free(console.Image);
    console.Image = 0;
    memset(&console, 0, sizeof(console));
}