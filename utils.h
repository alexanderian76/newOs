#pragma once

#ifndef UTILS_H
#define UTILS_H

#include "types.h"

extern uint32 strlen(const char*);
extern uint32 digit_count(int);
extern void itoa(int, char *);
extern void outb(unsigned short, unsigned char);
extern unsigned char inb(unsigned short);

#endif
