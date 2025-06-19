# Problema das N-Rainhas com Processamento Paralelo

## 📋 Visão Geral

Este projeto implementa uma versão modificada do problema clássico das N-rainhas, adicionando **posições bloqueadas dinâmicas** e **análise de simetrias**. A implementação inclui versões sequencial e paralela usando MPI, permitindo comparação de performance.

## 🔍 Diferenças do Problema Clássico das N-Rainhas

### Problema Clássico
- **Objetivo**: Colocar N rainhas em um tabuleiro N×N sem que se ataquem
- **Restrições**: Apenas as regras básicas do xadrez (mesma linha, coluna ou diagonal)
- **Soluções**: Todas as configurações válidas são consideradas soluções

### Versão Modificada (Este Projeto)
- **Posições Bloqueadas**: Áreas do tabuleiro onde rainhas não podem ser colocadas
- **Análise de Simetrias**: Identificação e contagem de simetrias únicas
- **Complexidade Aumentada**: Bloqueios tornam o problema mais desafiador
- **Métricas Avançadas**: Contagem de simetrias únicas por solução

## 🏗️ Arquitetura dos Códigos

### Estrutura de Dados
```c
typedef struct {
    int tabuleiro[MAX];           // Posição da rainha em cada linha
    int tamanho;                  // Tamanho do tabuleiro (4-20)
    bool bloqueado[MAX][MAX];     // Matriz de posições bloqueadas
} Estado;
```

### Características Comuns
- **Suporte**: Tabuleiros de 4×4 até 20×20
- **Argumentos**: Tamanho passado via linha de comando
- **Validação**: Verificação de entrada e limites
- **Medição**: Tempo de execução usando `clock()`

## 🔒 Sistema de Posições Bloqueadas

### Bloqueios Dinâmicos
```c
// Área central adaptativa
int centro = estado.tamanho / 2;
int area = estado.tamanho / 6;  // Proporcional ao tamanho

// Bloqueios progressivos por tamanho
if (estado.tamanho >= 8) {
    estado.bloqueado[2][2] = true;
    estado.bloqueado[3][3] = true;
}
if (estado.tamanho >= 12) {
    estado.bloqueado[4][4] = true;
    estado.bloqueado[estado.tamanho-3][estado.tamanho-3] = true;
}
```

### Tipos de Bloqueios
1. **Área Central**: Bloqueia região central proporcional ao tamanho
2. **Bordas**: Bloqueia cantos opostos (0,0) e (N-1,N-1)
3. **Diagonais Progressivas**: Mais bloqueios para tabuleiros maiores

## 🔄 Análise de Simetrias

### Transformações Implementadas
```c
// 8 transformações possíveis
rotacionar90()      // Rotação 90° horário
rotacionar180()     // Rotação 180°
rotacionar270()     // Rotação 270° (90° anti-horário)
espelharHorizontal() // Espelhamento horizontal
espelharVertical()   // Espelhamento vertical
// + combinações: espelho + rotações
```

### Detecção de Duplicatas
- **Algoritmo**: Comparação de todas as transformações
- **Resultado**: Contagem apenas de simetrias únicas
- **Complexidade**: O(n²) para cada solução

## 📊 Comparação: Sequencial vs Paralelo

### Código Sequencial (`sequencial.c`)

#### Características
- **Execução**: Processamento em um único thread
- **Estrutura**: Recursão simples com backtracking
- **Saída**: Todas as soluções encontradas
- **Dependências**: Apenas bibliotecas padrão C

#### Algoritmo Principal
```c
void resolver(Estado* estado, int linha, int* total, int* total_simetrias) {
    if (linha == estado->tamanho) {
        // Solução encontrada
        (*total)++;
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
```

### Código Paralelo (`paralelo.c`)

#### Características
- **Execução**: 4 processos MPI obrigatórios
- **Distribuição**: Por colunas iniciais (rank-based)
- **Sincronização**: MPI_Reduce para resultados finais
- **Dependências**: MPI + bibliotecas padrão C

#### Estratégia de Paralelização
```c
// Distribuição do trabalho
for (int col_inicial = rank; col_inicial < estado.tamanho; col_inicial += size) {
    if (posicaoSegura(&estado, 0, col_inicial)) {
        estado.tabuleiro[0] = col_inicial;
        resolver(&estado, 1, &total_local, &simetrias_local, rank);
    }
}

// Redução de resultados
MPI_Reduce(&total_local, &total_global, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
MPI_Reduce(&simetrias_local, &simetrias_global, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
```

## 🔍 Análise Detalhada das Diferenças

### 1. **Estrutura e Dependências**

| Aspecto | Sequencial | Paralelo |
|---------|------------|----------|
| **Bibliotecas** | `stdio.h`, `stdlib.h`, `stdbool.h`, `string.h`, `time.h` | + `mpi.h` |
| **Inicialização** | Direta | `MPI_Init()`, `MPI_Comm_rank()`, `MPI_Comm_size()` |
| **Finalização** | `return 0` | `MPI_Finalize()` |
| **Validação** | Tamanho do tabuleiro | Tamanho + número de processos |

### 2. **Distribuição do Trabalho**

#### Sequencial
- **Estratégia**: Backtracking completo
- **Exploração**: Todas as possibilidades sequencialmente
- **Memória**: Stack de recursão

#### Paralelo
- **Estratégia**: Distribuição por colunas iniciais
- **Processo 0**: Colunas 0, 4, 8, 12, 16, 20...
- **Processo 1**: Colunas 1, 5, 9, 13, 17...
- **Processo 2**: Colunas 2, 6, 10, 14, 18...
- **Processo 3**: Colunas 3, 7, 11, 15, 19...

