#ifndef _W_MESSAGE_H
#define _W_MESSAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct message {
    char *first_line;
    char *second_line;
    char *third_line;
    char *fourth_line;
    char *fifth_line;
};

void update_message(struct message *msg, const char *first_line, const char *second_line, const char *third_line, const char *fourth_line, const char *fifth_line);
void print_message(const struct message *msg);

#endif
