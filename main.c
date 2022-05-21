#include "keyboard_map.h"

#include "utils.h"
#include "main.h"
#include "consts.h"

/* there are 25 lines each of 80 columns; each element takes 2 bytes */
#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01
/* there are 25 lines each of 80 columns; each element takes 2 bytes */





void kprint(const char *str)
{
	unsigned int i = 0;
	while (str[i] != '\0') {
		vidptr[current_loc++] = str[i++];
		vidptr[current_loc++] = 0x02;
	}
	set_cursor(current_loc);
}


void kprintch(char str)
{
		vidptr[current_loc++] = str;
		vidptr[current_loc++] = 0x02;
	set_cursor(current_loc);
}

void kprint_newline(void)
{
	unsigned int line_size = BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE;
	current_loc = current_loc + (line_size - current_loc % (line_size));
}

void clear_screen(void)
{
	unsigned int i = 0;
	while (i < SCREENSIZE) {
		vidptr[i++] = ' ';
		vidptr[i++] = 0x07;
	}
	current_loc = 0;
	set_cursor(current_loc);
}
void set_cursor(int offset) {
    offset /= 2; // Covert from cell offset to char offset.

    // This is similar to get_cursor, only now we write
    // bytes to those internal device registers.
    outb(REG_SCREEN_CTRL, 14);
    outb(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
    outb(REG_SCREEN_CTRL, 15);
    outb(REG_SCREEN_DATA, offset);
}
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************

void main()
{
    const char *str = "my first kernel";
    char *vidptr = (char*)0xb8000;
    unsigned int i = 0;
    unsigned int j = 0;

    while(j < 80 * 25 * 2) {
        vidptr[j] = ' ';
        vidptr[j+1] = 0x07;         
        j = j + 2;
    }

    j = 0;

    while(str[j] != '\0') {
        vidptr[i] = str[j];
        vidptr[i+1] = 0x02;
        ++j;
        i = i + 2;
    }
    
	kprint_newline();
	kprint_newline();


	while(1)__asm__("hlt\n\t");
    return;
} 
