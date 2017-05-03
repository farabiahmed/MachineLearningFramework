#include <stdio.h>

void swap(int *x1,int *x2)
{
	int x=*x1;
	*x1=*x2;
	*x2=x;
}
void per(int *arr,int st,int ls)
{
	int i=0;
	if(st==ls)
	{
		int k;
		for(k=0;k<ls;k++)
		{
			printf("%d ",arr[k]);
		}
	printf("\n");
}
	else
	{
		for(i=st;i<ls;i++)
		{
			swap(arr+st,arr+i);
			per(arr,st+1,ls);
			swap(arr+st,arr+i);
		}
	}
}


int main()
{
    int arr[4]={0,1,2,3};
    int st=0;
    int ls=3;
    per(arr,st,ls);
}
