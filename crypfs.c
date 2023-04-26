#include "crypfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Inicializa o arquivo no sistema de arquivos hospedeiro.
 *
 *  Deve ser chamada apenas uma vez para um determinado sistema de arquivos.
 *  Caso o nome de arquivo já exista, deve retornar erro.
 *  Também deve retornar erro caso o número de blocos não seja o suficiente
 *  para armazenar os metadados pelo menos.
 *
 *  @param arquivo nome do arquivo no sistema de arquivos hospedeiro
 *  @param blocos número de blocos do arquivo
 *  @return SUCESSO ou FALHA
 */


cry_desc_t * f_sys = NULL;

typedef struct bloco_ind{
    indice_arquivo_t prox;
    indice_arquivo_t bloco_d[1023]; // MUDEI PARA 1023
 } bloco_ind;



typedef struct bit_map{

    int n_blocos;
    int * bloc;

 } bitmap;

 bitmap * bmap;

 long unsigned int metadados;

int initfs(char * arquivo, int blocos){
    if(blocos < 0 || arquivo == "" || arquivo == "." || ((sizeof(int)*(blocos+1)+sizeof(struct descritor)*256)/(float)BLOCO)>blocos){
        return FALHA;
    }
    long unsigned int tam_arq, tam_desc, tam_bitmap;//, metadados;//tam_total;
    int i,j, NUM=0;
    FILE * arq = fopen(arquivo, "rb");
    // -------- COMPARA NOME ---------
    if(arq != NULL){        // comparar nome do arquivo com os atuais se exitir retona erro 
        fclose(arq);
        return FALHA;
    }
    // --------- OPENING FILE ---------
    FILE *f = fopen(arquivo, "w+b");

    if (f == NULL){     //Duvidas em relação a utilização dos blocos
        printf("Erro na abertura \n");
        return FALHA;
    }
    // ---------- BLOCOS ----------     Cria ponteiro para DESCRITOR_T      OBS:    numero de blocos insuficiente retorna erro

    tam_bitmap = sizeof(int)*(blocos+1);    // bitmap
    tam_desc = (sizeof(struct descritor)*256);      //  n_blocos_desc = sizeof(struct descritor)*256/BLOCO;     //17
    //printf("Blocos:////------/ %i\n", (tam_bitmap));///BLOCO) );
    //tam_arq = (tam_desc+(BLOCO*blocos)+tam_bitmap)/sizeof(int);

    float tam_total =(tam_bitmap+tam_desc)/(float)BLOCO;
    metadados = ceil(tam_total);
    //printf("TAM_TOTAL: %i\n", metadados);



    tam_arq = (BLOCO*blocos)/sizeof(int);

    for(j = 0; j < tam_arq;j++){            // Criar o arquivo com tamanho TOTAL
        fwrite(&NUM,sizeof(int),1,f);
        fflush(f);
    }

    rewind(f); 
    descritor_t * cad = malloc(sizeof(descritor_t));
    strcpy(cad->nome,"\0");
    cad->tempo = 0;
    cad->indices = -1;
    cad->tamanho = 0;
    

    for (i = 0; i < 256; i++) {                         // Aloca descritores no inicio do arquivo
        fwrite(cad, sizeof(struct descritor),1,f);
        fflush(f);
    }
    //---------------------------------------------------------------------------
    // ------------ ALOCANDO BITMAP E SETANDO VARIAVIES ------------------
    
    bitmap * bp = (bitmap*)malloc(sizeof(bitmap));
    bmap = bp;
    bmap->bloc = malloc(sizeof(int) * blocos);
    fwrite(&blocos,sizeof(int),1,f);
    ////fflush(f_sys->arquivo_host);
    for(i = 0; i < blocos; i++){
        bmap->bloc[i] = -1;
        fwrite(&bmap->bloc[i],sizeof(int),1,f);
        fflush(f);
    }
    bmap->n_blocos = blocos - metadados;
    //printf("----- %d\n", bmap->n_blocos);
    // -------------------------------------------------------------------
    free(cad);
    fclose(f);

    return SUCESSO;
}

