#ifndef UTILS_INC
#define UTILS_INC

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

//Colors for printf
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define BOLD  "\x1B[1m"
#define RESET "\x1B[0m"

#define SET_BIT(bits, pos)   (bits |= 1 << (pos))
#define RESET_BIT(bits, pos) (bits &= ~(1 << (pos)))

#ifndef __FILENAME__
#define __FILENAME__ (strrchr("/" __FILE__, '/') + 1)
#endif

#ifndef ERROR
#define ERROR(x, ...)     (fprintf(stderr, RED "Error : " GRN "%s:%d " RESET x, __FILENAME__, __LINE__, ## __VA_ARGS__))
#endif

#ifndef WARNING
#define WARNING(x, ...)   (fprintf(stdout, YEL "Warning : " GRN "%s:%d " RESET x, __FILENAME__, __LINE__, ## __VA_ARGS__))
#endif

#ifndef INFO
#define INFO(x, ...)   (fprintf(stdout, BOLD WHT "INFO : " RESET GRN "%s:%d " RESET x , __FILENAME__, __LINE__, ## __VA_ARGS__))
#endif

#endif
