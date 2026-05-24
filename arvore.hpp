#include <iostream>
using namespace std;

// No da arvore de Huffman
// Nos internos tem caractere = '\0' e frequencia = soma dos filhos
struct NoArvore {
    char caractere;
    int frequencia;
    NoArvore *esq; // filho esquerdo (bit 0)
    NoArvore *dir; // filho direito (bit 1)
};

// Inicializa a raiz da arvore
void inicializar(NoArvore *&raiz) {
    raiz = NULL;
}

// Cria um no folha com o caractere e sua frequencia
NoArvore* criar_no(char c, int freq) {
    NoArvore *novo = new NoArvore;
    if (novo == NULL) return NULL;
    novo->caractere = c;
    novo->frequencia = freq;
    novo->esq = NULL;
    novo->dir = NULL;
    return novo;
}

// Cria um no interno juntando dois nos
// A frequencia do novo no = soma das frequencias dos dois filhos
NoArvore* juntar_nos(NoArvore *esq, NoArvore *dir) {
    NoArvore *novo = new NoArvore;
    if (novo == NULL) return NULL;
    novo->caractere = '\0'; // no interno nao tem caractere
    novo->frequencia = esq->frequencia + dir->frequencia;
    novo->esq = esq;
    novo->dir = dir;
    return novo;
}

// Libera toda a memoria da arvore (pos-fixado)
void liberar(NoArvore *&raiz) {
    if (raiz == NULL) return;
    liberar(raiz->esq);
    liberar(raiz->dir);
    delete raiz;
    raiz = NULL;
}

bool remover_no(NoArvore *&raiz, NoArvore *alvo) {
    if (raiz == NULL || alvo == NULL) return false;

    // caso especial: o proprio no raiz eh o alvo
    if (raiz == alvo) {
        liberar(raiz); // raiz passa a ser NULL
        return true;
    }

    // percorre a arvore procurando o pai do no alvo
    // usa uma pilha manual para nao precisar de recursao com retorno duplo
    // implementado de forma iterativa com busca pre-fixada
    NoArvore *pilha[512];
    int topo = 0;
    pilha[topo++] = raiz;

    while (topo > 0) {
        NoArvore *atual = pilha[--topo];

        // verifica se o filho esquerdo eh o alvo
        if (atual->esq == alvo) {
            liberar(atual->esq); // remove o filho e seus descendentes
            atual->esq = NULL;
            return true;
        }

        // verifica se o filho direito eh o alvo
        if (atual->dir == alvo) {
            liberar(atual->dir);
            atual->dir = NULL;
            return true;
        }

        // empilha os filhos para continuar a busca
        if (atual->dir != NULL) pilha[topo++] = atual->dir;
        if (atual->esq != NULL) pilha[topo++] = atual->esq;
    }

    return false; // no nao encontrado na arvore
}


// Salva a arvore no arquivo usando pre-fixado
// Formato: '1' + caractere para folha, '0' para no interno
void salvar_arvore(NoArvore *raiz, FILE *arq) {
    if (raiz == NULL) return;

    if (raiz->esq == NULL && raiz->dir == NULL) {
        // no folha: escreve '1' e o caractere
        fputc('1', arq);
        fputc(raiz->caractere, arq);
    } else {
        // no interno: escreve '0'
        fputc('0', arq);
        salvar_arvore(raiz->esq, arq);
        salvar_arvore(raiz->dir, arq);
    }
}

// Le a arvore do arquivo (reconstrucao)
NoArvore* ler_arvore(FILE *arq) {
    int tipo = fgetc(arq);
    if (tipo == EOF) return NULL;

    if (tipo == '1') {
        // no folha: le o caractere
        char c = (char)fgetc(arq);
        return criar_no(c, 0);
    } else {
        // no interno: reconstroi os dois filhos
        NoArvore *esq = ler_arvore(arq);
        NoArvore *dir = ler_arvore(arq);
        return juntar_nos(esq, dir);
    }
}