/** Abre um sistema de arquivos.
 *
 *  @param nome do arquivo no sistema de arquivos hospedeiro
 *  @return ponteiro para descritor de sistema de arquivos ou FALHA
 */

cry_desc_t * cry_openfs(char * arquivo){
    int i;
    FILE *f = fopen(arquivo, "r+b");

    if (f == NULL){     // Se o arquivo não existir, retorna erro
        printf("Erro na abertura \n");
        return NULL;
    }

    cry_desc_t *file_sys = (cry_desc_t*)malloc(sizeof(cry_desc_t));
    for(i = 0; i < 256; i++){
        strcpy(file_sys->descritores[i].nome,"\0");
        file_sys->abertos[i].arquivo = NULL;
        file_sys->abertos[i].acesso = -1;
    }
  

    file_sys->arquivo_host = f;
    f_sys = file_sys;
    return file_sys;
}

/** Abre um arquivo criptografado.
 *
 *
 * @param cry_desc o descritor do sistema de arquivos criptografado
 * @param nome o arquivo a ser aberto
 * @param acesso LEITURA, ESCRITA ou LEITURAESCRITA
 * @param chave deslocamento a ser usado nos acessos
 * @return índice do arquivo aberto, FALHA se não abrir
 */
indice_arquivo_t cry_open(cry_desc_t *cry_desc, char * arq, int acesso, char deslocamento){ // NECESSITA TESTAR QUANDO ESTIVER COM ARQUIVO NOS DESCRITORES E TER SIDO FECHADO E ABERTO NOVAMENTE, ASSIM CRIA UM NOVO ARQUIVO ABERTO PARA ELE.
    //pthread_mutex_lock(&(sincron->alteracao[indice]));
    if((acesso < 0 || acesso > 2) || cry_desc == NULL){
        //pthread_mutex_unlock(&(sincron->alteracao[indice]));
        return FALHA;
    }
    clock_t start_t, end_t, total_t;
    start_t = clock();
    int i, j;
    for(i = 0; i < 256; i++){

        // ---------------VERIFICAR ARQUIVO EXISTE NO DESCRITOR_T ----------------
        if(strcmp(cry_desc->descritores[i].nome , arq) == 0){
            for(j = 0; j < 256; j++){
                if(cry_desc->abertos[j].arquivo == NULL){
                    cry_desc->abertos[j].arquivo = &cry_desc->descritores[i];
                    cry_desc->abertos[j].acesso = acesso;
                    cry_desc->abertos[j].chave = deslocamento;
                    // ---------- ALOCAR ESPACO NO ARQUIVO PARA SER USADO PARA BLOCO DE INDICES E BLOCO DE DADOS ----------------
                    //cry_desc->descritores[i].indices = find_bi(cry_desc); 
                    end_t = clock();
                    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
                    //cry_desc->descritores[i].tempo = total_t;
                    //cry_desc->descritores[i].tamanho = BLOCO*2;
                    //printf("Total time taken by CPU: %lf\n", total_t );
                    //pthread_mutex_unlock(&(sincron->alteracao[indice]));
                    return (j+1); //     <<---------------AQUI---------------------
                }
            }

            for(j = 0; j < 256; j++){
                //--------------- SE EXISTIR DESCRITOR, VERIFICA SE JA ESTA ABERTO ESTE DESCRITOR --------------
                if(strcmp(cry_desc->abertos[j].arquivo->nome, arq) == 0){
                    //pthread_mutex_unlock(&(sincron->alteracao[indice]));
                    cry_desc->abertos[j].chave = deslocamento; // olhar pq ele pode alterar a chave e perder a informacao dessa forma
                    cry_desc->abertos[j].acesso = acesso;
                    end_t = clock();
                    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
                    return (j+1);
                }
            }
            // --------------SE NAO EXISTIR ARQUIVO ABERTO, ACHAR ARQUIVO_ABERTO_T LIVRE E ALOCAR ESPACO NO SISTEMA DE ARQUIVOS --------        
        }
    }
    if(acesso == LEITURA){
        return FALHA;
    }
    // NAO ESTA NOS DESCRITORES, NESTE CASO ACHA DESCRITOR PARA SER USADO E ACHAR ARQUIVO_ABERTO_T PARA SER USADO
    // FALTA SINCRONIZAR COM O ARQUIVO DO SISTEMA DE ARQUIVOS
    char str1[15];
    strcpy(str1, "\0");
    for(i = 0; i < 256; i++){
        if(!strcmp(cry_desc->descritores[i].nome,str1)){
            strcpy(cry_desc->descritores[i].nome, arq);
            cry_desc->descritores[i].indices = -1;
            cry_desc->descritores[i].tamanho = BLOCO*2; 
            descritor_t * cad = &cry_desc->descritores[i];
            rewind(cry_desc->arquivo_host);
            fseek(cry_desc->arquivo_host,i*sizeof(struct descritor),SEEK_SET);
            fwrite(cad,sizeof(struct descritor) ,1,cry_desc->arquivo_host);
            fflush(cry_desc->arquivo_host);
            
            for(j = 0; j < 256; j++){
                if(cry_desc->abertos[j].arquivo == NULL){
                    cry_desc->abertos[j].arquivo = cad;
                    cry_desc->abertos[j].acesso = acesso;
                    cry_desc->abertos[j].chave = deslocamento;
                    // ---------- ALOCAR ESPACO NO ARQUIVO PARA SER USADO PARA BLOCO DE INDICES E BLOCO DE DADOS ----------------
                    cry_desc->descritores[i].indices = find_bi(cry_desc);
                    //--------------------------------------------------------------------
                    end_t = clock();
                    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
                    cry_desc->descritores[i].tempo = total_t;
                    //printf("Total time taken by CPU: %lf\n", total_t );
                    //pthread_mutex_unlock(&(sincron->alteracao[indice]));
                    return (j+1);
                }
            }
        }
    }
    // OBSERVAR A ESTRUTURA DE BLOCOS DE INDICES CRIADAS PARA CONTROLAR BLOCOS DE DADOS
    // COMO VAMOS TRABALHAR OS BLOCOS LIVRES => IDEIA SEPARAR O PRIMEIRO OU X BYTES DE CADA BLOCO PARA DIZER SE ESTA LIVRE OU NAO
    // SEMPRE CUIDANDO QUE TEREMOS ESPACO UTIL PARA ESCRITA DE 4096 - X BYTES UTILIZADOS PARA SABER SE O BLOCO ESTA LIVRE OU NAO
    //pthread_mutex_unlock(&(sincron->alteracao[indice]));
    return FALHA;

}

