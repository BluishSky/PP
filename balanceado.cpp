#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <algorithm> 

using namespace std;

mutex mtx;                 // Mutex para sincronização de threads
queue<vector<int>> fila;    // Fila compartilhada entre as threads
int contador = 0;           // Contador de cliques globais

// Estrutura básica de um Grafo
struct Grafo {
    vector<vector<int>> adj; // Lista de adjacências

    Grafo(int n) : adj(n) {}

    void addAresta(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u); // Grafo não direcionado
    }

    const vector<int>& vizinhos(int v) const {
        return adj[v];
    }

    int numVertices() const {
        return adj.size();
    }
};

// Função para processar cliques a partir da fila de trabalho
void processa_cliques(Grafo& g, int k) {
    while (true) {
        vector<int> clique;

        // Protege o acesso à fila usando mutex
        {
            lock_guard<mutex> lock(mtx);
            if (fila.empty()) {
                return;  // Se a fila estiver vazia, a thread termina
            }
            clique = fila.front();
            fila.pop();
        }

        // Se o clique já tiver k vértices, incrementa o contador
        if (clique.size() == k) {
            lock_guard<mutex> lock(mtx); // Protege o contador
            contador++;
            continue;
        }

        // Último vértice do clique atual
        int ultimo_vertice = clique.back();

        // Expande o clique
        for (int vizinho : g.vizinhos(ultimo_vertice)) {
            if (find(clique.begin(), clique.end(), vizinho) == clique.end()) {
                vector<int> novo_clique = clique;
                novo_clique.push_back(vizinho);

                // Adiciona o novo clique na fila
                lock_guard<mutex> lock(mtx);
                fila.push(novo_clique);
            }
        }
    }
}

// Função principal para configurar e iniciar o balanceamento de carga com threads
int contagem_cliques(Grafo& g, int k, int num_threads) {
    // Inicializa a fila de cliques com um vértice para cada vértice do grafo
    for (int v = 0; v < g.numVertices(); ++v) {
        fila.push({v});
    }

    // Cria as threads
    vector<thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.push_back(thread(processa_cliques, ref(g), k));
    }

    // Espera todas as threads terminarem
    for (auto& t : threads) {
        t.join();
    }

    return contador;
}

int main() {
    int num_vertices = 8;
    Grafo g(num_vertices);

    // Adiciona as arestas conforme a figura 1 (exemplo)
    g.addAresta(0, 1);
    g.addAresta(0, 5);
    g.addAresta(0, 6);
    g.addAresta(1, 2);
    g.addAresta(1, 5);
    g.addAresta(2, 3);
    g.addAresta(3, 4);
    g.addAresta(4, 5);
    g.addAresta(4, 6);
    g.addAresta(4, 7);
    g.addAresta(5, 6);

    int k = 3; // Tamanho dos cliques a serem contados
    int num_threads = 4; // Número de threads

    int total_cliques = contagem_cliques(g, k, num_threads);
    cout << "Total de cliques de tamanho " << k << ": " << total_cliques << endl;

    return 0;
}
