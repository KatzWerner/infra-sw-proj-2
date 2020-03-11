#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

//FUNCTION TO ALOCATE
int **loc_matrix(int n)
{
    int **arr = malloc(n * sizeof(int *));

    for(int i=0; i<n; i++)
    {
        arr[i] = malloc(n *sizeof(int));
    }
    return arr;
}

//FUNCTION TO GET MATRIX SIZE.
int getsize(FILE *fp)
{
    char ch;
    int n = 1;
    if(fp == NULL)
    {
        printf("Unable to open file.\n");
        return -1;
    }

    rewind(fp);
    while( (ch = fgetc(fp)) != EOF)
    {
        if(ch == '\n')
        {
            n++;
        }
    }
    return n;
}

//FUNCTION TO MAKE MATRIX
int **makeMatrix(FILE *fp, int size, int **matriz)
{
    rewind(fp);
    for (int i = 0; i < size ; i++)
    {
        for (int j = 0; j < size; j++)
        {
            fscanf(fp, "%d", &matriz[i][j]);
        }
    }
    return matriz;
}

struct rusage r_usage;

int main(int argc, char *argv[])
{
    int memoryusage;
    memoryusage = getrusage(RUSAGE_SELF, &r_usage);

    clock_t start, end; 
    double total_time;
    

    int i, j, k;
    FILE *fd = fopen(argv[1], "r");

    int size = getsize(fd);
    rewind(fd);

    int **matrizA;
    int **matrizB;
    matrizA = loc_matrix(size);
    matrizB = loc_matrix(size);

    matrizA = makeMatrix(fd, size, matrizA);
    fclose(fd);

    fd = fopen(argv[2], "r");
    matrizB = makeMatrix(fd, size, matrizB);
    fclose(fd);

    //MULTIPLICATION
    int **matriz_result;
    matriz_result = loc_matrix(size);

    #pragma omp parallel for
    for(i = 0; i < size; i++)
    {
        #pragma omp parallel for
        for(j = 0; j < size; j++)
        {
            matriz_result[i][j] = 0;
            #pragma omp parallel for 
            for(k = 0; k < size; k++)
            {
                matriz_result[i][j] += matrizA[i][k] * matrizB[k][j];
            }
        }
    }

    fd = fopen("matrizFinal.txt", "w");

    //WRITING TO FILE
    for ( i = 0; i < size; i++)
    {
        for ( j = 0; j < size; j++)
        {
            fprintf(fd, "%d ", matriz_result[i][j]);
        }
    }

    fclose(fd);
    end = clock ();
    total_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Tempo: %fs\n", total_time);
    printf("Memória utilizada: %ld KB\n", r_usage.ru_maxrss);

    return 0;
}
