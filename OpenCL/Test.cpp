#include<stdio.h>
#include<stdlib.h>
#include<CL/cl.h>
#include<iostream>
using namespace std;

#define MAX_SOURCE_SIZE (0x100000)

int areAllVisited(int visited[], int size)
{
	for(int i = 0; i < size; i++)
	{
		if(visited[i] == 0)
			return 0;
	}
	return 1;
}

int main()
{
	int no_of_vertices, size, source_vertex;

	//Entering the number of vertices
	cout<<"Enter the number of vertices in the graph:\n"; cin>>no_of_vertices;

	//Size of the adjacency matrix
	size = no_of_vertices * no_of_vertices;

	//Creating the Adjacency Matrix
	int* adjacency_matrix = (int*)malloc(size * sizeof(int));

	//Entering the Adjacency Matrix
	cout<<"Enter the Adjacency Matrix:\n";
	for(int i = 0; i < size; i++)
	{
		cin>>adjacency_matrix[i];
	}

	//Entering the Source Vertex
	cout<<"Enter the Source Vertex:\n";
	cin>>source_vertex;

	//Loading the Kernel code
	FILE *fp;
	char* source_str = NULL;
	size_t source_size;

	fp = fopen("bfs.cl", "r");
	if(!fp)
	{
		cout<<"Failed to load Kernel\n";
		exit(1);
	}

	source_str = (char*)malloc(MAX_SOURCE_SIZE);

	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);

	//OpenCL code
	cl_int status;

	cl_platform_id platform_id;
	clGetPlatformIDs(1, &platform_id, NULL);
	
	cl_device_id device_id;
	clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_CPU, 1, &device_id, NULL);

	cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &status);

	cl_command_queue command_queue = clCreateCommandQueue(context, device_id, NULL, &status);

	cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, size * sizeof(int), NULL, &status);
	cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size * sizeof(int), NULL, &status);

	status = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0, size * sizeof(int), adjacency_matrix, 0, NULL, NULL);

	cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source_str, (const size_t*)&source_size, &status);
	status = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	cl_kernel kernel = clCreateKernel(program, "bfs", &status);
	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&a_mem_obj);
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&b_mem_obj);
	status = clSetKernelArg(kernel, 2, sizeof(int), (void*)&source_vertex);
	status = clSetKernelArg(kernel, 3, sizeof(int), (void*)&no_of_vertices);

	size_t global_item_size = no_of_vertices;
	size_t local_item_size = 1;

	status = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

	//Creating the BFS Traversal Queue
	int *B = (int*)malloc(size * sizeof(int));

	status = clEnqueueReadBuffer(command_queue, b_mem_obj, CL_TRUE, 0, size * sizeof(int), B, 0, NULL, NULL);

	//Displaying the BFS Traversal
	cout<<"The BFS Traversal:\n";
	cout<<source_vertex;
	
	//Creating the array of visited vertices and initializing it
	int *visited = (int*)malloc(no_of_vertices * sizeof(int));
	for(int i = 0; i < no_of_vertices; i++)
	{
		visited[i] = 0;
	}

	//Removing Redundancy
	for(int i = 0; i < size; i++)
	{
		//Exit Condition
		if(areAllVisited(visited, no_of_vertices))
		{
			break;
		}

		if(B[i] != -1)
		{
			if(visited[B[i]] == 0)
			{
				cout<<"->"<<B[i];
				visited[B[i]] = 1;
			}
		}
	}

	getchar();

	//Releasing Resources
	status = clReleaseKernel(kernel);
	status = clReleaseProgram(program);
	status = clReleaseMemObject(a_mem_obj);
	status = clReleaseMemObject(b_mem_obj);
	status = clReleaseCommandQueue(command_queue);
	status = clReleaseContext(context);

	free(adjacency_matrix);
	free(B);

	getchar();
	return 0;
}
