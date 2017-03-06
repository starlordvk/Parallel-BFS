#include<mpi.h>
#include<stdio.h>
#include<vector>
#include<iostream>
using namespace std;

#define MAX_SIZE 10

//int find(int arr[], int item, int size)
int main(int argc, char* argv[])
{
	int size = 0, rank = 0, no_of_vertices = 0, A[100] = {0}, b[100] = {0}, Q[100] = {0}, R[500] = {0}, R1[500] = {0}, source_vertex = -1, ind = 0, c[100] = {0};
	MPI_Init(&argc, &argv);
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(rank == 0)
	{
		//Entering the number of vertices
		cout<<"Enter the number of vertices: "; cin>>no_of_vertices;

		//Entering the Adjacency Matrix
		cout<<"Enter the Adjacency Matrix\n";
		for(int i = 0; i < no_of_vertices * no_of_vertices; i++)
		{
			cin>>A[i];
		}
		cout<<endl;
		cout<<endl;

		cout<<"Enter the Source Vertex: "; cin>>source_vertex;
		cout<<endl;
	}
	
	//Broadcasting the Number of vertices
	MPI_Bcast(&no_of_vertices, 1, MPI_INT, 0, MPI_COMM_WORLD);

	//Scattering the Adjacency Matrix evenly across the processes
	MPI_Scatter(A, no_of_vertices, MPI_INT, b, no_of_vertices, MPI_INT, 0, MPI_COMM_WORLD);

	//Finding the Adjacent vertices of the concerned vertex
	for(int i = 0; i < MAX_SIZE; i++)
	{
		if(i < no_of_vertices)
		{
			if(b[i] == 1)
				Q[i] = i;
			else
				Q[i] = -1;
		}
		else if(i >= no_of_vertices)
			Q[i] = -1;
	}

	/*for(int i = 0; i < MAX_SIZE; i++)
		cout<<Q[i]<<" ";
	cout<<endl;*/

	//Gathering the Found Adjacent vertices to the Resultant array in the root
	MPI_Gather(Q, MAX_SIZE, MPI_INT, R, MAX_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

	//Displaying the Resultant Array
	if(rank == 0)
	{
		/*for(int i = 0; i < MAX_SIZE * no_of_vertices; i++)
			cout<<R[i]<<" ";
		cout<<endl;*/

		//Inititializing the Visited Array
		int* visited = (int*)malloc(no_of_vertices * sizeof(int));
		for(int j = 0; j < no_of_vertices; j++)
			visited[j] = 0;
		
		//Displaying the Source Vertex
		visited[source_vertex] = 1;

		//Eliminating the delimiters and redundancy and storing into the final array
		int i = 0;
		while(i < MAX_SIZE * no_of_vertices)
		{
			if(R[i] == -1)
				i++;
			else if(R[i] != -1)
			{
				R1[ind++] = R[i];
				i++;
			}
		}

		/*for(int i = 0; i < ind; i++)
			cout<<R1[i]<<" ";
		cout<<endl;*/

		//Displaying the Final Resultant Array
		vector<int> queue;
		queue.push_back(source_vertex);
		cout<<queue.front()<<" ";
		while(!queue.empty())
		{
			for(int j = 0; j < no_of_vertices; j++)
			{
				if(A[queue.front() * no_of_vertices + j] == 1 && visited[j] == 0)
					queue.push_back(j);
			}
			int k = queue.front();
			//queue.clear();
			for(int i = 0; i < ind; i++)
			{
				if(k == R1[i] && visited[k] == 0)
				{
					visited[R1[i]] = 1;
					cout<<k<<" ";
					break;
				}
			}
			queue.erase(queue.begin());
		}
	}
	MPI_Finalize();
	return 0;
}