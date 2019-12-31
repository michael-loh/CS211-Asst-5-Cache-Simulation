#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct Node{
	unsigned long int tag;
	int valid;
	
}node;

int main(int argc, char **argv){
	
	
//	printf("HERE\n");
	int cacheSize = atoi(argv[1]);

	char *assoc = argv[2];

	char *prefPolicy = argv[3];

//	char *repPolicy = argv[4];

	int blockSize = atoi(argv[5]);

//	printf("%d %s %s %s %d\n", cacheSize, assoc, prefPolicy, repPolicy, blockSize);
	
	FILE *fp = fopen(argv[6],"r");

	//loop indexes
	int i;
	int j;
	int z;
	char *temp;	
	
	
	//check associativity
	int isDirect = 0;
	int isAssoc = 0;
	int isAssocN = 0;	
	int n = 1;					//n is number of blocks in a set

	if(strcmp("direct",assoc) == 0){
		isDirect = 1;
	}

	else if(strcmp("assoc", assoc) == 0){
		isAssoc = 1;
	}
	
	else{
		isAssocN = 1;
		int length = strlen(assoc);
		temp = malloc(sizeof(char*)*10);
		j = 0;
		for(i = 6; i < length; i++){				//getting the 'n' in assoc:n
			temp[j] = assoc[i];
			j++;
		}
//		printf("temp: %s\n", temp);
		n = atoi(temp);
	}

	
//	printf("=HERE=\n");	
	
//determine block offset and set bits======================================
	//block offset is equal to log base 2 of blocksize
	int blockOffset = log(blockSize)/log(2);
//	printf("block offset: %d\n", blockOffset);	
	//find the number of sets
	int numberOfSets = 0;
	if(isDirect){
		numberOfSets = cacheSize / blockSize;
	}
	else if(isAssoc){
		numberOfSets = 1;
		n = cacheSize / blockSize;
	}
	else if(isAssocN){
		numberOfSets = cacheSize / (blockSize * n);
	}
	
	int setBits = log(numberOfSets) / log(2);
//	printf("set bits: %d\n", setBits);
	
//initialize array of nodes
	
	node *table[numberOfSets][n];
	for(i = 0; i < numberOfSets; i++){

		for(j = 0; j < n; j++){

			table[i][j] = malloc(sizeof(node));

		}
	
	}
	
	int indexForReplace[numberOfSets];		//for replacing tags
	for(i = 0; i < numberOfSets; i++){
		indexForReplace[i] = 0;
	}


	
//read the file of inputs
	//variables that I need
	unsigned long int address;
	char cmdChar;
	char* read = malloc(sizeof(char*)*10);
//	unsigned long int address2;
	int setIndex;
	unsigned long int tag;
	
/*	fscanf(fp, "%lx", &address);
	printf("%lu\n", address);
	fscanf(fp, "%c", &colon);
	printf("%c\n", colon);
	fscanf(fp, " %c", &cmdChar);
	printf("%c\n", cmdChar);
	fscanf(fp, "%lx", &address);
	printf("%lu\n", address);
*/	
	int memoryReads = 0;
	int memoryWrites = 0;
	int cacheHits = 0;
	int cacheMisses = 0;
//	int prefetchOn = 0;
//	int count = 0;	

	while(1){
//		printf("loop here 1\n");
//		printf("======%d=====\n", count);
//		printf("MR: %d\n", memoryReads);
//		printf("MW: %d\n", memoryWrites);
//		printf("CH: %d\n", cacheHits);
//		printf("CM: %d\n", cacheMisses);
//		count++;
//		printf("\n");
		
		fscanf(fp, "%s ", read);
//		printf("read: %s\n", read);
//		printf("%s\n", read);
		if(strcmp("#eof", read) == 0 || strcmp("of", read) == 0){
			break;
		}
		fscanf(fp, " %c", &cmdChar);
//		printf("%c\n", cmdChar);
		fscanf(fp, "%lx", &address);
//		printf("%lu\n", address);
	
/*		else{
//			printf("here2\n");
			if(strcmp("#eof", read) == 0){
				break;
			}
//			printf("prefetched\n");
			prefetchOn = 0;
			cacheHits++;
			if(cmdChar == 'W'){
				memoryWrites++;
			}
			continue;
			
		}
*/
//		printf("%s %c %lu\n", read, cmdChar, address);
		
		unsigned long int t1 = address >> blockOffset;
		double t2 = pow(2,setBits);
		int intT2 = (int) t2;
		setIndex = t1 % intT2;
		tag = t1 >> setBits;
		
//		printf("set index: %d\n", setIndex);
//		printf("tag: %lu\n", tag);				
		for(i = 0; i < n; i++){						//searching the given set
//			printf("loop here 2\n");
			if(table[setIndex][i]->valid == 1){			//checking for valid cache lines
				if(tag == table[setIndex][i]->tag){			//if tag is found in table
					cacheHits++;
					if(cmdChar == 'W'){
						memoryWrites++;
					}
					break;					//break out if tag is found
//					prefetchOn = 0;
				}
				
				if(i == n-1){						//if tag not found and cache lines for set are full, replace
					if(cmdChar == 'W'){
						memoryWrites++;
					}
					
					cacheMisses++;
					memoryReads++;
					j = indexForReplace[setIndex];
					table[setIndex][j]->tag = tag;			//replace the tag of the oldest node
					j++;
					
					if(j == n){
						j = 0;
					}
					indexForReplace[setIndex] = j;			//update the indexForReplace with current oldest node
					
					if(strcmp(prefPolicy,"p1") == 0){		//if prefetch policy is on
//						printf("HERE\n");
						
//						memoryReads++;
						
//						prefetchOn = 1;
//						fscanf(fp, "%s", read);
						if(strcmp(read, "#eof") == 0){
							break;
						}
//						fscanf(fp, " %c", &cmdChar);
//						fscanf(fp, "%lx", &address);
//						printf("cmdChar: %c\n", cmdChar);
//						printf("address: %lu\n", address);
						address = address + blockSize;
						t1 = address >> blockOffset;
						t2 =  pow(2, setBits);
						intT2 = (int) t2;
						setIndex = t1 % intT2;
						tag = t1 >> setBits;
//						printf("setIndex: %d\n", setIndex);
//						printf("tag: %lu\n", tag);

						for(z = 0; z < n; z++){
							if(table[setIndex][z]->valid == 1){
								if(tag == table[setIndex][z]->tag){
//									printf("HERE\n");
									break;
								}
							
								if(z == n-1){
//									printf("HERE2\n");
									memoryReads++;
									int tempJ = indexForReplace[setIndex];
									table[setIndex][tempJ]->tag = tag;
									table[setIndex][tempJ]->valid = 1;
									tempJ++;
	
									if(tempJ == n){
										tempJ = 0;
									}
									indexForReplace[setIndex] = tempJ;
								}
							}
							else{
//								printf("HERE3\n");
								memoryReads++;
								table[setIndex][z]->tag = tag;
								table[setIndex][z]->valid = 1;
								break;
							}

						}


					}
					



				}
				
			}
			else if(table[setIndex][i]->valid == 0){			//if the tag is not found
//				printf("here\n");
				if(cmdChar == 'W'){
					memoryWrites++;
				}
				cacheMisses++;
				memoryReads++;
				
				table[setIndex][i]->tag = tag;				//save the tag
				table[setIndex][i]->valid = 1;				//make the cache line valid
				
				if(strcmp(prefPolicy,"p1") == 0){
//					printf("HERE2\n");
					
//					memoryReads++;				//this could be WRONG
					
//					prefetchOn = 1;
//					fscanf(fp, "%s", read);
//					printf("pref read: %s\n", read);
					if(strcmp(read, "#eof") == 0){
						break;
					}
//					fscanf(fp, " %c", &cmdChar);
//					fscanf(fp, "%lx", &address);
//					printf("cmdChar: %c\n", cmdChar);
//					printf("address: %lu\n", address);
					address = address + blockSize;
					t1 = address >> blockOffset;
					t2 = pow(2,setBits);
					intT2 = (int) t2;
					setIndex = t1 % intT2;
					tag = t1 >> setBits;
//					printf("setIndex: %d\n", setIndex);
//					printf("tag: %lu\n", tag);
					
					for(z = 0; z < n; z++){
						if(table[setIndex][z]->valid == 1){
							if(tag == table[setIndex][z]->tag){
//								printf("HERE4\n");
								break;
							}
						

							if(z == n-1){
//								printf("HERE5\n");
								memoryReads++;
								int tempJ2 = indexForReplace[setIndex];
								table[setIndex][tempJ2]->tag = tag;
								table[setIndex][tempJ2]->valid = 1;
								tempJ2++;
								
								if(tempJ2 == n){
									tempJ2 = 0;
								}
								indexForReplace[setIndex] = tempJ2;
							}
						}
						else{
//							printf("HERE6\n");
//							printf("tag: %lu\n", tag);
							memoryReads++;
							table[setIndex][z]->tag = tag;
							table[setIndex][z]->valid = 1;
//							printf("%d %d\n", setIndex, z);
//							printf("%lu\n", table[setIndex][z]->tag);
							break;
						}
						
					}
					
				}	
				
				
				break;						//break out if tag is found
			}
			
		}

	}	
	
//	for(i = 0; i < numberOfSets; i++){
//		printf("indexForReplace[%d] = %d\n", i, indexForReplace[i]);
//	}

	
//	printf("%d\n", numberOfSets);

	printf("Memory reads: %d\n", memoryReads);
	printf("Memory writes: %d\n", memoryWrites);
	printf("Cache hits: %d\n", cacheHits);
	printf("Cache misses: %d\n", cacheMisses);


	
	return 0;


}








