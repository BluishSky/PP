#include <algorithm>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <queue>
#include <semaphore.h>
#include <stdio.h>
#include <thread>
#include <vector>

typedef struct Graph {

  int *edgelistSize;
  int **edgelist;
  int vertices = 0;
  int *countersPerVertex;

  void initialize(const char *graphDataset) {
    FILE *fp = fopen(graphDataset, "r");
    int src, dst;
    int maxVertexId = -1;

    // Primeira passada para encontrar o número máximo de vérticesS
    while (fscanf(fp, "%d %d", &src, &dst) != EOF) {
      if (src > maxVertexId)
        maxVertexId = src;
      if (dst > maxVertexId)
        maxVertexId = dst;
    }

    vertices = maxVertexId + 1;
    rewind(fp);

    // Inicialização dos tamanhos das listas de adjacências
    edgelistSize = (int *)calloc(vertices, sizeof(int));

    // Segunda passada para contar as arestas e alocar espaço, incluindo as
    // bidirecionais
    while (fscanf(fp, "%d %d", &src, &dst) != EOF) {
      edgelistSize[src]++;
      edgelistSize[dst]++; // Considerando a adição bidirecional
    }

    // Alocação de memória para as arestas
    edgelist = (int **)malloc(vertices * sizeof(int *));
    for (int i = 0; i < vertices; i++) {
      edgelist[i] = (int *)malloc(edgelistSize[i] * sizeof(int));
    }

    countersPerVertex = (int *)calloc(vertices, sizeof(int));
    rewind(fp);
  }

  int getVertices() { return vertices; }

  int getEdgelistSize(int src) { return edgelistSize[src]; }

  int getEdge(int src, int pos) { return edgelist[src][pos]; }

  void addEdge(int src, int dst) {
    int pos = countersPerVertex[src]; // Armazena o índice atual
    edgelist[src][pos] = dst;         // Adiciona a aresta na posição correta
    countersPerVertex[src]++;         // Incrementa o índice após a adição
    // printf(" ADDEDGE: src: %d dst: %d %d\n", src, dst, edgelist[src][pos]);
    // // Usa o valor armazenado
  }

  bool isNeighbour(int src, int dst) {
    for (int i = 0; i < getEdgelistSize(src); i++) {
      if (edgelist[src][i] == dst) {
        // printf("sim");
        return true;
      }
    }
    // printf("nao");
    return false;
  }

  void release() {
    for (int i = 0; i < vertices; i++)
      free(edgelist[i]);
    free(edgelist);
    free(countersPerVertex);
    free(edgelistSize);
  }
  void printEdgelist() {
    for (int i = 0; i < getVertices(); i++) {
      for (int j = 0; j < getEdgelistSize(i); j++) {
        printf("src: %d dst: %d\n", i, getEdge(i, j));
      }
    }
  }

} Graph;

std::mutex mtx;

std::condition_variable cv;
std::vector<std::queue<std::vector<int>>> trabalhos_threads;
std::vector<int> contadores;
std::vector<sem_t> semaforos;

void processa_cliques_b(int tid, Graph *grafo, int k, int r) {
  std::vector<std::vector<int>> cliques_save;
  printf("2");
  std::vector<int> nova_clique;

  while (true) {
    std::vector<int> clique;

    {
      std::unique_lock<std::mutex> lock(mtx);
      cv.wait(lock, [&] {
        return !trabalhos_threads[tid].empty() ||
               std::all_of(trabalhos_threads.begin(), trabalhos_threads.end(), [](const auto& q) { return q.empty(); });

      });

      if (trabalhos_threads[tid].empty()) {
        // Roubo de cliques
        int max_tid = -1;
        size_t max_size = 0;
        for (int i = 0; i < trabalhos_threads.size(); ++i) {
          if (i != tid && trabalhos_threads[i].size() > max_size) {
            max_size = trabalhos_threads[i].size();
            max_tid = i;
          }
        }

        if (max_tid != -1 && max_size > 0) {
          for (int i = 0; i < r && !trabalhos_threads[max_tid].empty(); ++i) {
            trabalhos_threads[tid].push(trabalhos_threads[max_tid].front());
            trabalhos_threads[max_tid].pop();
          }
        } else {
          break; // Todas as threads estão vazias
        }
      }

      if (!trabalhos_threads[tid].empty()) {
        clique = trabalhos_threads[tid].front();
        trabalhos_threads[tid].pop();
      }
    }

    if (clique.empty())
      continue;

    if (clique.size() == k) {
      std::lock_guard<std::mutex> lock(mtx);
      if (std::find(cliques_save.begin(), cliques_save.end(), clique) ==
          cliques_save.end()) {
        contadores[tid]++;
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
            std::all_of(clique.begin(), clique.end(), [&](int v) {
              return grafo->isNeighbour(v, vizinho);
            })) {
          nova_clique = clique;
          nova_clique.push_back(vizinho);
          {
            std::lock_guard<std::mutex> lock(mtx);
            trabalhos_threads[tid].push(nova_clique);
          }
          cv.notify_all();
        }
      }
    }
  }
}

