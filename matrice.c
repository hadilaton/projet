//section include..
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
//define
#define MAX_SIZE 20
#define BUFFER_SIZE MAX_SIZE * MAX_SIZE

//variable globales 
//les matrices
int B[MAX_SIZE][MAX_SIZE], C[MAX_SIZE][MAX_SIZE], A[MAX_SIZE][MAX_SIZE];
int n1, m1, n2, m2, P;
//le tampon
int T[BUFFER_SIZE];
int buffer_index = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* produit_matrice(void* arg) {
    int i = *((int*)arg);
    int j, k, sum;

    for (j = 0; j < P; j++) {
        sum = 0;
        for (k = 0; k < m1; k++) {
            sum += B[i][k] * C[k][j];
        }

        pthread_mutex_lock(&mutex);
        T[buffer_index++] = sum;
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

void* consommer_resultats(void* arg) {
    int thread_id = *((int*)arg);
    int start = thread_id * P / n1;
    int end = (thread_id + 1) * P / n1;

    for (int i = 0; i < n1; i++) {
        for (int j = start; j < end; j++) {
            pthread_mutex_lock(&mutex);
            A[i][j] = T[i * P + j];
            pthread_mutex_unlock(&mutex);
        }
    }

    pthread_exit(NULL);
}

void remplir_matrice_aleatoire(int matrice[][MAX_SIZE], int rows, int cols) {
    int i, j;

    srand(time(NULL));

    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            matrice[i][j] = rand() % 10;
        }
    }
}

void afficher_matrice(int matrice[][MAX_SIZE], int rows, int cols) {
    int i, j;

    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            printf("%d ", matrice[i][j]);
        }
        printf("\n");
    }
}

int Main ()
{
 int i, j;
    pthread_t threads[MAX_SIZE], consumer_threads[MAX_SIZE];

    printf("Enter the number of rows n1 for matrix B: ");
    scanf("%d", &n1);
    printf("Enter the number of columns m1 for matrix B: ");
    scanf("%d", &m1);

    printf("Enter the number of rows n2 for matrix C: ");
    scanf("%d", &n2);
    printf("Enter the number of columns m2 for matrix C: ");
    scanf("%d", &m2);

    if (m1 != n2) {
        printf("Error: The number of columns in matrix B must be equal to the number of rows in matrix C.\n");
        return 1;
    }

    P = m2; // Number of columns in matrix C is the number of columns in the result matrix A

    remplir_matrice_aleatoire(B, n1, m1);
    remplir_matrice_aleatoire(C, n2, m2);

    printf("Matrix B:\n");
    afficher_matrice(B, n1, m1);

    printf("\nMatrix C:\n");
    afficher_matrice(C, n2, m2);

    for (i = 0; i < n1; i++) {
        int* thread_args = (int*)malloc(sizeof(int));
        *thread_args = i;
        pthread_create(&threads[i], NULL, produit_matrice, (void*)thread_args);
    }

    // Wait for all producer threads to finish
    for (i = 0; i < n1; i++) {
        pthread_join(threads[i], NULL);
    }

    for (i = 0; i < n1; i++) {
        int* thread_args = (int*)malloc(sizeof(int));
        *thread_args = i;
        pthread_create(&consumer_threads[i], NULL, consommer_resultats, (void*)thread_args);
    }

    // Wait for all consumer threads to finish
    for (i = 0; i < n1; i++) {
        pthread_join(consumer_threads[i], NULL);
    }

    printf("\nResult of the matrix product B and C -Matrix A- :\n");
    afficher_matrice(A, n1, P);

    return 0;
}
