#include <stdio.h>

int main(int argc, char** argv)
{
    FILE* f = fopen(argv[1], "rb");
    FILE* of = fopen(argv[2], "w");
    char buffer[128] = {0};
    fprintf(of, " = {\n");
    while(1) {
        size_t s = fread(buffer, 1, 128, f);
        if (s > 0) {
            for (int i = 0; i < s; ++i) {
                fprintf(of, "0x%02hhx,", buffer[i]);
                if (i % 10 == 0) {
                    fprintf(of, "\n");
                }
            }
        }
        if (s < 128) {
            break;
        }
    }
    fprintf(of, " };\n");
    fclose(of);
    return 0;
}