### 3. **Saída e Visualização**

#### Sequencial
```c
void imprimirTabuleiroComSimetria(const Estado* estado, int simetrias) {
    printf("=== SOLUÇÃO COM %d SIMETRIAS ÚNICAS ===\n", simetrias);
    imprimirTabuleiro(estado);
}
```

#### Paralelo
```c
void imprimirTabuleiroComSimetria(const Estado* estado, int simetrias, int rank) {
    printf("=== COMPUTADOR %d: SOLUÇÃO COM %d SIMETRIAS ===\n", rank, simetrias);
    imprimirTabuleiro(estado);
}

// Limitação de saída para evitar sobrecarga
if (*total <= 3) {
    imprimirTabuleiroComSimetria(estado, simetrias, rank);
}
```

### 4. **Gestão de Resultados**

#### Sequencial
- **Acumulação**: Variáveis globais simples
- **Saída**: Resultados finais diretos

#### Paralelo
- **Acumulação Local**: `total_local`, `simetrias_local`
- **Redução Global**: `MPI_Reduce()` para somar resultados
- **Sincronização**: Todos os processos aguardam redução

## 📈 Análise de Performance

### Fatores que Afetam Performance

1. **Tamanho do Tabuleiro**
   - Complexidade exponencial: O(n!)
   - Tabuleiros 16×16+ podem demorar horas

2. **Posições Bloqueadas**
   - Reduzem espaço de busca
   - Aumentam dificuldade de encontrar soluções válidas

3. **Análise de Simetrias**
   - Overhead de O(n²) por solução
   - 8 transformações por solução encontrada

### Vantagens da Paralelização

1. **Speedup Teórico**: Até 4x com 4 processos
2. **Distribuição de Carga**: Trabalho dividido igualmente
3. **Escalabilidade**: Pode ser estendido para mais processos

### Limitações da Implementação Atual

1. **Processos Fixos**: Exatamente 4 processos obrigatórios
2. **Distribuição Simples**: Apenas por colunas iniciais
3. **Sincronização**: Overhead de comunicação MPI

## 🧮 Complexidade Algorítmica

### Análise Teórica

| Componente | Complexidade | Descrição |
|------------|--------------|-----------|
| **Backtracking** | O(n!) | Espaço de busca completo |
| **Verificação de Segurança** | O(n) | Por posição testada |
| **Análise de Simetrias** | O(n²) | Por solução encontrada |
| **Posições Bloqueadas** | O(1) | Verificação de matriz |

### Complexidade Total
- **Sequencial**: O(n! × n²) no pior caso
- **Paralelo**: O(n! × n² / p) onde p = número de processos

## 🔧 Configuração e Execução

### Compilação

#### Sequencial
```bash
cd Sequencial
gcc -o sequencial sequencial.c
```

#### Paralelo
```bash
cd Paralelo
mpicc -o paralelo paralelo.c
```

### Execução

#### Sequencial
```bash
./sequencial <tamanho>
# Exemplo: ./sequencial 8
```

#### Paralelo
```bash
mpirun -np 4 ./paralelo <tamanho>
# Exemplo: mpirun -np 4 ./paralelo 8
```

### Validação de Entrada

| Parâmetro | Sequencial | Paralelo |
|-----------|------------|----------|
| **Tamanho** | 4-20 | 4-20 |
| **Processos** | N/A | Exatamente 4 |
| **Argumentos** | 1 (tamanho) | 1 (tamanho) |

## 📊 Métricas de Saída

### Dados Coletados
1. **Tempo de Execução**: Em segundos
2. **Total de Soluções**: Número de configurações válidas
3. **Simetrias Únicas**: Total de simetrias distintas
4. **Média de Simetrias**: Simetrias por solução

### Formato de Saída
```
=== RESULTADOS FINAIS ===
Tempo de execução: X.XXX segundos
Total de soluções encontradas: X
Total de simetrias únicas: X
Média de simetrias por solução: X.XX
```

## 🎯 Conclusões e Insights

### Pontos Fortes
1. **Implementação Robusta**: Validação completa de entrada
2. **Análise de Simetrias**: Característica única e interessante
3. **Posições Bloqueadas**: Aumenta complexidade de forma controlada
4. **Medição de Performance**: Comparação direta entre abordagens

### Limitações Identificadas
1. **Escalabilidade Limitada**: 4 processos fixos
2. **Distribuição Simples**: Não considera balanceamento de carga
3. **Overhead de Simetrias**: Pode ser otimizado
4. **Memória**: Estruturas fixas para até 20×20

### Melhorias Possíveis
1. **Escalabilidade**: Suporte a número variável de processos
2. **Otimizações**: Bitboards, poda de árvore
3. **Balanceamento**: Distribuição dinâmica de carga
4. **Interface**: Argumentos mais flexíveis

## 🔬 Comparação com Implementações Clássicas

### Diferenças Principais
1. **Posições Bloqueadas**: Não existe no problema clássico
2. **Análise de Simetrias**: Geralmente não implementada
3. **Complexidade**: Maior que o problema original
4. **Aplicabilidade**: Mais próximo de problemas reais

### Similaridades
1. **Algoritmo Base**: Backtracking com verificação de segurança
2. **Estrutura de Dados**: Representação por colunas
3. **Validação**: Mesmas regras do xadrez
4. **Complexidade**: Natureza exponencial mantida
