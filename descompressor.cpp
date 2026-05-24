// Trabalho 2 - Compressao de Huffman
// Alunos: Leonardo Beduschi, Lucas Piva e Deusdith
// Disciplina: Estruturas de Dados
// Aplicacao de DESCOMPRESSAO

#include <iostream>
using namespace std;

#include "lista.hpp"
#include "arvore.hpp"

// -------------------------------------------------------
// Descompressao usando a arvore de Huffman
// -------------------------------------------------------

// Le o arquivo comprimido e reconstroi o texto original
// usando a arvore de Huffman carregada do disco
void descomprimir_arquivo(const char *nomeComprimido, const char *nomeSaida, NoArvore *raiz) {
    FILE *comprimido = fopen(nomeComprimido, "rb");
    if (comprimido == NULL) {
        cout << "Erro ao abrir arquivo comprimido!" << endl;
        return;
    }

    FILE *saida = fopen(nomeSaida, "w");
    if (saida == NULL) {
        cout << "Erro ao criar arquivo de saida!" << endl;
        fclose(comprimido);
        return;
    }

    // primeiro byte do arquivo: quantos bits sao validos no ULTIMO byte de dados
    int bits_validos_ultimo = (unsigned char)fgetc(comprimido);

    // le todos os bytes de dados para um array temporario
    // (precisamos saber qual eh o ultimo byte para usar bits_validos_ultimo)
    unsigned char bytes[1000000]; // array para os bytes comprimidos
    int total_bytes = 0;

    int b;
    while ((b = fgetc(comprimido)) != EOF) {
        bytes[total_bytes] = (unsigned char)b;
        total_bytes++;
    }
    fclose(comprimido);

    if (total_bytes == 0) {
        cout << "Arquivo comprimido vazio!" << endl;
        fclose(saida);
        return;
    }

    // percorre a arvore bit a bit para recuperar os caracteres
    NoArvore *atual = raiz;

    for (int i = 0; i < total_bytes; i++) {
        unsigned char byte_atual = bytes[i];

        // quantos bits desse byte sao validos?
        int bits_validos;
        if (i == total_bytes - 1)
            bits_validos = bits_validos_ultimo; // ultimo byte pode ter menos bits
        else
            bits_validos = 8; // bytes do meio tem sempre 8 bits

        // processa cada bit do byte (do mais significativo para o menos)
        for (int bit = 7; bit >= 8 - bits_validos; bit--) {
            int valor_bit = (byte_atual >> bit) & 1; // pega o bit na posicao 'bit'

            // navega na arvore: 0 = esquerda, 1 = direita
            if (valor_bit == 0)
                atual = atual->esq;
            else
                atual = atual->dir;

            // se chegou em uma folha, encontrou um caractere
            if (atual != NULL && atual->esq == NULL && atual->dir == NULL) {
                fputc(atual->caractere, saida);
                atual = raiz; // volta para a raiz para decodificar o proximo
            }

            // caso especial: arvore com so um tipo de caractere
            if (atual == NULL) {
                atual = raiz;
            }
        }
    }

    fclose(saida);
}

// -------------------------------------------------------
// Programa principal
// -------------------------------------------------------

int main() {
    char nomeComprimido[200];
    char nomeArvore[200];
    char nomeSaida[200];

    cout << "=== Descompressor de Huffman ===" << endl;
    cout << "Digite o nome do arquivo comprimido (.huf): ";
    cin >> nomeComprimido;

    // monta o nome do arquivo da arvore trocando .huf por .arv
    // e o nome do arquivo de saida adicionando _recuperado.txt
    int i = 0;
    while (nomeComprimido[i] != '\0') {
        nomeArvore[i] = nomeComprimido[i];
        nomeSaida[i] = nomeComprimido[i];
        i++;
    }

    // remove a extensao .huf dos nomes (volta 4 caracteres)
    i -= 4;

    // adiciona .arv
    char ext1[] = ".arv";
    int j = 0;
    while (ext1[j] != '\0') { nomeArvore[i + j] = ext1[j]; j++; }
    nomeArvore[i + j] = '\0';

    // adiciona _recuperado.txt
    char ext2[] = "_recuperado.txt";
    int k = 0;
    while (ext2[k] != '\0') { nomeSaida[i + k] = ext2[k]; k++; }
    nomeSaida[i + k] = '\0';

    // ---- Passo 1: carregar a arvore do disco ----
    cout << endl << "Carregando arvore de " << nomeArvore << "..." << endl;

    FILE *arqArvore = fopen(nomeArvore, "rb");
    if (arqArvore == NULL) {
        cout << "Erro ao abrir arquivo da arvore!" << endl;
        cout << "Certifique-se de que o arquivo " << nomeArvore << " existe." << endl;
        return 1;
    }

    NoArvore *raiz = ler_arvore(arqArvore);
    fclose(arqArvore);

    if (raiz == NULL) {
        cout << "Erro ao carregar a arvore!" << endl;
        return 1;
    }

    cout << "Arvore carregada com sucesso!" << endl;

    // ---- Passo 2: descomprimir o arquivo ----
    cout << "Descomprimindo " << nomeComprimido << "..." << endl;
    descomprimir_arquivo(nomeComprimido, nomeSaida, raiz);

    cout << endl << "Descompressao concluida!" << endl;
    cout << "Arquivo recuperado salvo em: " << nomeSaida << endl;

    // libera a memoria da arvore
    liberar(raiz);

    return 0;
}
