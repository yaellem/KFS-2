#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
 
/* Check if the compiler thinks you are targeting the wrong operating system. */
//#if defined(__linux__)
//#error "You are not using a cross-compiler, you will most certainly run into trouble"
//#endif


/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

void set_cursor(int offset);
int get_cursor();

/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

#define VGA_CTRL_REGISTER 	0x3d4
#define VGA_DATA_REGISTER 	0x3d5
#define VGA_OFFSET_LOW 		0x0f
#define VGA_OFFSET_HIGH 	0x0e

#define VIDEO_ADDRESS 		0xb8000
#define VGA_WIDTH 			80
#define  VGA_HEIGHT			25


// Copies value from DX (port) ==> AL (result) : READS THE PORT
unsigned char port_byte_in(unsigned short port) {
    unsigned char result;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

// Copies value from AL (data) ==> DX (port) : WRITES THE PORT
void port_byte_out(unsigned short port, unsigned char data) {
    __asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}


void set_cursor(int offset) {
   offset /= 2;
/* The index is already a position in a uint16_t * array */
    port_byte_out(VGA_CTRL_REGISTER, VGA_OFFSET_HIGH);
    port_byte_out(VGA_DATA_REGISTER, (unsigned char) (offset >> 8));
    port_byte_out(VGA_CTRL_REGISTER, VGA_OFFSET_LOW);
    port_byte_out(VGA_DATA_REGISTER, (unsigned char) (offset & 0xff));
}

int get_cursor() {
    port_byte_out(VGA_CTRL_REGISTER, VGA_OFFSET_HIGH); // tells VGA to write high byte of cursor position in the data register
    int offset = port_byte_in(VGA_DATA_REGISTER) << 8; // get content of data register
    port_byte_out(VGA_CTRL_REGISTER, VGA_OFFSET_LOW); // tells VGA to write low byte of cursor position in the data register
    offset |= port_byte_in(VGA_DATA_REGISTER); // get content of data register
//    offset *= 2;
/* The index is already a position in a uint16_t * array */
    return offset;
}


static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}
 

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}
 

size_t strlenk(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}


size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
 
void terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*)VIDEO_ADDRESS;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void scroll_term(int line) {
	int index_w = 0;
	int end_pos = VGA_WIDTH * VGA_HEIGHT;

	for (int index_r = VGA_WIDTH * line; index_r < end_pos + VGA_WIDTH; index_r++) {
		terminal_buffer[index_w++] = terminal_buffer[index_r];
	}
	terminal_row--;
}

int terminal_putchar(char c) 
{
	/* Newline management */
	if (c == '\n') {
		terminal_column = 0;
		/* Scrolling management */
		if (++terminal_row == VGA_HEIGHT)
			scroll_term(1);
	} else {
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
		if (++terminal_column == VGA_WIDTH) {
			terminal_column = 0;
		/* Scrolling management */
			if (++terminal_row == VGA_HEIGHT)
				scroll_term(1);
		}
	}
	
	/* Cursor management */
	return terminal_row * VGA_WIDTH + terminal_column;
}
 
int terminal_write(const char* data, size_t size, int cursor_offset) 
{
	size_t i = 0;
	
	for (i = 0; i < size; i++) {
		cursor_offset = terminal_putchar(data[i]);
	}
	/* Cursor management */
	set_cursor(cursor_offset * 2);
	return cursor_offset;
}


int terminal_writestring(const char* data, int cursor_offset) 
{
//	int cursor_offset = 0;

	return terminal_write(data, strlenk(data), cursor_offset);
}

#define C_POS VGA_WIDTH / 2
 
void kernel_main(void) 
{
	char fg = VGA_COLOR_LIGHT_GREY;
	char bg = VGA_COLOR_BLACK;
	long long j = 0;
	int cursor_offset = 0;
	/* Initialize terminal interface */
	
	terminal_initialize();
 	for (int i = 0; i < 999 + C_POS * 6; i++) {
		if ( 
			i == 0xf + 6*C_POS || i == 0x17 + 6*C_POS || i == 0x18 + 6*C_POS || i == 0x19 + 6*C_POS || i == 0x1a + 6*C_POS ||
			i == 0xe + 7*C_POS || i == 0x16 + 7*C_POS || i == 0x1b + 7*C_POS ||
			i == 0xd + 8*C_POS || i == 0x16 + 8*C_POS || i == 0x1b + 8*C_POS ||
			i == 0xc + 9*C_POS || i == 0x1b + 9*C_POS ||
			i == 0xb + 10*C_POS || i == 0x1a + 10*C_POS || 
			i == 0xa + 11*C_POS || i == 0x19 + 11*C_POS ||
			i == 0x9 + 12*C_POS || i == 0x18 + 12*C_POS ||
			i == 0x8 + 13*C_POS || i == 0x17 + 13*C_POS ||
			i == 0x7 + 14*C_POS || i == 0x16 + 14*C_POS ||
			i == 0x7 + 15*C_POS || i == 0x16 + 15*C_POS ||
			(i >= 0x7 + 16*C_POS && i<= 0xf + 16*C_POS) || (i >= 0x16 + 16*C_POS && i <= 0x1b + 16*C_POS) ||
			i == 0xf + 17*C_POS ||
			i == 0xf + 18*C_POS ||
			i == 0xf + 19*C_POS ||
			i == 0xf + 20*C_POS 
		)
		{
			fg = (fg + 1) % 16;
			bg = (bg + 1) % 16;
			if (fg == VGA_COLOR_LIGHT_GREY && bg == VGA_COLOR_BLACK) {
				fg = (fg + 1) % 16;
				bg = (bg + 1) % 16;
			}
			terminal_color = vga_entry_color(fg, bg);
		}
		else {
			terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
		}
		cursor_offset = terminal_writestring("42", 0);
		j = 0;	
		while (j < 40000)
		{ 
			terminal_writestring("", cursor_offset);
			j = j + 1;
		}
	}
	
}
