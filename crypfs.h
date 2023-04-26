#include <time.h>
#include <stdio.h>
#include <stdint.h>

#define SUCESSO 1
#define FALHA 0

#define LEITURA 0
#define ESCRITA 1
#define LEITURAESCRITA 2


#define BLOCO 4096
/** Descritor de um arquivo
 */
typedef struct descritor{
	char nome[256];
	time_t tempo;
	uint32_t indices;
	uint32_t tamanho;
} descritor_t;

/** Tabela de arquivos abertos.
 */
typedef struct arquivo_aberto{
	descritor_t * arquivo;
	int acesso; /* pode ser LEITURA, ESCRITA ou LEITURAESCRITA */
	char chave;
} arquivo_aberto_t;

/** Descritor de um sistema de arquivos criptografados
 */
typedef struct cry_descritor{
	FILE * arquivo_host;
	descritor_t descritores[256]; /* devem ser lidos do arquivo host e mantidos sincronizados */
	arquivo_aberto_t abertos[256];  /* tabela de arquivos abertos */
} cry_desc_t;

typedef int indice_arquivo_t;



/** Inicializa o arquivo no sistema de arquivos hospedeiro.
 *
 *  Deve ser chamada apenas uma vez para um determinado sistema de arquivos.
 *  Caso o nome de arquivo já exista, deve retornar erro.
 *  Também deve retornar erro caso o número de blocos não seja o suficiente
 *  para armazenar os metadados pelo menos.
 *
 *  @param arquivo nome do arquivo no sistema de arquivos hospedeiro
 *  @param blocos número de blocos do arquivo
 *  @return SUCCESSO ou FALHA
 */
int initfs(char * arquivo, int blocos);


/** Abre um sistema de arquivos.
 *
 *  @param nome do arquivo no sistema de arquivos hospedeiro
 *  @return ponteiro para descritor de sistema de arquivos ou FALHA
 */

cry_desc_t * cry_openfs(char * arquivo);


/** Abre um arquivo criptografado.
 *
 *
 * @param cry_desc o descritor do sistema de arquivos criptografado
 * @param nome o arquivo a ser aberto
 * @param acesso LEITURA, ESCRITA ou LEITURAESCRITA
 * @param chave deslocamento a ser usado nos acessos
 * @return índice do arquivo aberto, FALHA se não abrir
 */
indice_arquivo_t cry_open(cry_desc_t *cry_desc, char * arq, int acesso, char deslocamento);

/** Fecha um arquivo criptografado.
 *
 * @param arquivo índice para a tabela de arquivos abertos
 * @return SUCESSO OU FALHA
 */
int cry_close(indice_arquivo_t arquivo);

/** Lê bytes de um arquivo criptografado aberto.
 *
 * @param arquivo índice para a tabela de arquivos abertos
 * @param tamanho número de bytes a serem lidos
 * @param buffer ponteiro para buffer onde serão armazenados os bytes lidos
 * @return número de bytes lidos
 */
uint32_t cry_read(indice_arquivo_t arquivo, uint32_t tamanho, char *buffer);

/** Escreve bytes em um arquivo criptografado aberto.
 *
 * A escrita é sempre realizada no modo append, ou seja, no fim do arquivo.
 *
 * @param arquivo índice para a tabela de arquivos abertos
 * @param tamanho número de bytes a serem escritos
 * @param buffer ponteiro para conteúdo a ser escrito
 * @return SUCESSO ou FALHA
 */
int cry_write(indice_arquivo_t arquivo, uint32_t tamanho, char *buffer);

/** Apaga um arquivo e o fecha.
 *
 * @param arquivo índice para a tabela de arquivos abertos
 * @return SUCESSO ou FALHA
 */

int cry_delete(indice_arquivo_t arquivo);



int find_bd(cry_desc_t *cry_desc);

int find_bi(cry_desc_t *cry_desc);

char criptografa(char s, int cifra);

char descriptografa(char s, int cifra);