#include "message.h"

void update_message(struct message *msg, const char *first_line, const char *second_line, const char *third_line, const char *fourth_line, const char *fifth_line) {
    if (!msg) {
        return; // Handle null pointer
    }
    if (msg->first_line) {
        printf("free first_line\n");
        free(msg->first_line);
    }
    if (msg->second_line) {
        printf("free second_line\n");
        free(msg->second_line);
    }
    if (msg->third_line) {
        printf("free third_line\n");
        free(msg->third_line);
    }
    if (msg->fourth_line) {
        printf("free fourth_line\n");
        free(msg->fourth_line);
    }
    if (msg->fifth_line) {
        printf("free fifth_line\n");
        free(msg->fifth_line);
    }

    msg->first_line = strdup(first_line);
    msg->second_line = strdup(second_line);
    msg->third_line = strdup(third_line);
    msg->fourth_line = strdup(fourth_line);
    msg->fifth_line = strdup(fifth_line);
    printf("Message updated:\n");
    printf("First line: %s\n", msg->first_line);
    printf("Second line: %s\n", msg->second_line);
    printf("Third line: %s\n", msg->third_line);
    printf("Fourth line: %s\n", msg->fourth_line);
    printf("Fifth line: %s\n", msg->fifth_line);
}

void print_message(const struct message *msg) {
    if (msg->first_line) {
        printf("First line: %s\n", msg->first_line);
    } else {
        printf("First line is NULL\n");
    }

    if (msg->second_line) {
        printf("Second line: %s\n", msg->second_line);
    } else {
        printf("Second line is NULL\n");
    }
}

void update_first_line(struct message *msg, const char *first_line) {
    if (!msg || !first_line) {
        return;
    }
    if (msg->first_line) {
        free(msg->first_line);
    }
    msg->first_line = strdup(first_line);
}

void update_second_line(struct message *msg, const char *second_line) {
    if (!msg || !second_line) {
        return;
    }
    if (msg->second_line) {
        free(msg->second_line);
    }
    msg->second_line = strdup(second_line);
}

void update_third_line(struct message *msg, const char *third_line) {
    if (!msg || !third_line) {
        return;
    }
    if (msg->third_line) {
        free(msg->third_line);
    }
    msg->third_line = strdup(third_line);
}

void update_fourth_line(struct message *msg, const char *fourth_line) {
    if (!msg || !fourth_line) {
        return;
    }
    if (msg->fourth_line) {
        free(msg->fourth_line);
    }
    msg->fourth_line = strdup(fourth_line);
}

void update_fifth_line(struct message *msg, const char *fifth_line) {
    if (!msg || !fifth_line) {
        return;
    }
    if (msg->fifth_line) {
        free(msg->fifth_line);
    }
    msg->fifth_line = strdup(fifth_line);
}