/** Fecha um arquivo criptografado.
 *
 * @param arquivo índice para a tabela de arquivos abertos
 * @return SUCESSO OU FALHA
 */
int cry_close(indice_arquivo_t arquivo){
    //pthread_mutex_lock(&(sincron->alteracao[indice]));
    arquivo --;
    if(arquivo > 255 || arquivo < 0 || f_sys->abertos[arquivo].arquivo == NULL){
        //pthread_mutex_unlock(&(sincron->alteracao[indice]));
        return FALHA;
    }
    //printf("%s\n", f_sys->abertos[arquivo].arquivo->nome);
    f_sys->abertos[arquivo].acesso = -1;
    strcpy(&f_sys->abertos[arquivo].chave,"\0");
    f_sys->abertos[arquivo].arquivo = NULL;
    //pthread_mutex_unlock(&(sincron->alteracao[indice]));
    return SUCESSO;
    
    // VERIFICAR SE ESTA ABERTO
    // SE ESTIVER ABERTO
        //PERCORRER BLOCO DE INDICES E ZERAR TODOS OS IDICES DOS BLOCOS DE DADOS E BLOCOS DE INDICES PROXIMOS
        //ZERAR BLOCO DE INDICES
        //ZERAR AQUIVO ABERTO
        //ZERAR NO BITMAP PRA APARECER COMO LIVRE 
    
}

