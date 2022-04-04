#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define D 2
#define M 5
#define H1 5

float X[600][2];
float weights[H1][D];
float prev_weights[H1][D];


int random_int(int a, int b)
{
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return (int)a + r;
}


float randomFloat(float a, float b)
{
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

void read_from_file(char* filename)
{
    int pos = 0;
    int i = 0;
    FILE* fp;
    char buff[50];

    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("Error! opening %s", filename);
        exit(1);
    }

    while (fscanf(fp, "%s", buff) != EOF)
    {
        if (i % 3 == 0)
        {
            X[pos][0] = atof(buff);
        }
        else if (i % 3 == 1)
        {
            X[pos][1] = atof(buff);
        }
        else if (i % 3 == 2)
        {
            pos++;
        }
        i++;
    }
}


int* shuffling()
{
    int* array = (int*)malloc(600 * sizeof(int));
    int i, r, tmp;

    // Initialization
    for (i = 0; i < 600; i++)
    {
        array[i] = i;
    }
    // Shuffle
    for (i = 0; i < 600; i++)
    {

        r = (int)randomFloat(0, 599);
        tmp = array[i];
        array[i] = array[r];
        array[r] = tmp;
    }
	return array;
}



void initialization(int x, int y, float array[x][y])
{
    int i, j;
    for (i = 0; i < x; i++)
    {
        for (j = 0; j < y; j++)
        {
            array[i][j] = randomFloat(-1, 1);
        }
    }
}

void center_initialization()
{
    int i, index;
    for (i = 0; i < M; i++)
    {
        index = random_int(0, 599);
        weights[i][0] = X[index][0];
        weights[i][1] = X[index][1];
    }
}

int min_distance(float* nX)
{
    int j, index;
    float min = RAND_MAX;
    float dx, dy, dist;

    for (j = 0; j < M; j++)
    {
        dx = (nX[0] - weights[j][0]);
        dy = (nX[1] - weights[j][1]);
        dist = sqrt(dx * dx + dy * dy);
        if (dist < min)
        {
            min = dist;
            index = j;
        }
    }
    return index;
}


void learning_vector_quantization()
{
    int n, j, i, min;
    int epoch = 0, flag = 0;
    float learning_rate = 0.1;
    int* shuffled_indexes;
    int loop_counter = 0;               // elenxos termatismou gia ( tetragwnik oathroisma olwn twn apostasewn an ayto )
                                        // apo epoxh se apoxh allaksei poly ligo (empirika ) tote telos
                                        // xreiazomai enan pinaka pou na krataei ta kalytera kentra
                                        // ayta pou evgalan to min athroisma
    while ( flag < M  && learning_rate > 0.01)
    {
        flag = 0;
        shuffled_indexes = shuffling();
        for (j = 0; j < M; j++)
        {
            prev_weights[j][0] = weights[j][0];
            prev_weights[j][1] = weights[j][1];
        }

        for (n = 0; n < 600; n++)
        {
            i = shuffled_indexes[n];
            min = min_distance(X[i]);
            //printf("%f \n", (X[i][0] - weights[min][0] ));
            weights[min][0] = weights[min][0] + learning_rate * (X[i][0] - weights[min][0]);
            weights[min][1] = weights[min][1] + learning_rate * (X[i][1] - weights[min][1]);
            //printf("%f \n", learning_rate * (X[i][0] - weights[min][0] ));
            //printf("--------------------------------------\n");
        }

        for (j = 0; j < M; j++)
        {
            if ((weights[j][0] == prev_weights[j][0]) && (weights[j][1] == prev_weights[j][1]))
            {
                flag++;
                //printf("flag == %d\n", flag);
            }
        }

        //printf("%d\n", flag);
        learning_rate = learning_rate * 0.95;
        //printf("learning_rate == %f\n", learning_rate);
    }
    for (j = 0; j < M; j++)
    {
        //printf("%f\t%f \n", weights[j][0], weights[j][1]);
    }

	//printf("%f\t%f \n", weights[j][0], weights[j][1]);
}

    int main()
    {
        int j;
        srand(time(0));
        read_from_file("data_set_2.txt");
        initialization(H1, D , weights);
        center_initialization();
        learning_vector_quantization();
        for (j = 0; j < M; j++)
        {
            printf("%f\t%f \n", weights[j][0], weights[j][1]);
        }

        return 0;
    }
