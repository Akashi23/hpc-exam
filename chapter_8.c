/* File:     DZ_read.c
 *
 * Purpose:  Read Matrix from file
 *
 * Compile:  gcc -g -Wall -fopenmp -o a.out DZ_read.c
 * Run:      ./a.out <filename>
 *
 */

#define MM_MAX_LINE_LENGTH 102500
#define MatrixMarketBanner "%%MatrixMarket"
#define MM_MAX_TOKEN_LENGTH 64

/********************* Matrix Market error codes ***************************/

#define MM_COULD_NOT_READ_FILE 11
#define MM_PREMATURE_EOF 12
#define MM_NOT_MTX 13
#define MM_NO_HEADER 14
#define MM_UNSUPPORTED_TYPE 15
#define MM_LINE_TOO_LONG 16
#define MM_COULD_NOT_WRITE_FILE 17

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <omp.h>

int thread_count;

struct Matrix
{
   int rows;
   int columns;
   int *rowsWithoutZeros;
   int *colsWithoutZeros;
   double **matrix;
};

struct Matrix read_matrix_file(char *filename);
double *generate_vector(int nums);
double *init_result_vector(int nums);
double *multiply_vector_to_matrix_csr(double *vector, struct Matrix matrix, double *result);
void print_vector_to_stdout(double *vector, int nums);
void print_matrix_to_stdout(double **matrix, int rows, int columns);

int main(int argc, char *argv[])
{
   thread_count = strtol(argv[2], NULL, 10);

   struct Matrix matrixDetails = read_matrix_file(argv[1]);

   // print_matrix_to_stdout(matrixDetails.matrix, matrixDetails.rows, matrixDetails.columns);

   double *vec = generate_vector(matrixDetails.rows);
   double *result_csr = init_result_vector(matrixDetails.rows);

   printf("\nname matrix = %s\n", argv[1]);
   printf("thread count = %s\n", argv[2]);
   printf("matrix rows = %d\n", matrixDetails.rows);
   printf("matrix columns = %d\n", matrixDetails.columns);

   // print_vector_to_stdout(vec, matrixDetails.rows);

   double start = omp_get_wtime();
   multiply_vector_to_matrix_csr(vec, matrixDetails, result_csr);
   double finish = omp_get_wtime();

   printf("Elapsed time = %e seconds\n\n", finish - start);

   // print_vector_to_stdout(vectorMultiplied, matrixDetails.rows);

   return (0);
}

double *generate_vector(int nums)
{
   srand(0);

   double *vector = (double *)malloc(nums * sizeof(double));

   for (int i = 0; i < nums; i++)
      vector[i] = rand() % 100 + 1;

   return vector;
}

double *init_result_vector(int nums)
{
   double *vector = (double *)malloc(nums * sizeof(double));

   for (int i = 0; i < nums; i++)
      vector[i] = 0.0;

   return vector;
}

double *multiply_vector_to_matrix_csr(double *vector, struct Matrix matrix, double *result)
{
   int i, j;
   size_t nRowsWithoutZeros = sizeof(matrix.rowsWithoutZeros) / sizeof(matrix.rowsWithoutZeros[0]);
#pragma omp parallel num_threads(thread_count) default(none) \
    shared(matrix, vector, result, nRowsWithoutZeros) private(i, j)
   for (i = 0; i < nRowsWithoutZeros; i++)
   {
#pragma omp for schedule(static, 1)
      for (j = 0; j < matrix.columns; j++)
      {
         result[matrix.rowsWithoutZeros[i]] += vector[matrix.rowsWithoutZeros[i]] * matrix.matrix[matrix.rowsWithoutZeros[i]][j];
      }
   }

   return result;
}

struct Matrix read_matrix_file(char *filename)
{
   FILE *fp;
   char buff[100];

   fp = fopen(filename, "r");

   int bannerFlag = 1;

   int header[3];

   while (bannerFlag)
   {
      fgets(buff, 255, (FILE *)fp);
      if (buff[0] != '%')
      {
         bannerFlag = 0;

         char *token =
             strtok(buff, " ");
         header[0] = atoi(token) + 1;

         token = strtok(NULL, " ");
         header[1] = atoi(token) + 1;

         token = strtok(NULL, " ");
         header[2] = atoi(token) + 1;
      }
   }

   int rows[header[2]];
   int cols[header[2]];

   double **matrix = (double **)malloc(header[0] * sizeof(double *));

   for (int i = 0; i < header[0]; i++)
      matrix[i] = (double *)malloc(header[1] * sizeof(double));

   for (int i = 0; i < header[0]; i++)
   {
      for (int j = 0; j < header[1]; ++j)
      {
         matrix[i][j] = 0;
      }
   }

   for (int i = 0; i < header[2]; i++)
   {
      fscanf(fp, "%d %d ", &rows[i], &cols[i]);
      fscanf(fp, "%lg\n", &matrix[rows[i]][cols[i]]);
   }

   fclose(fp);

   struct Matrix matrixDetails;

   matrixDetails.matrix = matrix;
   matrixDetails.rows = header[0];
   matrixDetails.columns = header[1];
   matrixDetails.rowsWithoutZeros = rows;
   matrixDetails.colsWithoutZeros = cols;

   return matrixDetails;
}

void print_vector_to_stdout(double *vector, int nums)
{
   fprintf(stdout, "\nVectors\n");
   for (int i = 0; i < nums; i++)
   {
      if (i > 30)
         break;
      fprintf(stdout, "%f\t", vector[i]);
   }
}

void print_matrix_to_stdout(double **matrix, int rows, int columns)
{
   fprintf(stdout, "%d\t %d\n", rows, columns);

   for (int i = 0; i < rows; ++i)
   {
      for (int j = 0; j < columns; ++j)
      {
         fprintf(stdout, "%f\t", matrix[i][j]);
      }
      fprintf(stdout, "\n");
   }
}