#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_WORD_LENGTH 150
#define MAX_THREADS 100

// Global variables
int finalErr = 0;  // Total number of misspellings
int threadActiveCurrently = 0;  // Number of active threads

// Structure to represent a myWord and its count
typedef struct {
    char myWord[100];  // Word
    int count;  // Count of occurrences
} countWord;
// Structure to hold parameters for spell check task
typedef struct {
    const char *fileDictionaryName;  // Name of the dictionary file
    countWord **threeFinalMispellings;  // Pointer to an array of top three misspellings
    const char *fileTextName;  // Name of the text file
} paramatersForSpellCheck;

pthread_t tid[MAX_THREADS];  // Array to store thread IDs
pthread_mutex_t gMutex;  // Mutex for thread synchronization

// Function to clear input buffer
void bufClear() {
    int character;
    while ((character = getchar()) != '\n' && character != EOF);
}

void *spellCheckTask(void *arg) {
    // Variable declarations
    int errors = 0; // Count of errors found
    int mispellings = 0; // Count of misspelled words
    int wordIndex; // Index for keeping track of words
    
    char wordFromDictionary[MAX_WORD_LENGTH]; // Buffer for word from dictionary
    char myWord[MAX_WORD_LENGTH]; // Buffer for word from text

    // Extract parameters from argument
    paramatersForSpellCheck *myParamaters = (paramatersForSpellCheck *)arg; // Cast the argument to paramatersForSpellCheck type
    const char *fileTextName = myParamaters->fileTextName; // Name of the text file
    const char *fileDictionaryName = myParamaters->fileDictionaryName; // Name of the dictionary file
    countWord **threeFinalMispellings = myParamaters->threeFinalMispellings; // Array for three final misspellings

    // Allocate memory for misspelled words and top misspellings
    countWord **wordsMispelled = (countWord **)malloc(sizeof(countWord *)); // Allocate memory for pointers to countWord
    countWord **mispellingsTop = (countWord **)malloc(sizeof(countWord *)); // Allocate memory for pointers to countWord
    *wordsMispelled = NULL; // Initialize to NULL

    // Open text file for reading
    char threadTextFile[MAX_WORD_LENGTH]; // Buffer for thread-specific text file name
    strcpy(threadTextFile, fileTextName); // Copy the fileTextName to threadTextFile
    FILE *dictionary = fopen(fileDictionaryName, "r"); // Open dictionary file in read mode
    FILE *text = fopen(threadTextFile, "r"); // Open text file in read mode
    

    if (text == NULL) { // Check if text file is opened successfully
        printf("There has been an issue opening the text file.\n"); // Print error message
        exit(EXIT_FAILURE); // Exit program with failure status
    }

    if (dictionary == NULL) { // Check if dictionary file is opened successfully
        printf("There has been an issue opening the dictionary file.\n"); // Print error message
        exit(EXIT_FAILURE); // Exit program with failure status
    }

    // Loop through the text file until end of file is reached
    while (!feof(text)) {
        fscanf(text, "%s", myWord); // Read a word from the text file

        int i = 0; // Initialize loop variable
        int finder = 0; // Flag to indicate if word is found in dictionary
        
        // Convert the word to lowercase for case-insensitive comparison
        while (myWord[i] != '\0') {
            myWord[i] = tolower(myWord[i]); // Convert character to lowercase
            i++; // Move to next character
        }
        rewind(dictionary); // Reset file pointer to beginning of dictionary

        // Loop through the dictionary to find a match for the word
        while (!feof(dictionary)) {
            fscanf(dictionary, "%s", wordFromDictionary); // Read a word from the dictionary

            // Convert the word from the dictionary to lowercase
            i = 0; // Initialize loop variable
            while (wordFromDictionary[i] != '\0') {
                wordFromDictionary[i] = tolower(wordFromDictionary[i]); // Convert character to lowercase
                i++; // Move to next character
            }
            // Compare the words from text and dictionary
            if (strcmp(myWord, wordFromDictionary) == 0) {
                finder = 1; // Word found in dictionary
                break; // Exit the loop
            }
        }

        // If word is not found in the dictionary, increment error count and add to misspelled words
        if (!finder) {
            errors++; // Increment error count

            wordIndex = -1; // Initialize wordIndex
            int i = 0; // Initialize loop variable
            while (i < mispellings) { // Loop through existing misspelled words
                if (strcmp((*wordsMispelled)[i].myWord, myWord) == 0) { // Check if word already exists in list
                    wordIndex = i; // Update wordIndex
                    break; // Exit the loop
                }
                i++; // Move to next word
            }

            if (wordIndex == -1) { // If word is not in the list, add it
                *wordsMispelled = (countWord *)realloc(*wordsMispelled, (mispellings + 1) * sizeof(countWord)); // Reallocate memory for misspelled words
                strcpy((*wordsMispelled)[mispellings].myWord, myWord); // Copy the word to wordsMispelled array
                (*wordsMispelled)[mispellings].count = 1; // Initialize count to 1
                mispellings++; // Increment the count of misspelled words
            } 
            else { // If word already exists, increment its count
                (*wordsMispelled)[wordIndex].count++; // Increment count of the existing misspelled word
            }
        }
        pthread_testcancel(); // Allow cancellation of the thread
    }

    // Allocate memory for top misspellings
    *mispellingsTop = (countWord *)malloc(sizeof(countWord) * 3); // Allocate memory for top misspellings
    int i = 0; // Initialize loop variable
    while (i < 3) { // Loop through top misspellings array
        strcpy((*mispellingsTop)[i].myWord, ""); // Initialize word to empty string
        (*mispellingsTop)[i].count = 0; // Initialize count to 0
        i++; // Move to next element
    }

    // Find top 3 misspellings
    i = 0; // Reset loop variable
    while (i < mispellings) { // Loop through the misspelled words
        int j = 0; // Initialize loop variable
        while (j < 3) { // Loop through top misspellings array
            if ((*wordsMispelled)[i].count > (*mispellingsTop)[j].count) { // Check if count of current misspelled word is greater than count of top misspelling
                int k = 2; // Initialize loop variable
                while (k > j) { // Shift elements in top misspellings array to make space for new misspelling
                    strcpy((*mispellingsTop)[k].myWord, (*mispellingsTop)[k - 1].myWord); // Copy word
                    (*mispellingsTop)[k].count = (*mispellingsTop)[k - 1].count; // Copy count
                    k--; // Move to previous element
                }
                strcpy((*mispellingsTop)[j].myWord, (*wordsMispelled)[i].myWord); // Copy word
                (*mispellingsTop)[j].count = (*wordsMispelled)[i].count; // Copy count
                break; // Exit the loop
            }
            j++; // Move to next element in top misspellings array
        }
        i++; // Move to next misspelled word
    }

    // Close files
    fclose(text); // Close text file
    fclose(dictionary); // Close dictionary file

    // Update thread count
    pthread_mutex_lock(&gMutex); // Lock mutex to ensure atomicity
    threadActiveCurrently--; // Decrement active thread count

    // Write results to file
    FILE *file = fopen("patelo_A2.out", "a"); // Open output file in append mode
    if (file == NULL) { // Check if file is opened successfully
        printf("There has been an error opening the file.\n"); // Print error message
        exit(EXIT_FAILURE); // Exit program with failure status
    }
    fprintf(file, "%s %d ", threadTextFile, errors); // Write thread's text file name and error count
    i = 0; // Reset loop variable
    while (i < 3) { // Loop through top misspellings array
        fprintf(file, "%s ", (*mispellingsTop)[i].myWord); // Write misspelled word
        i++; // Move to next element
    }
    fprintf(file, "\n"); // Write newline character
    fclose(file); // Close output file

    // Update three final misspellings
    i = 0; // Reset loop variable
    while (i < 3) { // Loop through top misspellings
        int j = 0; // Initialize loop variable
        while (j < 3) { // Loop through three final misspellings
            if (((*mispellingsTop)[i].count > (*threeFinalMispellings)[j].count)) { // Compare counts
                int k = 2; // Initialize loop variable
                while (k > j) { // Shift elements to make space for new misspelling
                    strcpy((*threeFinalMispellings)[k].myWord, (*threeFinalMispellings)[k - 1].myWord); // Copy word
                    (*threeFinalMispellings)[k].count = (*threeFinalMispellings)[k - 1].count; // Copy count
                    k--; // Move to previous element
                }
                strcpy((*threeFinalMispellings)[j].myWord, (*mispellingsTop)[i].myWord); // Copy word
                (*threeFinalMispellings)[j].count = (*mispellingsTop)[i].count; // Copy count
                break; // Exit the loop
            }
            j++; // Move to next element
        }
        i++; // Move to next element
    }

    // Update final error count
    finalErr = errors + finalErr; // Add errors to final error count

    pthread_mutex_unlock(&gMutex); // Unlock mutex

    // Free memory
    free(*wordsMispelled); // Free memory for misspelled words
    free(*mispellingsTop); // Free memory for top misspellings
    free(wordsMispelled); // Free memory for pointer to misspelled words
    free(mispellingsTop); // Free memory for pointer to top misspellings

    // Return NULL to signify successful completion of the thread
    return NULL;
}

