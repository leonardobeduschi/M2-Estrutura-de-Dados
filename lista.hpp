#include <iostream>
using namespace std;

// No da lista duplamente encadeada
// Armazena um caractere e sua frequencia no arquivo
struct NoLista {
    char caractere;
    int frequencia;
    NoLista *ant;
    NoLista *prox;
};

// Inicializa a lista vazia
void inicializar(NoLista *&inicio, NoLista *&fim) {
    inicio = NULL;
    fim = NULL;
}

// Insere no final da lista
bool inserir(NoLista *&inicio, NoLista *&fim, char c, int freq) {
    NoLista *novo = new NoLista;
    if (novo == NULL) return false;
    novo->caractere = c;
    novo->frequencia = freq;
    novo->prox = NULL;
    novo->ant = fim;
    if (fim == NULL) {
        inicio = novo;
        fim = novo;
    } else {
        fim->prox = novo;
        fim = novo;
    }
    return true;
}

// Busca um caractere na lista, retorna ponteiro ou NULL
NoLista* buscar(NoLista *inicio, char c) {
    NoLista *atual = inicio;
    while (atual != NULL) {
        if (atual->caractere == c) return atual;
        atual = atual->prox;
    }
    return NULL;
}

// Remove um no especifico da lista
void remover(NoLista *&inicio, NoLista *&fim, NoLista *no) {
    if (no == NULL) return;
    if (no->ant != NULL) no->ant->prox = no->prox;
    else                 inicio = no->prox;
    if (no->prox != NULL) no->prox->ant = no->ant;
    else                  fim = no->ant;
    delete no;
}

// Imprime a lista na tela
void imprimir(NoLista *inicio) {
    NoLista *atual = inicio;
    while (atual != NULL) {
        if      (atual->caractere == '\n') cout << "'\\n': " << atual->frequencia << endl;
        else if (atual->caractere == ' ')  cout << "' ' : " << atual->frequencia << endl;
        else                               cout << "'" << atual->caractere << "'  : " << atual->frequencia << endl;
        atual = atual->prox;
    }
}

// Libera toda a memoria da lista
void liberar(NoLista *&inicio, NoLista *&fim) {
    NoLista *atual = inicio;
    while (atual != NULL) {
        NoLista *prox = atual->prox;
        delete atual;
        atual = prox;
    }
    inicio = NULL;
    fim = NULL;
}