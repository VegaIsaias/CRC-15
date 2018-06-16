//
//  Created by Isaias Perez Vega
//  ==================
//  CRC15 - CHECK

// LIBRARY INCLUDES //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// CONSTANTS //
#define MAX_CHAR_LINE 64
#define MAX_BYTES 512
#define D_SIZE 8079 // with padding
#define MAX_CHARS 504
#define D_P_INDEX 8064
#define BITS 16


// STRUCTURE FUCNTIONS //
char* readValidation(char* inputData, char* fileName);


int main(int argc, char** argv) {
    
    unsigned int poly[] = {1,0,1,0,0,0,0,0,0,1,0,1,0,0,1,1};

    if (argc < 3) {
        printf("Not enough parameters to run program\n");
        return 0;
    } else {
        
        // Setup info
        char action[2]; strcpy(action, argv[1]);
        char fileName[50]; strcpy(fileName, argv[2]);
        char* input = NULL; unsigned int* divident = NULL;
        
        int calculate = strcmp(action, "c");
        //calculate = 1;
        if (calculate == 0) {
            printf("\nCRC15 Input text from file\n\n");
            
            // Reading/Storing input
            input = readInput(input, fileName);
            
            // Convert HEX to binary
            divident = prepareDividend(input, divident);
            
            
            // Initiating CRC
            printf("CRC 15 calculation progress:\n\n");
            runCRC(input, divident, poly);
            
        } else {
            printf("\nCRC15 Input text from file\n\n");
            input = readValidation(input, fileName);
            
            
            // Initiating validation of CRC
            printf("\n\nCRC 15 calculation progress:\n\n");
            
            // Convert HEX to binary
            divident = prepareDividend(input, divident);
            
            validateCRC(input, divident, poly);
        }
    }
    return 0;
}

char* readValidation(char* inputData, char* fileName) {
    int breaker = 0;
    long fileLen;

    FILE* dataFile = fopen(fileName, "r");
    if (dataFile) {
        fseek(dataFile, 0L, SEEK_END);
        fileLen = ftell(dataFile);
        rewind(dataFile);

        // Allocate memeory
        inputData = calloc(MAX_BYTES, sizeof(char));
        if (inputData == NULL) {
            printf("Was not able to allocate enough memory.\n");
            return 0;
        }
        
        // Read data
        fread(inputData, fileLen, 1, dataFile);
        
        while (breaker < MAX_BYTES) {
            printf("%c", inputData[breaker]);
            if (breaker % (MAX_CHAR_LINE) == 0 && breaker != 0) {
                printf("\n");
            }
            breaker++;
        }
    } else {
        printf("File was not able to open.\n");
        exit(0);
    }

    return inputData;
    
}

