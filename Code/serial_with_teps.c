#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>

#define N 1000

// Define a structure for a node in the adjacency list
struct Node 
{
    int data;
    struct Node* next;
};

// Define a structure for the graph
struct Graph 
{
    int numVertices;
    struct Node** adjacencyLists;
};

// Create a new node for the adjacency list
struct Node* createNode(int data) 
{
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

// Create a graph with a given number of vertices
struct Graph* createGraph(int numVertices) 
{
    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
    graph->numVertices = numVertices;
    graph->adjacencyLists = (struct Node**)malloc(numVertices * sizeof(struct Node*));
    for (int i = 0; i < numVertices; i++) 
    {
        graph->adjacencyLists[i] = NULL;
    }
    return graph;
}

// Add an edge to the graph
void addEdge(struct Graph* graph, int src, int dest) 
{
    struct Node* newNode = createNode(dest);
    newNode->next = graph->adjacencyLists[src];
    graph->adjacencyLists[src] = newNode;
}

// Queue data structure for BFS
struct Queue 
{
    int* items;
    int front;
    int rear;
};

// Create a new queue
struct Queue* createQueue() 
{
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->items = (int*)malloc(N * sizeof(int));
    queue->front = -1;
    queue->rear = -1;
    return queue;
}

// Check if the queue is empty
bool isEmpty(struct Queue* queue) 
{
    return queue->front == -1;
}

// Enqueue an item
void enqueue(struct Queue* queue, int item) 
{
    if (queue->rear == N - 1) 
    {
        printf("Queue is full.\n");
    } 
    else 
    {
        if (queue->front == -1) 
        {
            queue->front = 0;
        }
        queue->rear++;
        queue->items[queue->rear] = item;
    }
}

// Dequeue an item
int dequeue(struct Queue* queue) 
{
    int item;
    if (isEmpty(queue)) 
    {
        printf("Queue is empty.\n");
        return -1;
    }
    else
    {
        item = queue->items[queue->front];
        queue->front++;
        if (queue->front > queue->rear) 
        {
            queue->front = queue->rear = -1;
        }
        return item;
    }
}

// Breadth-First Search
void serial_bfs(struct Graph* graph, int startVertex) 
{
    struct Queue* queue = createQueue();
    bool* visited = (bool*)malloc(N * sizeof(bool));
    for (int i = 0; i < N; i++) 
    {
        visited[i] = false;
    }

    visited[startVertex] = true;
    enqueue(queue, startVertex);

    while (!isEmpty(queue)) 
    {
        int currentVertex = dequeue(queue);
        printf("%d ", currentVertex + 1);

        struct Node* temp = graph->adjacencyLists[currentVertex];
        while (temp != NULL) 
        {
            int adjacentVertex = temp->data;
            if (!visited[adjacentVertex]) 
            {
                visited[adjacentVertex] = true;
                enqueue(queue, adjacentVertex);
            }
            temp = temp->next;
        }
    }

    free(visited);
}

int main() 
{
    struct Graph* graph = createGraph(N);
    FILE* file;
    int source, destination;
    double exe_time;
    struct timeval stop_time, start_time;

    if ((file = fopen("10_k_edge.txt", "r"))) 
    {
        while (fscanf(file, "%d %d", &source, &destination) == 2) 
        {
            addEdge(graph, source - 1, destination - 1);
            addEdge(graph, destination - 1, source - 1); // Since it's an undirected graph
        }

        gettimeofday(&start_time, NULL);
        
        serial_bfs(graph, 0);
    } 
    else 
    {
        printf("Error in reading file\n");
    }
    printf("\n");

    gettimeofday(&stop_time, NULL);
    
    exe_time = (stop_time.tv_sec + (stop_time.tv_usec / 1000000.0)) - (start_time.tv_sec + (start_time.tv_usec / 1000000.0));

    // Calculate TEPS (Traversed Edges Per Second)
    double TEPS = (2.0 * N - 1.0) / exe_time;

    printf("Execution time is = %lf seconds\n", exe_time);
    printf("TEPS (Traversed Edges Per Second) is = %lf\n", TEPS);

    return 0;
}
