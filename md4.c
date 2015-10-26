#include "md4.h"

#define S11		3
#define S12		7
#define S13		11
#define S14		19
#define S21		3
#define S22		5
#define S23		9
#define S24		13
#define S31		3
#define S32		9
#define S33		11
#define S34		15

static void md4_transform(unsigned int [4], const unsigned char [64]);
static void encode(unsigned char *, const unsigned int *, unsigned int);
static void decode(unsigned int *, const unsigned char *, unsigned int);
static void md4_memcpy(void *, const void *, unsigned int);
static void md4_memset(void *, int, unsigned int);

static const unsigned char PADDING[64] = {
	0x80,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
};

#define F(x,y,z)	(((x) & (y)) | ((~(x)) & (z)))
#define G(x,y,z)	(((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define H(x,y,z)	((x) ^ (y) ^ (z))

#define ROTATE_LEFT(x,n)	(((x) << (n)) | ((x) >> (32 - (n))))

#define FF(a,b,c,d,x,s)		\
	do {									\
		(a) += F((b),(c),(d)) + (x);		\
		(a) = ROTATE_LEFT((a),(s));			\
	} while(0)

#define GG(a,b,c,d,x,s)		\
	do {											\
		(a) += G((b),(c),(d)) + (x) + 0x5a827999;	\
		(a) = ROTATE_LEFT((a),(s));					\
	} while(0)

#define HH(a,b,c,d,x,s)		\
	do {											\
		(a) += H((b),(c),(d)) + (x) + 0x6ed9eba1;	\
		(a) = ROTATE_LEFT((a),(s));					\
	} while(0)

void md4_init(md4ctx *c){
	c->count[0] = c->count[1] = 0;
	c->state[0] = 0x67452301;
	c->state[1] = 0xefcdab89;
	c->state[2] = 0x98badcfe;
	c->state[3] = 0x10325476;
}

void md4_update(md4ctx *c, const unsigned char *input, unsigned int size){
	unsigned int i, idx, len;
	
	idx = (c->count[0] >> 3) & 0x3f;
	
	if( (c->count[0] += (size << 3)) < (size << 3) ){
		++c->count[1];
	}
	c->count[1] += (size >> 29);
	
	len = 64 - idx;
	
	if(size >= len){
		md4_memcpy(&c->buffer[idx],input,len);
		md4_transform(c->state,c->buffer);
		
		for(i = len; i + 63 < size; i += 64){
			md4_transform(c->state,&input[i]);
		}
		
		idx = 0;
	}else{
		i = 0;
	}
	
	md4_memcpy(&c->buffer[idx],&input[i],size-i);
}

void md4_final(unsigned char digest[16], md4ctx *c){
	unsigned char bits[8];
	unsigned int idx, len;
	
	encode(bits,c->count,8);
	idx = (c->count[0] >> 3) & 0x3f;
	len = idx < 56 ? 56 - idx : 120 - idx;
	
	md4_update(c,PADDING,len);
	md4_update(c,bits,8);
	encode(digest,c->state,16);
	md4_memset(c,0,sizeof(c[0]));
}

