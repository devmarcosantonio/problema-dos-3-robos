# Simulação de Fábrica de Robôs - Problema dos 3 Robôs

## Descrição do Projeto

Este projeto implementa uma simulação de uma fábrica automatizada onde **3 robôs** trabalham simultaneamente, compartilhando recursos limitados. O programa demonstra conceitos fundamentais de **programação concorrente** usando threads, semáforos e mutexes.

## Regras do Sistema

###  **Robôs (Threads)**
- **Quantidade**: 3 robôs trabalhando simultaneamente
- **Funcionamento**: Cada robô opera em um ciclo infinito de trabalho
- **Período de Atividade**: Cada robô executa uma tarefa a cada **5 segundos (5000ms)**

###  **Ferramentas (Recursos Compartilhados)**
- **Quantidade**: Apenas **2 ferramentas** disponíveis na fábrica
- **Acesso**: Controlado por semáforo (máximo 2 robôs podem trabalhar simultaneamente)
- **Regra**: Um robô só pode trabalhar se conseguir adquirir uma ferramenta

### ⏱ **Deadlines e Temporização**
- **Deadline Máximo**: **1300ms (1.3 segundos)** para completar cada tarefa
- **Tempo de Trabalho**: **600ms** simulando o processamento da tarefa
- **Penalidade**: Tarefas que excedem o deadline são contabilizadas como "perdidas"

###  **Monitoramento**
- **Contador de Tarefas**: Rastreia quantas tarefas foram executadas
- **Contador de Falhas**: Registra quantos deadlines foram perdidos
- **Logs em Tempo Real**: Exibe o status de cada robô durante a execução

## Como Funciona

### Fluxo de Execução de Cada Robô:

1. **Controle de Período**
   - Aguarda até completar o período de 5 segundos
   - Calcula o tempo decorrido desde a última tarefa

2. **Solicitação de Ferramenta**
   - Tenta adquirir uma das 2 ferramentas disponíveis
   - Se não houver ferramentas livres, fica bloqueado até uma ser liberada

3. **Execução da Tarefa**
   - Incrementa contador global de tarefas (seção crítica protegida por mutex)
   - Simula trabalho por 600ms
   - Exibe status atual no console

4. **Liberação de Recursos**
   - Devolve a ferramenta para outros robôs
   - Verifica se cumpriu o deadline de 1300ms

5. **Verificação de Performance**
   - Se deadline foi perdido: incrementa contador de falhas
   - Exibe resultado (sucesso ou falha) no console

## Conceitos de Programação Concorrente Demonstrados

### **Semáforos**
```c
sem_t ferramentas;  // Controla acesso às 2 ferramentas
sem_wait(&ferramentas);  // Adquire ferramenta
sem_post(&ferramentas);  // Libera ferramenta
```

### **Mutexes**
```c
pthread_mutex_t mutex;          // Protege contador de tarefas
pthread_mutex_t mutex_perdidas; // Protege contador de falhas
```

###  **Threads (Pthreads)**
```c
pthread_t threads[NUM_ROBOS];   // Array de threads
pthread_create(&threads[i], NULL, robo, &ids[i]);  // Criação
pthread_join(threads[i], NULL); // Aguarda término
```

## Compilação e Execução

### Pré-requisitos
- Compilador GCC
- Biblioteca pthread (Linux/Unix)
- Para Windows: MinGW ou WSL

### Comandos
```bash
# Compilar
gcc -o robos robos.c -lpthread

# Executar
./robos
```

Este projeto demonstra:
- **Sincronização de threads** em ambiente multi-core
- **Gerenciamento de recursos limitados** com semáforos
- **Proteção de seções críticas** com mutexes
- **Controle de timing** em sistemas de tempo real
- **Detecção de deadlines perdidos** em sistemas críticos

## Possíveis Extensões

- Adicionar mais tipos de ferramentas especializadas
- Implementar prioridades diferentes para robôs
- Criar algoritmos de escalonamento mais sofisticados
- Adicionar métricas de performance em tempo real
- Implementar recuperação automática de falhas