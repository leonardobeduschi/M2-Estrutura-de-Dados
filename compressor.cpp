// Trabalho 2 - Compressao de Huffman
// Disciplina: Estruturas de Dados
// Aplicacao de COMPRESSAO

#include <iostream>
using namespace std;

#include "lista.hpp"
#include "arvore.hpp"

// Tamanho maximo do codigo binario de cada caractere
// (no pior caso, com 256 caracteres, a arvore pode ter ate 255 nos internos)
#define MAX_CODIGO 256
#define MAX_CHARS  256

// Tabela de substituicao: para cada caractere (0-255),
// guarda o codigo binario como string de '0' e '1'
char tabela[MAX_CHARS][MAX_CODIGO];

// -------------------------------------------------------
// Funcoes para montar a tabela de frequencia
// -------------------------------------------------------

// Le o arquivo e conta quantas vezes cada caractere aparece
// Preenche a lista com os caracteres encontrados
void montar_tabela_frequencia(const char *nomeArq, NoLista *&inicio, NoLista *&fim) {
    FILE *arq = fopen(nomeArq, "r");
    if (arq == NULL) {
        cout << "Erro ao abrir o arquivo!" << endl;
        return;
    }

    int c;
    while ((c = fgetc(arq)) != EOF) {
        char caractere = (char)c;
        // busca se o caractere ja esta na lista
        NoLista *no = buscar(inicio, caractere);
        if (no == NULL) {
            // primeira vez que aparece: insere na lista com frequencia 1
            inserir(inicio, fim, caractere, 1);
        } else {
            // ja existe: incrementa a frequencia
            no->frequencia++;
        }
    }

    fclose(arq);
}

// -------------------------------------------------------
// Funcoes para construir a arvore de Huffman
// -------------------------------------------------------

// Busca o no da lista com menor frequencia e o remove
// Retorna o no da arvore criado com esse caractere
NoArvore* remover_menor(NoLista *&inicio, NoLista *&fim) {
    if (inicio == NULL) return NULL;

    // encontra o no com menor frequencia
    NoLista *menor = inicio;
    NoLista *atual = inicio->prox;
    while (atual != NULL) {
        if (atual->frequencia < menor->frequencia)
            menor = atual;
        atual = atual->prox;
    }

    // cria o no da arvore com os dados encontrados
    NoArvore *no = criar_no(menor->caractere, menor->frequencia);

    // remove da lista
    remover(inicio, fim, menor);

    return no;
}

// Verifica se a lista tem so um elemento
bool lista_tem_um(NoLista *inicio) {
    return (inicio != NULL && inicio->prox == NULL);
}

// Verifica se a lista esta vazia
bool lista_vazia(NoLista *inicio) {
    return (inicio == NULL);
}

// Constroi a arvore de Huffman a partir da lista de frequencias
// Algoritmo: repete ate restar um no na lista de arvores
NoArvore* construir_arvore_huffman(NoLista *&inicio, NoLista *&fim) {
    // Cria uma lista de ponteiros para nos de arvore
    // (faz uma segunda lista temporaria so com ponteiros)
    // Para simplificar, usamos um array de nos de arvore
    NoArvore *arvores[MAX_CHARS];
    int qtd = 0;

    // Cria um no folha para cada entrada da lista
    NoLista *atual = inicio;
    while (atual != NULL) {
        arvores[qtd] = criar_no(atual->caractere, atual->frequencia);
        qtd++;
        atual = atual->prox;
    }

    // Algoritmo de Huffman: junta os dois menores repetidamente
    while (qtd > 1) {
        // Encontra os dois menores
        int idx1 = 0; // indice do menor
        for (int i = 1; i < qtd; i++) {
            if (arvores[i]->frequencia < arvores[idx1]->frequencia)
                idx1 = i;
        }

        // Tira o menor do array trocando com o ultimo
        NoArvore *menor1 = arvores[idx1];
        arvores[idx1] = arvores[qtd - 1];
        qtd--;

        int idx2 = 0; // indice do segundo menor
        for (int i = 1; i < qtd; i++) {
            if (arvores[i]->frequencia < arvores[idx2]->frequencia)
                idx2 = i;
        }

        NoArvore *menor2 = arvores[idx2];
        arvores[idx2] = arvores[qtd - 1];
        qtd--;

        // Junta os dois menores em um no interno
        NoArvore *novo = juntar_nos(menor1, menor2);

        // Coloca o novo no no array
        arvores[qtd] = novo;
        qtd++;
    }

    // O ultimo elemento do array eh a raiz da arvore de Huffman
    if (qtd == 1)
        return arvores[0];
    return NULL;
}

