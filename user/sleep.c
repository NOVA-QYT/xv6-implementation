#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(2, "Argument number incorrect!");
        exit(1);
    }
    char *s = argv[1];
    int time = atoi(s);
    sleep(time);
    exit(0);
}
