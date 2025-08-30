#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s arquivo.b\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Erro ao abrir arquivo");
        return 1;
    }

    void *lib = dlopen("./funcs.so", RTLD_LAZY);
    if (!lib) {
        fprintf(stderr, "Não foi possível carregar biblioteca: %s\n", dlerror());
        return 1;
    }

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0; // remover newline

        if (strncmp(line, "call ", 5) == 0) {
            char *cmd = line + 5;
            char *func_name = strtok(cmd, " ");
            char *arg1 = strtok(NULL, " ");
            char *arg2 = strtok(NULL, " ");

            if (!func_name) continue;

            void (*func_void)() = dlsym(lib, func_name);
            char *error;
            if ((error = dlerror()) != NULL)  {
                fprintf(stderr, "Função não encontrada: %s\n", error);
                continue;
            }

            if (arg1 && arg2) {
                int a = atoi(arg1);
                int b = atoi(arg2);
                void (*func_int_int)(int,int) = (void (*)(int,int))func_void;
                func_int_int(a,b);
            } else {
                func_void();
            }
        } else {
            printf("Comando desconhecido: %s\n", line);
        }
    }

    fclose(f);
    dlclose(lib);
    return 0;
}