// -------------------------------------------------------
// Funcoes para gerar a tabela de codigos binarios
// -------------------------------------------------------

// Percorre a arvore recursivamente gerando o codigo de cada folha
// codigo: string acumulada de '0' e '1'
// nivel: posicao atual no codigo
void gerar_codigos(NoArvore *raiz, char codigo[], int nivel) {
    if (raiz == NULL) return;

    if (raiz->esq == NULL && raiz->dir == NULL) {
        // no folha: salva o codigo na tabela
        codigo[nivel] = '\0';
        // copia o codigo para a tabela usando o caractere como indice
        int idx = (unsigned char)raiz->caractere;
        int i = 0;
        while (codigo[i] != '\0') {
            tabela[idx][i] = codigo[i];
            i++;
        }
        tabela[idx][i] = '\0';
        return;
    }

    // vai para a esquerda: adiciona '0'
    codigo[nivel] = '0';
    gerar_codigos(raiz->esq, codigo, nivel + 1);

    // vai para a direita: adiciona '1'
    codigo[nivel] = '1';
    gerar_codigos(raiz->dir, codigo, nivel + 1);
}

// -------------------------------------------------------
// Funcoes para compressao e salvamento
// -------------------------------------------------------

// Comprime o arquivo original usando a tabela de codigos
// Salva os bits agrupados em bytes no arquivo comprimido
// Retorna a quantidade de bits validos no ultimo byte
int comprimir_arquivo(const char *nomeOriginal, const char *nomeComprimido) {
    FILE *original = fopen(nomeOriginal, "r");
    if (original == NULL) {
        cout << "Erro ao abrir arquivo original!" << endl;
        return -1;
    }

    FILE *comprimido = fopen(nomeComprimido, "wb");
    if (comprimido == NULL) {
        cout << "Erro ao criar arquivo comprimido!" << endl;
        fclose(original);
        return -1;
    }

    unsigned char byte_atual = 0; // byte sendo montado
    int bits_no_byte = 0;         // quantos bits ja foram colocados
    int total_bits_ultimo = 0;    // bits validos no ultimo byte

    int c;
    while ((c = fgetc(original)) != EOF) {
        char caractere = (char)c;
        int idx = (unsigned char)caractere;
        char *codigo = tabela[idx];

        // para cada bit do codigo
        int i = 0;
        while (codigo[i] != '\0') {
            // desloca o byte e coloca o proximo bit
            byte_atual = byte_atual << 1;
            if (codigo[i] == '1')
                byte_atual = byte_atual | 1;

            bits_no_byte++;

            // quando completou 8 bits, escreve o byte
            if (bits_no_byte == 8) {
                fputc(byte_atual, comprimido);
                byte_atual = 0;
                bits_no_byte = 0;
            }
            i++;
        }
    }

    // escreve o ultimo byte se sobrou algum bit
    if (bits_no_byte > 0) {
        // completa o byte com zeros a direita
        byte_atual = byte_atual << (8 - bits_no_byte);
        fputc(byte_atual, comprimido);
        total_bits_ultimo = bits_no_byte;
    } else {
        total_bits_ultimo = 8; // o ultimo byte estava completo
    }

    fclose(original);
    fclose(comprimido);

    return total_bits_ultimo;
}

// -------------------------------------------------------
// Programa principal
// -------------------------------------------------------

