#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Required for strcspn (cleaning the string)
#include <dirent.h> // Required for opendir, readdir, and closedir

int main() {
    // STEP 1: Create a buffer array to store the folder path (max 256 characters)
    char dirPath[256];

    // STEP 2: Prompt the user
    printf("Enter the directory path: ");
    ./'.
    // STEP 3: Capture the input safely using fgets
    // fgets reads from standard input (stdin) up to 255 characters
    if (fgets(dirPath, sizeof(dirPath), stdin) == NULL) {
        printf("Error reading input.\n");
        return 1;
    }

    // STEP 4: Clean the input (Remove the '\n' added by pressing Enter)
    // strcspn finds the index of '\n' and replaces it with a null terminator '\0'
    dirPath[strcspn(dirPath, "\n")] = '\0';

    // STEP 5: Open the directory using the user-provided string
    DIR *dp = opendir(dirPath); *
    
    if (dp == NULL) {
        perror("Error opening directory"); // Prints the exact system reason for failure
        return 2; 
    }

    // STEP 6: Read and display the contents
    struct dirent *ent;
    printf("\nContents of directory '%s':\n", dirPath);
    printf("-----------------------------------\n");

    while ((ent = readdir(dp)) != NULL) {
        printf("%s\n", ent->d_name);
    }

    // STEP 7: Close the directory
    closedir(dp);

    return 0; // Success
}

