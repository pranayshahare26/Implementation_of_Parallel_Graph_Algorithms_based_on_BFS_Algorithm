#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <mpi.h>

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
void parallel_bfs(struct Graph* graph, int startVertex, int numVertices, int rank, int size, int* edgesTraversed) 
{
    struct Queue* queue = createQueue();
    bool* visited = (bool*)malloc(numVertices * sizeof(bool));
    for (int i = 0; i < numVertices; i++) 
    {
	    visited[i] = false;
    }

    visited[startVertex] = true;
    enqueue(queue, startVertex);

    int localEdgesTraversed = 0; // Track edges traversed by this process

    while (!isEmpty(queue)) 
    {
	
	int currentVertex = dequeue(queue);
	printf("Process %d: %d\n", rank, currentVertex + 1);

	// Distribute the work between processes
	int chunkSize = numVertices / size;
	int start = rank * chunkSize;
	int end = (rank == size - 1) ? numVertices : (rank + 1) * chunkSize;

	for (int v = start; v < end; v++) 
	{
	    if (!visited[v]) 
	    {
		visited[v] = true;
		enqueue(queue, v);
		localEdgesTraversed++; // Count each edge traversal
	    }
	}
    }

    // Reduce to get the total edges traversed by all processes
    MPI_Allreduce(&localEdgesTraversed, edgesTraversed, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    free(visited);
}

int main(int argc, char* argv[]) 
{
    int numVertices = N;
    struct Graph* graph = createGraph(numVertices);
    FILE* file;
    int source, destination;
    double exe_time;
    struct timeval stop_time, start_time;

    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Variables to store individual process execution times
    double process_exe_time = 0.0;

    // Variable to store total edges traversed by all processes
    int totalEdgesTraversed = 0;

    if ((file = fopen("10_k_edge.txt", "r"))) 
    {
        while (fscanf(file, "%d %d", &source, &destination) == 2) 
        {
            addEdge(graph, source - 1, destination - 1);
            addEdge(graph, destination - 1, source - 1); // Since it's an undirected graph
        }

        gettimeofday(&start_time, NULL);

        parallel_bfs(graph, 0, numVertices, rank, size, &totalEdgesTraversed);

        gettimeofday(&stop_time, NULL);
        exe_time = (stop_time.tv_sec + (stop_time.tv_usec / 1000000.0)) - (start_time.tv_sec + (start_time.tv_usec / 1000000.0));

        // Gather execution times from all processes
        MPI_Reduce(&exe_time, &process_exe_time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    }
    else
    {
        printf("Error in reading file\n");
    }

    MPI_Finalize();

    // Calculate average execution time on rank 0
    if (rank == 0) 
    {
        double avg_exe_time = process_exe_time / size;
        printf("Average Execution time is = %lf seconds\n", avg_exe_time);
        
        // Calculate TEPS (Traversed Edges Per Second)
        double TEPS = (double)totalEdgesTraversed / avg_exe_time;
        printf("TEPS (Traversed Edges Per Second) is = %lf\n", TEPS);
    }
    return 0;
}