int contagem_de_cliques_b(Graph *grafo, int k, int t, int r) {
  int contagem = 0;
  printf("oi");
  std::vector<std::vector<int>> cliques;
  for (int i = 0; i < grafo->getVertices(); i++) {
    cliques.push_back({i});
  }

  trabalhos_threads.resize(t);
  contadores.resize(t, 0);
  semaforos.resize(t);

  for (int i = 0; i < t; i++) {
    sem_init(&semaforos[i], 0, 0);
  }

  int tamanho_parte = cliques.size() / t;
  for (int i = 0; i < t; i++) {
    for (int j = i * tamanho_parte; j < (i + 1) * tamanho_parte; j++) {
      trabalhos_threads[i].push(cliques[j]);
    }
  }

  std::vector<std::thread> threads;
  for (int i = 0; i < t; i++) {
    threads.push_back(std::thread(processa_cliques_b, i, grafo, k, r));
  }

  for (auto &th : threads) {
    th.join();
  }

  for (int c : contadores) {
    contagem += c;
  }

  for (int i = 0; i < t; i++) {
    sem_destroy(&semaforos[i]);
  }
  printf("check");
  return contagem;
}

void processa_cliques(std::vector<std::vector<int>> cliques, Graph *grafo,
                      int k, int &contador) {
  std::vector<std::vector<int>> cliques_save;
  std::vector<int> nova_clique;

  while (!cliques.empty()) {
    std::vector<int> clique = cliques.back();
    cliques.pop_back();

    if (clique.size() == k) {
      std::lock_guard<std::mutex> lock(mtx);
      if (std::find(cliques_save.begin(), cliques_save.end(), clique) ==
          cliques_save.end()) {
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
            std::all_of(clique.begin(), clique.end(), [&](int v) {
              return grafo->isNeighbour(v, vizinho);
            })) {
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
    std::vector<std::vector<int>> parte_cliques(
        cliques.begin() + i * tamanho_parte,
        cliques.begin() + (i + 1) * tamanho_parte);
    threads.push_back(std::thread(processa_cliques, parte_cliques, grafo, k,
                                  std::ref(contadores[i])));
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

    if (clique.size() == k) {

      if (std::find(cliques_save.begin(), cliques_save.end(), clique) ==
          (cliques_save.end())) {
        contagem++;

        cliques_save.push_back(clique);
      }

      continue;
    }

    int ultimo_vertice = clique.back();

    for (int vertice : clique) {

      // verifica os vizinhos do vertice
      for (int j = 0; j < grafo->getEdgelistSize(vertice); j++) {
        int vizinho = grafo->getEdge(vertice, j);

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
        }
      }
    }
  }

  return contagem;
}

int main(int argc, char *argv[]) {
  int k = atoi(argv[1]);
  int t = atoi(argv[2]);
 int r = atoi(argv[3]);
  printf("Valor de k: %d\n", k);
  Graph *graph = new Graph;
  int cont = 0;
  graph->initialize("graph.edgelist");
  FILE *fp = fopen("graph.edgelist", "r");
  int src = 0, dst = 0;

  while (fscanf(fp, "%d %d", &src, &dst) != EOF) {
    graph->addEdge(src, dst);
    printf("src: %d dst: %d \n", src, dst);
    if (!(graph->isNeighbour(dst, src))) {
      graph->addEdge(dst, src);
    }

    cont++;
  }
  // printf("Cliques serial: %d", contagem_de_cliques_serial(graph, k));
  // printf("Cliques paralela: %d", contagem_de_cliques_paralela(graph, k, t));
  printf("check1");
  printf("Cliques paralela balanceada: %d",contagem_de_cliques_b(graph, k, t,r));
  // graph->printEdgelist();

  fclose(fp);

  graph->release();
  free(graph);
  return 0;
}

// Salvar as arestas ida e volta na mão 1 0 e 0 1