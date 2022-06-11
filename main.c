#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#ifdef _WIN64
    #define CLEAR_METODO "cls"
    #include <Windows.h>
    #define SLEEP Sleep(200) // tempo em MS
#else
    #define CLEAR_METODO "clear"
    #include <unistd.h>
    #define SLEEP usleep(0.2 * 1000000) // coeficiente (0.2) em segundos
#endif

#define SIZE 30

#define X_INICIAL 1
#define Y_INICIAL 1

#define LIVRE 0
#define PAREDE 1
#define VISITADA 2
#define BECO 3
#define BONECO 4
#define SAIDA 5

#define CHAR_LIVRE 32
#define CHAR_PAREDE 35
#define CHAR_VISITADA 46
#define CHAR_BECO 120
#define CHAR_BONECO 79
#define CHAR_SAIDA 32

#define COEFICIENTE_PAREDES_INTERNAS 1/2

#define MALLOC_ERROR "ERRO DE ALOCAÇÃO\n"
#define INVALID_MEMORY_ACCESS "ACESSO DE MEMÓRIA INVÁLIDO\n"

#define MSG_VITORIA "\n\n           VENCEU\n\n\n"
#define MSG_DERROTA "\n\n           PERDEDOR\n\n\n"

typedef struct node {
    int valor;
    struct node *prox;
    struct node *ant;
} Node;

typedef struct Root {
    int tamanho;
    struct node *topo;
    struct node *inicio;
} Root;

int matriz[SIZE][SIZE];

Root root;

int char_map[6];

int paredes_internas;

// metodos da pilha
void push(int x, int y);
int pop();
void freeAll();
void freeNode(Node *node);

void constroiMatriz();
void initialize();
void printMatriz();
void printPath();
int getValor(int x, int y);
void getCoordenadas(int valor, int *x, int *y);

int podeMover(int x, int y) {
    return matriz[x][y] == LIVRE || matriz[x][y] == SAIDA;
}

int main() {
    initialize();

    int top = root.topo->valor;
    
    int x_do_boneco;
    int y_do_boneco;

    getCoordenadas(top, &x_do_boneco, &y_do_boneco);

    int total_passos = 0;

    while(top != getValor(SIZE - 2, SIZE - 1)) {
        system(CLEAR_METODO);
        total_passos++;

        int andou = 1;
        matriz[x_do_boneco][y_do_boneco] = VISITADA;

        if(podeMover(x_do_boneco + 1, y_do_boneco)) {
            x_do_boneco++;
        } else if(podeMover(x_do_boneco, y_do_boneco + 1)) {
            y_do_boneco++;
        } else if(podeMover(x_do_boneco - 1, y_do_boneco)) {
            x_do_boneco--;
        } else if(podeMover(x_do_boneco, y_do_boneco - 1)) {
            y_do_boneco--;
        } else {
            andou = 0;

            matriz[x_do_boneco][y_do_boneco] = BECO;

            int a = pop();
            if(root.tamanho == 0) {
                system(CLEAR_METODO);
                printMatriz();
                printf(MSG_DERROTA);
                freeAll();
                return 0;
            }

            top = root.topo->valor;
            getCoordenadas(top, &x_do_boneco, &y_do_boneco);
        }

        matriz[x_do_boneco][y_do_boneco] = BONECO;

        if(andou) {
            push(x_do_boneco, y_do_boneco);
        }

        printMatriz();

        printf("\n\n");
        printf("ATUAL: (%d, %d) | TOTAL PASSOS: %d\n", x_do_boneco, y_do_boneco, total_passos);
        printf("PAREDES INTERNAS %d\n", paredes_internas);
        printf("PILHA:\n");
        printPath(root.inicio);

        SLEEP;
        
        top = getValor(x_do_boneco, y_do_boneco);
    }

    system(CLEAR_METODO);
    printMatriz();
    printf(MSG_VITORIA);

    return 0;
}

void getCoordenadas(int valor, int *x, int *y) {
    *x = valor / 100;
    *y = valor % 100;
}

int getValor(int x, int y) {
    return (x * 100 + y);
}

void push(int x, int y) {
    int value = getValor(x, y);

    Node *atual = (Node *) malloc(sizeof(Node));
    if(atual == NULL) {
        printf(MALLOC_ERROR);
        freeAll();
        exit(1);
    }

    atual->valor = value;
    atual->prox = NULL;
    atual->ant = root.topo;

    if(root.topo != NULL) {
        root.topo->prox = atual;
    } else {
        root.inicio = atual;
    }
    root.topo = atual;
    root.tamanho++;
}

int pop() {
    Node *topo = root.topo;
    if(topo == NULL) {
        printf(INVALID_MEMORY_ACCESS);
        freeAll();
        exit(1);
    }

    root.topo = topo->ant;
    if(root.topo != NULL) {
        root.topo->prox = NULL;
    } else {
      root.inicio = NULL;
    }

    root.tamanho--;

    int valor = topo->valor;
    
    free(topo);
    
    return valor;
}

void freeAll() {
  freeNode(root.inicio);
  root.inicio = NULL;
  root.topo = NULL;
  root.tamanho = 0;
}

void freeNode(Node *node) {
  if(node == NULL) {
    return;
  }
  
  Node *prox = node->prox;
  free(node);

  freeNode(prox);
}

void initialize() {
    root.topo = NULL;
    root.inicio = NULL;
    push(X_INICIAL, Y_INICIAL);

    char_map[0] = CHAR_LIVRE;
    char_map[1] = CHAR_PAREDE;
    char_map[2] = CHAR_VISITADA;
    char_map[3] = CHAR_BECO;
    char_map[4] = CHAR_BONECO;
    char_map[5] = CHAR_SAIDA;

    srand(time(NULL));

    constroiMatriz();
}

void printMatriz() {
    // PRINTANDO A MATRIZ
    for(int x = 0; x < SIZE; x++) {
        for(int y = 0; y < SIZE; y++) {
            printf("%c", char_map[matriz[x][y]]);
        }
        printf("\n");
    }
}

void printPath(Node *node) {
    if(node == NULL) {
        printf("\n");
        return;
    }

    int x;
    int y;
    getCoordenadas(node->valor, &x, &y);
    printf("(%d,%d) ", x, y);

    printPath(node->prox);
}

void constroiMatriz() {
    // CONSTRÓI PAREDES EXTERNAS
    for(int i = 0; i < SIZE; i++) {
        matriz[0][i] = PAREDE;
        matriz[SIZE - 1][i] = PAREDE;
        matriz[i][0] = PAREDE;
        matriz[i][SIZE - 1] = PAREDE;
    }

    // CONSTRÓI PAREDES INTERNAS
    paredes_internas = rand() % (((SIZE - 1) * (SIZE - 1)) * COEFICIENTE_PAREDES_INTERNAS)  + 1;
    
    for(int i = 0; i < paredes_internas; i++) {
        int x = rand() % (SIZE - 2) + 1;
        int y = rand() % (SIZE - 2) + 1;

        matriz[x][y] = PAREDE;
    }

    // DEFINE POSIÇÃO INICIAL DO BONECO
    matriz[X_INICIAL][Y_INICIAL] = BONECO;

    // DEFINE POSIÇÃO DE SAIDA DO BONECO
    matriz[SIZE - 2][SIZE - 1] = SAIDA;
}
