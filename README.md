# Trabalho 2 - Sistema de Arquivos Criptografado

## Descrição 

Para o trabalho 2, um sistema de arquivos criptografado será implementado em nível de usuário para sistemas de acordo com POSIX, especialmente Linux. Os alunos poderão realizar o trabalho em duplas.

Normalmente, para implementar um sistema de arquivos é necessário ter acesso direito aos dispositivos. Para simplificar a implementação e depuração, um arquivo grande sobre um outro sistema de arquivos será usado. Este arquivo será usado para armazenar os descritores de arquivos e os blocos que os compõem.

O tamanho de bloco a ser usado é 4 KiB. 

A implementação deverá prover um diretório de um nível com 256 entradas, armazenado nos primeiros blocos do sistema de arquivos implementado. Cada descritor de arquivo deverá conter:

- O nome do arquivo com no máximo 255 caracteres UTF-8 (todos em zero se a entrada não estiver sendo usada)
- O tempo de criação do arquivo
- O índice do primeiro bloco de índices
- O tamanho em bytes 

Cada bloco de indíces possui como última entrada o índice do próximo bloco de índices do arquivo. O índice ZERO é reservado como NULL.

## Criptografia

Os blocos de dados dos arquivos devem ser criptografados com uma chave simples de deslocamento. Cada byte armazenado é rotacionado **n** vezes, onde **n** é a chave utilizada. Mais detalhes podem sem encontrados no artigo sobre Cifra de César na Wikipedia.

A chave **NÃO** deve ser armazenada junto ao arquivo. Ela deve ser informada quando o arquivo é aberto e armazenada em estrutura de dados temporária de arquivos abertos do sistema. 

## Funções a serem implementadas

Detalhes dos parâmetros e da semântica das funções estão descritas na documentação do arquivo crypfs.h. Outras funções podem ser implementadas, mas não devem ser visíveis aos usuários finais. O arquivo crypfs.h **NÃO** deve ser modificado.

### initfs()

Função para inicializar um arquivo no sistema de arquivos hospedeiro com os blocos necessários.

Aloca os blocos, incluindo os blocos de metadados da estrutura do diretório.

### cry_openfs()

Função usada para abrir um sistema de arquivos já inicializado por initfs(). 


### cry_open()

Abre um arquivo criptografado com uma chave **n**. Deve ser chamada antes de qualquer outra operação com o arquivo e especificar se o arquivo é aberto para leitura, escrita ou ambos.

Retorna um file handler, NULL se houve erro.

### cry_read() 

Lê **n** bytes de um arquivo. Os bytes são entregues decriptografados com a chave usada para abrir o arquivo. Retorna o número de bytes lidos.

### cry_write()

Escreve **n** bytes. Retorna **1** se sucesso, **0** se erro.

### cry_delete()

Remove o arquivo e o fecha.  Retorna **1** se sucesso, **0** se erro.

### cry_close() 

Fecha o arquivo.  Retorna **1** se sucesso, **0** se erro.

## Casos de Teste

O trabalho deverá conter um Makefile com pelo menos quatro testes, acessáveis através de `make test1`, `make test2`, ... Além de compilar o caso de teste, o comando `make testN` deverá executá-lo.

Os casos de teste deverão conter operações de criação, escrita, leitura e remoção que mostrem o funcionamento correto do sistema. Sugere-se fortemente o uso da biblioteca `assert.h` e um estilo de saída similar ao caso de teste `main.c` fornecido com o primeiro trabalho.

## Avaliação

Serão pontuados:

- Implementação da `crypfs`: **6 pontos**
- Casos de teste: **4 pontos**

## Cronograma

- Entrega no github: **20/06/2016**
- Apresentações (em ordem definida pelo professor): **21/06/2016 e 22/06/2016**

## Referências

* [Cifra de César -- Wikipedia] (https://pt.wikipedia.org/wiki/Cifra_de_C%C3%A9sar)

