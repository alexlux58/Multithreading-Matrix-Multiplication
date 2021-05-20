#include "matrixMult.h"

/*
 * name: Alex Lux
 * lab: Lab05 Matrix Mult.
 * date: 3/2/2021
 */


int main(int argc, char *argv[])
{

  // printf("%s\n", argv[1]);

   if (freopen(argv[1], "r", stdin) == 0)
      oops("Cannot open the input file.\n", -1);

   int **a1, **b1, **c1, **a2, **b2, **c2; // matrices
   int m1, k1, n1, m2, k2, n2; // dimensions of the matices m x k and k x n

   allocateAndLoadMatrices(&a1, &b1, &c1, &m1, &k1, &n1);
   allocateAndLoadMatrices(&a2, &b2, &c2, &m2, &k2, &n2);


   // the real magic happens in here
   pthread_t **tids1 = multiply(a1, b1, c1, m1, k1, n1);
   pthread_t **tids2 = multiply(a2, b2, c2, m2, k2, n2);

   free_tids(tids1, m1);
   free_tids(tids2, m2);

   // dispaly results of matrix multiplication
   printf("\nMATRIX A1\n");
   displayMatrix(a1, m1, k1);
   freeMatrix(a1, m1);
   printf("\nMATRIX B1\n");
   displayMatrix(b1, k1, n1);
   freeMatrix(b1, k1);
   printf("\nMATRIX A1 x B1\n");
   displayMatrix(c1, m1, n1);
   freeMatrix(c1, m1);

   printf("\nMATRIX A2\n");
   displayMatrix(a2, m2, k2);
   freeMatrix(a2, m2);
   printf("\nMATRIX B2\n");
   displayMatrix(b2, k2, n2);
   freeMatrix(b2, k2);
   printf("\nMATRIX A2 x B2\n");
   displayMatrix(c2, m2, n2);
   freeMatrix(c2, m2);

   return 0;
}

void allocateAndLoadMatrices(int ***a, int ***b, int ***c, int *m, int *k, int *n)
// takes pointers to two-dimensional matrices, so they can be allocated in here
// and used by the caller
// should call the function loadMatrix(int ***matrix, int m, int n)
{
   if (scanf("%d %d %d", m, k, n) == 0)
      oops("Cannot read matrix sizes.\n", -2);

    int i;

    *a = malloc(*m * sizeof(int *));
    for(i = 0; i < *m; i++)
    {
      (*a)[i] = malloc(*k * sizeof(int));
    }

    *b = malloc(*k * sizeof(int *));
    for(i = 0; i < *k; i++)
    {
      (*b)[i] = malloc(*n * sizeof(int));
    }

    *c = malloc(*m * sizeof(int *));
    for(i = 0; i < *m; i++)
    {
      (*c)[i] = malloc(*n * sizeof(int));
    }

    loadMatrix(a, *m, *k);

    loadMatrix(b, *k, *n);


}

void loadMatrix(int ***matrix, int m, int n)
{
    int num;
    for (int i = 0; i < m; i++)
    {
      for (int j = 0; j < n; j++)
      {
        scanf("%d", &num);
        (*matrix)[i][j] = num;
      }
    }
}

void freeMatrix(int **matrix, int m)
{
   for(int i = 0; i < m; i++)
   {
       free(matrix[i]);
   }
   free(matrix);
}

pthread_t **multiply(int **a, int **b, int **c, int m, int k, int n)
{
   pthread_t **tids = alloc_tids(m, n);

   MATRIX_CELL param;

   param.a = a;
   param.b = b;
   param.c = c;
   param.i = m;
   param.k = k;
   param.j = n;

    for(int i = 0; i < m; i++)
    {
        for(int j = 0; j < n; j++)
        {
            pthread_create(&tids[i][j], NULL, matrixThread, (void *)&param);
        }
    }

   join(tids, m ,n);

   return tids;
}

void *matrixThread(void *param)
{
    // map the parameter onto the structure
    MATRIX_CELL *cell = (MATRIX_CELL *)param;

    for(int i = 0; i < cell->i; i++)
    {
        for(int j = 0; j < cell->j; j++)
        {
            cell->c[i][j] = 0;
            for(int k = 0; k < cell->k; k++)
            {
                cell->c[i][j] += cell->a[i][k] * cell->b[k][j];
            }
        }
    }

    // bellow causes double free/corruption(out)
    // so I commented it out
    // free(cell);

    return NULL;
}

pthread_t **alloc_tids(int m, int n)
{
   pthread_t **tids;

   tids = malloc(m * sizeof(pthread_t*));
   for(int i = 0; i < m; i++)
   {
       tids[i] = malloc(n * sizeof(pthread_t));
   }

   return tids;
}

void free_tids(pthread_t **tids, int m)
{
   for (int i = 0; i < m; i++)
   {
       free(tids[i]);
   }
   free(tids);
}

void join(pthread_t **tids, int m, int n)
{
   for(int i = 0; i < m; i++)
   {
       for(int j = 0; j < n; j++)
       {
           pthread_join(tids[i][j], NULL);
       }
   }
}

void displayMatrix(int **matrix, int m, int n)
{
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d\t", matrix[i][j]);
        }
        printf("\n");
    }

    printf("\n");
}
