#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
    int n = 0;

    if(fp == NULL)
    {
        printf("Unable to open file.\n");
        return -1;
    }

    rewind(fp);
    for(char ch = fgetc(fp); ch != EOF; ch = fgetc(fp))
    {
        if(ch == '\n')
        {
            n++;
        }
    }
    return n;
}

//FUNCTION TO MAKE MATRIX
void makeMatrix(FILE *fp, int size, int **matriz)
{
    rewind(fp);
    for (int i = 0; i < size ; i++)
    {
        for (int j = 0; j < size; j++)
        {
            fscanf(fp, "%d", &matriz[i][j]);
        }
    }
}

struct rusage r_usage;

int main(int argc, char *argv[])
{
    int memoryusage;
    memoryusage = getrusage(RUSAGE_SELF, &r_usage);

    clock_t start, end; 
    double total_time;

    int i, j, k;
    FILE *fd1 = fopen(argv[1], "r");
    FILE *fd2 = fopen(argv[2], "r");

    int sizeA = getsize(fd1);
    int sizeB = getsize(fd2);

    int **matrizA;
    int **matrizB;
    matrizA = loc_matrix(sizeA);
    matrizB = loc_matrix(sizeB);

    makeMatrix(fd1, sizeA, matrizA);
    makeMatrix(fd2, sizeB, matrizB);

    fclose(fd1);
    fclose(fd2);

    //MULTIPLICATION
    int matriz_result[sizeA][sizeB];

    start = clock();
    //inializing elements as '0'.
    for(i = 0; i < sizeA; i++)
    {
        for(j = 0; j < sizeB; j++)
        {
            matriz_result[i][j] = 0;
        }
    }

    //multiply
    for(i = 0; i < sizeA; i++)
    {
        for(j = 0; j < sizeB; j++)
        {
            for(k = 0; k < sizeB; k++)
            {
                matriz_result[i][j] += matrizA[i][k] * matrizB[k][j];
            }
        }
    }
    end = clock ();

    FILE *fd3 = fopen("matrizFinal.txt", "w");
    //WRITING TO FILE
    for ( i = 0; i < sizeA; i++)
    {
        for ( j = 0; j < sizeB; j++)
        {
            fprintf(fd3, "%d ", matriz_result[i][j]);
        }
        fprintf(fd3, "\n");
    }
    fclose(fd3);

    total_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Tempo: %fs\n", total_time);
    printf("Memória utilizada: %ld KB\n", r_usage.ru_maxrss);

    return 0;
}
