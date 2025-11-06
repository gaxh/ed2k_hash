#include "ed2k_hash.h"
#include "md4.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define SOMESIZE	9728000

unsigned char buf[SOMESIZE];

int ed2k_hash(unsigned char digest[16], const char *file){
	FILE *f = fopen(file,"rb");
	if(!f){return -1;}
{
	md4ctx one;
	md4ctx all;
	unsigned char oneOut[16];
	unsigned char allOut[16];
	int size;
	int loopNb = 0;
	md4_init(&all);
	while((size = fread(buf,1,SOMESIZE,f)) != 0){
		md4_init(&one);
		md4_update(&one,buf,size);
		md4_final(oneOut,&one);
		md4_update(&all,oneOut,sizeof(oneOut));
		++loopNb;
	}
	md4_final(allOut,&all);
	if(loopNb > 1){
		memcpy(digest,allOut,sizeof(allOut));
	}else{
		memcpy(digest,oneOut,sizeof(oneOut));
	}
}
	fclose(f);
	return 0;
}

int main(int argc, char **argv) {
    for(int i = 1; i < argc; ++i) {
        unsigned char digest[16];
        const char *name = argv[i];

        int result = ed2k_hash(digest, name);

        if(result == 0) {
            printf("%s: ", name);
            for(int x = 0; x < sizeof(digest) / sizeof(digest[0]); ++x) {
                printf("%02x", digest[x]);
            }
            printf("\n");
        } else {
            printf("%s: CAN_NOT_OPEN_FILE\n", name);
        }
    }

    return 0;
}

