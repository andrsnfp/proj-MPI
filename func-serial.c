#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include"doc-serial.h"
#include<stdbool.h> 

/*Estrutura de um Documento*/
typedef struct documento{
    double *assunto;
    int armarioActual;
    int ID;
    bool assigned;
    int posicaoArmario;
}Documento;

/*Estrutura de um Armario*/
typedef struct armario{
    int ID;
    double *avgAssuntos;
    int numDocs;
    Documento *docs;
}Armario;

//Funcao que le o ficheiro e organiza os caracteres num array excluindo espacos e newlines
double *readfile(){
    
    int nArm = -1, rows, cols, i, j, count = 0,cond = 0;
    char filename[200];

    printf("\nEscreva o caminho do ficheiro a ser lido\n");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = 0; // remove newline character

    //FILE *file = fopen("doc1000-50d.in", "r");
    FILE *file = fopen(filename, "r");  
    if (file == NULL) {
        printf("\nError opening file.\n");
        return NULL;
    }

    printf("\nFile opened successfully.\n");
    printf("\nDeseja digitar a quantidade de armarios?\n (1)Sim | (2)Nao ");
    scanf("%d",&cond);
    
    if (cond == 1){
        fscanf(file, "%d",&nArm);
        printf("\nDigite a quantidade de armarios\n");
        scanf("%d",&nArm);
    }

    if(nArm == -1){
        fscanf(file, "%d",&nArm);
    }

    fscanf(file, "%d %d",&rows,&cols);
    
    int tam = 3 + (rows * (cols + 1));
    double *arr = malloc(tam * sizeof(double));
    if (arr == NULL) {
        printf("\nError allocating memory.\n");
        return NULL;
    }

    arr[count++] = (double)nArm;
    arr[count++] = (double)rows;
    arr[count++] = (double)cols;

    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols+1; j++) {
            double num;

            if (fscanf(file, "%lf", &num) != 1){
                printf("Error reading file.");
                fclose(file);
                free(arr);
                return NULL;
            }
            arr[count++] = num;
        }
    }
    fclose(file);  
    return arr;
}

//Funcao que apresenta a saida np ficheiro de saida
void writeFile(Documento *doc, int nDocs){
    int i;
    FILE *file = fopen("docs.out", "w");
    for (i = 0; i < nDocs; i++){
        fprintf(file, "%d %d\n", doc[i].ID, doc[i].armarioActual);
    }
    fclose(file);   
}

//Fucao que recebe os dados e os atribui ao respectivo destino
void process(){
    int i,j;
    bool change = false;
    double *arr = readfile();
    int nArmarios = (int)arr[0];
    int nDocs = (int)arr[1];
    int nAssuntos = (int)arr[2];
    Armario armarios[nArmarios];
    Documento docs[nDocs];

    initializeArmarios(armarios,nArmarios,nDocs,nAssuntos);
    initializeDocs(docs,nDocs,nAssuntos,arr);
    initialAssignment(armarios,docs,nArmarios,nDocs);

    avgCalculator(armarios,docs,nArmarios,nDocs,nAssuntos,change);
    //reallocateDocs(armarios,docs,nArmarios,nDocs,nAssuntos,change);
    writeFile(docs,nDocs);
    free(arr);
    printf("\nPrograma Executado com Sucesso. Verificar Ficheiros.");
}

//funcao que atribui os documentos aos armarios usando round-robin
void initialAssignment(Armario *armario, Documento *docs, int nArmarios, int nDocs){
    // for each document, assign it to a cabinet using round-robin
    int i,j,k;
    for (i = 0; i < nDocs; i++) {
        int index = i % nArmarios;
        int posicaoNoArmario = getDocIndex(armario[index],nDocs);
        
        if (posicaoNoArmario < 0){
            printf("Error: index out of bounds for cabinet %d\n", index);
            exit(1);
        }else{
            // atribuir documento ao armario
            docs[i].armarioActual = index;
            docs[i].posicaoArmario = posicaoNoArmario;
            armario[index].docs[posicaoNoArmario] = docs[i];
            armario[index].docs[posicaoNoArmario].assigned = true;
            armario[index].numDocs++;
        }
    }
}

