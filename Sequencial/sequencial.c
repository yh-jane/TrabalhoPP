#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define MAX 20

typedef struct {
    int tabuleiro[MAX];
    int tamanho;
    bool bloqueado[MAX][MAX];
} Estado;

// Funções de simetria
int compararTabuleiros(int* a, int* b, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        if (a[i] != b[i]) return 0;
    }
    return 1;
}

// Rotaciona 90 graus
void rotacionar90(int* origem, int* destino, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        destino[origem[i]] = tamanho - 1 - i;
    }
}

// Espelha horizontalmente
void espelharHorizontal(int* origem, int* destino, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        destino[i] = tamanho - 1 - origem[i];
    }
}

// Rotaciona 180 graus
void rotacionar180(int* origem, int* destino, int tamanho) {
    int temp[MAX];
    rotacionar90(origem, temp, tamanho);
    rotacionar90(temp, destino, tamanho);
}

// Rotaciona 270 graus
void rotacionar270(int* origem, int* destino, int tamanho) {
    int temp[MAX];
    rotacionar180(origem, temp, tamanho);
    rotacionar90(temp, destino, tamanho);
}

// Espelha verticalmente (linha 0 troca com linha N-1, etc.)
void espelharVertical(int* origem, int* destino, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        destino[tamanho - 1 - i] = origem[i];
    }
}

// Conta quantas simetrias únicas existem
int contarSimetriasUnicas(int* original, int tamanho) {
    int variantes[8][MAX];
    int unicas = 1;

    memcpy(variantes[0], original, tamanho * sizeof(int));

    rotacionar90(original, variantes[1], tamanho);
    rotacionar180(original, variantes[2], tamanho);
    rotacionar270(original, variantes[3], tamanho);
    espelharHorizontal(original, variantes[4], tamanho);
    espelharVertical(original, variantes[5], tamanho);
    rotacionar90(variantes[4], variantes[6], tamanho);  // espelho + rotação
    rotacionar90(variantes[5], variantes[7], tamanho);

    // Verifica duplicatas
    int total = 1;
    for (int i = 1; i < 8; i++) {
        int unica = 1;
        for (int j = 0; j < total; j++) {
            if (compararTabuleiros(variantes[i], variantes[j], tamanho)) {
                unica = 0;
                break;
            }
        }
        if (unica) {
            memcpy(variantes[total], variantes[i], tamanho * sizeof(int));
            total++;
        }
    }
    return total;
}

// Verifica se uma solução é válida considerando posições bloqueadas
bool solucaoValidaComBloqueios(int* tabuleiro, bool bloqueado[MAX][MAX], int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        if (bloqueado[i][tabuleiro[i]]) {
            return false;
        }
    }
    return true;
}

bool posicaoSegura(const Estado* estado, int linha, int coluna) {
    if (estado->bloqueado[linha][coluna]) {
        return false; // posição bloqueada
    }

    for (int i = 0; i < linha; i++) {
        int q_col = estado->tabuleiro[i];
        if (q_col == coluna || abs(q_col - coluna) == abs(i - linha)) {
            return false;
        }
    }
    return true;
}

void imprimirTabuleiro(const Estado* estado) {
    for (int i = 0; i < estado->tamanho; i++) {
        for (int j = 0; j < estado->tamanho; j++) {
            if (estado->tabuleiro[i] == j) printf(" Q ");
            else if (estado->bloqueado[i][j]) printf(" X ");
            else printf(" . ");
        }
        printf("\n");
    }
    printf("\n");
}

void imprimirTabuleiroComSimetria(const Estado* estado, int simetrias) {
    printf("=== SOLUÇÃO COM %d SIMETRIAS ÚNICAS ===\n", simetrias);
    imprimirTabuleiro(estado);
}

void resolver(Estado* estado, int linha, int* total, int* total_simetrias) {
    if (linha == estado->tamanho) {
        (*total)++;

        // Calcula simetrias únicas
        int simetrias = contarSimetriasUnicas(estado->tabuleiro, estado->tamanho);
        (*total_simetrias) += simetrias;

        imprimirTabuleiroComSimetria(estado, simetrias);
        return;
    }
    for (int col = 0; col < estado->tamanho; col++) {
        if (posicaoSegura(estado, linha, col)) {
            estado->tabuleiro[linha] = col;
            resolver(estado, linha + 1, total, total_simetrias);
        }
    }
}

int main(int argc, char *argv[]) {
    Estado estado;
    clock_t inicio, fim;
    double tempo_gasto;

    // Verifica argumentos de linha de comando
    if (argc != 2) {
        printf("Uso: %s <tamanho>\n", argv[0]);
        printf("Exemplo: %s 20\n", argv[0]);
        printf("Tamanho deve estar entre 4 e 20\n");
        return 1;
    }

    estado.tamanho = atoi(argv[1]);

    // Valida o tamanho
    if (estado.tamanho < 4 || estado.tamanho > 20) {
        printf("ERRO: Tamanho deve estar entre 4 e 20\n");
        printf("Tamanho fornecido: %d\n", estado.tamanho);
        return 1;
    }

    // Inicializa tabuleiro e bloqueios
    for (int i = 0; i < estado.tamanho; i++) {
        for (int j = 0; j < estado.tamanho; j++) {
            estado.bloqueado[i][j] = false;
        }
    }

    // Bloqueia múltiplas áreas para aumentar significativamente a dificuldade
    // Área central baseada no tamanho (menor para tabuleiros pequenos)
    int centro = estado.tamanho / 2;
    int area = estado.tamanho / 6; // Reduzido de /4 para /6

    for (int i = centro - area; i <= centro + area; i++) {
        for (int j = centro - area; j <= centro + area; j++) {
            if (i >= 0 && i < estado.tamanho && j >= 0 && j < estado.tamanho) {
                estado.bloqueado[i][j] = true;
            }
        }
    }

    // Bloqueia apenas algumas bordas (não todas)
    estado.bloqueado[0][0] = true;
    estado.bloqueado[estado.tamanho-1][estado.tamanho-1] = true;

    // Bloqueia apenas algumas posições diagonais (menos para tabuleiros pequenos)
    if (estado.tamanho >= 8) {
        estado.bloqueado[2][2] = true;
        estado.bloqueado[3][3] = true;
    }
    if (estado.tamanho >= 12) {
        estado.bloqueado[4][4] = true;
        estado.bloqueado[estado.tamanho-3][estado.tamanho-3] = true;
    }
    if (estado.tamanho >= 16) {
        estado.bloqueado[estado.tamanho-4][estado.tamanho-4] = true;
        estado.bloqueado[estado.tamanho-5][estado.tamanho-5] = true;
    }

    printf("Tamanho do tabuleiro: %dx%d\n", estado.tamanho, estado.tamanho);
    printf("Posições bloqueadas: área central + bordas + diagonais\n");
    printf("Análise de simetrias: rotações e espelhamentos\n\n");

    int total = 0;
    int total_simetrias = 0;

    // Inicia medição de tempo
    inicio = clock();

    resolver(&estado, 0, &total, &total_simetrias);

    // Finaliza medição de tempo
    fim = clock();
    tempo_gasto = ((double)(fim - inicio)) / CLOCKS_PER_SEC;

    printf("=== RESULTADOS FINAIS ===\n");
    printf("Tempo de execução: %.3f segundos\n", tempo_gasto);
    printf("Total de soluções encontradas: %d\n", total);
    printf("Total de simetrias únicas: %d\n", total_simetrias);
    printf("Média de simetrias por solução: %.2f\n",
           total > 0 ? (float)total_simetrias / total : 0.0);

    return 0;
}
