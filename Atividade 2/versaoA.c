// Fernanda Lyra Alves
// Ivan Dos Santos Muniz
// Programação Concorrente e Distribuída - 2020.2

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

static const int cel_morta = 0;
static const int cel_viva = 1;

static const unsigned int num_geracoes = 2000;
static const unsigned int srand_value = 1985;
static const unsigned int n_threads = 8;

int tabuleiro_n = 2048;

int coord_lim(int coord);
void copia_tabuleiro(int **origem, int **destino);
int vivos(int **tab);
int vizinhos(int **tab, int l, int c);

int decide_vida(int **tab, int **tab_novo, int l, int c);

int main()
{
    struct timeval inicio_exe;
    gettimeofday(&inicio_exe, NULL);
    
    // Declaração e inicialização da memória dos tabuleiros
    int **tabuleiro = NULL;
    int **tabuleiro_novo = NULL;
    
    tabuleiro = calloc(tabuleiro_n, sizeof(int *));
    for (int col = 0; col < tabuleiro_n; col++)
        tabuleiro[col] = calloc(tabuleiro_n, sizeof(int));
    
    tabuleiro_novo = calloc(tabuleiro_n, sizeof(int *));
    for (int col = 0; col < tabuleiro_n; col++)
        tabuleiro_novo[col] = calloc(tabuleiro_n, sizeof(int));
    
    // Geração do tabuleiro inicial e cópia para a nova geração
    srand(srand_value);
    for (int i = 0; i < tabuleiro_n; i++)
        for (int j = 0; j < tabuleiro_n; j++)
            tabuleiro[i][j] = rand() % 2;
    
    printf("Condição inicial: %d\n", vivos(tabuleiro));
    
    struct timeval inicio_ger;
    gettimeofday(&inicio_ger, NULL);
    
    // Execução das gerações
    for (unsigned int g = 0; g < num_geracoes; g++)
    {
        int l = 0;
        int c = 0;
#pragma omp parallel private(l, c) shared(tabuleiro, tabuleiro_novo) num_threads(n_threads)
        {
#pragma omp for
            for (l = 0; l < tabuleiro_n; l++)
                for (c = 0; c < tabuleiro_n; c++)
                    decide_vida(tabuleiro, tabuleiro_novo, l, c);
        }
        
        copia_tabuleiro(tabuleiro_novo, tabuleiro);
        printf("Geração %u: %d\n", g + 1, vivos(tabuleiro));
    }
    
    // Liberação da memória dos tabuleiros
    for (int col = 0; col < tabuleiro_n; col++)
        free(tabuleiro[col]);
    free(tabuleiro);
    
    for (int col = 0; col < tabuleiro_n; col++)
        free(tabuleiro_novo[col]);
    free(tabuleiro_novo);
    
    struct timeval fim;
    gettimeofday(&fim, NULL);
    
    printf("Tempo de execução total: %lf\nTempo de execução das gerações: %lf\n",
           (double)(fim.tv_usec - inicio_exe.tv_usec)/1000000 + (double)(fim.tv_sec - inicio_exe.tv_sec),
           (double)(fim.tv_usec - inicio_ger.tv_usec)/1000000 + (double)(fim.tv_sec - inicio_ger.tv_sec));
    
    return 0;
}

int coord_lim(int coord)
{
    int r;
    if (coord >= 0)
        r = coord % tabuleiro_n;
    else
        r =  tabuleiro_n + coord;
    return r;
}

void copia_tabuleiro(int **origem, int **destino)
{
    int l = 0;
#pragma omp parallel shared(origem, destino) private(l) num_threads(n_threads)
    {
#pragma omp for
        for (l = 0; l < tabuleiro_n; l++)
            memcpy(destino[l], origem[l], sizeof(int) * tabuleiro_n);
    }
}

int vivos(int **tab)
{
    int n_vivos = 0;
    int l = 0;
    int c = 0;
#pragma omp parallel shared(tab) private (l, c) num_threads(n_threads)
    {
#pragma omp for
        for (l = 0; l < tabuleiro_n; l++)
            for (c = 0; c < tabuleiro_n; c++)
#pragma omp critical
                n_vivos += tab[l][c];
    }
    
    return n_vivos;
}

int vizinhos(int **tab, int l, int c)
{
    int vizinhos_linhaacima = tab[coord_lim(l - 1)][coord_lim(c - 1)] + tab[coord_lim(l - 1)][coord_lim(c)] + tab[coord_lim(l - 1)][coord_lim(c + 1)];
    int vizinhos_linhaatual = tab[coord_lim(l)][coord_lim(c - 1)] + tab[coord_lim(l)][coord_lim(c + 1)];
    int vizinhos_linhaabaixo = tab[coord_lim(l + 1)][coord_lim(c - 1)] + tab[coord_lim(l + 1)][coord_lim(c)] + tab[coord_lim(l + 1)][coord_lim(c + 1)];
    return vizinhos_linhaabaixo + vizinhos_linhaatual + vizinhos_linhaacima;
}

int decide_vida(int **tab, int **tab_novo, int l, int c)
{
    int vizinhos_celula = vizinhos(tab, l, c);
    
    if (tab[l][c] == cel_viva && (vizinhos_celula < 2 || vizinhos_celula >= 4))
        tab_novo[l][c] = cel_morta;
    else if (tab[l][c] == cel_morta && vizinhos_celula == 3)
        tab_novo[l][c] = cel_viva;
    else
        tab_novo[l][c] = tab[l][c];
}
