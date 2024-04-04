#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

FILE *keyptr, *plntxtptr;

//Prints out the key
void printKey();
//Removes all of the non alphabet characters and lowers all uppercase characters
char *cleanText(char *rawText, int *length);
//Encrypts cleaned plaintext using the Hill Cipher encryption method
char *hillCipher(char *cleanedText, int cleanedTextLen, int *matrix, int keyRows);

int main(int argc, char **argv) {
    printf("Hill Cipher Encryption tool\n\n");
    //Opens up the key text file and the plaintext file
    keyptr = fopen(argv[1], "r");
    plntxtptr = fopen(argv[2], "r");

    //If any of the files were mistyped, the program exits out
    if (keyptr == NULL || plntxtptr == NULL) {
        printf("Error opening input file\n");
        return 1;
    }

    //Key Output
    printKey();

    int numKeyRows;
    fscanf(keyptr, "%d", &numKeyRows);
    //Populates the key matrix
    int keyMatrix[numKeyRows][numKeyRows];
    for (int i = 0; i < numKeyRows; i++) {
        for (int j = 0; j < numKeyRows; j++)
            fscanf(keyptr, "%d", &keyMatrix[i][j]);
    }

    char rawPlainText[10000];
    char rawPlainLine[10000];
    rawPlainText[0] = '\0';
    //Concatenates each line into a string to contain the entire raw plaintext
    while (fgets(rawPlainLine, 10000, plntxtptr))
        strcat(rawPlainText, rawPlainLine);
    int cleanedTextLength;
    char *cleanedPlainText = cleanText(rawPlainText, &cleanedTextLength);

    //Appends the x's at the end of uneven plaintexts
    if(cleanedTextLength % numKeyRows != 0){
        for(int i = 0; i < numKeyRows; i++){
            cleanedPlainText[cleanedTextLength] = 'x';
            cleanedTextLength ++;

            if(cleanedTextLength % numKeyRows == 0)
                break;
        }
    }
        

    //Plaintext output
    printf("Plaintext:\n");
    int i = 0, j = 0;
    while (cleanedPlainText[i] != '\0') {
        //Divides the plaintext by lines of 80 characters max
        if(j == 80) {
            printf("\n");
            j = 0;
        }
        printf("%c", cleanedPlainText[i]);
        i++;
        j++;
    }
    printf("\n\n");

    char *cipherText = hillCipher(cleanedPlainText, cleanedTextLength, (int *)keyMatrix, numKeyRows);
    //Ciphertext output
    printf("Ciphertext:\n");
    i = 0, j = 0;
    while (cipherText[i] != '\0') {
        //Divides the plaintext by lines of 80 characters max
        if(j == 80) {
            printf("\n");
            j = 0;
        }
        printf("%c", cipherText[i]);
        i++;
        j++;
    }
    printf("\n\n");

    //Closes the text files
    fclose(keyptr);
    fclose(plntxtptr);
    //Frees what was dynamically allocated in cleanText() and hillCipher() functions
    free(cleanedPlainText);
    free(cipherText);

    return 0;
}

void printKey() {
    int keyMatrixLines;
    fscanf(keyptr, "%d", &keyMatrixLines);
    printf("Key Matrix:");
    char keyOutput[keyMatrixLines][40];
    for (int i = 0; i < keyMatrixLines; i++) {
    while(fgets(keyOutput[i], 40, keyptr))
        printf("   %s", keyOutput[i]);
    }
    printf("\n\n");
    //resets the file pointer to the beginning of the file
    rewind(keyptr);
}

char *cleanText(char *rawText, int *length) {
    //Dynamically allocates enough memory to hold 80 characters + null pointer
    char *cleanedText = malloc(10000);
    //int i follows the rawText string
    int i = 0;
    //int j follows the cleanedText string
    int j = 0;
    while (rawText[i] != '\0') {
        //The following 3 if-statements skip non-alphabetical characters
        if(rawText[i] < 65) {
            i++;
            continue;
        }
        if (rawText[i] > 90 && rawText[i] < 97) {
            i++;
            continue;
        }
        if (rawText[i] > 122) {
            i++;
            continue;
        }
        //Converts uppercase character in raw text into a lowercase character, moves it to cleaned text
        if (rawText[i] > 64 && rawText[i] < 91) {
            cleanedText[j] = tolower(rawText[i]);
            i++;
            j++;
            continue;
        }
        //Assuming current raw character is lowercase, moves it to cleaned text
        if (rawText[i] > 96 && rawText[i] < 123) {
            cleanedText[j] = rawText[i];
            i++;
            j++;
            continue;
        }
    }

    *length = j;

    return cleanedText;
}

char *hillCipher(char *cleanedText,  int cleanedTextLen, int *keyMatrix, int keyRows) {
    //Dynamically allocates enough to hold 80 characters + null pointer
    char *cipheredText = malloc(10000);
    int textMatrix[keyRows];
    int transferMatrix[keyRows];

    for (int i = 0; i < cleanedTextLen; i+=keyRows) {
   
        for (int i = 0; i < keyRows; i++) {
            transferMatrix[i] = 0;
        }

        for (int j = 0; j < keyRows; j++) {
            if(cleanedText[i] != '\0') {
                textMatrix[j] = cleanedText[i];
                textMatrix[j] -= 'a'; 
                i++;
            }
            else {
                textMatrix[j] = 'x';
                textMatrix[j] -= 'a'; 

                i++;
            }
        }

        for (int k = 0; k < keyRows; k++) {
            for (int l = 0; l < keyRows; l++) {
                transferMatrix[k] += (*((keyMatrix + k * keyRows) + l)) * (textMatrix[l]);
            }

            transferMatrix[k] %= 26;
            transferMatrix[k] += 'a';
        }
        i -= keyRows;
        for (int m = 0; m < keyRows; m++) {
            cipheredText[i] = transferMatrix[m];
            i++;
        }
        i -= keyRows;
    }

    return cipheredText;
}