#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define VERTICES 5000
#define EDGES 24000

typedef struct 
{
    int src;
    int dest;
} Edge;

// Function to shuffle an array of integers
void shuffle(int* array, int size)
{
    if (size > 1) 
    {
        for (int i = 0; i < size - 1; i++) 
        {
            int j = i + rand() / (RAND_MAX / (size - i) + 1);
            int temp = array[j];
            array[j] = array[i];
            array[i] = temp;
        }
    }
}

void generateRandomGraph(Edge* edges, int numVertices, int numEdges) 
{
    srand(time(NULL));

    int maxPossibleEdges = numVertices * (numVertices - 1) / 2;
    int* allPossibleEdges = (int*)malloc(maxPossibleEdges * sizeof(int));

    int edgeCount = 0;

    for (int i = 1; i <= numVertices; i++) 
    {
        for (int j = 1; j < i; j++) 
        {
            if (i != j) 
            {
                allPossibleEdges[edgeCount] = edgeCount; // Store edge indices
                edgeCount++;
            }
        }
    }

    // Shuffle the indices
    shuffle(allPossibleEdges, maxPossibleEdges);

    for (int i = 0; i < numEdges; i++) 
    {
        int index = allPossibleEdges[i]; // Get a shuffled index
        edges[i].src = index / (numVertices - 1) + 1;
        edges[i].dest = index % (numVertices - 1) + 1;
        if (edges[i].dest >= edges[i].src) 
        {
            edges[i].dest++; // Adjust destination if needed
        }
    }

    free(allPossibleEdges);
}

int main() 
{
    Edge edges[EDGES];

    generateRandomGraph(edges, VERTICES, EDGES);

    FILE* fp = fopen("25_k_edge.txt", "w");
    if (fp == NULL) 
    {
        perror("Error opening file");
        return 1;
    }

    for (int i = 0; i < EDGES; i++) 
    {
        fprintf(fp, "%d %d\n", edges[i].src, edges[i].dest);
        fprintf(fp, "%d %d\n", edges[i].dest, edges[i].src); // Reverse edge
    }

    fclose(fp);
    printf("Generated a random graph with %d vertices and %d edges are saved.\n", VERTICES, EDGES);
    return 0;
}
