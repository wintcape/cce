/**
 * @file filesystem.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Filesystem interface.
 */
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "common.h"

// Type definition for a file handle.
typedef struct
{
    void*   handle;
    bool    valid;
}
file_handle_t;

// File modes.
typedef enum
{
    FILE_MODE_READ  = 0x1
,   FILE_MODE_WRITE = 0x2
}
FILE_MODE;

/**
 * @brief Tests if a file exists.
 * @param p The filepath to test.
 * @return true if file exists; false otherwise.
 */
bool
file_exists
(   const char* p
);

/**
 * @brief Attempts to open a file.
 * @param p The filepath.
 * @param mode Mode flag.
 * @param binary Open in binary mode? Y/N
 * @param handle Output buffer for file handle.
 * @return true if file opened successfully; false otherwise.
 */
bool
file_open
(   const char*     p
,   FILE_MODE       mode
,   bool            binary
,   file_handle_t*  f
);

/**
 * @brief Closes a file.
 * @param f Handle to the file to close.
 */
void
file_close
(   file_handle_t* f
);

/**
 * @brief Reads a file into an output buffer until EOF or line break
 * encountered.
 * @param f Handle to the file to read.
 * @param buf Output buffer for the read data.
 * @return true if file read into buf successfully; false otherwise.
 */
bool
file_read_line
(   file_handle_t*  f
,   char**          buf
);

/**
 * @brief Writes a string to the file and appends a line break.
 * @param f Handle to the file to write to.
 * @param s The string to write.
 * @return true if s written successfully; false otherwise.
 */
bool
file_write_line
(   file_handle_t*  f
,   const char*     s
);

/**
 * @brief Reads a specified amount of data into an output buffer.
 * @param f Handle to the file to read.
 * @param size Number of bytes to read.
 * @param buf Output buffer for the read data.
 * @param read Output buffer to hold number of bytes read.
 * @return true if file read into buf successfully; false otherwise.
 */
bool
file_read
(   file_handle_t*  f
,   u64             size
,   void*           buf
,   u64*            read
);

/**
 * @brief Reads an entire file into an output buffer.
 * @param f Handle to the file to read.
 * @param buf Output buffer for the read data.
 * @param read Output buffer to hold number of bytes read.
 * @return true if file read into buf successfully; false otherwise.
 */
bool
file_read_all
(   file_handle_t*  f
,   u8**            buf
,   u64*            read
);

/**
 * @brief Writes a specified amount of data into an output buffer.
 * @param f Handle to the file to write to.
 * @param size Number of bytes to write.
 * @param data The data to write.
 * @param read Output buffer to hold number of bytes written.
 * @return true if data written to file successfully; false otherwise.
 */
bool
file_write
(   file_handle_t*  f
,   u64             size
,   const void*     data
,   u64*            written
);

#endif  // FILESYSTEM_H
