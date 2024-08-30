#include <stdio.h>
#include <stdlib.h>
#include <cstring>

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

int main() {
    Graph* graph = new Graph;
    graph->initialize("graph.edgelist");

    FILE* fp = fopen("graph.edgelist", "r");

    int src, dst;
    while(fscanf(fp, "%d %d", &src, &dst) != EOF) {       
        graph->addEdge(src,dst);        
    }

    fclose(fp);

    for(int i = 0 ; i < graph->getVertices() ; i++) {
        for(int j = 0 ; j < graph->getEdgelistSize(i) ; j++) {
            printf("%d %d\n", i, graph->getEdge(i, j));
        }
    }

    graph->release();

    return 0;
}