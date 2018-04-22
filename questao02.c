#include<stdio.h>
#include<pthread.h>
#include<stdlib.h> 
#include<time.h>

int tamanho;
int *vetor = alocarVetor(tamanho);
pthread_mutex_t mutex_flag_vez;
pthread_cond_t mutex_cond_flag;
pthread_mutex_t acesso_estrutura = PTHREAD_MUTEX_INITIALIZER;

void inicializaFlags(){
    pthread_mutex_init(&mutex_flag_vez, NULL);
    pthread_cond_init(&mutex_cond_flag, NULL);
    flag_vez = 1;
}

void set_flag_vez(int valor){
    pthread_mutex_lock(&mutex_flag_vez);
    flag_vez = valor;
    //acorda alguem que precisa da flag
    //pthread_cond_signal(&mutex_cond_flag);
    //acorda todo mundo que precisa da flag
    pthread_cond_broadcast(&mutex_cond_flag);
    pthread_mutex_unlock(&mutex_flag_vez);
}

void esperar_somapares(){
    pthread_mutex_lock(&mutex_flag_vez);
    while(flag_vez != 1)
        pthread_cond_wait(&mutex_cond_flag, &mutex_flag_vez);
    pthread_mutex_unlock(&mutex_flag_vez);
};

void esperar_gerar_primos(){
    pthread_mutex_lock(&mutex_flag_vez);
    while(flag_vez != 0 && flag_vez != 2)
        pthread_cond_wait(&mutex_cond_flag, &mutex_flag_vez);
    pthread_mutex_unlock(&mutex_flag_vez);
};

int primo(int leitor){
        int i;
	int num = leitor;
        int primo = 0;
        for(i = 2; i < num;i++){
            if(num%i==0){
                primo = 1;
            }
        }
        if(primo==0){
            return 1;
        }
        else{
            return 0;
        }
}

void *gerarprimos(void *arg){
	
	int i;
	int aux;
        
         
        printf("thread um: gerando numeros\n");
        srand(time(NULL));
        for(i = 0; i < tamanho; i += 1)
	{  
            aux = 4+(rand() %40);
            while(primo(aux) == 0  ){
		aux = 4+(rand() %40);
            }
            pthread_mutex_lock(&acesso_estrutura);
            vetor[i] = aux;
            pthread_mutex_unlock(&acesso_estrutura);

       	}
        set_flag_vez(0);
        esperar_somapares();
}

void *somapares(void *arg){
	 int result;
         int j,k;
         
	printf("\nthread dois: verificando se é par\n");
        pthread_mutex_lock(&acesso_estrutura);
        for(j = 0 ; j < tamanho;j++){
            for(k = 0;k<tamanho; k++){
                result = vetor[j]+vetor[k];
            	if((result % 2) == 0){
                	printf("conjectura de Goldbach: %d + %d = %d \n",vetor[j],vetor[k],result);
            	}
                
            }
        }
        pthread_mutex_unlock(&acesso_estrutura);
}

int *alocarVetor(int tam){
    vetor = (int *)calloc(tam,sizeof(int)) ;
    return (vetor);
}

void desalocarVetor(){
    int *v = vetor;
    free(v);
}

int main(int argc, char** argv) {
    pthread_t t1,t2;
    
    printf("digite a quantidade de numeros a ser gerados: ");
    scanf("%d", &tamanho);
    
    inicializaFlags();
    
    pthread_create(&t1, NULL,gerarprimos, NULL);
    pthread_create(&t2, NULL,somapares, NULL);
        
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    
    desalocarVetor();
    
    return (EXIT_SUCCESS);
}