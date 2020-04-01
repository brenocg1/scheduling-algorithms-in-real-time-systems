/**
******************************************************************************
*-> file: rate-monotonic.c
*-> author: Breno Campos - brenocg@alu.ufc.br
*-> version: 1.0
*-> date: 30/March/2020
*-> Description: implementacao do algoritmo rate monotonic 	
******************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define BUFFER_SIZE 1000
#define STRING_MAX_NAME 40
//todos os arquivos mandados pelo professor eram 
// apenas 3 processos...
// mude se seu arquivo tiver mais de 3 processos
#define TASKS_NUMBER 3 


// ** IMPORTANTE **
// FORMATO PADRAO DO ARQUIVO DE LEITURA DAS TAREFAS
// P C D
// 4 1 4
// 5 2 5
// 20 4 20

// ps: funciona tanto em formato LF e CRLF, no linux. 

typedef struct Task{
    int id;
    int period;
    int timeExecution;
    int deadline;
}Task;

// funcao para o quick sort saber comparar cada objeto da estrutura
int cmpTasks(const void * a, const void * b){
    return (int)(((Task *)a)->period - ((Task *)b)->period);
}

// funcao que devolve mmc de N numeros
int get_mmc(int * t, int tSize){
    int initialArray[100];
    
    for(int i = 0; i < tSize; i++){
        initialArray[i] = t[i];
    }

    int index, m, x, b = 1;

    while(b == 1){
        b = 0;
        x = t[0];
        m = t[0];
        index = 0;
        for(int i = 0; i < tSize; i++){
            if(x != t[i]) b = 1;
            if(m > t[i]){
                m = t[i];
                index = i;
            }
        }

        if(b == 1) 
            t[index] = t[index] + initialArray[index];   
    }
    return t[0];
}

int main(void){
    // pegando arquivo de teste
    char str[STRING_MAX_NAME];
    puts("Digite o nome do arquivo");
    fgets(str, STRING_MAX_NAME, stdin);
    str[strcspn(str, "\n")] = 0;
    FILE * file = fopen(str, "rw");

    if (file == NULL)
    {
        puts("nao foi possivel abrir o arquivo");
        return 0;
    }
    // Tirando a primeira linha "P C D" 
    char buff[BUFFER_SIZE];
    fgets(buff, BUFFER_SIZE, file); 
    
    // colocando os valores numa matrix
    // ps: vc pode melhorar esse trecho escrevendo direto no vetor tarefas
    int mat[TASKS_NUMBER][3];
    int x = -1;
    for (int i = 0; i < TASKS_NUMBER; i++){
        for (int j = 0; j < 3; j++){
            fscanf(file, "%d", &x);
            mat[i][j] = x;
        }
    }

    // Calculando Somario do limiar de utilizacao
    // Teste de escalabilidade
    float res = 0;
    for(int i = 0; i < TASKS_NUMBER; i++){
        res += (float)mat[i][1] / (float)mat[i][0];
    }
    

    // TASKS_NUMBER * (pow(2, (float)1 / (float)TASKS_NUMBER) - 1)
    // para 3 TASKS sera 0,779
    // nao irei arrendodar, como livro faz, usando 0,78.
    printf("\n##### TESTE DE ESCALABILIDADE #####\n");
    if(res <= TASKS_NUMBER * (pow(2, (float)1 / (float)TASKS_NUMBER) - 1)){
        puts("O sistema atendera todos os deadlines");
    }else{
        puts("Nao podemos garantir que todos os deadlines serão sempre cumpridos");
    }
    puts("###################################");

    // fazendo mmc, para achar o ciclo
    int periodos[TASKS_NUMBER];
    for(int i = 0; i < TASKS_NUMBER; i++)
        periodos[i] = mat[i][0];
    int mmc = get_mmc(periodos, TASKS_NUMBER);    
    
    // armazenando as tarefas num vetor de Tarefas
    Task tarefas[TASKS_NUMBER];
    for(int i = 0; i < TASKS_NUMBER; i++){
        tarefas[i].id = i;
        tarefas[i].period = mat[i][0];
        tarefas[i].timeExecution = mat[i][1];
        tarefas[i].deadline = mat[i][2];
    }

    // Ordenando as tarefas
    // usando quick sort da stdlib
    qsort (tarefas, TASKS_NUMBER, sizeof(Task), cmpTasks);


    // for(int i = 0; i < TASKS_NUMBER; i++){
    //     printf("%d %d %d \n", tarefas[i].period, tarefas[i].timeExecution, tarefas[i].deadline);
    // }
    

    // escalonamento onde vou armezar as tarefas
    int faixa[mmc];

    // tirando lixo do vetor
    // e preenchendo com -1
    for(int i = 0; i < mmc; i++){
        faixa[i] = -1;
    }
    
    // escalonamento em si 
    // * estou considerando que os periodos estao ordenados na matrix
    // tratarei isso dps
    for(int i = 0; i < TASKS_NUMBER; i++){
        int taskPeriod = tarefas[i].period;
        int taskTimeExecution = tarefas[i].timeExecution;
        for(int j = 0; j < mmc; j++){
            if(faixa[j] == -1){
                faixa[j] = tarefas[i].id + 1; //atribuindo tarefa
                taskTimeExecution--;
            }
            if(taskTimeExecution == 0){
                j = taskPeriod - 1;
                taskPeriod = taskPeriod  + tarefas[i].period;
                taskTimeExecution = tarefas[i].timeExecution;
            }
        }
    }

    for(int i = 0; i < mmc; i++){
        if(faixa[i] == -1){
            printf("TEMPO %d - %d: Nenhuma Tarefa\n", i, i + 1);
        }else{
            printf("TEMPO %d - %d: Tarefa: %d\n", i, i + 1, faixa[i]);
        }
    }
    

    fclose(file);
    return 0;
}

