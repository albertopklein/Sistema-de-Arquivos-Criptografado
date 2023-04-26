#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "crypfs.h"

int main(){
    int i, k, write, tam=100;//juca2, write, read;
    indice_arquivo_t x1,x2,x3,x4,x5;

    char *arq = "arquivo.txt", *bufferLeitura[tam], *bufferEscrita[tam];
    i = initfs(arq,500);

    cry_desc_t *file_sys;
    file_sys = cry_openfs(arq);

    char *nome = "arq1";
    char *nome1 = "arq2";
    char *nome2 = "arq3";
    char *nome3 = "arq4";
    char *nome4 = "arq5";

    x1 = cry_open(file_sys, nome, 1, "2");
    printf("NOME: %i\n", x1);

    x2 = cry_open(file_sys, nome1, 1, "2");
    printf("NOME: %i\n", x2);

    x3 = cry_open(file_sys, nome2, 1, "2");
    printf("NOME: %i\n", x3);

    x4 = cry_open(file_sys, nome3, 1, "2");
    printf("NOME: %i\n", x4);

    x5 = cry_open(file_sys, nome4, 1, "2");
    printf("NOME: %i\n", x5);

    x5 = cry_open(file_sys, "nome4", 1, "2");
    printf("NOME: %i\n", x5);

    x5 = cry_open(file_sys, "nome5", 1, "2");
    printf("NOME: %i\n", x5);

    x5 = cry_open(file_sys, "nome6", 1, "2");
    printf("NOME: %i\n", x5);

    x5 = cry_open(file_sys, "nome7", 1, "2");
    printf("NOME: %i\n", x5);

    x5 = cry_open(file_sys, "nome8", 1, "2");
    printf("NOME: %i\n", x5);

    for(i=0; i<350; i++){
        char name[80];
        char tmp[80];
        sprintf(name,"a%d",i);

        x5 = cry_open(file_sys, name, 1, "2");
        printf("NAME: %s NOME: %i\n", name, x5);

    }



    return 0;
}
