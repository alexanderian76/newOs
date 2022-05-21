#include <stdint.h>
#include "interrupts.h"

#include "../types.h"
#include "../main.h"
#include "../utils.h"
#include "../consts.h"

/* current cursor location */
uint32_t current_loc1 = 0;
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
      
      current_loc1 = (unsigned int)((current_loc1 + (int)(x % 160)/10 + (int)((y % 50) / 10) * 160 ) % (SCREENSIZE));
     set_cursor(current_loc1); 
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
void PIC_EndSlave1(){
    outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}
__attribute__((interrupt)) void MMouseHandler(struct interrupt_frame* frame) {

  MouseInt_Handler();
        PIC_EndSlave1();

  asm ("sti");
}