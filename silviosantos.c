#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//variavel global, visivel a todos os threads
//flag_vez == 0, significa que o auditorio deve apresentar o convidado
//flag_vez == 1, significa que é a vez do silvio apresentar
//flag_vez == 2, Silvio Santos ja apresentou todo mundo!
int flag_vez;
pthread_mutex_t mutex_flag_vez;
pthread_cond_t mutex_cond_flag; //acordar quando for a ver

//acesso a estrutura de forma unica
pthread_mutex_t acesso_estrutura = PTHREAD_MUTEX_INITIALIZER;

void inicializaFlags(){
    pthread_mutex_init(&mutex_flag_vez, NULL);
    pthread_cond_init(&mutex_cond_flag, NULL);
    flag_vez = 1;
};

void set_flag_vez(int valor){
    pthread_mutex_lock(&mutex_flag_vez);
    flag_vez = valor;
    //acorda alguem que precisa da flag
    //pthread_cond_signal(&mutex_cond_flag);
    //acorda todo mundo que precisa da flag
    pthread_cond_broadcast(&mutex_cond_flag);
    pthread_mutex_unlock(&mutex_flag_vez);
};


//estrutura que os threads vao compartilhar
//vai ser fornecida no momento de criacao
struct convidados{
    char **nomes;
    int qtdConvidados; //total de convidados
    int convidadosApresentados; //quantos convidados foram apresentados
};

void esperar_Auditorio(){
    pthread_mutex_lock(&mutex_flag_vez);
    while(flag_vez != 1)
        pthread_cond_wait(&mutex_cond_flag, &mutex_flag_vez);
    pthread_mutex_unlock(&mutex_flag_vez);
};

void esperar_Silvio(){
    pthread_mutex_lock(&mutex_flag_vez);
    while(flag_vez != 0 && flag_vez != 2)
        pthread_cond_wait(&mutex_cond_flag, &mutex_flag_vez);
    pthread_mutex_unlock(&mutex_flag_vez);
};

void *silvio_apresenta_convidados(void *param){
    struct convidados *meus_convidados = (struct convidados *) param;
    int qtdConvidados, qtdConvidadosApresentados;
    
    //acessando estrutura compartilhada
    pthread_mutex_lock(&acesso_estrutura);
    qtdConvidados = meus_convidados->qtdConvidados;
    qtdConvidadosApresentados = meus_convidados->convidadosApresentados;
    pthread_mutex_unlock(&acesso_estrutura);
    
    for(int i =0; i < qtdConvidados; i++){
        printf("Silvio: Mahhh Oeeee haha hihi\n");sleep(1);
        
        pthread_mutex_lock(&acesso_estrutura);
        printf("Silvio: La vem O(A) %s\n",meus_convidados->nomes[meus_convidados->convidadosApresentados]); sleep(1);
        pthread_mutex_unlock(&acesso_estrutura);
        
        printf("Silvio: lalalalalala\n");sleep(1);
        printf("Silvio: lalalalalala\n");sleep(1);
        
        set_flag_vez(0); //coloca a vez do auditorio
        esperar_Auditorio(); //implementar;
    }
    printf("Acabou os convidados! Mah Oeeee\n");sleep(1);
    printf("Quem quer aviaozinho\n");sleep(1);
    set_flag_vez(2);
    
};

void *auditorio_apresenta_convidados(void *param){
    struct convidados *nossos_convidados = (struct convidados *)param;
     
    for(;;){
        esperar_Silvio();
        if(flag_vez ==2 ) //a rigor deveria ser feito de maneira segura
			break;
        pthread_mutex_lock(&acesso_estrutura);
        printf("Auditorio: O(A) %s\n", nossos_convidados->nomes[nossos_convidados->convidadosApresentados]); sleep(1);
        nossos_convidados->convidadosApresentados++;
        pthread_mutex_unlock(&acesso_estrutura);
        
        printf("Auditorio: lalalalallaa\n");sleep(1);
        printf("Auditorio: lalalalallaa\n");sleep(1);
        printf("Auditorio: lalalalallaa\n");sleep(1);      
        printf("\n\n");  
        set_flag_vez(1);
    }
    
};

void alocarConvidados(struct convidados *entrada, int qtd){
    entrada->nomes = (char **)malloc(sizeof(char *)*qtd);
    for(int i =0; i < qtd; i++)
        entrada->nomes[i] = (char *)malloc(sizeof(char)*100);
    
    //ler o nome dos convidados
    for(int i =0; i < qtd; i++){
        printf("Convidado %d: ",i+1);
        scanf(" %[^\n]s",entrada->nomes[i]);
    }
    
    entrada->qtdConvidados = qtd;
    entrada->convidadosApresentados = 0;
};

void desalocarConvidados(struct convidados *entrada){
    int qtdConvidados = entrada->qtdConvidados;
    for(int i=0; i < qtdConvidados; i++)
        free(entrada->nomes[i]);
    free(entrada->nomes);
};


int main(){
    int qtdConvidados;
    struct convidados convidados_do_silvio;
    
    printf("Lombarde: Quantos convidados teremos hoje, Silvio? ");
    scanf("%d",&qtdConvidados);
    //aloca e fornece os convidados
    alocarConvidados(&convidados_do_silvio, qtdConvidados);
    
    //inicializa as flags
    inicializaFlags();
    
    pthread_t pthread_silvio, pthread_auditorio;
    pthread_create(&pthread_silvio, NULL, &silvio_apresenta_convidados,&convidados_do_silvio);
    pthread_create(&pthread_auditorio, NULL, &auditorio_apresenta_convidados, &convidados_do_silvio);
    pthread_join(pthread_silvio, NULL);
    pthread_join(pthread_auditorio,NULL);
    
    desalocarConvidados(&convidados_do_silvio);
    
    return 0;    
}
