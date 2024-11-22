// referenced code in section 7.2 to translate the program to C
// https://sysprog21.github.io/lkmpg/#read-and-write-a-proc-file

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// define constants for file name and max buffer size
#define FILE_NAME "buffer.txt"
#define MAX_SIZE 1024
// buffer to store data for file
char buffer[MAX_SIZE];

// function to read from file and returns number bytes read (or -1 when error)
ssize_t readFile(FILE *file) {
    // checks if file successfully opened
    if (file == NULL) {
        perror("File open failed");
        return -1;
    }
    // reads number of bytes up to size of buffer
    size_t bytesRead = fread(buffer, 1, sizeof(buffer), file);
    // checks if data was read
    if (bytesRead > 0) {
        printf("Read from file: %s\n", buffer);
    // checks if no data read
    } else if (bytesRead == 0) {
        printf("End of file reached or no data to read.\n");
    // checks if there was an error when reading file
    } else {
        perror("File read failed");
    }
    // returns number of bytes read from file
    return bytesRead;
}

// function to write to file and returns num bytes written (or -1 when error)
ssize_t writeFile(FILE *file, const char *data, size_t len) {
    // checks if file successfully opened
    if (file == NULL) {
        perror("File open failed");
        return -1;
    }
    // ensure writing within maximaxmum buffer size
    if (len >= MAX_SIZE) {
        len = MAX_SIZE - 1;  // leave space for null terminator
    }
    // writes data to file up to specified length
    size_t bytesWritten = fwrite(data, 1, len, file);
    // checks if data written
    if (bytesWritten > 0) {
        printf("Wrote to file: %s\n", data);
    // otherwise an error occurred during writing
    } else {
        perror("File write failed");
    }
    // returns num bytes written to file
    return bytesWritten;
}

// main method opens a file, writes to it, reads to it, and closes it
int main() {
    // open file for reading and writing
    FILE *file = fopen(FILE_NAME, "w+");
    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // writing data to file
    const char *writeData = "HelloWorld!\n";
    size_t len = strlen(writeData);
    writeFile(file, writeData, len);
    // reposition file pointer to beginning of file before reading
    rewind(file);
    // reading data from file
    readFile(file);
    // closes file
    fclose(file);
}
