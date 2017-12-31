#ifndef __AUTOGENTOO_COMMAND_H__
#define __AUTOGENTOO_COMMAND_H__

typedef struct __Command Command;

struct __Command {
    char* format;
    int argc;
    
    int ret;
    char* output;
};

Command* command_new (char* fmt, int argc);
void command_run (Command* cmd, ...);
void command_reset (Command* cmd);

#endif