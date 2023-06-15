#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

typedef struct documento {
    int ID;
    double *assunto;
    int armarioAtual;
} Documento;

typedef struct armario {
    int ID;
    double *avgAssuntos;
    int numDocs;
} Armario;

int calcularArmarioMaisProx(Armario *armarios, Documento doc, int nArmarios, int nAssuntos);

double *readfile(int rank) {
    int nArm = -1, rows, cols, i, j, count = 0, cond = 0;
    char filename[200];

    if (rank == 0) {
        printf("\nEscreva o caminho do ficheiro a ser lido\n");
        fgets(filename, sizeof(filename), stdin);
        filename[strcspn(filename, "\n")] = 0; // remove newline character

        FILE *file = fopen(filename, "r");
        if (file == NULL) {
            printf("\nError opening file.\n");
            return NULL;
        }

        printf("\nFile opened successfully.\n");
        printf("\nDeseja digitar a quantidade de armarios?\n (1)Sim | (2)Nao ");
        scanf("%d", &cond);

        if (cond == 1) {
            fscanf(file, "%d", &nArm);
            printf("\nDigite a quantidade de armarios\n");
            scanf("%d", &nArm);
        }

        if (nArm == -1) {
            fscanf(file, "%d", &nArm);
        }

        fscanf(file, "%d %d", &rows, &cols);

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
            for (j = 0; j < cols + 1; j++) {
                double num;

                if (fscanf(file, "%lf", &num) != 1) {
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

    return NULL;
}

void writeFile(Documento *doc, int nDocs) {
    int i;
    FILE *file = fopen("docs.out", "w");
    for (i = 0; i < nDocs; i++) {
        fprintf(file, "%d %d\n", doc[i].ID, doc[i].armarioAtual);
    }
    fclose(file);
}

void initializeArmarios(Armario *armarios, int numArmarios, int numAssuntos) {
    int i;
    for (i = 0; i < numArmarios; i++) {
        armarios[i].ID = i;
        armarios[i].numDocs = 0;
        armarios[i].avgAssuntos = malloc(numAssuntos * sizeof(double));
    }
}

void initializeDocs(Documento *docs, int numDocs, int numAssuntos, double *input) {
    int i, j, count = 3;
    for (i = 0; i < numDocs; i++) {
        docs[i].ID = i;
        docs[i].armarioAtual = -1;
        docs[i].assunto = malloc(numAssuntos * sizeof(double));

        for (j = 0; j < numAssuntos; j++) {
            docs[i].assunto[j] = input[count++];
        }
    }
}

void freeMemory(Armario *armarios, Documento *docs, int numArmarios, int numDocs) {
    int i;
    for (i = 0; i < numArmarios; i++) {
        free(armarios[i].avgAssuntos);
    }

    for (i = 0; i < numDocs; i++) {
        free(docs[i].assunto);
    }

    free(armarios);
    free(docs);
}

void initialAssignment(Armario *armario, Documento *docs, int nArmarios, int nDocs){
    // for each document, assign it to a cabinet using round-robin
    int i;
    for (i = 0; i < nDocs; i++) {
        int index = i % nArmarios;
        // atribuir documento ao armario
        docs[i].armarioAtual = index;
        armario[index].numDocs++;
    }
}

void avgCalculator(Armario *armario, Documento *docs,int nArmarios, int nDocs,int nAssuntos) {
    int i, j, k;
    for (i = 0; i < nArmarios; i++) {
        for (k = 0; k < nAssuntos; k++) {
            double num = 0;
            for (j = 0; j < nDocs; j++) {
                if (docs[j].armarioAtual == armario[i].ID){
                    num += docs[j].assunto[k];
                }
            }
            armario[i].avgAssuntos[k] = num / armario[i].numDocs;
        }
    }
}

int reallocateDocs(Armario *armarios, Documento *docs, int nArmarios, int nDocs, int nAssuntos) {
    int i;
    int changed = 0;
    for (i = 0; i < nDocs; i++){
        
        int antigo = docs[i].armarioAtual;
        int newArmario = calcularArmarioMaisProx(armarios,docs[i],nArmarios,nAssuntos);

        if (antigo != newArmario){
            //update the documents current cabinet and position
            docs[i].armarioAtual = newArmario;
            armarios[newArmario].numDocs++;
            armarios[antigo].numDocs--;
            changed = 1;
        }
    }
    return changed;
}

int calcularArmarioMaisProx(Armario *armarios, Documento doc, int nArmarios, int nAssuntos) {
    int i, j;
    int armarioMaisProx = -1;
    double menorDist = INFINITY;

    for (i = 0; i < nArmarios; i++) {
        double dist = 0;

        for (j = 0; j < nAssuntos; j++) {
            double diff = doc.assunto[j] - armarios[i].avgAssuntos[j];
            dist += diff * diff;
        }

        if (dist < menorDist) {
            menorDist = dist;
            armarioMaisProx = i;
        }
    }

    return armarioMaisProx;
}


int main(int argc, char *argv[]) {
    int rank, numProcs, nArmarios, nDocs, nAssuntos;
    int change = 1;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

    double *input = readfile(rank);

    if (input == NULL) {
        MPI_Finalize();
        return 0;
    }

    if (rank == 0) {
        nArmarios = (int)input[0];
        nDocs = (int)input[1];
        nAssuntos = (int)input[2];
    }

    MPI_Bcast(&nArmarios, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&nDocs, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&nAssuntos, 1, MPI_INT, 0, MPI_COMM_WORLD);

    Armario *armarios = malloc(nArmarios * sizeof(Armario));
    Documento *docs = malloc(nDocs * sizeof(Documento));

    initializeArmarios(armarios, nArmarios, nAssuntos);
    initializeDocs(docs, nDocs, nAssuntos, input);
    initialAssignment(armarios,docs,nArmarios,nDocs);

    while(change > 0){
        avgCalculator(armarios,docs,nArmarios,nDocs,nAssuntos);
        change = reallocateDocs(armarios,docs,nArmarios,nDocs,nAssuntos);
    }

    writeFile(docs, nDocs);
    freeMemory(armarios, docs, nArmarios, nDocs);
    MPI_Finalize();
    printf("\nPrograma Executado com Sucesso. Verificar Ficheiros.");
    return 0;
}