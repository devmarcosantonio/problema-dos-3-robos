#include <stdio.h>     
#include <stdlib.h>   
#include <pthread.h>   
#include <unistd.h>   
#include <semaphore.h>  
#include <time.h>       

#define DEADLINE_MS 1300     // 1.5 segundos para completar tarefa
#define PERIOD_MS 5000         // Tarefa a cada 5 segundos
int tarefas_perdidas = 0;      // Contador de deadlines perdidos
int tarefas_executadas = 0;       // Contador global: quantas tarefas foram feitas

// Define constante: número de robôs (threads) na fábrica
#define NUM_ROBOS 3

// Variáveis globais compartilhadas entre todas as threads
sem_t ferramentas;                // Semáforo: controla acesso às 2 ferramentas
pthread_mutex_t mutex;            // Mutex: protege a variável tarefas_executadas
pthread_mutex_t mutex_perdidas;   // Mutex: protege tarefas_perdidas

// Função que cada thread (robô) vai executar
void* robo(void* arg) {
    int id = *(int*)arg;
    struct timespec inicio_periodo, inicio_tarefa, fim_tarefa;
    long duracao_ms;
    
    // Pega tempo inicial para controle de período
    clock_gettime(CLOCK_MONOTONIC, &inicio_periodo);
    
    while (1) {
        // CONTROLE DE PERÍODO
        struct timespec agora;
        clock_gettime(CLOCK_MONOTONIC, &agora);

        // calcula tempo decorrido desde o início do período
        long tempo_decorrido = (agora.tv_sec - inicio_periodo.tv_sec) * 1000 +
                              (agora.tv_nsec - inicio_periodo.tv_nsec) / 1000000;
        
        if (tempo_decorrido < PERIOD_MS) {
            usleep((PERIOD_MS - tempo_decorrido) * 1000); // Espera resto do período
        }
        
        // Atualiza início do próximo período
        clock_gettime(CLOCK_MONOTONIC, &inicio_periodo);
        
        // INÍCIO DA TAREFA
        clock_gettime(CLOCK_MONOTONIC, &inicio_tarefa);
        
        // 1. SOLICITAR FERRAMENTA
        printf("\n[ROBO %d] Solicitando ferramenta...\n", id);

        // Decrementa semáforo (pega 1 das 2 ferramentas)
        sem_wait(&ferramentas);
        
        printf("[ROBO %d] Ferramenta adquirida!\n", id);

        // 2. SEÇÃO CRÍTICA: atualizar contador compartilhado
        pthread_mutex_lock(&mutex);
        tarefas_executadas++;
        printf("[ROBO %d] TRABALHANDO | Tarefa #%d\n", id, tarefas_executadas);
        pthread_mutex_unlock(&mutex);

        // 3. SIMULAR TRABALHO
        usleep(600000); // 600ms de trabalho

        // 4. DEVOLVER FERRAMENTA
        printf("[ROBO %d] Devolvendo ferramenta\n", id);
        sem_post(&ferramentas);
        
        // FIM DA TAREFA - verifica deadline
        clock_gettime(CLOCK_MONOTONIC, &fim_tarefa);

        // Calcula duração da tarefa em ms
        duracao_ms = (fim_tarefa.tv_sec - inicio_tarefa.tv_sec) * 1000 +
                     (fim_tarefa.tv_nsec - inicio_tarefa.tv_nsec) / 1000000;
        
        if (duracao_ms > DEADLINE_MS) {
            pthread_mutex_lock(&mutex_perdidas);
            tarefas_perdidas++;
            printf("[ROBO %d] DEADLINE PERDIDO! | Tempo: %ldms/%dms | Falhas: %d\n",
                   id, duracao_ms, DEADLINE_MS, tarefas_perdidas);
            pthread_mutex_unlock(&mutex_perdidas);
        } else {
            printf("[ROBO %d] Deadline OK: %ldms/%dms\n", id, duracao_ms, DEADLINE_MS);
        }
        printf("[ROBO %d] Descansando ate proximo periodo...\n", id);
    }

    // Esta linha nunca será executada (loop infinito)
    return NULL;
}

// Função principal do programa
int main() {
    pthread_t threads[NUM_ROBOS];
    int ids[NUM_ROBOS];

    // Inicializa semáforo com valor 2 (2 ferramentas disponíveis)
    // Parâmetros: &semáforo, 0=compartilhado entre threads, valor_inicial=2
    sem_init(&ferramentas, 0, 2);
    
    // Inicializa mutexes com configurações padrão
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex_perdidas, NULL);
    
    printf("\n" 
           "===============================================\n"
           "        FABRICA DE ROBOS INICIADA             \n"
           "===============================================\n"
           "Robos ativos: %d\n"
           "Deadline maximo: %dms\n" 
           "Periodo de tarefas: %dms\n"
           "Ferramentas disponiveis: 2\n"
           "===============================================\n\n",
           NUM_ROBOS, DEADLINE_MS, PERIOD_MS);

    // CRIAÇÃO DAS THREADS (ROBÔS)
    for (int i = 0; i < NUM_ROBOS; i++) {
        ids[i] = i + 1;
        
        // Cria thread que executa função robo() com argumento &ids[i]
        pthread_create(&threads[i], NULL, robo, &ids[i]);
    }

    // AGUARDAR THREADS TERMINAREM (nunca acontece devido ao while(1))
    for (int i = 0; i < NUM_ROBOS; i++) {
        pthread_join(threads[i], NULL);
    }

    // LIMPEZA DOS RECURSOS (nunca executado devido ao loop infinito)
    sem_destroy(&ferramentas);
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex_perdidas);

    return 0;
}
