#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(_linux_)
#error "kamu gak pake cross compiler, kamu mungkin akan mengalami beberapa masalah"
#endif

#if defined(_i386_)
#error "butuh compiler dengan ix86-elf compiler"
#endif

extern void idt_init();

enum vga_color {
	VGA_COLOR_BLACK					=	0,	
	VGA_COLOR_BLUE 					=	1,
	VGA_COLOR_GREEN					=	2,
	VGA_COLOR_CYAN 					=	3,
	VGA_COLOR_RED 					=	4,
	VGA_COLOR_MAGENTA 				=	5,
	VGA_COLOR_BROWN					=	6,
	VGA_COLOR_LIGHT_GREY 			=	7,
	VGA_COLOR_DARK_GREY 			=	8,
	VGA_COLOR_LIGHT_BLUE 			=	9,
	VGA_COLOR_LIGHT_GREEN 			=	10,
	VGA_COLOR_LIGHT_CYAN 			=	11,
	VGA_COLOR_LIGHT_RED 			=	12,
	VGA_COLOR_LIGHT_MAGENTA 		=	13,
	VGA_COLOR_LIGHT_BROWN 			=	14,
	VGA_COLOR_WHITE 				=	15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str)
{
	size_t len = 0;
	while(str[len])
		len++;
	return len;
}

#define VGA_WIDTH	80
#define VGA_HEIGHT	25
#define VGA_MEMORY	0xB8000

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;

void terminal_intialize(void)
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE);

	for(size_t y = 0; y < VGA_HEIGHT; y++)
	{
		for(size_t x = 0; x < VGA_WIDTH; x++)
		{
			const size_t index = y * VGA_WIDTH + x;
			if(index < VGA_HEIGHT * VGA_WIDTH)
			{
				terminal_buffer[index] = vga_entry(' ',terminal_color);
			}
		}
	}
}

void terminal_setcolor(uint8_t color)
{
	terminal_color = color;
}

void terminal_put_entry_at(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c,color);
}

void terminal_clear_screen(void)
{
	for(size_t y = 0;y<VGA_HEIGHT;y++)
	{
		for(size_t x = 0;x<VGA_WIDTH;x++)
		{
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ',terminal_color);
		}
	}
}

void terminal_clear_line(size_t line)
{
	for(size_t i = 0; i<VGA_WIDTH;i++)
	{
		const size_t index = line * VGA_WIDTH + i;
		terminal_buffer[index] = vga_entry(' ', terminal_color);
	}
}

void terminal_scrolling()
{
	terminal_clear_line(0);
	for(size_t i = 1;i < VGA_HEIGHT;i++)
	{
		for(size_t j = 0;j<VGA_WIDTH;j++)
		{
			size_t index_before = (i - 1) * VGA_WIDTH + j;
			size_t index_after = i * VGA_WIDTH + j;
			terminal_buffer[index_before] = terminal_buffer[index_after];
		}
	}
	terminal_clear_line(24);
}

void terminal_putchar(char c)
{
	if(c != '\n')
	{
		terminal_put_entry_at(c,terminal_color,terminal_column,terminal_row);
	}
	if(++terminal_column >= VGA_WIDTH || c == '\n')
	{
		terminal_column = 0;
		if(++terminal_row >= VGA_HEIGHT)
		{
			terminal_scrolling();
			terminal_row = 24;
		}
	}
}

void terminal_write(const char* str, size_t size)
{
	for(size_t i = 0; i < size;i++)
	{
		terminal_putchar(str[i]);
	}
}

void terminal_write_string(const char* str)
{
	terminal_write(str,strlen(str));
}



void kernel_main(void)
{
	terminal_intialize();
	idt_init();

	terminal_write_string("halo");

	// uint8_t i = 10;
	// uint8_t j = 0;
	// char k = i / j;

	// terminal_putchar(k);

	

	// int* ptr = (int*) 0xDEADBEEF; // An invalid address for testing
    // *ptr = 42; // This will cause a page f/ault when dereferencing the invalid pointer

	terminal_write_string("bisa bjir");

	// __asm__ volatile("ud2");
	// __asm__ volatile("int $0x6");
	int numerator = 10;
    int divisor = 0;
    
    __asm__ __volatile__(
        "movl %0, %%eax;"      // Load numerator into EAX register
        "movl %1, %%ebx;"      // Load divisor into EBX register
        "div %%ebx;"           // Perform division (EAX / EBX)
        :
        : "r" (numerator), "r" (divisor)  // Input operands
        : "%eax", "%ebx"       // Clobbered registers
    );
	terminal_write_string("owewoeirpjkjdsjkjf;sakj");

	// while(1)
	// {
	// 	__asm__ volatile ("cli; hlt");
	// }
}