/*************************************************************************
	> File Name: dp.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Wed 13 Sep 2017 02:07:41 PM CST
 ************************************************************************/

#include <stdio.h>

typedef struct test_str
{
	int i;
}TEST_STR;

TEST_STR g1 = {1};
TEST_STR g2 = {2};

int main()
{
	unsigned char c[8] = {1,9,8,7,6,5,4,3};
	int a = 1;
	int b = 2;

	TEST_STR ** array;

	array = (TEST_STR ** )c;

	printf("c addr= %p,value=%d\n",c,*c);
	printf("array addr= %p,value=%x\n",array,*array);
	printf("array[0] = %x\n",array[0]);
	printf("array[1] = %x\n",array[1]);



	array[0] = &g1;
	printf("array addr= %p,value=%x\n",array,*array);
	printf("array[0] = %x\n",array[0]);
//	array[1] = &g1;
//	array[2] = &g1;
//	array[3] = &g2;

	printf("1\n");
//	array[5] = &g2;

//	printf("array[5]->i = %p\n",array[1]);
//
//	printf("a = %d, b = %d \n",a,b);
//	printf("a = %p array[1] = %p",&a,array[1]);

	return 0;


}


