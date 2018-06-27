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

// READ FILE AND STORE IN ARRAY AND RETURN POINTER TO IT //
char* readInput(char* inputData, char* fileName) {
    int x = 0; char pad = '.'; int breaker = 0;
    long fileLen;
    // Open file
    FILE* dataFile = fopen(fileName, "r");
    if (dataFile) {
        fseek(dataFile, 0L, SEEK_END);
        fileLen = ftell(dataFile);
        rewind(dataFile);

        // Allocate memeory
        inputData = calloc(MAX_CHARS, sizeof(char));
        if (inputData == NULL) {
            printf("Was not able to allocate enough memory.\n");
            return 0;
        }
        
        // Read data
        fread(inputData, fileLen, 1, dataFile);
        
        while (breaker < MAX_CHARS) {
            printf("%c", inputData[breaker]);
            if (breaker % (MAX_CHAR_LINE) == 0 && breaker != 0) {
                printf("\n");
            }
            breaker++;
        }
        
        while (inputData[x] != 10) {
            x++;
        }
        
        // Padding
        while (x < MAX_CHARS) {
            inputData[x] = pad;
            x++;
        }
        
        // Close file and return pointer data
        fclose(dataFile);
        
    // Input Stream exception handling
    } else {
        printf("File was not able to open.\n");
        exit(0);
    }
    return inputData;
}

// STORE BINARY REPRESENTATION OF HEX VALUE INTO ARRAY //
unsigned int* hexBin(unsigned int* binary, char hex) {
    int x; int value = 8;
    binary = calloc(BITS, sizeof(unsigned int));
    
    // Storing bin rep of hex value
    for (x = 7; x >= 0; --x) {
        (hex & (1 << x)) ? (binary[value++] = 1) : (binary[value++] = 0);
    }
    return binary;
}

// CONVERT HEX DATA TO ITS BINARY EQUIVALENT, 1 HEX = 16 BITS //
unsigned int* prepareDividend(char* inputData, unsigned int* dividend) {
    
    // Prepare memory
    dividend = calloc(D_SIZE, sizeof(unsigned int));
    unsigned int* binRep = NULL;
    
    // Append hex to binary as a string
    for (int x = 0; x < MAX_CHARS; x++) {
        binRep = hexBin(binRep, inputData[x]);
        append(binRep, dividend, x);
        free(binRep); binRep = NULL;
    }
    
    return dividend;
}

// APPEND BINARY VALUE TO DIVIDENT //
void append(unsigned int* binRep, unsigned int* dividend, int position) {
    int binIndex = 0;
    for (int x = position * BITS; x < (position + 1) * BITS; x++) {
        dividend[x] = binRep[binIndex++];
    }
}

// GIVEN CRC POLY, AND PADDED DIVIDEND, RUN CRCHECK //
void runCRC(char* inputData, unsigned int* dividend, unsigned int poly[]) {
    
    unsigned int* result = NULL;
    unsigned int* passedBits = NULL; 
    
    unsigned int* tmp = NULL; tmp = calloc(BITS, sizeof(unsigned int));
    int divIndex = 0; int remIndex = 0; int printIndex = 0; int y = 0;
    
    // Running CRC calculation
    for (int x = divIndex; x < D_SIZE; ) {
        
        // Printing line to CRC
        for (y = printIndex; y < printIndex + MAX_CHAR_LINE; y++) {
            printf("%c", inputData[y]);
        }
        
        // Updating index for new line to print
        printIndex = y;
        
        if (divIndex != 7176) {
            printf(" - ");
        }
        
        
        int tmp = x;

        result = getPassBits(result, dividend, &divIndex);

        int sigbits = getSigBits(result);
        
        if (sigbits < BITS) {
            fixBits(result, dividend, &divIndex, sigbits);

        } else {
            // XOR with poly
            result = XOR(result, result, poly);
        }
        
        
        // Run until dividend is exhausted
        while (divIndex < tmp + (MAX_CHAR_LINE * BITS)) {
            
            int sigbits = getSigBits(result);
            
            // If left most 1 is not at index 0, drop needed bits from dv
            if (sigbits < BITS) {
                remIndex = divIndex;
                fixBits(result, dividend, &divIndex, sigbits);
                for (int x = 0; x < BITS; x++) {
                }
            } else {
                result = XOR(result, result, poly);
                for (int x = 0; x < BITS; x++) {
                }
            }
        }
        
        // Checking remainder
        int sigBits = getSigBits(result);
        if (sigBits == BITS) {
            result = XOR(result, result, poly);
        }

        // CRC progress
        unsigned int* convertedResult = NULL;
        convertedResult = resultToHex(result);
        
        if (divIndex > 7176) {
            printf(" - ");
        }
        
        // Print Progress
        for (int x = 0; x < 8; x++) {
            printf("%d", convertedResult[x]);
        }
        printf("\n");    
        
        if (divIndex > 7176) {
            printf("\nCRC15 result : ");
            for (int x = 0; x < 8; x++) {
                printf("%d", convertedResult[x]);
            }
        }

        x = divIndex;
        
        free(passedBits); passedBits = NULL;
    }
}


// Completes a redundency check 
void validateCRC(char* inputData, unsigned int* dividend, unsigned int poly[]) {
    
    unsigned int* result = NULL;
    unsigned int* passedBits = NULL;
    
    unsigned int* tmp = NULL; tmp = calloc(BITS, sizeof(unsigned int));
    int divIndex = 0; int remIndex = 0; int printIndex = 0; int y = 0;
    
    // Validating CRC calculation
    for (int x = divIndex; x < D_SIZE; ) {
        
        // Printing line to CRC
        for (y = printIndex; y < printIndex + MAX_CHAR_LINE; y++) {
            printf("%c", inputData[y]);
        }
        
        // Updating index for new line to print
        printIndex = y;
        
        if (divIndex != 7176) {
            printf(" - ");
        }
        
        int tmp = x;
        
        // Get next 16 bits
        result = getPassBits(result, dividend, &divIndex);

        // Getting 16 bits from divident
        int sigbits = getSigBits(result);
        
        // Shift bits
        if (sigbits < BITS) {
            fixBits(result, dividend, &divIndex, sigbits);
        } else {
            // XOR with poly
            result = XOR(result, result, poly);
        }
        
        // Run until dividend is exhausted
        while (divIndex < tmp + (MAX_CHAR_LINE * BITS)) {
            
            int sigbits = getSigBits(result);
            
            // If left most 1 is not at index 0, drop needed bits from divident
            if (sigbits < BITS) {
                remIndex = divIndex;
                fixBits(result, dividend, &divIndex, sigbits);
            } else {
                result = XOR(result, result, poly);
            }
        }
        
        // Checking remainder
        int sigBits = getSigBits(result);
        if (sigBits == BITS) {
            result = XOR(result, result, poly);
        }

        // CRC progress
        unsigned int* convertedResult = NULL;
        convertedResult = resultToHex(result);
        
        if (divIndex > 7176) {
            printf(" - ");
        }
        
        
        // Print Progress
        for (int x = 0; x < 8; x++) {
            printf("%d", convertedResult[x]);
        }
        printf("\n");
        
        if (divIndex > 7176) {
            printf("\nCRC15 result : ");
            for (int x = 0; x < 8; x++) {
                printf("%d", convertedResult[x]);
            }
        }
        
        x = divIndex;
        free(convertedResult); convertedResult = NULL;
        free(passedBits); passedBits = NULL;
    }
}

