#include <command.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <hacksaw/tools.h>

Command* command_new (char* fmt, int argc) {
    Command* out = malloc (sizeof (Command));
    out->format = strdup (fmt);
    out->argc = argc;
    
    out->output = NULL;
    out->ret = -1;
}
void command_run (Command* cmd, char** output, int* ret, ...) {
    va_list args;
    va_start (args, ret);
    
    char* dest;
    vasprintf(&dest, cmd->format, args);
    linfo ("running %s", dest);
    
    FILE* fp = popen (dest, "r");
    if (!fp) {
        lerror("Failed to run command!");
        return;
    }
    
    size_t alloc_increm = 128;
    size_t output_allocation = 256;
    size_t cread, read_len = 0;
    char buffer[256];
    cmd->output = malloc (output_allocation);
    
    while ((cread = fread(buffer, 1, sizeof(buffer), fp)) != 0) {
        if (cread + read_len >= output_allocation) {
            output_allocation = cread + read_len + alloc_increm;
            cmd->output = realloc(cmd->output, output_allocation);
        }
        memcpy (cmd->output + read_len, buffer, cread);
        read_len += cread;
    }
    
    cmd->ret = pclose(fp);
}

void command_reset (Command* cmd) {
    if (cmd->output != NULL) {
        free (cmd->output);
    }
    cmd->output = NULL;
    cmd->ret = -1;
}

void command_free (Command* cmd) {
    command_reset(cmd);
    free(cmd->format);
    free (cmd);
}

void command(char *top, char *bottom, char **output, int *ret, ...) {
    
}
