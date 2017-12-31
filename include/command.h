#ifndef __AUTOGENTOO_COMMAND_H__
#define __AUTOGENTOO_COMMAND_H__

#include <hacksaw/tools.h>

typedef struct __Command Command;

struct __Command {
    char* format;
    int argc;
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
 * @param ... the arguments to pass to the command
 */
void command_run (Command* cmd, char** output, int* ret, ...);

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
 */
void command (char* top, char* bottom, char** output, int* ret);

/*
 * Registered commands
 * All commands are held in small maps
 * 
 */

SmallMap* 

#endif