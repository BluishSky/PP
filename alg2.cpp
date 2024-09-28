#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>

typedef struct {

    int *edgelistSize;
    int** edgelist;
    int vertices;
    int *countersPerVertex;

    void initialize(const char* graphDataset) {
        FILE* fp = fopen("graph.edgelist", "r");
        int src, dst;
        int maxVertexId = -1;

        // Find total number of vertices by the highest value
        while(fscanf(fp, "%d %d", &src, &dst) != EOF) {
            if(src > maxVertexId)
                maxVertexId = src;
            if(dst > maxVertexId)
                maxVertexId = dst;
        }
        vertices = maxVertexId+1;    

        // Find total number of edges on each vertice
        rewind(fp);
        edgelistSize = (int*)malloc(vertices*sizeof(int)); // Memory allocation for every vertice
        while(fscanf(fp, "%d %d", &src, &dst) != EOF) {
            edgelistSize[src]++;                            // +1 on every new edge
        }

        // edgelist is a pointer to an array (**) of pointers (*)
        edgelist = (int**)malloc(vertices * sizeof(int*)); // Memory allocation for every vertice
        for(int i = 0 ; i < vertices ; i++) {
            edgelist[i] = (int*)malloc(edgelistSize[i] * sizeof(int)); // Memory allocation for every edge on every vertice
        }

        countersPerVertex = (int*)malloc(vertices*sizeof(int));
        memset(countersPerVertex, 0, vertices*sizeof(int));

        rewind(fp);
    }

    int getVertices() {
        return vertices;
    }

    int getEdgelistSize(int src) {
        return edgelistSize[src];
    }

    int getEdge(int src, int pos) {
        return edgelist[src][pos];
    }

    void addEdge(int src, int dst) {
        edgelist[src][countersPerVertex[src]++] = dst;
    }

    //TODO
    bool isNeighbour(int src, int dst) {
        return true;
    }
    void release() {
        for(int i = 0 ; i < vertices ; i++)
            free(edgelist[i]);
        free(edgelist);
        // TODO Release the other structures.
    }
} Graph;


#include <queue>

std::mutex mtx;
std::mutex mtx_roubo;

void processa_cliques(std::queue<std::vector<int>> &cliques, Graph *grafo, int k, int &contador, int r, std::vector<std::queue<std::vector<int>>> &trabalhos_threads) {
    std::vector<std::vector<int>> cliques_save;
    std::vector<int> nova_clique;

    while (true) {
        std::vector<int> clique;
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (!cliques.empty()) {
                clique = cliques.front();
                cliques.pop();
            } else {
                // Tentar roubar cliques de outra thread
                bool roubou = false;
                for (auto &trabalhos : trabalhos_threads) {
                    std::lock_guard<std::mutex> lock_roubo(mtx_roubo);
                    if (!trabalhos.empty()) {
                        for (int i = 0; i < r && !trabalhos.empty(); ++i) {
                            cliques.push(trabalhos.front());
                            trabalhos.pop();
                        }
                        roubou = true;
                        break;
                    }
                }
                if (!roubou) break; // Se não conseguiu roubar, termina a execução
                continue;
            }
        }

        if (clique.size() == k) {
            std::lock_guard<std::mutex> lock(mtx);
            if (std::find(cliques_save.begin(), cliques_save.end(), clique) == cliques_save.end()) {
                contador++;
                cliques_save.push_back(clique);
            }
            continue;
        }

        int ultimo_vertice = clique.back();
        for (int vertice : clique) {
            for (int j = 0; j < grafo->getEdgelistSize(vertice); j++) {
                int vizinho = grafo->getEdge(vertice, j);
                if (vizinho > ultimo_vertice &&
                    std::find(clique.begin(), clique.end(), vizinho) == clique.end() &&
                    std::all_of(clique.begin(), clique.end(), [&](int v) { return grafo->isNeighbour(v, vizinho); })) {
                    nova_clique = clique;
                    nova_clique.push_back(vizinho);
                    std::lock_guard<std::mutex> lock(mtx);
                    cliques.push(nova_clique);
                }
            }
        }
    }
}

int contagem_de_cliques_paralela_balanceada(Graph *grafo, int k, int t, int r) {
    int contagem = 0;
    std::vector<std::queue<std::vector<int>>> trabalhos_threads(t);
    for (int i = 0; i < grafo->getVertices(); i++) {
        trabalhos_threads[i % t].push({i});
    }

    std::vector<std::thread> threads;
    std::vector<int> contadores(t, 0);

    for (int i = 0; i < t; i++) {
        threads.push_back(std::thread(processa_cliques, std::ref(trabalhos_threads[i]), grafo, k, std::ref(contadores[i]), r, std::ref(trabalhos_threads)));
    }

    for (auto &th : threads) {
        th.join();
    }

    for (int c : contadores) {
        contagem += c;
    }

    return contagem;
}



