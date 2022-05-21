#pragma once


struct interrupt_frame;



__attribute__((interrupt)) void MMouseHandler(struct interrupt_frame* frame);