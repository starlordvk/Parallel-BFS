#include<stdio.h>
#include<CL/cl.h>
#include<stdlib.h>
#include<iostream>
using namespace std;

int areAllVisited(int visited[], int size)
{
	for(int i = 0; i < size; i++)
	{
		if(visited[i] == 0)
			return 0;
	}
	return 1;
}

#define max_source_size (0x100000)
int main()
{
		int n,i;
		cout<<"\n enter the number of vertices in the graph ";
		cin>>n;
		
		int *arr=(int *)malloc(sizeof(int)*(n*n));
		cout<<"input the adjacency matrix\n";
		for(i=0;i<n*n;i++) 
			cin>>arr[i];
		
		int source;
		cout<<"\n enter the source vertex ";
		cin>>source;

		int *visited=(int*)malloc(n*sizeof(int));

		FILE*fp;
		char *source_str=(char*)malloc(max_source_size);

		size_t source_size;
		fp=fopen("bfs.cl","r");
		if(!fp)
		{
			fprintf(stderr,"Failed to load kernel\n");
			getchar();
			exit(1);
		}

		source_size=fread(source_str,1,max_source_size,fp);

		fclose(fp);

		cl_platform_id platform_id=NULL;
		cl_device_id device_id=NULL;
		cl_uint ret_num_devices;
		cl_uint ret_num_platforms;

		cl_int ret=clGetPlatformIDs(1,&platform_id,&ret_num_platforms);
		ret=clGetDeviceIDs(platform_id,CL_DEVICE_TYPE_CPU,1,&device_id,&ret_num_devices);

		cl_context context=clCreateContext(NULL,1,&device_id,NULL,NULL,&ret);

		cl_command_queue command_queue=clCreateCommandQueue(context,device_id,NULL,&ret);

		cl_mem a_mem_obj=clCreateBuffer(context,CL_MEM_READ_ONLY,(n*n)*sizeof(int),NULL,&ret);
		cl_mem b_mem_obj=clCreateBuffer(context,CL_MEM_WRITE_ONLY,(n*n)*sizeof(int),NULL,&ret);
		//cl_mem c_mem_obj=clCreateBuffer(context,CL_MEM_WRITE_ONLY,LIST_SIZE*sizeof(int),NULL,&ret);

		ret=clEnqueueWriteBuffer(command_queue,a_mem_obj,CL_TRUE,0,(n*n)*sizeof(int),arr,0,NULL,NULL);
		//ret=clEnqueueWriteBuffer(command_queue,b_mem_obj,CL_TRUE,0,LIST_SIZE*sizeof(int),B,0,NULL,NULL);
		
		cl_program program=clCreateProgramWithSource(context,1,(const char **)&source_str,(const size_t *)&source_size,&ret);

		ret=clBuildProgram(program,1,&device_id,NULL,NULL,NULL);

		cl_kernel kernel=clCreateKernel(program,"bfs",&ret);
		ret=clSetKernelArg(kernel,0,sizeof(cl_mem),(void*)&a_mem_obj);
		ret=clSetKernelArg(kernel,1,sizeof(cl_mem),(void*)&b_mem_obj);
		ret=clSetKernelArg(kernel,2,sizeof(cl_mem),(int*)&source);
		ret=clSetKernelArg(kernel,3,sizeof(cl_mem),(int*)&n);
			
		size_t global_item_size=n;
		size_t local_item_size=1;
		cl_event event;
		ret=clEnqueueNDRangeKernel(command_queue,kernel,1,NULL,&global_item_size,&local_item_size,0,NULL,NULL);

		ret=clFinish(command_queue);

		int *B=(int*)malloc(sizeof(int)*(n*n));
		
		ret=clEnqueueReadBuffer(command_queue,b_mem_obj,CL_TRUE,0,(n*n)*sizeof(int),B,0,NULL,NULL);
		cout<<"the values of adj_queue are ";
		for(i=0;i<n*n;i++) cout <<" "<<B[i];

		cout<<"\nBFS Traversal: "<<endl;
		cout<<source<<" ";
		for(int i = 0; i < n*n; i++)
		{
			//Exit Condition
			if(areAllVisited(visited, n))
			{
				break;
			}

			if(B[i] != -1)
			{
				if(visited[B[i]] == 0)
				{
					cout<<B[i]<<" ";
					visited[B[i]] = 1;
				}
			}
			else
			{
				continue;
			}
		}
		
		ret=clFlush(command_queue);
		ret=clReleaseKernel(kernel);
		ret=clReleaseProgram(program);
		ret=clReleaseMemObject(a_mem_obj);
		ret=clReleaseMemObject(b_mem_obj);
		ret=clReleaseCommandQueue(command_queue);
		ret=clReleaseContext(context);
		getchar();
		getchar();
		return 0;

}