/** Lê bytes de um arquivo criptografado aberto.
 *
 * @param arquivo índice para a tabela de arquivos abertos
 * @param tamanho número de bytes a serem lidos
 * @param buffer ponteiro para buffer onde serão armazenados os bytes lidos
 * @return número de bytes lidos
 */
uint32_t cry_read(indice_arquivo_t arquivo, uint32_t tamanho, char *buffer){
    // VERIFICAR SE ARQUIVO ABERTO TEM PONTEIRO DIFERENTE DE NULL E ACESSO DIFERENTE DE 1;
    // LEITURA DO PRIMEIRO BLOCO DE INDICES
    // WHILE 
    //pthread_mutex_lock(&(sincron->alteracao[indice]));
    uint32_t bytes_lidos=0;
    arquivo --;
    if(f_sys->abertos[(arquivo)].arquivo == NULL || f_sys->abertos[(arquivo)].acesso == ESCRITA){
        //pthread_mutex_unlock(&(sincron->alteracao[indice]));
        return FALHA;
    }
    int ind = f_sys->abertos[(arquivo)].arquivo->indices;
    unsigned long int tam_bitmap = sizeof(int)*(bmap->n_blocos + 1);
    unsigned long int tam_desc = sizeof(descritor_t)*256;

    fseek(f_sys->arquivo_host, (metadados*BLOCO)+(BLOCO*ind), SEEK_SET);
    bloco_ind * bi = (bloco_ind*)malloc(sizeof(bloco_ind));
    fread(bi, sizeof(bloco_ind), 1, f_sys->arquivo_host);
    int i = 0;
    int j = 0,k = 0;
    
    int end_bd = bmap->bloc[bi->bloco_d[i]];
    char juca;
    
    while(tamanho > 0){ //e arquivo chegar ao fim
        // --------- IF ONDE PAROU A LEITURA(J) É IGUAL AO FINAL VALOR DA ULTIMA ESCRITA NO ARQUIVO ------
        if(end_bd == j){
            // ---------------- IF ESSE VALOR DA ULTIMA ESCRITA DIFERENTE DE 4096, ESTE É O ULTIMO BLOCO QUE PRECISAVA SER LIDO, SUCESSO
            if(end_bd != 4096){
                free(bi);
                //pthread_mutex_unlock(&(sincron->alteracao[indice]));
                //printf("Bytes_lidos ========= %d\n", bytes_lidos);
                return bytes_lidos;
            }
            else{
                if(i == 1022){
                    if(bi->prox != -1){
                        fseek(f_sys->arquivo_host, (metadados*BLOCO)+(BLOCO*(bi->prox)), SEEK_SET);
                        fread(bi, sizeof(bloco_ind), 1, f_sys->arquivo_host);
                        i = 0;
                        end_bd = bmap->bloc[bi->bloco_d[i]];
                        j = 0;
                    }
                    else{
                        free(bi);
                        //pthread_mutex_unlock(&(sincron->alteracao[indice]));
                        return bytes_lidos;
                    }
                }
                else{
                    
                    i++;
                    if(i > bmap->n_blocos){
                        //pthread_mutex_unlock(&(sincron->alteracao[indice]));
                        return bytes_lidos;
                    }
                    
                    end_bd = bmap->bloc[bi->bloco_d[i]];
                    j = 0;
                    if(end_bd == j)
                    {
                        //pthread_mutex_unlock(&(sincron->alteracao[indice]));
                        return bytes_lidos;
                    }
                    //printf("%u\n", bytes_lidos);
                }
            }
        }

        fseek(f_sys->arquivo_host, (metadados*BLOCO)+(BLOCO*(bi->bloco_d[i])+j), SEEK_SET);
        fread(&juca, sizeof(char), 1, f_sys->arquivo_host);
        juca = descriptografa(juca, f_sys->abertos[(arquivo)].chave);
        buffer[k]=juca;
        k++;
        j++;
        bytes_lidos++;
        tamanho--;
        //if(bytes_lidos > 36864)
        //{
        //  printf("--------------------------%u\n", bytes_lidos);
        //}
    }

    free(bi);
    //printf("BUFFER: %s\n", buffer);
    //pthread_mutex_unlock(&(sincron->alteracao[indice]));
    return bytes_lidos;
}

