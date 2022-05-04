/* Para executar o programa gcc -o pipe pipe.c  */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

#define READ_END 0                                    // Parâmetros de controle.
#define WRITE_END 1                                  // Parâmetros de controle.

// Variáveis globais.
double *resultados;
unsigned n_process;

// Criação da função PI.
double pi(int id)
{
    long ID = (long)id;
    double sum = 0.0, x;
    unsigned long long int i;
    unsigned long long int passos = 1000000000;       // 1 Bilhão.
    double h = 1.0 / passos;

    for (i = ID + 1; i <= passos; i += n_process)
    {
        x = h * ((double)i - 0.5);
        sum += 4.0 / (1.0 + x * x);
    }
    resultados[ID] = h * sum;
    return resultados[ID];
}

int main(int argc, char *argv[], char *envp[])
{
    double value, sum;
    int fd[2];                                        // Definição de um pipe de inteiros.
    int new_process;

    if (pipe(fd) == -1)                               // Criação de um pipe de inteiros.
    {
        fprintf(stderr, "Pipe failed");
        return 1;
    }

    if (argc == 2)
    {
        n_process = atoi(argv[1]);
    }
    else
    {
        printf("Informe a quantidade desejada de processos. Ex: './pipe 2'\n");
        exit(1);
    }

    resultados = malloc(n_process * sizeof(double));

    printf("Ola, sou o processo pai %5d\n", getpid());
    printf("\n"); 

    for (int i = 0; i < n_process; i++)
    {
        value = pi(i);
        new_process = fork();                         // Cria um novo processo.

        if (new_process == 0)
        {
            printf("Sou o %5d, filho de %5d ", getpid(), getppid());
            break;
        }
    }

    sleep(1);

    if (new_process == 0)                             // Processo filho.
    {
        close(fd[READ_END]);                          // 0- Fecha a extremidade não utilizada do pipe.

        printf("e estou escrevendo no pipe");  
        write(fd[WRITE_END], &value, sizeof(double)); // 1- Escreve o valor no pipe.
        printf(" o valor %f\n", value);

        close(fd[WRITE_END]);                         // 1- Fecha a extremidade de gravação do pipe.
        sleep(1);
        exit(1);
    }
    else                                              // Processo pai.
    {
        sleep(1); 
        close(fd[WRITE_END]);                         // 1- Fecha a extremidade não utilizada do pipe.

        for (int i = 0; i < n_process; i++)          
        {
            printf("O processo %d terminou e ", wait(0));
            read(fd[WRITE_END], &value, sizeof(double));// Le do pipe o valor.
            printf("o pai leu do pipe o valor: %f\n", value);
            sum += value;
        }
        close(fd[READ_END]);                          // 0- Fecha a extremidade de gravação do pipe.                 
    } 
    sleep(1);
    printf("\n");
    printf("Valor de PI é: %f\n", sum);
    return 0;
}
