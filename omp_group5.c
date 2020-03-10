#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

//FUNCTION TO ALOCATE
int **loc_matrix(int n)
{
    int **arr = malloc(n * sizeof(int *));

    #pragma omp parallel for
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
    int n = 0;

    if(fp == NULL)
    {
        printf("Unable to open file.\n");
        return -1;
    }

    rewind(fp);

    #pragma omp parallel
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
void makeMatrix(FILE *fp, int size, int **matriz)
{
    rewind(fp);
    #pragma omp parallel
    {
        #pragma omp critical
        for (int i = 0; i < size ; i++)
        {
            for (int j = 0; j < size; j++)
            {
                fscanf(fp, "%d", &matriz[i][j]);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    clock_t start, end; 
    double total_time;
    start = clock();

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

    //inializing elements as '0'.
    #pragma omp parallel
    {
        #pragma omp critical
        for(i = 0; i < sizeA; i++)
        {
            for(j = 0; j < sizeB; j++)
            {
                matriz_result[i][j] = 0;
            }
        }

        //multiply
        #pragma omp critical
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
    }

    FILE *fd3 = fopen("matrizFinal.txt", "w");

    //WRITING TO FILE
    #pragma omp parallel num_threads(1)
    {
        #pragma omp critical
        for ( i = 0; i < sizeA; i++)
        {
            for ( j = 0; j < sizeB; j++)
            {
                fprintf(fd3, "%d ", matriz_result[i][j]);
            }
            fprintf(fd3, "\n");
        }
    }

    fclose(fd3);
    end = clock ();
    total_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Tempo: %fs\n", total_time);

    return 0;
}
