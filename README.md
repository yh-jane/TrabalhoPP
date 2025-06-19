# Problema das N-Rainhas com Processamento Paralelo

Este projeto implementa a solução do problema das N-Rainhas usando processamento sequencial e paralelo com MPI, incluindo análise de simetrias e posições bloqueadas dinâmicas.

## Características Principais

### ✅ Medição de Tempo
- **Código Sequencial**: Medição de tempo usando `clock()` da biblioteca `time.h`
- **Código Paralelo**: Medição de tempo usando `clock()` da biblioteca `time.h`
- Ambos os códigos exibem o tempo de execução em segundos

### ✅ Suporte a Tabuleiros Dinâmicos
- **Suporte até 20x20**: Ambos os códigos suportam tabuleiros de 4x4 até 20x20
- **Argumentos de linha de comando**: Tamanho do tabuleiro é passado como parâmetro
- Estruturas de dados otimizadas para tabuleiros maiores

### ✅ Posições Bloqueadas Dinâmicas
- **Área central adaptativa**: Baseada no tamanho do tabuleiro (tamanho/6)
- **Bloqueios progressivos**: Mais bloqueios para tabuleiros maiores
- **Bordas e diagonais**: Bloqueios estratégicos para aumentar dificuldade

## Estrutura do Projeto

```
Processamento Paralelo Código/
├── Sequencial/
│   ├── sequencial.c          # Implementação sequencial
│   └── sequencial.exe        # Executável (gerado)
├── Paralelo/
│   ├── paralelo.c            # Implementação paralela com MPI
│   └── paralelo.exe          # Executável (gerado)
└── README.md                 # Este arquivo
```

## Funcionalidades

### Análise de Simetrias
- **Rotação 90°**: Gira o tabuleiro 90 graus no sentido horário
- **Rotação 180°**: Gira o tabuleiro 180 graus
- **Rotação 270°**: Gira o tabuleiro 270 graus (ou 90° anti-horário)
- **Espelhamento Horizontal**: Espelha o tabuleiro horizontalmente
- **Espelhamento Vertical**: Espelha o tabuleiro verticalmente
- **Combinações**: Espelhamento + rotações
- **Detecção de duplicatas**: Conta apenas simetrias únicas

### Posições Bloqueadas Dinâmicas
- **Área central**: Bloqueia área central baseada no tamanho (tamanho/6)
- **Bordas**: Bloqueia cantos opostos (0,0) e (N-1,N-1)
- **Diagonais progressivas**: Mais bloqueios para tabuleiros maiores
  - 8x8+: posições (2,2) e (3,3)
  - 12x12+: posições (4,4) e (N-3,N-3)
  - 16x16+: posições (N-4,N-4) e (N-5,N-5)

### Processamento Paralelo
- **4 processos MPI**: Distribuição do trabalho entre 4 processos
- **Distribuição por colunas**: Cada processo processa colunas iniciais diferentes
- **Redução de resultados**: MPI_Reduce para somar soluções e simetrias

## Compilação e Execução

### Windows
```bash
# Código sequencial
cd Sequencial
gcc -o sequencial sequencial.c
sequencial.exe <tamanho>

# Código paralelo
cd Paralelo
mpicc -o paralelo paralelo.c
mpirun -np 4 paralelo.exe <tamanho>
```

### Linux/Mac
```bash
# Código sequencial
cd Sequencial
gcc -o sequencial sequencial.c
./sequencial <tamanho>

# Código paralelo
cd Paralelo
mpicc -o paralelo paralelo.c
mpirun -np 4 ./paralelo <tamanho>
```

### Exemplos de Uso
```bash
# Tabuleiro 8x8
./sequencial 8
mpirun -np 4 ./paralelo 8

# Tabuleiro 12x12
./sequencial 12
mpirun -np 4 ./paralelo 12

# Tabuleiro 16x16 (pode demorar muito!)
./sequencial 16
mpirun -np 4 ./paralelo 16
```

## Requisitos

### Para o Código Sequencial
- Compilador C (gcc, clang, etc.)