/** Escreve bytes em um arquivo criptografado aberto.
 *
 * A escrita é sempre realizada no modo append, ou seja, no fim do arquivo.
 *
 * @param arquivo índice para a tabela de arquivos abertos
 * @param tamanho número de bytes a serem escritos
 * @param buffer ponteiro para conteúdo a ser escrito
 * @return SUCESSO ou FALHA
 */
int cry_write(indice_arquivo_t arquivo, uint32_t tamanho, char *buffer){
    //SE F_SYS.ABERTOS[ARQUIVO].ARQUIVO == NULL && F_SYS.ABERTOS[ARQUIVO].ACESSO != 1 && F_SYS.ABERTOS[ARQUIVO].ACESSO != 2 RETORNA FALHA;
    //VARIAVEL INT PARA O BLOCO
    //ENQUANTO BUFFER[I] NAO ENCONTRAR \0 E TAMANHO MAIOR QUE ZERO
        //IF BMAP->BLOC[MEU_BLOCO] == 4096
            // FIND BD
            // CUIDAR PARA O CASO DE FALTAR MEMORIA
        // 
        //I++;
        //BMAP->BLOC[MEU_BLOCO]++
    //pthread_mutex_lock(&(sincron->alteracao[indice]));
    int bytes_escritos=0;
    arquivo --;
    if(f_sys->abertos[(arquivo)].arquivo == NULL || f_sys->abertos[(arquivo)].acesso < 1 || f_sys->abertos[(arquivo)].acesso > 2){
        //pthread_mutex_unlock(&(sincron->alteracao[indice]));
        //return bytes_escritos;
        return FALHA;
    }
    //printf("ENTROUUU\n");

    // Retonar bytes escritos e bytes lidos no read

    int i = 0;
    int ind = f_sys->abertos[(arquivo)].arquivo->indices;
    unsigned long int tam_bitmap = sizeof(int)*(bmap->n_blocos + 1);
    unsigned long int tam_desc = sizeof(descritor_t)*256;
    
    // -------------- ACESSA E LE BLOCO DE INCIDES ---------
    fseek(f_sys->arquivo_host, (metadados*BLOCO)+(BLOCO*ind), SEEK_SET);
    bloco_ind * bi = (bloco_ind*)malloc(sizeof(bloco_ind));
    fread(bi, sizeof(bloco_ind), 1, f_sys->arquivo_host);
    // -----------------------------------------------------
    // ---------------- SE BLOCO DE INDICES TIVER PROXMO, ACESSAR ULTIMO BLOCO DE INDICES ----------
    while(bi->prox > -1 && bi->prox < bmap->n_blocos){
        ind = bi->prox;
        fseek(f_sys->arquivo_host, (metadados*BLOCO)+(BLOCO*(bi->prox)), SEEK_SET);
        fread(bi, sizeof(bloco_ind), 1, f_sys->arquivo_host);

    }
    // ---------------------------------------------------------------------------------------------
    // PERCORRER BLOCO DE INDICE PAR ACHAR UM BLOCO DE DADOS LIVRE PARA ESCRITA;
    for(i = 0; i < 1024; i++){
        if(bi->bloco_d[i] > -1){
            if(bmap->bloc[bi->bloco_d[i]] < 4096){
                break;
            }
        }
    }
    int j = 0;
    // i É NOSSO É O INDICE DO NOSSO BLOCO DE DADOS PRONTO PARA ESCRITA
    while(tamanho > 0){// && buffer[j] != '\0'){
        int ind_d = bi->bloco_d[i];
        if(bmap->bloc[ind_d] == 4096){
            //CASO BLOCO DE INDICES TER ACABAD TODOSO OS BLOCO_D, CRIAR NOVO BLOOC DE INDICES
            //ALOCAR OUTRO BLOCO DE DADOS PARA SER ESCRITO
            //LEMBRAR DE TROCAR IND_D
            if(i == 1022){
                bi->prox = find_bi(f_sys);
                f_sys->abertos[(arquivo)].arquivo->tamanho = f_sys->abertos[(arquivo)].arquivo->tamanho + BLOCO*2;
                if(bi->prox == -1){
                    free(bi);
                    //pthread_mutex_unlock(&(sincron->alteracao[indice]));
                    //return bytes_escritos; // faltou memoria
                    //printf("----------bytes_escritos: %d\n", bytes_escritos);
                    return SUCESSO;
                }
                // --- ESCREVE BI ----
                fseek(f_sys->arquivo_host, (metadados*BLOCO)+(BLOCO*ind), SEEK_SET);
                fwrite(bi, sizeof(struct bloco_ind),1,f_sys->arquivo_host);
                fflush(f_sys->arquivo_host);
                ind = bi->prox;
                // -------------------
                fseek(f_sys->arquivo_host, (metadados*BLOCO)+(BLOCO*(bi->prox)), SEEK_SET);
                fread(bi, sizeof(bloco_ind), 1, f_sys->arquivo_host);
                i = 0;
                ind_d = bi->bloco_d[i];

            }
            else{
                i++;
                if(i > bmap->n_blocos){
                    //pthread_mutex_unlock(&(sincron->alteracao[indice]));
                    return FALHA;
                }

                bi->bloco_d[i] = find_bd(f_sys);
                f_sys->abertos[(arquivo)].arquivo->tamanho = f_sys->abertos[(arquivo)].arquivo->tamanho + BLOCO;
                if(bi->bloco_d[i] == -1){
                    free(bi);
                    //pthread_mutex_unlock(&(sincron->alteracao[indice]));
                    return SUCESSO;
                }
                ind_d = bi->bloco_d[i];
                fseek(f_sys->arquivo_host, (metadados*BLOCO)+(BLOCO*ind), SEEK_SET);
                fwrite(bi, sizeof(struct bloco_ind),1,f_sys->arquivo_host);
                fflush(f_sys->arquivo_host);
            }
        }
        if(i > bmap->n_blocos){
            //pthread_mutex_unlock(&(sincron->alteracao[indice]));
            //return bytes_escritos;
            return FALHA;
        }
        char aux = criptografa(buffer[j], f_sys->abertos[(arquivo)].chave);
        //buffer[j] = criptografa(buffer[j], f_sys->abertos[(arquivo-1)].chave);
        
        //printf("---- CHAR CRIPTOGRAFADO: %c\n", buffer[j]);
        fseek(f_sys->arquivo_host, (metadados*BLOCO)+(BLOCO*(ind_d))+(bmap->bloc[ind_d]), SEEK_SET);
        fwrite(&aux, sizeof(char), 1, f_sys->arquivo_host);
        bmap->bloc[ind_d]++;
        tamanho--;
        bytes_escritos++;
        j++;
    }
    free(bi);
    //pthread_mutex_unlock(&(sincron->alteracao[indice]));
    //printf("----------bytes_escritos: %d\n", bytes_escritos);
    return SUCESSO;
}

