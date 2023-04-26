#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "crypfs.h"

int main(){
    int i, tam = 46000;    

    char *arq = "arquivo.txt", *bufferLeitura[tam], *bufferEscrita[tam];
    i = initfs(arq,-10);

    printf("Retorno initfs: %i\n", i);

    return 0;
}
