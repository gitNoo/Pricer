#include "sort.h"
#include <stdio.h>

void main()
{
	int n;
	int *a;
	printf("Nhap n: ");
	scanf("%d",&n);

	a = malloc(n*sizeof(int));
	for(int i = 0; i < n; ++i)
		scanf("%d",&a[i]);

	selectionSort(a,n);
	printArr(a,n);
}