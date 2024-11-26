/* Code example referenced from:
https://support.microsoft.com/en-us/topic/how-to-open-a-file-from-a-kernel-mode-device-driver-and-how-to-read-from-or-write-to-the-file-45f7de35-cac4-8d83-9520-a7e480ca404d */

// Kernel-mode device drivers refer to a file with its object name
// refer to file by object name
UNICODE_STRING uniName; // holds path to file
OBJECT_ATTRIBUTES objAttr; // object attributes for file

// initialize Unicode string with file path
RtlInitUnicodeString(&uniName, L"\\DosDevices\\C:\\WINDOWS\\example.txt");
// initialize object attributes with file's Unicode string
InitializeObjectAttributes(&objAttr, &uniName,
                            OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                            NULL, NULL);


// obtain file handle
HANDLE handle; // declare handle for file
NTSTATUS ntstatus; // variable for status codes
IO_STATUS_BLOCK ioStatusBlock; // stores status of I/O operation
// check if operating at correct IRQL level (passive leve) for file I/O
// IRQL defines hardware priority where a processor operates at any given time
if(KeGetCurrentIrql() != PASSIVE_LEVEL)
    return STATUS_INVALID_DEVICE_STATE;
// creates/opens file for writing
// uses file handle and object attributes
ntstatus = ZwCreateFile(&handle,
                        GENERIC_WRITE,
                        &objAttr, &ioStatusBlock, NULL,
                        FILE_ATTRIBUTE_NORMAL,
                        0,
                        FILE_OVERWRITE_IF, 
                        FILE_SYNCHRONOUS_IO_NONALERT,
                        NULL, 0);


// write to file with ZwWriteFile fnc
#define BUFFER_SIZE 30 // preprare size of buffer to write data to file
CHAR buffer[BUFFER_SIZE]; // define actual buffer to hold data
size_t cb; // variable to hold length of string

// check if file created/opened successfully
if(NT_SUCCESS(ntstatus)) {
    // format string into buffer
    ntstatus = RtlStringCbPrintfA(buffer, sizeof(buffer), "This is %d test\r\n", 0x0);
if(NT_SUCCESS(ntstatus)) {
        // get length of string to be written
        ntstatus = RtlStringCbLengthA(buffer, sizeof(buffer), &cb);
        if(NT_SUCCESS(ntstatus)) {
            // writes string to file with ZwWriteFile
            ntstatus = ZwWriteFile(handle, NULL, NULL, NULL, &ioStatusBlock,
                buffer, cb, NULL, NULL);
        }
}
    // closes file handle after writing
    ZwClose(handle);
}


// read from file ZwReadFile fnc
LARGE_INTEGER byteOffset; // offset to start reading from
// creates/opens file for writing
// uses file handle and object attributes
ntstatus = ZwCreateFile(&handle,
                        GENERIC_READ,
                        &objAttr, &ioStatusBlock,
                        NULL,
                        FILE_ATTRIBUTE_NORMAL,
                        0,
                        FILE_OPEN, 
                        FILE_SYNCHRONOUS_IO_NONALERT,
                        NULL, 0);
// checks if file opened successfully and tries to read from it
if(NT_SUCCESS(ntstatus)) {
    // sets read offset to start of file (0 bytes)
    byteOffset.LowPart = byteOffset.HighPart = 0;
    // reads data from file using ZwReadFile
    ntstatus = ZwReadFile(handle, NULL, NULL, NULL, &ioStatusBlock,
                            buffer, BUFFER_SIZE, &byteOffset, NULL);
    // checks if read operation successful
    if(NT_SUCCESS(ntstatus)) {
        buffer[BUFFER_SIZE-1] = '\0'; // null-terminates string
        DbgPrint("%s\n", buffer); // prints contents of buffer
    }
    // close file handle after reading
    ZwClose(handle);
}
