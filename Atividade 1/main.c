#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define nullptr NULL
#define protecao_critica 0

static const int num_client_threads = 4;
static pthread_t request = 0;
static pthread_t respond = 0;
static int soma = 0;

void *client_thread(void *args)
{
    pthread_t my_id = pthread_self();
    for (uint64_t i = 0; i < 1000000000ul; i++)
    {
#if protecao_critica
        while (respond != my_id)
            request = my_id;
#endif
        printf("%lu entrou na seção critica\n", my_id);
        int local = soma;
        sleep(rand()%2);
        soma = local + 1;
#if protecao_critica
        respond = 0;
#endif
        printf("%lu saiu da seção critica\n", my_id);
    }
    return nullptr;
}

void *server_thread(void *args)
{
#if protecao_critica
    while (1)
    {
        while (request == 0);
        respond = request;
        while (respond != 0);
        request = 0;
    }
#endif
    return nullptr;
}

int main()
{
    // Inicializa o gerador de números aleatórios
    srand(123);
    
    // Cria thread de servidor
    pthread_t serverId;
    pthread_create(&serverId, nullptr, server_thread, nullptr);
    
    // Cria threads de cliente
    pthread_t clientIds[num_client_threads];
    
    for (int i = 0; i < num_client_threads; i++)
        pthread_create(&clientIds[i], nullptr, client_thread, nullptr);
    
    // Espera pelas threads de cliente
    for (int i = 0; i < num_client_threads; i++)
        pthread_join(clientIds[i], nullptr);
    
    printf("soma = %d", soma);
    
    return 0;
}
