/*Estrutura de um Documento*/
typedef struct documento Documento;
/*Estrutura de um Armario*/
typedef struct armario Armario;
//Funcao que le o ficheiro e organiza os caracteres num array excluindo espacos e newlines
double *readfile();
//funcao que atribui os documentos aos armarios usando round-robin
void initialAssignment(Armario *armario, Documento *docs, int nArmario, int nDocs);
//funcao run
void process();
//Funcao que apresenta a saida no ficheiro de saida
void writeFile(Documento *doc, int nDocs);
//f auxiliar para posicionamento corrrecto de documentos
int getDocIndex(Armario armario, int numDocs);
//f para visualiza;\ao e debugging
void printState(Armario *armarios, Documento *docs, int nDocs, int nAssuntos, int nArmarios);
//f para calcular a media dos armarios
void avgCalculator(Armario *armario,int nArmario, int nAssuntos,bool change);
//f para realocar os docs de acordo com a distancia minima
void reallocateDocs(Armario *armarios, Documento *docs, int nArmarios, int nDocs, int nAssuntos, bool change);
//f que inicializa armarios
void initializeArmarios(Armario *armarios, int numArmarios, int numDocs,int numAssuntos);
//f que inicializa documentos
void initializeDocs(Documento *docs, int numDocs, int numAssuntos, double *input);