#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <climits>
#include "tipoLoja.hpp"

#define NENHUM -15

float distanciaLojas (tipoLoja a, tipoLoja b) {
  int x1,y1,x2,y2;

  x1 = a.getX();
  y1 = a.getY();
  // std::cout << x1 << "\t" << y1;
  x2 = b.getX();
  y2 = b.getY();
  // std::cout << x2 << "\t" << y2;

  float res = sqrt(pow(x1-x2,2) + pow(y1-y2,2));
  // std::cout << res << "\t";
  return res;  
}

float * prim (float**matriz, int numeroLojas) {
  float * pesos = new float[numeroLojas];

  int * pai = new int[numeroLojas];

  bool * onTree = new bool[numeroLojas];

  for (int i = 0; i < numeroLojas; i++) {
    pesos[i] = INT_MAX;
    onTree[i] = false;
    pai[i] = NENHUM; //
  }

  pesos[0] = 0;

  for (int i = 0; i < numeroLojas; i++) {
    

    /*
    1° - Definimos inicialmente o peso menor como infinito afim de substitui-lo o mais brevemente por qualquer outro
    2° - Definimos uma variavel que vai receber o indice da loja com menor peso para ser adicionada à arvore geradora mínima
    3° - Iteramos pelo vetor de pesos dos vertices que não foram adicionados à arvore. 
    4° - Na primeira iteração, somente temos o vertice 0 como menor que infinito pois será por onde começaremos, logo ele será selecionado
    5° - Nas proximas iterações, como vamos atualizar futuramente o array de pesos para a distancia presente na matriz de adjacencia, o resultado será aquela loja com distancia menor daquelas disponiveis.
    
    
    */
    int menorPeso = INT_MAX;
    int verticeMenorPeso;
    for (int j = 0; j < numeroLojas; j++) {
      if (pesos[j] < menorPeso && onTree[j] == false) {
        menorPeso = pesos[j];
        verticeMenorPeso = j;
      }
    }

    // adicionamos o primeiro vertice ou
    // adicionamos o vertice com menor distancia daqueles alcançaveis pelos nós na árvore
    onTree[verticeMenorPeso] = true;

    //agora precisamos atualizar o valor de peso para aqueles vértices que são conectados ao vertice recem adicionado
    //
    //na primeira iteração adicionaremos valores de peso para todos os vertices uma vez que todos tem distancia entre si
    //
    //nas proximas iterações temos que verificar se a nova distância para o vertice recem adicionado é menor do que aquela já existente no array de pesos
    //caso seja menor -> substituimos o valor de peso
    //caso não seja -> não alteramos

    for (int j = 0; j < numeroLojas; j++) {
      //se o vertice proximo j não foi adicionado a AGM e o peso entre o verticeMenorPeso(recem descoberto) e o vertice j for menor do que aquele especificado em peso[v](peso para conectar a componente ao grafo antes da descoberta do verticeMenorPeso) -> peso = novoPeso
      //obs: também atualizamos o valor de pai para tais vertices uma vez que se houver futuramente um menor peso ele será substituido
      if (onTree[j] == false && pesos[j] > matriz[verticeMenorPeso][j]) {
        pesos[j] = matriz[verticeMenorPeso][j];
        pai[j] = verticeMenorPeso;
      }
    }

  }

// debug
  // for (int i = 1; i < numeroLojas; i++) {
  //    std::cout << "A loja mais próxima da loja " << i<<  " é a loja:  " << pai[i] << "\nA distância entre as duas lojas é: " << pesos[i] << std::endl << std::endl; 
  // }

  
  return pesos;
  
}

int main (int argc, char ** argv) {
  int numeroDeLojas, kmMaxMoto, numeroDeDrones, custoKmMoto, custoKmCaminhao;

  // INPUT DE ARQUIVO
  std::fstream entrada(argv[1]);
  entrada >> numeroDeLojas;
  entrada >> kmMaxMoto;
  entrada >> numeroDeDrones;
  entrada >> custoKmMoto;
  entrada >> custoKmCaminhao;

// debug
  // std::cout << "soma de todos os parametros  " << numeroDeLojas+kmMaxMoto+numeroDeDrones+custoKmMoto+custoKmCaminhao << std::endl;

// criação de array de lojas
  tipoLoja * listaLojas = new tipoLoja[numeroDeLojas];
  int x,y;
  int i = 0;
  
  while (entrada >> x && entrada >> y) {
    tipoLoja aux(x,y);
    listaLojas[i] = aux;
    i++;   
  }


// ALOCAÇÃO DINÂMICA DE MATRIZ DE ADJACÊNCIA
  float **matrizAdjDistancias = new float*[numeroDeLojas];
  for (int i = 0; i < numeroDeLojas; i++) {
    matrizAdjDistancias[i] = new float[numeroDeLojas];
  }

// POPULAÇÃO DE MATRIZ DE ADJACÊNCIA
  for (int i = 0; i < numeroDeLojas; i++) {
    for (int j = 0; j < numeroDeLojas; j++) {
      matrizAdjDistancias[i][j] = distanciaLojas(listaLojas[i], listaLojas[j]);
       //debug
       //std::cout << matrizAdjDistancias[i][j] << "\t";
    }
    //debug
    //std::cout << std::endl;
  }

// CHAMADA DE FUNÇÃO PARA OBTER VETOR DE DISTÂNCIAS DA AGM
float * vetorDistancias = prim(matrizAdjDistancias, numeroDeLojas);




// POP DE MAIORES DISTÂNCIAS PARA USO DO DRONE
// COMO SÓ PODEMOS TRANSPORTAR ENTRE DUAS LOJAS QUE POSSUEM DRONES, A QUANTIDADE DE ARESTAS PERCORRIDAS SERA IGUAL A NUMERODEDRONES-1
if (numeroDeDrones >= 2){
  for (int i = 0; i < numeroDeDrones - 1; i++) {
    float auxMaiorDist = 0;
    int auxLojaMaiorDist;

    for (int j = 0; j < numeroDeLojas; j++) {
      if (auxMaiorDist < vetorDistancias[j]) {
        auxMaiorDist = vetorDistancias[j];
        auxLojaMaiorDist = j;
      }
    }

    for (int j = auxLojaMaiorDist; j < (numeroDeLojas - 1); j++) {
      vetorDistancias[j] = vetorDistancias[j+1];
    }
    numeroDeLojas--;
  }
}

// DEBUG


// CALCULO DE TRAJETOS DE MOTO E TRAJETOS DE CAMINHÃO
float custoMoto = 0, custoCaminhao = 0;

for (int i = 0; i < numeroDeLojas; i++) {
  if (vetorDistancias[i] <= kmMaxMoto) {
    custoMoto = custoMoto + vetorDistancias[i]*custoKmMoto;
  }
  else {
    custoCaminhao = custoCaminhao + vetorDistancias[i]*custoKmCaminhao;
  }
}

// SAÍDA DO PROGRAMA
std::cout << custoMoto << " " << custoCaminhao << std::endl;



return 0;

}



// TODO -> resolver problema de drones nao contando