#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm> 

using namespace std;

mutex mtx; // mutex para garantir o acesso seguro ao contador

// Estrutura básica de um Grafo
struct Grafo {
    vector<vector<int>> adj; // Lista de adjacências

    Grafo(int n) : adj(n) {}

    void addAresta(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u); // grafo não direcionado
    }

    const vector<int>& vizinhos(int v) const {
        return adj[v];
    }

    int numVertices() const {
        return adj.size();
    }
};

// Função que conta os cliques em uma parte do grafo
void contagem_de_cliques_paralela(Grafo& g, int k, int inicio, int fim, int& contador) {
    vector<vector<int>> cliques;

    // Inicializa a lista de cliques com os vértices da parte do grafo designada
    for (int v = inicio; v < fim; ++v) {
        cliques.push_back({v});
    }

    while (!cliques.empty()) {
        vector<int> clique = cliques.back();
        cliques.pop_back();

        // Se o clique já tiver k vértices, incrementa o contador
        if (clique.size() == k) {
            lock_guard<mutex> lock(mtx); // Protege a operação de incremento do contador
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
                cliques.push_back(novo_clique);
            }
        }
    }
}

// Função principal que divide o trabalho entre várias threads
int contagem_cliques(Grafo& g, int k, int num_threads) {
    int contador = 0;
    vector<thread> threads;
    int n = g.numVertices();
    int bloco = n / num_threads;

    // Cria as threads, dividindo o grafo entre elas
    for (int i = 0; i < num_threads; ++i) {
        int inicio = i * bloco;
        int fim = (i == num_threads - 1) ? n : inicio + bloco;

        threads.push_back(thread(contagem_de_cliques_paralela, ref(g), k, inicio, fim, ref(contador)));
    }

    // Aguarda todas as threads terminarem
    for (auto& t : threads) {
        t.join();
    }

    return contador;
}

int main() {
    int num_vertices = 8;
    Grafo g(num_vertices);

    // Adiciona as arestas conforme a figura 1
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