int main() {
    char nomeOriginal[200];
    char nomeComprimido[200];
    char nomeArvore[200];

    cout << "=== Compressor de Huffman ===" << endl;
    cout << "Digite o nome do arquivo a comprimir: ";
    cin >> nomeOriginal;

    // define os nomes dos arquivos de saida
    // adiciona extensoes para identificar os arquivos
    int i = 0;
    while (nomeOriginal[i] != '\0') {
        nomeComprimido[i] = nomeOriginal[i];
        nomeArvore[i] = nomeOriginal[i];
        i++;
    }
    // arquivo comprimido: .huf
    nomeComprimido[i] = '\0';
    nomeArvore[i] = '\0';

    // concatena as extensoes manualmente
    char ext1[] = ".huf";
    char ext2[] = ".arv";
    int j = 0;
    int k = 0;
    while (ext1[j] != '\0') { nomeComprimido[i + j] = ext1[j]; j++; }
    nomeComprimido[i + j] = '\0';
    while (ext2[k] != '\0') { nomeArvore[i + k] = ext2[k]; k++; }
    nomeArvore[i + k] = '\0';

    // ---- Passo 1: montar a tabela de frequencia ----
    cout << endl << "Lendo arquivo e montando tabela de frequencia..." << endl;
    NoLista *inicio, *fim;
    inicializar(inicio, fim);
    montar_tabela_frequencia(nomeOriginal, inicio, fim);

    if (inicio == NULL) {
        cout << "Arquivo vazio ou nao encontrado!" << endl;
        return 1;
    }

    cout << "Tabela de frequencias:" << endl;
    imprimir(inicio);

    // ---- Passo 2: construir a arvore de Huffman ----
    cout << endl << "Construindo arvore de Huffman..." << endl;
    NoArvore *raiz;
    inicializar(raiz);
    raiz = construir_arvore_huffman(inicio, fim);

    if (raiz == NULL) {
        cout << "Erro ao construir arvore!" << endl;
        return 1;
    }

    // ---- Passo 3: gerar tabela de codigos ----
    cout << "Gerando tabela de codigos binarios..." << endl;

    // inicializa a tabela com strings vazias
    for (int x = 0; x < MAX_CHARS; x++)
        tabela[x][0] = '\0';

    char codigo[MAX_CODIGO];

    // caso especial: arquivo com so um caractere diferente
    if (raiz->esq == NULL && raiz->dir == NULL) {
        tabela[(unsigned char)raiz->caractere][0] = '0';
        tabela[(unsigned char)raiz->caractere][1] = '\0';
    } else {
        gerar_codigos(raiz, codigo, 0);
    }

    // mostra a tabela de codigos
    cout << endl << "Tabela de codigos:" << endl;
    for (int x = 0; x < MAX_CHARS; x++) {
        if (tabela[x][0] != '\0') {
            char ch = (char)x;
            if (ch == '\n')
                cout << "'\\n' -> " << tabela[x] << endl;
            else if (ch == ' ')
                cout << "' '  -> " << tabela[x] << endl;
            else
                cout << "'" << ch << "'  -> " << tabela[x] << endl;
        }
    }

    // ---- Passo 4: salvar a arvore em disco ----
    cout << endl << "Salvando arvore em disco (" << nomeArvore << ")..." << endl;
    FILE *arqArvore = fopen(nomeArvore, "wb");
    if (arqArvore == NULL) {
        cout << "Erro ao criar arquivo da arvore!" << endl;
        return 1;
    }
    salvar_arvore(raiz, arqArvore);
    fclose(arqArvore);

    // ---- Passo 5: comprimir e salvar o arquivo ----
    cout << "Comprimindo arquivo e salvando em " << nomeComprimido << "..." << endl;
    int bits_ultimo = comprimir_arquivo(nomeOriginal, nomeComprimido);

    if (bits_ultimo < 0) {
        cout << "Erro na compressao!" << endl;
        return 1;
    }

    // salva a quantidade de bits validos no ultimo byte no inicio do .huf
    // (necessario para a descompressao saber quantos bits do ultimo byte sao validos)
    // Vamos reescrever o arquivo colocando esse valor no inicio
    // Lemos o arquivo comprimido, adicionamos o byte de controle no inicio
    FILE *temp = fopen("temp_huf.tmp", "wb");
    fputc((unsigned char)bits_ultimo, temp); // primeiro byte = bits validos no ultimo byte

    FILE *huf = fopen(nomeComprimido, "rb");
    int b;
    while ((b = fgetc(huf)) != EOF)
        fputc(b, temp);
    fclose(huf);
    fclose(temp);

    // substitui o arquivo comprimido pelo temp
    remove(nomeComprimido);

    // copia temp para o nome correto (sem usar rename pois pode falhar entre sistemas)
    FILE *src = fopen("temp_huf.tmp", "rb");
    FILE *dst = fopen(nomeComprimido, "wb");
    while ((b = fgetc(src)) != EOF)
        fputc(b, dst);
    fclose(src);
    fclose(dst);
    remove("temp_huf.tmp");

    cout << endl << "Compressao concluida com sucesso!" << endl;
    cout << "Arquivo comprimido: " << nomeComprimido << endl;
    cout << "Arvore salva em:    " << nomeArvore << endl;

    // libera a memoria
    liberar(inicio, fim);
    liberar(raiz);

    return 0;
}
