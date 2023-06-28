#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/shm.h>


pthread_barrier_t mybarrier;
void* funcion_hilo(void*);
/*declarando las estructuras de datos*/
/*esta estructura se le enviara al hilo, para que sepa donde comienza y termina su rango*/
struct data{
    int start;
    int end;
};
/*matriz de datos*/
int **matriz;
/*matriz de resultados*/
int **resultado;
/*variable con la cantidad de registros del vector*/
int nCol;
/*variable con la cantidad de registros del vector*/
int nRow;
/*variable con la cantidad de hilos*/
int nH;
int max_lenght = 20;
int change=1;
int times =0;
int nTimes =0;
int* numbers(char *word){
    char number[strlen(word)];
    int *response = (int *)malloc(strlen(word)*sizeof(int));
    int count=0;
    int count2=0;
    for(int i = 0;i<strlen(word);i++){
        char var = word[i];
    if (var != ' ' ) {
        number[count] = word[i];
        count++;
    } else {
        if(count !=0){
            response[count2] = atoi(number);
            for(int k = 0;k<count+1;k++){
                number[k] = '\0';
            }
            count = 0;
            count2++;
        }
    }
    }
    response[count2] = atoi(number);
    count = 0;
    count2++;
    return response;
}

int main(int args, char ** argsv){
    FILE *file;
    file = fopen("jacobi.txt","rt");
    char *line = (char*) malloc( max_lenght*sizeof(char));
    fgets(line, max_lenght*sizeof(char), file);
    /*guardando el numero de registros del vector*/
    int *values = numbers(line);
    nCol = values[0];
    nRow = values[1];
    printf("el numero de columnas es: %d \n",nCol);
    printf("el numero de filas es: %d \n",nRow);
    int cont =0;
    matriz = (int **) malloc(nRow*sizeof(int*));
    while (fgets(line, max_lenght*sizeof(char), file)!= NULL){
        int *vect = (int *) malloc(nRow*sizeof(int));
        vect = numbers(line);
        matriz[cont] = vect;
        cont ++;
    }
    fclose(file);
    resultado = (int **) malloc(nRow*sizeof(int*));
    for(int i = 0;i <nCol;i++){
        int *vect = (int *) malloc(nRow*sizeof(int));
        for(int j = 0;j <nRow;j++){
            vect[j] = 0;
        }
        resultado[i]=vect;
    }
    for(int i = 0;i <nCol;i++){
        resultado[nCol-1][i] = matriz[nCol-1][i];
        resultado[0][i] = matriz[0][i];
        resultado[i][nCol-1] = matriz[i][nCol-1];
        resultado[i][0] = matriz[i][0];
    }
    printf("matriz: \n");
    for(int i = 0;i <nCol;i++){
        for(int j = 0;j <nRow;j++){
            printf("%d ",matriz[i][j]);
        }
        printf("\n");
    }
    printf("resultado: \n");
    for(int i = 0;i <nCol;i++){
        for(int j = 0;j <nRow;j++){
            printf("%d ",resultado[i][j]);
        }
        printf("\n");
    }

    
    do{
        printf("Ingrese el numero de hilos: ");
        scanf("%d",&nH);
    }while(nCol % nH != 0);
        printf("Ingrese el numero de veces que se ejecutara la operacion: ");
        scanf("%d",&nTimes);
    pthread_barrier_init(&mybarrier, NULL, nH);
    pthread_t ids[nH];
    /*declarando los valores de inicio y final con la estructura que creamos*/
    struct data val[nH];
    /*definiendo el salto, para saber que rango de datos le tocara*/
    int salto = nCol/nH; 
    /*creando los hilos y asignandoles su rango*/
    for(int i = 0;i<nH;i++){
        val[i].start = salto*i;
        val[i].end = salto*(i+1);
        pthread_create(&ids[i],NULL,funcion_hilo,&val[i]);
    }
    while(times <nTimes){
        if(!change){
            sleep(3);
            printf("matriz: \n");
            for(int i = 0;i <nCol;i++){
                for(int j = 0;j <nRow;j++){
                    printf("%d ",matriz[i][j]);
                }
                printf("\n");
            }
            printf("resultado: \n");
            for(int i = 0;i <nCol;i++){
                for(int j = 0;j <nRow;j++){
                    printf("%d ",resultado[i][j]);
                }
                printf("\n");
            }
            for(int i = 0;i<nCol;i++){
                for(int j = 0;j<nRow;j++){
                    matriz[i][j]=resultado[i][j];
                }
            }
            printf("nueva matriz: \n");
            for(int i = 0;i <nCol;i++){
                for(int j = 0;j <nRow;j++){
                    printf("%d ",matriz[i][j]);
                }
                printf("\n");
            }
            printf("\n");
            times ++;
            change =1;
        }
    }
    
    for(int i = 0;i<nH;i++){
        pthread_join(ids[i],NULL);
    }
    pthread_barrier_destroy(&mybarrier);
    
    return 0;
}
int mult(int r,int c){
    int res = (matriz[r+1][c] + matriz[r-1][c] + matriz[r][c+1] + matriz[r][c-1])/4;
    return res;
}
/*declarando la funcion que obtendra el valor que le corresponde a cada casilla del vector resultante*/
void* funcion_hilo(void* arg){
    struct data* data = (struct data*)arg;
    printf("al hilo [%lu] le toco el rango[%d]-[%d]\n",pthread_self(),data->start,data->end);
    while (times<nTimes){
        if(change){
            for(int i = data->start; i<data->end;i++){
                if(i!= 0 && i!=nRow-1){
                    for(int j = 1; j<nCol;j++){
                        if(j!= nCol-1){
                            resultado[i][j]=mult(i,j);
                        }
                    }
                }
            }
            pthread_barrier_wait(&mybarrier);
            change =0;
            pthread_barrier_wait(&mybarrier);

        }
    }
    
    pthread_exit(0);
}