static void md4_transform(unsigned int state[4], const unsigned char block[64]){
	unsigned int a = state[0], b = state[1], c = state[2], d = state[3], x[16];
	
	decode(x,block,64);
	
	FF (a, b, c, d, x[ 0], S11); /* 1 */
	FF (d, a, b, c, x[ 1], S12); /* 2 */
	FF (c, d, a, b, x[ 2], S13); /* 3 */
	FF (b, c, d, a, x[ 3], S14); /* 4 */
	FF (a, b, c, d, x[ 4], S11); /* 5 */
	FF (d, a, b, c, x[ 5], S12); /* 6 */
	FF (c, d, a, b, x[ 6], S13); /* 7 */
	FF (b, c, d, a, x[ 7], S14); /* 8 */
	FF (a, b, c, d, x[ 8], S11); /* 9 */
	FF (d, a, b, c, x[ 9], S12); /* 10 */
	FF (c, d, a, b, x[10], S13); /* 11 */
	FF (b, c, d, a, x[11], S14); /* 12 */
	FF (a, b, c, d, x[12], S11); /* 13 */
	FF (d, a, b, c, x[13], S12); /* 14 */
	FF (c, d, a, b, x[14], S13); /* 15 */
	FF (b, c, d, a, x[15], S14); /* 16 */
	
	GG (a, b, c, d, x[ 0], S21); /* 17 */
	GG (d, a, b, c, x[ 4], S22); /* 18 */
	GG (c, d, a, b, x[ 8], S23); /* 19 */
	GG (b, c, d, a, x[12], S24); /* 20 */
	GG (a, b, c, d, x[ 1], S21); /* 21 */
	GG (d, a, b, c, x[ 5], S22); /* 22 */
	GG (c, d, a, b, x[ 9], S23); /* 23 */
	GG (b, c, d, a, x[13], S24); /* 24 */
	GG (a, b, c, d, x[ 2], S21); /* 25 */
	GG (d, a, b, c, x[ 6], S22); /* 26 */
	GG (c, d, a, b, x[10], S23); /* 27 */
	GG (b, c, d, a, x[14], S24); /* 28 */
	GG (a, b, c, d, x[ 3], S21); /* 29 */
	GG (d, a, b, c, x[ 7], S22); /* 30 */
	GG (c, d, a, b, x[11], S23); /* 31 */
	GG (b, c, d, a, x[15], S24); /* 32 */
	
	HH (a, b, c, d, x[ 0], S31); /* 33 */
	HH (d, a, b, c, x[ 8], S32); /* 34 */
	HH (c, d, a, b, x[ 4], S33); /* 35 */
	HH (b, c, d, a, x[12], S34); /* 36 */
	HH (a, b, c, d, x[ 2], S31); /* 37 */
	HH (d, a, b, c, x[10], S32); /* 38 */
	HH (c, d, a, b, x[ 6], S33); /* 39 */
	HH (b, c, d, a, x[14], S34); /* 40 */
	HH (a, b, c, d, x[ 1], S31); /* 41 */
	HH (d, a, b, c, x[ 9], S32); /* 42 */
	HH (c, d, a, b, x[ 5], S33); /* 43 */
	HH (b, c, d, a, x[13], S34); /* 44 */
	HH (a, b, c, d, x[ 3], S31); /* 45 */
	HH (d, a, b, c, x[11], S32); /* 46 */
	HH (c, d, a, b, x[ 7], S33); /* 47 */
	HH (b, c, d, a, x[15], S34); /* 48 */
	
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	
	md4_memset(x,0,sizeof(x));
}

static void encode(unsigned char *output, const unsigned int *input, unsigned int size){
	unsigned int i,j;
	for(i = 0, j = 0; j < size; ++i, j += 4){
		output[j] = input[i] & 0xff;
		output[j + 1] = (input[i] >> 8) & 0xff;
		output[j + 2] = (input[i] >> 16) & 0xff;
		output[j + 3] = (input[i] >> 24) & 0xff;
	}
}

static void decode(unsigned int *output, const unsigned char *input, unsigned int size){
	unsigned int i,j;
	for(i = 0, j = 0; j < size; ++i, j += 4){
		output[i] = (unsigned)input[j] | ((unsigned)input[j + 1] << 8)
			| ((unsigned)input[j + 2] << 16) | ((unsigned)input[j + 3] << 24);
	}
}

static void md4_memcpy(void *dst, const void *src, unsigned int size){
	unsigned int i;
	for(i = 0; i < size; ++i){
		((char *)dst)[i] = ((const char *)src)[i];
	}
}

static void md4_memset(void *dst, int val, unsigned int size){
	unsigned int i;
	for(i = 0; i < size; ++i){
		((char *)dst)[i] = (char)val;
	}
}

