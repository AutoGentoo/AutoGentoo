//
// Created by atuser on 10/22/17.
//

#ifndef HACKSAW_LOG_H
#define HACKSAW_LOG_H

typedef enum __bash_t bash_t;

enum __bash_t {
    BASH_NORMAL,
    BASH_BOLD,
    BASH_NORMAL_WEIGHT,
    BASH_NORMAL_COLOR,
    BASH_RED,
    BASH_GREEN,
    BASH_YELLOW,
    BASH_BLUE,
};

char* bash_code (bash_t code);
void lerror (char* format, ...);
void lwarning(char* format, ...);
void linfo(char* format, ...);

#endif //HACKSAW_LOG_H