// Define the main function with command-line arguments
int main(int argc, char **argv) {

    // Declare variables to track thread count, file saving option, user response, and total files processed
    int threads = 0; 
    int fileSaver = 0;
    int userResponse;
    int fileTotal = 0;
    
    // Dynamically allocate memory for an array to store the three most common misspellings
    countWord *threeFinalMispellings = (countWord *)malloc(sizeof(countWord) * 3);
    
    // Initialize counter variable
    int i = 0;
    
    // Initialize the misspellings array with empty strings and zero counts
    while (i < 3) {
        strcpy(threeFinalMispellings[i].myWord, "");
        threeFinalMispellings[i].count = 0;
        i++;
    }

    // Initialize mutex for thread synchronization
    pthread_mutex_init(&gMutex, NULL);

    // Check if the program is run with a specific flag for file saving
    if (argc > 1 && strcmp(argv[1], "-l") == 0) {
        fileSaver = 1; // Set fileSaver flag to true
    }

    // Main loop
    do {
        // Display main menu options
        printf("\n -----------> Main Menu <-----------\n");
        printf("(Enter 1) : To start a new spellchecking task.\n");
        printf("(Enter 2): Exit\n");
        printf("Enter your response: ");
        scanf("%d", &userResponse); // Take user input
        bufClear(); // Clear input buffer

        switch(userResponse){

            case 1: // Start a new spellchecking task
                
                // Check if maximum thread limit is reached
                if (threads >= MAX_THREADS) {
                    printf("Maximum number of ongoing threads reached.\n");
                    break;
                }

                printf("\nStarting a new spell checking task ....\n\n");

                // Declare arrays to store file names
                char fileTextName[100];
                char dictFileName[100];

                while(1) {
                    // Prompt user to enter dictionary file name
                    printf("Enter the filename of the dictionary file (Enter 0 to return to the main menu): ");
                    scanf("%s", dictFileName);

                    // If user wants to return to the main menu
                    if (strcmp(dictFileName, "0") == 0) {
                        printf("Going back to the main menu.\n");
                        break;
                    }

                    // Prompt user to enter text file name
                    printf("\nEnter the filename of the text file (Enter 0 to return to the main menu): ");
                    scanf("%s", fileTextName);

                    // If user wants to return to the main menu
                    if (strcmp(fileTextName, "0") == 0) {
                        printf("Going back to the main menu.\n");
                        break;
                    }                

                    // Open dictionary and text files in read mode
                    FILE *dictionary = fopen(dictFileName, "r");
                    FILE *text = fopen(fileTextName, "r");
                    

                   // Check if files are successfully opened
                   if (text == NULL || dictionary == NULL) {
                        // Print error messages for file opening failures
                        if (text == NULL && dictionary == NULL) {
                            printf("\nError: Unable to open both the text and dictionary files. Please check the file names and try again.\n");
                        } else if (text == NULL) {
                            printf("\nError: Unable to open the text file.\n");
                            printf("Please check the file name and try again.\n");
                        } else if (dictionary == NULL) {
                            printf("\nError: Unable to open the dictionary file.\n");
                            printf("Please check the file name and try again.\n");
                        }
                        // Close files if they are opened
                        if (text != NULL) {
                            fclose(text);
                        } 
                        if (dictionary != NULL) {
                            fclose(dictionary);
                        }
                        continue; // Continue to the next iteration of the loop
                    }

                    // Prepare parameters for spell check task
                    paramatersForSpellCheck parameters;
                    parameters.fileTextName = fileTextName;
                    parameters.fileDictionaryName = dictFileName;
                    parameters.threeFinalMispellings = &threeFinalMispellings;

                    // Create a new thread for spell check task
                    pthread_create(&tid[threads], NULL, spellCheckTask, (void *)&parameters);

                    // Lock mutex to update active thread count
                    pthread_mutex_lock(&gMutex);
                    threadActiveCurrently++;
                    pthread_mutex_unlock(&gMutex);
                    threads++; // Increment thread count
                    fileTotal++; // Increment total file count

                    break;
                }

                break;
    
            case 2: // Exit the program
                pthread_mutex_lock(&gMutex);
                // If no threads are active
                if (threadActiveCurrently == 0) {
                    pthread_mutex_unlock(&gMutex);

                    // If file saving is enabled
                    if(fileSaver){
                        FILE *file = fopen("patelo_A2.out", "a"); // Open output file in append mode
                        if (file == NULL) {
                            printf("Error: Unable to open file for writing.\n");
                            return 1;
                        }
                        // Write final summary to the output file
                        fprintf(file, "-----> Final Summary <-----\n");
                        fprintf(file, "Number of files processed: %d\n", fileTotal);
                        fprintf(file, "Number of spelling errors: %d\n", finalErr);
                        fprintf(file, "\nTop three most common misspellings:\n");
                        i = 0;
                        while (i < 3) {
                            fprintf(file, "%s - %d times\n", threeFinalMispellings[i].myWord, threeFinalMispellings[i].count);
                            i++;
                        }
                        fclose(file); // Close the file
                    }

                    // If file saving is not enabled
                    else{
                        printf("No threads are running. Will be exiting the program.\n");
                        printf("\n-----> Final Summary <-----\n");
                        pthread_mutex_lock(&gMutex);
                        printf("Number of files processed: %d\n", fileTotal);
                        printf("Number of spelling errors: %d\n", finalErr);

                        printf("\nTop three most common misspellings:\n");
                        i = 0;
                        while (i < 3) {
                            printf("%s - %d times\n", threeFinalMispellings[i].myWord, threeFinalMispellings[i].count);
                            i++;
                        }
                        pthread_mutex_unlock(&gMutex);
                    }
                    i = 0;
                    // Wait for all threads to finish
                    while (i < threadActiveCurrently) {
                        pthread_join(tid[i], NULL);
                        i++;
                    }
                    free(threeFinalMispellings); // Free allocated memory
                } 
                else { // If there are active threads

                    printf("Cancelling running threads ........\n");
                    i = 0;
                    // Cancel all running threads
                    while (i < threads) {
                        pthread_cancel(tid[i]);
                        i++;
                    }

                    i = 0;
                    // Wait for all threads to finish
                    while (i < threadActiveCurrently) {
                        pthread_join(tid[i], NULL);
                        i++;
                    }

                    // If file saving is enabled
                    if(fileSaver){
                        FILE *file = fopen("patelo_A2.out", "a"); // Open output file in append mode
                        if (file == NULL) {
                            printf("Error: Unable to open file for writing.\n");
                            return 1;
                        }
                        // Write final summary to the output file
                        fprintf(file, "-----> Final Summary <-----\n");
                        fprintf(file, "Number of files processed: %d\n", fileTotal);
                        fprintf(file, "Number of spelling errors: %d\n", finalErr);
                        fprintf(file, "\nTop three most common misspellings:\n");
                        i = 0;
                        while (i < 3) {
                            fprintf(file, "%s - %d times\n", threeFinalMispellings[i].myWord, threeFinalMispellings[i].count);
                            i++;
                        }
                        fclose(file); // Close the file
                    }

                    // If file saving is not enabled
                    else{
                        printf("\n-----> Final Summary <-----\n");
                        printf("Number of running threads: %d\n", threadActiveCurrently);
                        printf("Number of files processed: %d\n", fileTotal - threadActiveCurrently);
                        printf("Number of spelling errors: %d\n", finalErr);
                        printf("\nTop three most common misspellings:\n");
                        i = 0;
                        while (i < 3) {
                            printf("%s - %d times\n", threeFinalMispellings[i].myWord, threeFinalMispellings[i].count);
                            i++;
                        }
                    }
                    free(threeFinalMispellings); // Free allocated memory
                }
                break;

            default: // Invalid user input
                printf("Invalid input. Please enter 1 or 2.\n");
        }

    } while (userResponse != 2); // Repeat until user chooses to exit

    return 0; // Exit the program
}
