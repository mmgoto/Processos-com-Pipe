#### CEFET-MG - Engenharia de Computação

#### Sistemas Operacionais

---

# Processos com Pipe

O objetivo deste trabalho é entendermos melhor o funcionamento dos sistemas operacionais e como funciona a criação de processos que se comunicam através de pipes.

## Metodologia

1. **Criando o pipe**

Inicialmente criamos um vetor de inteiros _fd[2]_, com uma posição representando a direção da leitura e a outra de escrita.
Realizamos um teste de criação, que em caso de retorno de erro o programa é encerrado.

```c
    if (pipe(fd) == -1)                               // Criação de um pipe de inteiros.
    {
        fprintf(stderr, "Pipe failed");
        return 1;
    }
```

2. **Criando processos filhos**

Por meio de um loop, em que o número de iterações foi definido anteriormente pelo usuário, os valores parciais de _pi_ são calculados.
Em seguida, para cada iteração, é criado um processo filho.

```C
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
```

3. **Processo filho**

Caso o processo seja um filho, ele fecha a extremidade de leitura, podendo apenas escrever.
Em seguida, escreve o valor parcial de _pi_ no pipe e fecha tambem a outra extremidade.

```C
    if (new_process == 0)                             // Processo filho.
    {
        close(fd[READ_END]);                          // 0- Fecha a extremidade não utilizada do pipe.

        printf("e estou escrevendo no pipe");
        write(fd[WRITE_END], &value, sizeof(double)); // 1- Escreve o valor no pipe.
        printf(" o valor %f\n", value);

        close(fd[WRITE_END]);                         // 1- Fecha a extremidade de gravação do pipe.
        sleep(1);
```

4. **Processo pai**

Caso o processo seja pai, ele fecha a extremidade da escrita para que possa apenas ler os valores.
Após ler os valores, ele soma todos na variável “sum” e imprime o resultado.

```C
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
```

## Resultado

Para compilação devemos utilizar:

> gcc -o pipe pipe.c

Para iniciarmos devemos utilizar:

> ./pipe **x**

em que **x** deve ser um valor inteiro, representando a quantidade de processos desejados.
