#pragma once

enum vga_color {
    BLACK,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    GREY,
    DARK_GREY,
    BRIGHT_BLUE,
    BRIGHT_GREEN,
    BRIGHT_CYAN,
    BRIGHT_RED,
    BRIGHT_MAGENTA,
    YELLOW,
    WHITE,
};


//***********************************************8

extern unsigned char keyboard_map[128];
extern void test_func(void);
extern void set_cursor(int);

/* current cursor location */
uint32_t current_loc = 0;
/* video memory begins at address 0xb8000 */
char *vidptr = (char*)0xb8000;



extern void kprint(const char *str);

struct func_in_menu {
	int row;
    int deep;
    void (*func)(void);
};
//struct func_in_menu menu[2];



struct interrupt_frame;



__attribute__((interrupt)) void MMouseHandler(struct interrupt_frame* frame);
__attribute__((interrupt)) void MKeyHandler(struct interrupt_frame* frame);