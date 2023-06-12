# Projecto com MPI
   Este projecto representa a versão paralela com MPI
# Como rodar o projeto em ambiente local
   Este documento descreve os passos necessários para rodar o projeto em ambiente local:
   Passo 1: Instale o OpenMPI
   Abra um terminal.
   Execute o seguinte comando para instalar o OpenMPI:
    ```sh 
     sudo apt-get install openmpi-bin openmpi-common libopenmpi-dev
    ```
   Passo 2: Compile o programa MPI em C
   Certifique-se de ter o código-fonte do programa MPI em C (arquivo .c).
   Navegue até o diretório onde o arquivo .c está localizado usando o comando cd.
   Use o comando mpicc para compilar o programa:
     ```sh 
      mpicc seu_programa_mpi.c -o seu_programa_mpi
     ```
   Passo 3: Execute o programa MPI
   Após a compilação bem-sucedida, você pode executar o programa MPI usando o comando mpirun ou mpiexec.
   Você pode usar a opção -np para especificar o número de processos a serem usados.
   Por exemplo, se você quiser executar o programa MPI em 4 processos, use o seguinte comando:
     ```sh 
      mpirun -np 4 ./seu_programa_mpi
     ```
