#define _GNU_SOURCE

#include <stdio.h>
#include <autogentoo/command.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

SmallMap* all_commands = NULL;
SmallMap* tar = NULL;
SmallMap* wget = NULL;

// Private functions
SmallMap* init_cmd_tar (char* dest) {
    strcpy (dest, "tar");
    tar = small_map_new (sizeof (Command*), 5);
    small_map_insert (tar, "extract", command_new ("tar -xf %s", 1));
    small_map_insert (tar, "extract to", command_new ("tar -xf %s -C %s", 2));
    small_map_insert (tar, "compress", command_new ("tar -cf %s %s", 2));
    return tar;
}

SmallMap* init_cmd_wget (char* dest) {
    strcpy (dest, "wget");
    wget = small_map_new (sizeof (Command*), 5);
    small_map_insert (wget, "download", command_new ("wget %s", 1));
    small_map_insert (wget, "download to", command_new ("wget %s -O %s", 2));
    small_map_insert (wget, "download quiet", command_new ("wget %s --quiet", 1));
    small_map_insert (wget, "download to quiet", command_new ("wget %s -O %s --quiet", 2));
    
    return wget;
}

SmallMap* (* cmd_init_list[]) (char* dest) = {
        init_cmd_tar,
        init_cmd_wget
};

void init_commands (void) {
    all_commands = small_map_new (sizeof (SmallMap*), 5);
    
    int i;
    for (i = 0; i != sizeof (cmd_init_list) / sizeof (cmd_init_list[0]); i++) {
        char c_cmd[32];
        SmallMap* t = cmd_init_list[i] (c_cmd);
        small_map_insert (all_commands, c_cmd, t);
    }
}

void free_commands (void) {
    int i;
    for (i = 0; i != all_commands->n; i++) {
        SmallMap* c = (*(SmallMap***)vector_get (all_commands, i))[1];
        int j;
        for (j = 0; j != c->n; j++) {
            command_free ((*(Command***)vector_get (c, j))[1]);
        }
        small_map_free (c, 0);
    }
    small_map_free (all_commands, 0);
}

Command* command_new (char* fmt, int argc) {
    Command* out = malloc (sizeof (Command));
    out->format = strdup (fmt);
    out->argc = argc;
    
    return out;
}

void command_run (Command* cmd, char** output, int* ret, va_list args) {
    char* dest;
    vasprintf (&dest, cmd->format, args);
    linfo ("running %s", dest);
    
    FILE* fp = popen (dest, "r");
    if (!fp)
        return lerror ("Failed to run command!");
    
    if (output) {
        size_t alloc_increm = 128;
        size_t output_allocation = 256;
        size_t cread, read_len = 0;
        char buffer[256];
        *output = malloc (output_allocation);
        
        while ((cread = fread (buffer, 1, sizeof (buffer), fp)) != 0) {
            if (cread + read_len >= output_allocation) {
                output_allocation = cread + read_len + alloc_increm;
                *output = realloc (*output, output_allocation);
            }
            memcpy (*output + read_len, buffer, cread);
            read_len += cread;
        }
    }
    
    if (ret)
        *ret = pclose (fp);
}

void command_free (Command* cmd) {
    free (cmd->format);
    free (cmd);
}

void command (char* top, char* bottom, char** output, int* ret, ...) {
    SmallMap* top_level = small_map_get (all_commands, top);
    if (!top_level) {
        *ret = -1;
        return lerror ("Command '%s' could not be found", top);
    }
        
    Command* com = small_map_get (top_level, bottom);
    
    if (!com) {
        *ret = -1;
        return lerror ("Command '%s' '%s' could not be found", top, bottom);
    }
    
    va_list args;
    va_start (args, ret);
    command_run (com, output, ret, args);
    va_end (args);
}

