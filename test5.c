#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "crypfs.h"

int main(){
    int i, k, write, tam=36000;//juca2, write, read;
    uint32_t read;
    char *nome = "arq1";

    char *arq = "arquivo.txt", *bufferLeitura[tam], *bufferEscrita[tam];
    i = initfs(arq,10);

    cry_desc_t *file_sys;
    file_sys = cry_openfs(arq);

    indice_arquivo_t x = cry_open(file_sys, nome, 0, "2");

    
    for(k =0; k<tam;k++){
        strcat(bufferEscrita,"a");
    }
   // strcat(bufferEscrita,"\0");
    //------------------------------------------------------------------------
    write = cry_write(x, tam, bufferEscrita);

    read = cry_read(x, tam, bufferLeitura);

    //printf("buffer:%s\n",bufferLeitura);

    printf("Números de bytes inseridos: %i\n", write);
    printf("Números de bytes lidos: %u\n", read);
    
    //printf("%lu\n", file_sys->descritores[0].tamanho);

    return 0;
}
