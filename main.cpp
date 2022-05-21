
#include <stdint.h>
#include "BasicRenderer.h"
#include "cstr.h"
#include "main.h"
#include "consts.h"
#include "types.h"
#include "keyboard_map.h"

//--------------------------------

void outb(unsigned short port, unsigned char val){
  asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

unsigned char inb(unsigned short port){
  unsigned char returnVal;
  asm volatile ("inb %1, %0"
  : "=a"(returnVal)
  : "Nd"(port));
  return returnVal;
}
void io_wait(){
    asm volatile ("outb %%al, $0x80" : : "a"(0));
}

//---------------------------------------------------------------------

void PIC_EndMaster(){
    outb(PIC1_COMMAND, PIC_EOI);
}

void PIC_EndSlave(){
    outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}
   

void RemapPIC(){
    uint8_t a1, a2; 

    a1 = inb(PIC1_DATA);
    io_wait();
    a2 = inb(PIC2_DATA);
    io_wait();

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    outb(PIC1_DATA, 0x20);
    io_wait();
    outb(PIC2_DATA, 0x28);
    io_wait();

    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    outb(PIC1_DATA, a1);
    io_wait();
    outb(PIC2_DATA, a2);

}

//----------------------------------------------------------------------------

uint32 strlen(const char* str)
{
  uint32 length = 0;
  while(str[length])
    length++;
  return length;
}

uint32 digit_count(int num)
{
  uint32 count = 0;
  if(num == 0)
    return 1;
  while(num > 0){
    count++;
    num = num/10;
  }
  return count;
}

void itoa(int num, char *number)
{
  int dgcount = digit_count(num);
  int index = dgcount - 1;
  char x;
  if(num == 0 && dgcount == 1){
    number[0] = '0';
    number[1] = '\0';
  }else{
    while(num != 0){
      x = num % 10;
      number[index] = x + '0';
      index--;
      num = num / 10;
    }
    number[dgcount] = '\0';
  }
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





#define IDT_TA_InterruptGate    0b10001110
#define IDT_TA_CallGate         0b10001100
#define IDT_TA_TrapGate         0b10001111

struct IDTDescEntry {
    uint16_t offset0; 
    uint16_t selector;  
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset1;
    uint32_t offset2;
    uint32_t ignore;
    void SetOffset(uint64_t offset);
    uint64_t GetOffset();
};

struct IDTR {
    uint16_t Limit;
    uint64_t Offset;
} __attribute__((packed));

void IDTDescEntry::SetOffset(uint64_t offset){
    offset0 = (uint16_t)(offset & 0x000000000000ffff);
    offset1 = (uint16_t)((offset & 0x00000000ffff0000) >> 16);
    offset2 = (uint32_t)((offset & 0xffffffff00000000) >> 32);

    ist = 0;
    ignore = 0;

}

uint64_t IDTDescEntry::GetOffset(){ 
    uint64_t offset = 0;
    offset |= (uint64_t)offset0;
    offset |= (uint64_t)offset1 << 16;
    offset |= (uint64_t)offset2 << 32;
    return offset;
}

//--------------------------------------
void PageFault_Handler(struct interrupt_frame* frame){
  clear_screen();
    kprint("Page fault detected");
   // int *t = (int*)0x80000000000;
   // *t = 2;
    while(true);
}
void DoubleFault_Handler(struct interrupt_frame* frame){
  clear_screen();
    kprint("Double fault detected");
    while(true);
}
void GeneralProtectionFault_Handler(struct interrupt_frame* frame){
  clear_screen();
    char* str = "00000000";
    uint8_t t = inb(0xE9);
    itoa(t, str);
    kprint(str);
    kprint_newline();
    kprint("General protection fault detected");
    while(true);
}
void KeyboardInt_Handler(){
    
   // uint8_t scancode = inb(0x60);

unsigned char status;
	char keycode;
    unsigned int line_size = BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE;
	/* write EOI */
    //outb(0x20, 0x20);
//	outb(0xa0, 0x20);
        
	status = inb(KEYBOARD_STATUS_PORT);
  keycode = inb(KEYBOARD_DATA_PORT);
	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01 && keycode > 0) {
		
		
        
        switch (keycode) {
            case UP:
                if(current_loc/160 > 1){
                current_loc = (current_loc/160 - 1)*80*2 + current_loc%160;
                set_cursor(current_loc);
                
                }
                return;
                break;
            case DOWN:
                if(current_loc/160 < 23){
                current_loc = (current_loc/160 + 1)*80*2 + current_loc%160;
                set_cursor(current_loc);
                }
                return;
                break;
            case LEFT:
                if(current_loc%160 > 2){
                current_loc = current_loc - 2;
                set_cursor(current_loc);
                }
                return;
                break;
            case RIGHT:
                if(current_loc%160 < 156){
                current_loc = current_loc + 2;
                set_cursor(current_loc);
                }
                return;
                break;
            default:
                break;
        }
        
        
        
        
		if(keycode == ENTER_KEY_CODE) {
            if(current_loc % 80 == 2 ) {
              return;
        }
          //  if(current_loc/160 == menu[0].row){
           //     menu[0].func();
        //    }
			kprint_newline();
            set_cursor(current_loc);

			return;
		}
		
		
        
		vidptr[current_loc++] = keyboard_map[(unsigned char) keycode];
		vidptr[current_loc++] = 0x07;
        set_cursor(current_loc);

   // while(true);
}

}

__attribute__((interrupt)) void MKeyHandler(struct interrupt_frame* frame) {
  KeyboardInt_Handler();
PIC_EndMaster();
    asm ("sti");
}

void MouseWait(){
    uint64_t timeout = 100000;
    while (timeout--){
        if ((inb(0x64) & 0b10) == 0){
            return;
        }
    }
}

void MouseWaitInput(){
    uint64_t timeout = 100000;
    while (timeout--){
        if (inb(0x64) & 0b1){
            return;
        }
    }
}

void MouseWrite(uint8_t value){
    MouseWait();
    outb(0x64, 0xD4);
    MouseWait();
    outb(0x60, value);
}

uint8_t MouseRead(){
    MouseWaitInput();
    return inb(0x60);
}

void InitPS2Mouse(){

    outb(0x64, 0xA8); //enabling the auxiliary device - mouse

    MouseWait();
    outb(0x64, 0x20); //tells the keyboard controller that we want to send a command to the mouse
    MouseWaitInput();
    uint8_t status = inb(0x60);
    status |= 0b10;
    MouseWait();
    outb(0x64, 0x60);
    MouseWait();
    outb(0x60, status); // setting the correct bit is the "compaq" status byte

    MouseWrite(0xF6);
    MouseRead();

    MouseWrite(0xF4);
    MouseRead();
}
   void OnMouseDown(uint8 button)
    {
      kprint("q");
    }
    
    void OnMouseUp(uint8 button)
    {
      kprint("w");
    }
    
    void OnMouseMove(int x, int y)
    {
     // char * str = NULL;
     // itoa(y, str);
     // kprint(str);
      
      current_loc = (unsigned int)((current_loc + (int)(x % 160)/10 + (int)((y % 50) / 10) * 160 ) % (SCREENSIZE));
     set_cursor(current_loc); 
    }

uint8 offset = 0;
uint8 buttons = 0;
unsigned char buffer[3];
void MouseInt_Handler() {
 // uint8_t scancode = inb(0x60);
  unsigned short commandport = 0x64;
  unsigned short dataport = 0x60;

 // kprint("m");
  
 // outb(0x20, 0x60);
//	outb(0xa0, 0x64);


/*for(int i = 0; i < 3; i++) 
{
  kprint_newline();
  char* str = "00000000";
  itoa(buffer[i], str);
  kprint(str);
}*/
          uint8 status = inb(commandport);
       // commandport.Read();
        if (!(status & 0x20)) {
            
            return;
        }
buffer[offset] = inb(dataport);
        //dataport.Read();
        
      //  if(handler == 0)
         //   return;
        
        offset = (offset + 1) % 3;

        if(offset == 0)
        {
            if(buffer[1] != 0 || buffer[2] != 0)
            {
                OnMouseMove((char)buffer[1], -((char)buffer[2]));
            }

            for(uint8 i = 0; i < 3; i++)
            {
                if((buffer[0] & (0x1<<i)) != (buttons & (0x1<<i)))
                {
                    if(buttons & (0x1<<i))
                        OnMouseUp(i+1);
                    else
                        OnMouseDown(i+1);
                }
            }
            buttons = buffer[0];
        }
       // return;
 
  
}

__attribute__((interrupt)) void MMouseHandler(struct interrupt_frame* frame) {

  MouseInt_Handler();
        PIC_EndSlave();
  asm ("sti");
}
//--------------------------------------

extern unsigned long long int p4_table;

IDTR idtr;
void PrepareInterrupts(){
    idtr.Limit = 0x0FFF;
    idtr.Offset = (uint64_t)p4_table;

    IDTDescEntry* int_PageFault = (IDTDescEntry*)(idtr.Offset + 0xE * sizeof(IDTDescEntry));
    int_PageFault->SetOffset((uint64_t)PageFault_Handler);
    int_PageFault->type_attr = IDT_TA_InterruptGate;
    int_PageFault->selector = 0x08;


    IDTDescEntry* int_DoubleFault = (IDTDescEntry*)(idtr.Offset + 0x8 * sizeof(IDTDescEntry));
    int_DoubleFault->SetOffset((uint64_t)DoubleFault_Handler);
    int_DoubleFault->type_attr = IDT_TA_InterruptGate;
    int_DoubleFault->selector = 0x08;


    IDTDescEntry* int_GeneralFault = (IDTDescEntry*)(idtr.Offset + 0xD * sizeof(IDTDescEntry));
    int_GeneralFault->SetOffset((uint64_t)GeneralProtectionFault_Handler);
    int_GeneralFault->type_attr = IDT_TA_InterruptGate;
    int_GeneralFault->selector = 0x08;


    IDTDescEntry* int_Keyboard = (IDTDescEntry*)(idtr.Offset + 0x21 * sizeof(IDTDescEntry));
    int_Keyboard->SetOffset((uint64_t)MKeyHandler);
    int_Keyboard->type_attr = IDT_TA_InterruptGate;
    int_Keyboard->selector = 0x08;


    IDTDescEntry* int_Mouse = (IDTDescEntry*)(idtr.Offset + 0x2C * sizeof(IDTDescEntry));
    int_Mouse->SetOffset((uint64_t)MMouseHandler);
    int_Mouse->type_attr = IDT_TA_InterruptGate;
    int_Mouse->selector = 0x08;

    asm volatile ("lidt %0" : : "m" (idtr));

    RemapPIC();

    

    

}



extern "C" void __start__(PSF1_FONT* psf1_font){
   /* char* str = "00000000";
    int t = framebuffer->PixelsPerScanLine;
    itoa(t, str);
    kprint(str);*/
    clear_screen();
    PrepareInterrupts();
    InitPS2Mouse();
    outb(PIC1_DATA,  0b11111001);
    outb(PIC2_DATA,  0b11101111);

    asm volatile ("sti");
    
    //asm ("ret");
    
   // int *t = (int*)0x80000000000;
   // *t = 2;
  //  for(int i = 0; i < 300; i++){
  //    outb(0x60, 0xffff);
  //    outb(0x64, 0x20);
  //   asm ("int $0x21");
  //  }
    //clear_screen();
   /* Framebuffer *framebuffer;
    framebuffer->BufferSize =  80 * 25;
    framebuffer->Width = 80;
    framebuffer->Height = 25;
    framebuffer->PixelsPerScanLine = 2;
   // kprint("1234567890");
    BasicRenderer newRenderer = BasicRenderer(framebuffer, psf1_font); 
    newRenderer.Print(to_string((uint64_t)1234976));
    newRenderer.CursorPosition = {0, 16}; 
    newRenderer.Print(to_string((int64_t)-1234976));
    newRenderer.CursorPosition = {0, 32};
    newRenderer.Print(to_string((double)-13.14));
    newRenderer.CursorPosition = {0, 48};
    newRenderer.Print(to_hstring((uint64_t)0xF0));
    newRenderer.CursorPosition = {0, newRenderer.CursorPosition.Y + 16};
    newRenderer.Print(to_hstring((uint32_t)0xF0));
    newRenderer.CursorPosition = {0, newRenderer.CursorPosition.Y + 16};
    newRenderer.Print(to_hstring((uint16_t)0xF0));
    newRenderer.CursorPosition = {0, newRenderer.CursorPosition.Y + 16};
    newRenderer.Print(to_hstring((uint8_t)0xF0));*/
    while(1)__asm__("hlt\n\t");
    return ;
}