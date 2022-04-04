#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define M 5

float X[600][2];
float W[M][2];


int random_int(int a, int b)
{
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return (int) a + r;
}

void read_from_file(char* filename)
{
    int pos = 0;
    int i = 0;
    FILE* fp;
    char buff[50];

    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("Error! opening %s" , filename);
        exit(1);
    }


    while(fscanf(fp, "%s", buff) != EOF)
    {
        if(i%3 == 0)
        {
            X[pos][0] = atof(buff);
        }
        else if(i%3 == 1)
        {
            X[pos][1] = atof(buff);
        }
        else if(i%3 == 2)
        {
            pos ++;
        }
        i++;
    }

}

void center_initialization()
{
    int i, index ;
    for (i = 0; i < M; i++)
    {
        index = random_int(0,599);
        W[i][0] = X[index][0];
        W[i][1] = X[index][1];

    }

}
int min_distance(float* nX)
{
    int j, index;
    float min = RAND_MAX;
    float dx, dy, dist;

    for(j = 0; j < M; j++)
    {
        dx = (nX[0] - W[j][0]);
        dy = (nX[1] - W[j][1]);
        dist = sqrt(dx*dx + dy*dy);
        if( dist < min)
        {
            min = dist;
            index = j;
        }
    }
    return index;
}

void k_means()
{
    int j, n, tmp;
    int cluster;
    int flag = 0;
    float total_scatter = 0;
    float sumX1, sumX2;
    float new_centerX1, new_centerX2;
    float dx, dy, dist;
    int** O;
    int sizes[M];

// ==================================================================

    FILE *points_fp;
    if ((points_fp = fopen("general_points.txt", "w+")) == NULL){
        printf("Error! opening file");
        exit(1);
    }

    for (n = 0; n < 600; n++) {
        fprintf(points_fp, "%f %f\n", X[n][0], X[n][1]);
    }
    fclose(points_fp);
// ------------------------------------------------------------------
    FILE *centers_fp;

    if ((centers_fp = fopen("centroid_points.txt", "w+")) == NULL){
        printf("Error! opening file");
        exit(1);
    }
// ==================================================================

    while(flag < M-1)
    {
        flag = 0;

        O = (int**) calloc(M, sizeof(int*));
        for( j = 0; j < M; j++)
        {
            O[j]= (int*) calloc(600, sizeof(int));
            sizes[j] = 0;
        }

        for(n = 0; n < 600; n++)
        {
            cluster = min_distance(X[n]);
            O[cluster][sizes[cluster]] = n;
            sizes[cluster] ++;
        }

        for(j = 0; j < M; j++)
        {
            sumX1 = 0;
            sumX2 = 0;
            for(n = 0; n < sizes[j]; n++)
            {
                tmp = O[j][n];
                sumX1 += X[tmp][0];
                sumX2 += X[tmp][1];
            }
            new_centerX1 = sumX1 /(float) sizes[j];
            new_centerX2 = sumX2 /(float) sizes[j];

            if( (new_centerX1 == W[j][0]) && (new_centerX2 == W[j][1]) )
            {
                flag++;
            }
            W[j][0] = new_centerX1;
            W[j][1] = new_centerX2;
        }



    }

    for (j = 0; j < M; j++)
    {
        fprintf(centers_fp, "%f %f\n", W[j][0], W[j][1]); // For Visualization
    }
    fclose(centers_fp);


    // Compute Total Scatter
    for(j=0; j<M; j++)
    {
        for(n=0; n<sizes[j] ; n++)
        {
            tmp = O[j][n];
            dx = ( X[tmp][0] - W[j][0] );
            dy = ( X[tmp][1] - W[j][1] );
            dist = sqrt(dx*dx + dy*dy);
            total_scatter += dist;
        }
    }



    //--------------------------------------------------------------------------------
    FILE *gnuplot = popen("gnuplot", "w");
    fprintf(gnuplot, "plot 'centroid_points.txt' w p pt 1 lc rgb \"red\",\"general_points.txt\" w p pt 2 lc rgb \"blue\" \n");
    fflush(gnuplot);
    scanf("%d\n", &j); // Use only for Visualization (for PAUSE)
    //--------------------------------------------------------------------------------

    printf("Total Scatter = %f\n", total_scatter );

}

int main()
{
    srand(time(0));
    int j;
    read_from_file("data_set_2.txt");
    center_initialization();
    k_means();
    for ( j = 0; j < M; j++)
        {
            printf("%f\t%f \n", W[j][0], W[j][1]);
        }

    return 0;
}
