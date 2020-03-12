#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

//defenindo variaveis globais
int size, half_size, cont = 0;
char *file_A;
char *file_B;
int **matrizAux;
int **matriz_result;


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
void makeMatrix(FILE *fp, int **matriz)
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

//FUNCTION TO CREATE THE MATRIX 1
void *criarMatriz1(void *matrizX){
    int **matriz = (int**) matrizX;//salvando valor da variavel
    char *file1 = file_A;//pegando a variavel global
    FILE *fd = fopen(file1, "r");
    size = getsize(fd);
    rewind(fd);
    matriz = loc_matrix(size);
    rewind(fd);
    makeMatrix(fd, matriz);
    rewind(fd);
    fclose(fd);
    return matriz;
}

//FUNCTION TO CREATE THE MATRIX 2
void *criarMatriz2(void *matrizX){
    int **matriz = (int**) matrizX;
    char *file2 = file_B;
    FILE *fd = fopen(file_B, "r");
    size = getsize(fd);
    rewind(fd);
    matriz = loc_matrix(size);
    rewind(fd);
    makeMatrix(fd, matriz);
    rewind(fd);
    fclose(fd);
    return matriz;
}


void *multiplicarMatriz(void *matrizX){
	printf("entrou na multiplicação de matrizes\n");

	printf("cont = %d\n", cont);
	if(cont == 0)
	{//contador = 0 quer dizer q estamos operando com a primeira metade da matriz
		printf("entrada 1\n");
		int **matrizInteira = matrizAux;//salvando variavel global		
		int **mtz = (int **) matrizX;//pegando o inut da função
		int matriz_partida[size/2][size/2];//criando a matriz que vamos retornar
		for(int i = 0; i < (size/2); i++)
	    {
	        for(int j = 0; j < (size/2); j++)
	        {
	            for(int k = 0; k < (size/2); k++)
	            {
	            	printf("estrou no i = %d j = %d k = %d\n", i, j, k);
	            	printf("mtz = %d\n", mtz[i][k]);
	            	printf("matriz = %d\n", matrizInteira[i][k]);
	            	printf("%d\n", size/2);
	                matriz_partida[i][j] += mtz[i][k] * matrizInteira[k][j];//so roda uma vez msmo mamao
	            }
	        }
	    }
	    return matriz_partida;
	}
	else
	{//contador = 1 quer dizer q estamos operando com a segunda metade da matriz
		printf("\n ENTRADA 2");
		int **matrizInteira2 = matrizAux;		
		int **mtz = (int **) matrizX;
		int matriz_partida[half_size][half_size];
		for(int i = half_size; i < size; i++)
	    {
	        for(int j = half_size; j < size; j++)
	        {
	            for(int k = half_size; k < size; k++)
	            {	            	
	            	printf("ENTROU no i = %d j = %d k = %d\n", i, j, k);
	            	printf("MTZ = %d\n", mtz[i][k]);
	            	printf("MATRIZ = %d\n", matrizInteira2[k][j]);
	            	printf("%d\n", half_size);
	                matriz_partida[i][j] += mtz[i][k] * matrizInteira2[k][j];
	            }
	        }
	    }
	    return matriz_partida;
	}
}

int main(int argc, char *argv[])
{
    clock_t start, end;
    double total_time;
    start = clock();

    int i, j;

    pthread_t matriz1;
    pthread_t matriz2;

    int **ma, **mA, **matrizA;
    int **mB, **matrizB;

    file_A = argv[1];
    file_B = argv[2];

    pthread_create(&matriz1, NULL, criarMatriz1, (void *)mA);//pegando os dois arquivos ao mesmo tempo
    pthread_create(&matriz2, NULL, criarMatriz2, (void *)mB);

    pthread_join(matriz1, &ma);
    pthread_join(matriz2, &matrizB);

    matrizAux = matrizB;
    int matriz_result[size][size];

    //inializing elements as '0'///////////////////////////////////////////////////////
    for(i = 0; i < size; i++)
    {
        for(j = 0; j < size; j++)
        {
            matriz_result[i][j] = 0;
        }
    }

    //multiply//////////////////////////////////////////////////////////////////////////
    pthread_t thread1;
    pthread_t thread2;
    //vendo se o tamanho da matriz eh impar ou par
    printf("size = %d\n", size);
    if ((size % 2) == 0)
    {
    	half_size = (size/2) - 1;
    }
    else 
    {
    	half_size = ((size)/2);
    }
    
    int mh[half_size][half_size];
    //criando a segunda metade da matriz
	for (int i = half_size; i < size; i++)
	{
		int ia = 0;
		for (int j = half_size; j < size; j++)
		{
			int ja = 0;
			mh[ia][ja]  = ma[i][j];
			printf("%d ", mh[ia][ja]);
			ja++;
		}
		printf("\n");
		ia++;
	}


    for ( i = 0; i < size; i++)
    {
        for ( j = 0; j < size; j++)
        {
            printf("%d ", ma[i][j]);
        }
        printf("\n");
    }

    printf("dividiu a matriz\n");
    pthread_create(&thread1, NULL, multiplicarMatriz, (void *)ma);
    pthread_join(thread1, &mh);
    cont = cont + 1;
    printf("aumentando contador\n");
    
    pthread_create(&thread2, NULL, multiplicarMatriz, (void *)mh);

    pthread_join(thread1, &ma);

    //printar no file/////////////////////////////////////////////////////////////////
    FILE *fd3 = fopen("matrizFinal.txt", "w");

    //WRITING TO FILE
    for ( i = 0; i < size; i++)
    {
        for ( j = 0; j < size; j++)
        {
            fprintf(fd3, "%d ", matriz_result[i][j]);
        }
        fprintf(fd3, "\n");
    }
    fclose(fd3);

    end = clock ();
    total_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Tempo: %fs\n", total_time);

    return 0;
}