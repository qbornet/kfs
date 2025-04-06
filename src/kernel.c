#include <stdint.h>

void kernel_main(void) {
    char *video_memory = (char *)0xB8000;
    const char *message = "Hello World!";
    int i = 0;

    while (message[i] != '\0') {
        video_memory[i * 2] = message[i];
        video_memory[i * 2 + 1] = 0x07; // attribute 0x07 (light gray on black)
        i++;
    }
    
    while (1) { }
}
