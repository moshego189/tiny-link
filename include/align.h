#ifndef ALIGN_H
#define ALIGN_H

#define ALIGN_UP(x,size)   (((x) + ((size)-1)) & (-(size)))
#define ALIGN_DOWN(x,size) ((x) & (-(size)))

#endif