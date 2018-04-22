#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>


int main(){
	int n=9;
	//para compilar gcc nomearquivo.c -pthread -o saida.o -lrt
    
    //declarar e inicializar um semaforo com o valor 1
    sem_unlink("semaforo"); //retira o semaforo antigo se houver resquicio
    sem_t *semaforo = sem_open("semaforo",O_CREAT | O_EXCL, 0, 1);
    if(semaforo == NULL){
		printf("falha em alocar semaforo");
		exit(1);
	}
    
    //tamanho da pagina (no meu computador)
    int tamanho_pagina = getpagesize();

    //memoria compartilhada
    int memoria_compartilhada = shm_open("memoria", O_CREAT | O_RDWR, 0);    
    
    //trunca para um tamanho exato do tamanho de pagina (caso nao seja desalocado de uma vez anterior isso eh necessario)
    //ftruncate(memoria_compartilhada, tamanho_pagina);
    
    int *vetor = mmap(0,tamanho_pagina, PROT_WRITE | 
                    PROT_READ, MAP_SHARED, memoria_compartilhada, 0);
    
    vetor[0]=0;
    vetor[1]=0;
    vetor[2]=0;
    
    pid_t filho1, filho2;
    filho1 = fork(); // crio um processo filho
	int i,j,k;
    if(filho1 != 0){
		for(i=0;i<n;i++){
			printf("Huguinho está cavando");
        //processo pai
        sem_wait(semaforo); //corresponde ao down
        //regiao critica
        vetor[0] +=1000;
        vetor[1] +=1000;
        vetor[2] +=1000;
        sem_post(semaforo); //corresponde ao up
        wait(0); // espera o processo filho terminar
        printf("Resultado final:\n");
        printf("primeira posicao: %d\n", vetor[0]);
        printf("segunda posicao: %d\n", vetor[1]);
        printf("terceira posicao: %d\n", vetor[2]);
        
        //desalocando recursos
        sem_unlink("semaforo"); //desaloca semaforo
        munmap("memoria", tamanho_pagina); //desaloca pagina na m.v.
        close(memoria_compartilhada); 
    }}else{
        //processo filho
        filho2 = fork(); // gera um neto | filho do filho
        if(filho2 != 0){
			
			for(j=i+1;j<n-1;j++){
				printf("Zezinho está colocando a semente.");
            //processo filho (pai atual)
            sem_wait(semaforo); //down no semaforo
            //regiao critica
            vetor[1] += 1000;
            vetor[2] += 1000;
            
            sem_post(semaforo); //up no semaforo
            wait(0); //aguarda o neto terminar (o filho do pai atual)
            }}else{
				
			for(k=j+1;k<n-2;k++){
				printf("Luizinho está tampando o buraco.");
            //processo neto
            sem_wait(semaforo);
            //regiao critica
            vetor[2] += 1000;
            
            sem_post(semaforo);
        }
    }
    }
    return 0;
    }
    