### Para o Código Paralelo
- MPI (Message Passing Interface)
- Compilador MPI (mpicc)
- **Exatamente 4 processos** (não mais, não menos)

## Saída dos Programas

### Código Sequencial
```
Tamanho do tabuleiro: 8x8
Posições bloqueadas: área central + bordas + diagonais
Análise de simetrias: rotações e espelhamentos

=== SOLUÇÃO COM X SIMETRIAS ÚNICAS ===
 .  .  .  .  .  .  .  .
 .  .  .  .  .  .  .  .
 .  .  X  X  .  .  .  .
 .  .  X  X  .  .  .  .
 .  .  .  .  .  .  .  .
 .  .  .  .  .  .  .  .
 .  .  .  .  .  .  .  .
 .  .  .  .  .  .  .  .

=== RESULTADOS FINAIS ===
Tempo de execução: X.XXX segundos
Total de soluções encontradas: X
Total de simetrias únicas: X
Média de simetrias por solução: X.XX
```

### Código Paralelo
```
=== EXECUÇÃO PARALELA COM 4 PROCESSOS ===
Tamanho do tabuleiro: 8x8
Processos MPI: 4
Análise de simetrias: rotações e espelhamentos
Distribuição do trabalho:
  Processo 0: colunas 0 4
  Processo 1: colunas 1 5
  Processo 2: colunas 2 6
  Processo 3: colunas 3 7
Iniciando processamento...

=== COMPUTADOR X: SOLUÇÃO COM X SIMETRIAS ===
[Visualização do tabuleiro]

=== RESULTADOS PARALELOS ===
Tempo de execução: X.XXX segundos
Total de soluções encontradas: X
Total de simetrias únicas: X
Média de simetrias por solução: X.XX
Processamento concluído com sucesso!
```

## Validação de Entrada

### Tamanho do Tabuleiro
- **Mínimo**: 4x4 (problema das 4 rainhas)
- **Máximo**: 20x20 (limite da estrutura de dados)
- **Validação**: Ambos os programas validam o tamanho fornecido

### Processos MPI
- **Exato**: 4 processos (não mais, não menos)
- **Validação**: Programa paralelo verifica número correto de processos

## Comparação de Performance

Com as implementações atuais, é possível comparar:

1. **Tempo de execução** entre versões sequencial e paralela
2. **Escalabilidade** para diferentes tamanhos de tabuleiro
3. **Eficiência** do processamento paralelo
4. **Impacto dos bloqueios** na complexidade do problema

## Configuração de Bloqueios

Os bloqueios são configurados dinamicamente baseados no tamanho:

```c
// Área central adaptativa
int centro = estado.tamanho / 2;
int area = estado.tamanho / 6;

// Bloqueios progressivos por tamanho
if (estado.tamanho >= 8) {
    // Bloqueios adicionais para tabuleiros 8x8+
}
if (estado.tamanho >= 12) {
    // Bloqueios adicionais para tabuleiros 12x12+
}
```

## Limitações e Considerações

### Performance
- **Tabuleiros grandes**: 16x16 e 20x20 podem demorar muito tempo
- **Complexidade exponencial**: O problema cresce exponencialmente com o tamanho
- **Memória**: Estruturas de dados otimizadas para até 20x20

### Processamento Paralelo
- **4 processos fixos**: Não é escalável para mais processos
- **Distribuição simples**: Por colunas iniciais
- **Sincronização**: Usa MPI_Reduce para resultados finais

## Próximos Passos

- [x] ✅ Argumentos de linha de comando para tamanho do tabuleiro
- [x] ✅ Medição de tempo de execução
- [x] ✅ Suporte a tabuleiros maiores (até 20x20)
- [x] ✅ Posições bloqueadas dinâmicas
- [ ] Implementar escalabilidade para mais processos MPI
- [ ] Adicionar mais métricas de performance (memória, CPU)
- [ ] Implementar otimizações adicionais (bitboards, etc.)
- [ ] Criar interface gráfica para visualização
- [ ] Adicionar testes automatizados