/** Apaga um arquivo e o fecha.
 *
 * @param arquivo índice para a tabela de arquivos abertos
 * @return SUCESSO ou FALHA
 */

int cry_delete(indice_arquivo_t arquivo){
    
    // VERIFICAR SE ESTA ABERTO
    // SE ESTIVER ABERTO
        //PERCORRER BLOCO DE INDICES E ZERAR TODOS OS IDICES DOS BLOCOS DE DADOS E BLOCOS DE INDICES PROXIMOS
        //ZERAR BLOCO DE INDICES
        //ZERAR AQUIVO ABERTO
        //ZERAR NO BITMAP PRA APARECER COMO LIVRE 
    //ZERAR DESCRITOR 
    //pthread_mutex_lock(&(sincron->alteracao[indice]));
    if(arquivo > 256 && arquivo < 1  && f_sys->abertos[arquivo].arquivo == NULL){
        //pthread_mutex_unlock(&(sincron->alteracao[indice]));
        return FALHA;
    }
    else{
        descritor_t * desc = malloc(sizeof(descritor_t*));
        desc = f_sys->abertos[(arquivo-1)].arquivo;
        int aux;
        int tam_desc = (sizeof(struct descritor)*256);
        int tam_bitmap = sizeof(int)*(bmap->n_blocos+1);
        int next = f_sys->abertos[(arquivo-1)].arquivo->indices; //LEMBRAR DE ZERAR
        int i;
        int temp = 0;
        //WHILE PROX != DE -1, LER BLOCO DE INDICES E ZERAR BLOCOS DE DADOS.
        while(next != -1){
            fseek(f_sys->arquivo_host, (metadados*BLOCO)+(BLOCO*next), SEEK_SET);
            bloco_ind * bi = (bloco_ind*)malloc(sizeof(bloco_ind));
            fread(bi, sizeof(bloco_ind), 1, f_sys->arquivo_host);
            
            for (i = 0; i < 1024; ++i){         
                
                if(bi->bloco_d[i] != -1){
                    int j;
                    fseek(f_sys->arquivo_host, (metadados*BLOCO)+(BLOCO*i), SEEK_SET);

                    for (j = 0; j < (BLOCO/sizeof(int)); j++){
                        fwrite(&temp, sizeof(int), 1, f_sys->arquivo_host);         //Verificar o TEMP
                        fflush(f_sys->arquivo_host);
                    }
                    bmap->bloc[bi->bloco_d[i]] = -1; 
                    // SINCRONIZAR BITMAP DO ARQUIVO
                    fseek(f_sys->arquivo_host, tam_desc+(sizeof(int)*(i+1)), SEEK_SET);
                    fwrite(&bmap->bloc[bi->bloco_d[i]],sizeof(int),1,f_sys->arquivo_host);
                    fflush(f_sys->arquivo_host);
                    //
                    bi->bloco_d[i] = -1; 
                }
                else{ //COMO É CONTINUO PODEMOS SAIR AO MOMENTO QUE ACHARMOS UM -1
                    break;
                }
            }
            // -------- ZERANDO BLOCO DE INDICES
            int k;
            fseek(f_sys->arquivo_host, (tam_desc+tam_bitmap)+(BLOCO*next), SEEK_SET);

            for (k = 0; k < (BLOCO/sizeof(int)); k++)           {
                fwrite(&temp, sizeof(int), 1, f_sys->arquivo_host);                 //Verificar o TEMP
                fflush(f_sys->arquivo_host);
            }
            // -----------------
            bmap->bloc[next] = -1; // SINCRONIZAR BITMAP DO ARQUIVO
            //
            fseek(f_sys->arquivo_host, tam_desc+(sizeof(int)*(next+1)), SEEK_SET);
            fwrite(&bmap->bloc[next],sizeof(int),1,f_sys->arquivo_host);
            fflush(f_sys->arquivo_host);
            //
            next = bi->prox;
            aux = bi->bloco_d[i];
            free(bi);               //-----Verificar FREE
        }

        //int aux = bi->bloco_d[i];

        bmap->bloc[aux] = -1;
        // -- VERIFICAR ABAIXO --
        f_sys->abertos[(arquivo-1)].arquivo->indices = -1; // SINCRONIZAR COM O ARQUIVO
        // -------
        fseek(f_sys->arquivo_host, tam_desc+(sizeof(int)*(next+1)), SEEK_SET);
        fwrite(f_sys->abertos[(arquivo-1)].arquivo, sizeof(descritor_t), 1, f_sys->arquivo_host); // se vai & ou nao na frente do f_sys
        fflush(f_sys->arquivo_host);
        // -------
        f_sys->abertos[(arquivo-1)].acesso = -1;
        strcpy(&f_sys->abertos[(arquivo-1)].chave,"\0");
        f_sys->abertos[(arquivo-1)].arquivo = NULL;
        i = 0;
        for(i = 0; i < 256; i++){
            if(desc->nome == f_sys->descritores[i].nome){ // VERIFICAR SE COMPARACAO FUNCIONA
                strcpy(desc->nome, "\0");
                desc->tempo = 0;
                desc->tamanho = 0;
                // --- SINCRONIZAR COM ARQUIVO
                fseek(f_sys->arquivo_host, (sizeof(descritor_t)*(i)), SEEK_SET);
                fwrite(desc, sizeof(descritor_t), 1, f_sys->arquivo_host);
                fflush(f_sys->arquivo_host);
                // -------------------
                //pthread_mutex_unlock(&(sincron->alteracao[indice]));
                return SUCESSO;
            }
        }
        //pthread_mutex_unlock(&(sincron->alteracao[indice]));
        return SUCESSO;
    }
}   