//f auxiliar a initialAssignment
int getDocIndex(Armario armario, int numDocs) {
    int i;
    for (i = 0; i < numDocs; i++) {
        if (!armario.docs[i].assigned) {
            return i;
        }
    }
    return -1; // no empty slots found
}

void avgCalculator(Armario *armario, Documento *docs,int nArmarios, int nDocs,int nAssuntos, bool change) {
    int i, j, k;
    for (i = 0; i < nArmarios; i++) {
        for (k = 0; k < nAssuntos; k++) {
            double num = 0;
            for (j = 0; j < armario[i].numDocs; j++) {
                num += armario[i].docs[j].assunto[k];
            }
            armario[i].avgAssuntos[k] = num / armario[i].numDocs;
        }
    }
    reallocateDocs(armario,docs,nArmarios,nDocs,nAssuntos,change);
}

void reallocateDocs(Armario *armarios, Documento *docs, int nArmarios, int nDocs, int nAssuntos, bool change) {
    int i,j,k;
    
    while(1){
        change = false;
        for (i = 0; i < nDocs; i++){
            
            int antigo = docs[i].armarioActual;
            int posAntiga = docs[i].posicaoArmario;
            int newArmario = calcularArmarioMaisProx(armarios,docs[i],nArmarios,nAssuntos);
            
            printf("%d",newArmario);

            if (antigo != newArmario){
                //update the documents current cabinet and position
                docs[i].armarioActual = newArmario;
                docs[i].posicaoArmario = getDocIndex(armarios[newArmario], nDocs);

                //move the document to the new cabinet
                armarios[newArmario].docs[docs[i].posicaoArmario] = docs[i];
                armarios[newArmario].docs[docs[i].posicaoArmario].assigned = true;
                armarios[newArmario].numDocs++;

                change = true;
                for (j = posAntiga + 1; j < armarios[antigo].numDocs; j++) {
                    armarios[antigo].docs[j - 1] = armarios[antigo].docs[j];
                    armarios[antigo].docs[j - 1].posicaoArmario = j - 1;
                }
                armarios[antigo].numDocs--;
            }
        }

        if(change){
            avgCalculator(armarios,docs,nArmarios,nDocs,nAssuntos,change);
        }else{
            break;
        }
    }
}

void initializeArmarios(Armario *armarios, int numArmarios, int numDocs, int numAssuntos){
    int i,j;
    for (i = 0; i < numArmarios; i++) {
        armarios[i].ID = i;
        armarios[i].numDocs = 0;
        armarios[i].avgAssuntos = malloc(numAssuntos * sizeof(double));
        armarios[i].docs = malloc(numDocs * sizeof(Documento));
    }
}

void initializeDocs(Documento *docs, int numDocs, int numAssuntos, double *input){
    int i,j,count=3;
    for (i = 0; i < numDocs; i++){
        docs[i].ID = (int)input[count++];
        docs[i].assunto = malloc(numAssuntos * sizeof(double));
        docs[i].assigned = false;
        for(j = 0; j < numAssuntos; j++){
            docs[i].assunto[j] = input[count++];
        }
    }
}

int calcularArmarioMaisProx(Armario *armarios, Documento doc, int nArmarios, int nAssuntos){
    int j,k;

    double menorDistancia = INFINITY;
    int newArmario = doc.armarioActual;
    
    for (j = 0; j < nArmarios; j++){
        double distancia = 0;

        for (k = 0; k < nAssuntos; k++){
            double diferenca = doc.assunto[k] - armarios[j].avgAssuntos[k];
            distancia += diferenca * diferenca;
        }

        if (distancia < menorDistancia) {
            menorDistancia = distancia;
            newArmario = j; 
        }
    }
    return newArmario;
}
