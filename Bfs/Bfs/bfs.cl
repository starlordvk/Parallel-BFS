__kernel void bfs(__global int*mat, __global int*adja_queue, int source, int vertices)
{
	int id=get_global_id(0);
	if(id>=source)
	{
		for(int i=0;i<vertices;i++)
		{
			if(mat[id*vertices+i]==1)
			{
				adja_queue[id*vertices+i]=i;
			}

			else
			{
				adja_queue[id*vertices+i]=-1;
			}
	}

}