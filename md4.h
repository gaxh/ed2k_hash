#ifndef _MD4_H_
#define _MD4_H_

typedef struct {
	unsigned int state[4];
	unsigned int count[2];
	unsigned char buffer[64];
} md4ctx;

extern void md4_init(md4ctx *);
extern void md4_update(md4ctx *, const unsigned char *input, unsigned int size);
extern void md4_final(unsigned char digest[16], md4ctx *);




#endif
