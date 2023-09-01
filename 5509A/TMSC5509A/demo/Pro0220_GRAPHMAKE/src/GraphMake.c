#include "stdio.h"

#define N 		(81)
#define M 		(81)
#define PI 		(3.1415926)


int main()
{
	int i, j, k;
	int y[M][N];
	double x[(N - 1)/2], r;
	int m, n, t;
	while (1)
	{
		for(t = 0; t <= 32; t++)
		{
			for(i = 0; i <= (N - 1)/2; i++)
			{
				x[i] = 127 * (1 + cos(2*PI*i/(N - 1)/2 - t*PI/16));
			}
		}

		for(m = 0; m <= (M - 1); m++)
		{
			for(n = 0; n <= N - 1; n++)
			{
				r = (m - 40) * (m - 40) + (n - 40) * (n - 40);
				k = (int)(sqrt(r));
				if(k <= 39)
				{
					y[m][n] = (int)x[k];
				}
				else
				{
					y[m][n] = 0;
				}
			}
		}
	}
}

