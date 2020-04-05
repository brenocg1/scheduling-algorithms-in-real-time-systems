/**
******************************************************************************
*-> file: deadline-monotonic.c
*-> author: Breno Campos - brenocg@alu.ufc.br
*-> version: 1.0
*-> date: 01/April/2020
*-> Description: none 	
******************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define BUFFER_SIZE 1000
#define STRING_MAX_NAME 40

// todos os arquivos mandados pelo professor eram 
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
    int timeExecutionTemp;
    int deadline;
    int apto;
}Task;

int get_next_abs_deadline(int * time, int i, int mmc){
    for(; i < mmc + 1; i++) 
        if(time[i] != 0) 
            return time[i];
}


Task * get_task_by_id(Task * tasks, size_t size, int id){
    for(int i = 0; i < size; i++){
        if(tasks[i].id == id) return &tasks[i];
    }

    return NULL;
}


int get_next_deadline(Task * tasks, size_t size){
    int nextDeadlineId = -1;
    int temp = 10000000;
    for(int i = 0; i < size; i++){
        if(tasks[i].apto == 1 && tasks[i].deadline < temp){
            nextDeadlineId = tasks[i].id;
            temp = tasks[i].deadline;
        }
    }
    return nextDeadlineId;
}


// funcao para o quick sort saber comparar cada objeto da estrutura
// de acordo com o deadline
int cmpTasks(const void * a, const void * b){
    return (int)(((Task *)a)->deadline - ((Task *)b)->deadline);
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
    
    printf("\n##### TESTE DE ESCALABILIDADE #####\n");
    if(res <= 1){
        puts("O sistema talvez podera atender a todos os deadlines");
    }else{
        puts("O sistema nao atendera os deadlines");
    }
    printf("###################################\n\n");

    // fazendo mmc, para achar o ciclo
    int periodos[TASKS_NUMBER];
    for(int i = 0; i < TASKS_NUMBER; i++)
        periodos[i] = mat[i][0];
    int mmc = get_mmc(periodos, TASKS_NUMBER);    
    
    // armazenando as tarefas num vetor de Tarefas
    Task tarefas[TASKS_NUMBER];
    for(int i = 0; i < TASKS_NUMBER; i++){
        tarefas[i].id = i + 1;
        tarefas[i].period = mat[i][0];
        tarefas[i].timeExecution = mat[i][1];
        tarefas[i].deadline = mat[i][2];
        tarefas[i].apto = 1;
        tarefas[i].timeExecutionTemp = tarefas[i].timeExecution; 
    }

    // Ordenando as tarefas
    // usando quick sort da stdlib
    qsort (tarefas, TASKS_NUMBER, sizeof(Task), cmpTasks);
    
    // escalonamento onde vou armezar as tarefas
    int faixa[mmc];

    // tirando lixo do vetor
    // e preenchendo com 0
    for(int i = 0; i < mmc; i++){
        faixa[i] = 0;
    }
    

    // marcando os deadlines na faixa de tempo
    int deadlines[mmc + 1];
    for(int i = 0; i < mmc; i++)
        deadlines[i] = 0;
        
    for(int i = 0; i < TASKS_NUMBER; i++){
        for(int j = 0; j < mmc;){
            j += tarefas[i].deadline;
            deadlines[j] = (-1) * tarefas[i].id;
            j += (tarefas[i].period - tarefas[i].deadline);
        }
    }
    
    // escalonamento em si 
    for(int i = 0; i < mmc; i++){        
        for(int j = 0; j < TASKS_NUMBER; j++){
            if(tarefas[j].apto == 0 && (i % tarefas[j].period == 0)){
                tarefas[j].apto = 1;
                tarefas[j].timeExecutionTemp = tarefas[j].timeExecution; 
            }
        }
        Task * t = NULL;
        int k = i - 1;
        int flag = 0;
        do{
            k++;
            int taskId = (-1) * get_next_abs_deadline(deadlines, k, mmc);
            t = get_task_by_id(tarefas, TASKS_NUMBER, taskId);
            
            if(t == NULL){
                flag = 1;
                break;
            }
        }while(t->apto != 1);

        if(flag == 1){
            for(int j = 0; j < TASKS_NUMBER; j++){
                if(tarefas[j].apto == 0 && (i % tarefas[j].period == 0)){
                    tarefas[j].apto = 1;
                    tarefas[j].timeExecutionTemp = tarefas[j].timeExecution; 
                }
            }
            continue;
        }
        
        faixa[i] = t->id;
        t->timeExecutionTemp--;

        if(t->timeExecutionTemp == 0) t->apto = 0;
    }

    for(int i = 0; i < mmc; i++){
        if(faixa[i] == 0){
            printf("TEMPO %d - %d: Nenhuma Tarefa\n", i, i + 1);
        }else{
            printf("TEMPO %d - %d: Tarefa: %d\n", i, i + 1, faixa[i]);
        }
    }
    

    fclose(file);
    return 0;
}