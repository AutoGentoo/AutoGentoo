#ifndef __AUTOGENTOO_COMMAND_H__
#define __AUTOGENTOO_COMMAND_H__

#include <hacksaw/tools.h>

/**
 * @brief Holds information regarding a commandline call
 */
typedef struct __Command Command;

/**
 * @brief Holds information regarding a commandline call
 */
struct __Command {
    char* format; //!< Format for the command in printf form
    int argc; //!< The number of arguments to look for when passing the command
};

/**
 * Creates a new command given a format and argument count
 * @param fmt printf command passed to terminal
 * @param argc argument count to pass to fmt
 * @return a pointer to the new command
 */
Command* command_new (char* fmt, int argc);

/**
 * Run a command given its arguments
 * @param cmd the command to run
 * @param output A pointer to the string to which the output is written
 * @param ret a pointer to which the return value of the command is written
 * @param args the arguments to pass to the command
 */
void command_run (Command* cmd, char** output, int* ret, va_list args);

/**
 * Free a command
 * @param cmd the command to free
 */
void command_free (Command* cmd);

/**
 * Call a registered command
 * @param top the main class of command to call (eg. tar)
 * @param bottom the sub-level class of command to call (eg. extract)
 * @param output a pointer to where the output will be written
 * @param ret a pointer to where the return value will be stored
 * @param ... the arguments to pass to the command
 */
void command (char* top, char* bottom, char** output, int* ret, ...);

void init_commands (void) __attribute__ ((constructor));

void free_commands (void) __attribute__ ((destructor));

/**
 * @brief All of the registered commands
 * 
 * All commands are held in small maps
 */
extern SmallMap* all_commands;


/**
 * @brief Extract and Compress files 
 * 
 * Valid commands
 *  - extract {file.tar} : Extract file.tar to the current directory
 *  - extract to {file.tar, path} : Extract file.tar to path
 *  - compress {file.tar, selector} : Compress files found through selector into file.tar
 */
extern SmallMap* tar;

/**
 * @brief Download files from the internet
 * 
 * Valid commands
 *  - download : Normal download (name will be the name of the file on the internet)
 *  - download to {path_to_file} : Download to file
 *  Both of the above have a 'quiet' vairant (append quiet to them to not print the logs)
 */
extern SmallMap* wget;

#endif