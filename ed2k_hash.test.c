#include "ed2k_hash.h"

#include <stdio.h>

int main(int argc, char **argv){
	int i;
	for(i = 1; i < argc; ++i){
		unsigned char out[16];
		if(ed2k_hash(out,argv[i]) == 0){
			printf("[%s]\n",argv[i]);
			{
				int j;
				for(j = 0; j < 16; ++j){
					printf("%02x",out[j]);
				}
			}
			putchar('\n');
		}
	}
	
	return 0;
}