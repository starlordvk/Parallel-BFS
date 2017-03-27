__kernel void bfs(__global int* adjacency_matrix, __global int* bfs_traversal, int source_vertex, int no_of_vertices)
{
	int id = get_global_id(0);

	int i;

	if(id >= source_vertex)
	{
		for(i = 0; i < no_of_vertices; i++)
		{
			if(adjacency_matrix[id * no_of_vertices + i] == 1)
			{
				bfs_traversal[id * no_of_vertices + i] = i;
			}
			else
			{
				bfs_traversal[id * no_of_vertices + i] = -1;
			}	
		}
	}
	else
	{
		for(i = 0; i < no_of_vertices; i++)
		{
			bfs_traversal[id * no_of_vertices + i] = -1;
		}
	}
}