int find_bd(cry_desc_t *cry_desc){
    int i,k;
    int tam_desc = (sizeof(struct descritor)*256);
    // PERCORRE O BIT MAP A PROCURA DE BLOCO LIVRE, OU SEJA, -1
    // MUDA O VALOR PARA 0
    // RETORNA INDICE DO BLOCO
    // for (k = 0; k < bmap->n_blocos; k++){
    //     printf("%d, " , bmap->bloc[k]);
    // }
    // printf("\n");

    for (i = 0; i < bmap->n_blocos; i++){
        if(bmap->bloc[i] == (-1)){
            bmap->bloc[i] = 0;
            fseek(cry_desc->arquivo_host, tam_desc+(sizeof(int)*(i+1)), SEEK_SET);
            fwrite(&bmap->bloc[i],sizeof(indice_arquivo_t), 1, cry_desc->arquivo_host);
            fflush(cry_desc->arquivo_host);
            return i;
        }
    }
    return -1; // ERRO - TODOS BLOCOS OCUPADOS
}

int find_bi(cry_desc_t *cry_desc){
    int tam_desc = (sizeof(struct descritor)*256);
    unsigned long int tam_bitmap = sizeof(int)*(bmap->n_blocos+1);
    int k;

    // printf("BI: ");
    // for (k = 0; k < bmap->n_blocos; k++){
    //     printf("%i, " , bmap->bloc[k]);
    // }
    // printf("\n");

    // BUSCAR BLOCO LIVRE PARA BLOCO DE INDICES E INICIALIZA-LO
    for (k = 0; k < bmap->n_blocos; k++){
        if(bmap->bloc[k] == (-1)){
            bmap->bloc[k] = 0;
            // --------- SINCRONIZANDO COM ARQUIVO O BITMAP -----------
            fseek(cry_desc->arquivo_host, tam_desc+(sizeof(int)*(k+1)), SEEK_SET);
            fwrite(&bmap->bloc[k],sizeof(indice_arquivo_t), 1, cry_desc->arquivo_host); 
            fflush(cry_desc->arquivo_host);
            // --------------------------------------------------------
            bloco_ind * bi = (bloco_ind*)malloc(sizeof(bloco_ind)); // -1 pq 0 pode ser indice de bloco
            int y;
            for(y = 0; y < 1023; y++){
                bi->bloco_d[y] = -1;
            }
            bi->prox = -1;
            bi->bloco_d[0] = find_bd(cry_desc);
            fseek(cry_desc->arquivo_host, (metadados*BLOCO)+(BLOCO*k), SEEK_SET);
            fwrite(bi, sizeof(struct bloco_ind),1,cry_desc->arquivo_host);
            fflush(cry_desc->arquivo_host);
            //printf("Alocou Bloco de indices --- \n");
            free(bi);
            return k;        
            }
        }
    return -1;  // ERRO - TODOS BLOCOS OCUPADOS
}

char criptografa(char s, int cifra) {
    while((s + cifra)>127){
        cifra = cifra - 127;
    }
    s = s + cifra;
    //printf("CRIPT: %i\n", (int*)s);
    return s;
}

char descriptografa(char s, int cifra){
    while((s - cifra)<0){
        cifra = cifra - 127;
    }
    s = s - cifra;
    //printf("%i\n", (int*)s);
    return s;
}