void processa_cliques(std::vector<std::vector<int>> cliques, Graph *grafo, int k, int &contador) {
    std::vector<std::vector<int>> cliques_save;
    std::vector<int> nova_clique;

    while (!cliques.empty()) {
        std::vector<int> clique = cliques.back();
        cliques.pop_back();

        if (clique.size() == k) {
            std::lock_guard<std::mutex> lock(mtx);
            if (std::find(cliques_save.begin(), cliques_save.end(), clique) == cliques_save.end()) {
                contador++;
                cliques_save.push_back(clique);
            }
            continue;
        }

        int ultimo_vertice = clique.back();
        for (int vertice : clique) {
            for (int j = 0; j < grafo->getEdgelistSize(vertice); j++) {
                int vizinho = grafo->getEdge(vertice, j);
                if (vizinho > ultimo_vertice &&
                    std::find(clique.begin(), clique.end(), vizinho) == clique.end() &&
                    std::all_of(clique.begin(), clique.end(),[&](int v){ return grafo->isNeighbour(v, vizinho); })) {
                    nova_clique = clique;
                    nova_clique.push_back(vizinho);
                    cliques.push_back(nova_clique);
                }
            }
        }
    }
}

int contagem_de_cliques_paralela(Graph *grafo, int k, int t) {
    int contagem = 0;
    std::vector<std::vector<int>> cliques;
    for (int i = 0; i < grafo->getVertices(); i++) {
        cliques.push_back({i});
    }

    std::vector<std::thread> threads;
    std::vector<int> contadores(t, 0);
    int tamanho_parte = cliques.size() / t;

    for (int i = 0; i < t; i++) {
        std::vector<std::vector<int>> parte_cliques(cliques.begin() + i * tamanho_parte, cliques.begin() + (i + 1) * tamanho_parte);
        threads.push_back(std::thread(processa_cliques, parte_cliques, grafo, k, std::ref(contadores[i])));
    }

    for (auto &th : threads) {
        th.join();
    }

    for (int c : contadores) {
        contagem += c;
    }

    return contagem;
}


int contagem_de_cliques_serial(Graph *grafo, int k) {
  int contagem = 0;
  std::vector<std::vector<int>> cliques;
  std::vector<int> nova_clique;
  std::vector<std::vector<int>> cliques_save;

  for (int i = 0; i < grafo->getVertices(); i++) {
    cliques.push_back({i});
  }

  while (!cliques.empty()) {
    std::vector<int> clique = cliques.back();

    cliques.pop_back();

    printf("Clique atual: ");
    for (int vertice : clique) {
      printf("%d ", vertice);
    }
    printf("\n");

    if (clique.size() == k) {

      if (std::find(cliques_save.begin(), cliques_save.end(), clique) ==
          (cliques_save.end())) {
        contagem++;
        printf("\nContagem: %d\n", contagem);
        cliques_save.push_back(clique);
      }

      continue;
    }

    int ultimo_vertice = clique.back();
    printf("Último vértice: %d\n", ultimo_vertice);
    for (int vertice : clique) {
      printf("Vértice: %d\n", vertice);

      // verifica os vizinhos do vertice
      for (int j = 0; j < grafo->getEdgelistSize(vertice); j++) {
        int vizinho = grafo->getEdge(vertice, j);
        printf("vizinho de %d: %d\n", vertice, vizinho);
        // so adiciona vizinhos que sao maiores que o ultimo vértice da clique
        // atual
        if (vizinho > ultimo_vertice &&
            std::find(clique.begin(), clique.end(), vizinho) ==
                clique.end() && // vizinho nao esta na clique
            std::all_of(clique.begin(), clique.end(), [&](int v) {
              return grafo->isNeighbour(v, vizinho);
            })) {

          nova_clique = clique;
          nova_clique.push_back(vizinho);
          cliques.push_back(nova_clique);

          printf("Nova clique formada: ");
          for (int nv : nova_clique) {
            printf("%d ", nv);
          }
          printf("\n");
        }
      }
    }
    printf("Estado atual das cliques:\n");
    for (const auto &c : cliques) {
      printf("[ ");
      for (int v : c) {
        printf("%d ", v);
      }
      printf("]\n");
    }
  }

  return contagem;
}

int main(int argc, char *argv[]) {
  int k = atoi(argv[1]);
  int t = atoi(argv[2]);
  int r = atoi(argv[3]);
  printf("Valor de k: %d\n", k);
  Graph* graph = new Graph;
    graph->initialize("graph.edgelist");

    FILE* fp = fopen("graph.edgelist", "r");

    int src, dst;
    while(fscanf(fp, "%d %d", &src, &dst) != EOF) {       
        graph->addEdge(src,dst);        
    }
    
  //printf("Cliques serial: %d", contagem_de_cliques_serial(graph, k));
  //printf("Cliques paralela: %d", contagem_de_cliques_paralela(graph, k, t));
  printf("Cliques paralela balanceada: %d", contagem_de_cliques_paralela_balanceada(graph, k, t,r));
//graph->printEdgelist();

  fclose(fp);

 graph->release();
  free(graph);
  return 0;
}

// Salvar as arestas ida e volta na mão 1 0 e 0 1