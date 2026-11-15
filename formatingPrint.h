#define RED_PRINTF(format, ...) printf("\033[1;31m" format "\033[0m\n" __VA_OPT__(,) __VA_ARGS__)
#define GREEN_PRINTF(format, ...) printf("\033[1;32m" format "\033[0m\n" __VA_OPT__(,) __VA_ARGS__)

