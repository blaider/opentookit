/*
 * =====================================================================================
 *
 *       Filename:  stdbool.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年12月04日 23时19分48秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  
 *
 * =====================================================================================
 */

/**********************************************************
 *purpose:
 *   &nbsp; 判断一个整数是否素数。
 *method:
 *   &nbsp; 从2开始，至这个整数的平方根，若能整除其中任何一个则非素数并返回。
 ***********************************************************/

#include <stdio.h>
#include <stdbool.h>

bool isPrime(int n);

int main(void)
{

	int n;

	printf("Please enter a digit to test is it a prime or not: ");
	scanf("%d", &n);
	if (isPrime(n))
		printf("%d is a prime.\n", n);
	else
		printf("%d is not a prime.\n", n);

	return 0;
}

bool isPrime(int n)
{

	for (int i = 2; i * i < n; i++)
	{
		if (n % 2 == 0)
			return false;
	}
	return true;
}
