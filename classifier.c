#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define D 2
#define K 3
#define H1 12
#define H2 9
#define F_H2 1 //  tanh:1   /   linear:0

#define BIAS_H1 D
#define BIAS_H2 H1
#define BIAS_0 H2


// Weights
float weights_H1[H1][D+1];
float weights_H2[H2][H1+1];
float weights_O[K][H2+1];

// Errors
float delta_H1[H1];
float delta_H2[H2];
float delta_O[K];

//Weights Error
float error_weights_H1[H1][D+1];
float error_weights_H2[H2][H1+1];
float error_weights_O[K][H2+1];

// Hidden Layers Outpout
float Y1[H1] = {0};
float Y2[H2] = {0};
// Final Outpout
float O[K] = {0};
// Inpout
float X[3000][2];
// Real Outpout
float T[3000][3];
// Label Coding
// C1 = {1,0,0}
// C2 = {0,1,0}
// C3 = {0,0,1}

typedef struct Data{
    float x1;
    float x2;
    char* label;
}Data;

Data train_set_1[3000];
Data test_set_1[3000];

float randomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

int* shuffling(){
    int* array = (int*) malloc (3000*sizeof(int));
    int i, r, tmp;

    // Initialization
    for(i=0; i<3000; i++){
        array[i] = i;
    }
    // Shuffle
    for(i=0; i<3000; i++){
        r = (int) randomFloat(0,2999);
        tmp = array[i];
        array[i] = array[r];
        array[r] = tmp;
    }

    return array;
}

float sigmoid(float x){
     float exp_value;
     float return_value;

     exp_value = exp((double) -x);
     return_value = 1 / (1 + exp_value);
     return return_value;
}

void read_from_file(char* filename){
    FILE* fp;
    int i=0, pos=0;
    char buff[50];
    if ((fp = fopen(filename, "r")) == NULL){
        printf("Error! opening %s" , filename);
        exit(1);
    }
    while(fscanf(fp, "%s", buff) != EOF){
        if(i%3 == 0){
            X[pos][0] = atof(buff);
        }else if(i%3 == 1){
            X[pos][1] = atof(buff);
        }else if(i%3 == 2){
            if(!(strcmp(buff,"c1"))){
                T[pos][0] = 1;
                T[pos][1] = 0;
                T[pos][2] = 0;
            }else if(!(strcmp(buff,"c2"))){
                T[pos][0] = 0;
                T[pos][1] = 1;
                T[pos][2] = 0;
            }else{
                T[pos][0] = 0;
                T[pos][1] = 0;
                T[pos][2] = 1;
            }
            pos ++;
        }
        i++;
    }
}

void initialization(int x, int y,float array[x][y]){
    int i, j;
    for(i=0; i<x; i++){
        for(j=0; j<y; j++){
            array[i][j] = randomFloat(-1, 1);
        }
    }
}

void forward_pass(float *x){
    int i,j;
    float u;
    // Hidden Layer 1
    for(i=0; i<H1; i++){
        u = weights_H1[i][BIAS_H1];
        for(j=0; j<D; j++){
            u = u + weights_H1[i][j]*x[j];
        }
        Y1[i] = sigmoid(u);
    }
    // Hidden Layer 2
    for(i=0; i<H2; i++){
        u = weights_H2[i][BIAS_H2];
        for(j=0; j<H1; j++){
            u = u + weights_H2[i][j]*Y1[j];
        }
        if(F_H2){ // tanh
            Y2[i] = tanh(u);
        }else{ // linear
            Y2[i] = u;
        }
    }
    // Outpout
    for(i=0; i<K; i++){
        u = weights_O[i][BIAS_0];
        for(j=0; j<H2; j++){
            u = u + weights_O[i][j]*Y2[j];
        }
        O[i] = sigmoid(u);
    }

}

void backprop(float *t, float *x){
    float sum;
    int i,j;

    // Outpout Layer
    for(i=0; i<K; i++){
        delta_O[i] = O[i]*(1 - O[i])*(O[i] - t[i]);
        for(j=0; j<H2; j++){
            error_weights_O[i][j] += delta_O[i]*Y2[j];
        }
        error_weights_O[i][BIAS_0] +=  delta_O[i]; // for Bias
    }

    // H2 Layer
    for(i=0; i<H2; i++){
        sum = 0;
        for(j=0; j<K; j++){
            sum = sum + weights_O[j][i]*delta_O[j];
        }
        if(F_H2){
            delta_H2[i] = (1-((Y2[i]) * ( Y2[i])))*sum; // tanh
        }else{
            delta_H2[i] = sum;  // linear
        }

        for(j=0; j<H1; j++){
            error_weights_H2[i][j] +=  delta_H2[i] * (Y1[j]);
        }
        error_weights_H2[i][BIAS_H2] +=  delta_H2[i];
    }

    // H1 Layer
    for(i=0; i<H1; i++){
        sum = 0;
        for(j=0; j<H2; j++){
            sum = sum + weights_H2[j][i]*delta_H2[j];
        }
        delta_H1[i] = Y1[i]*(1-Y1[i])*sum;

        for(j=0; j<D; j++){
            error_weights_H1[i][j] += delta_H1[i]*x[j];
        }
        error_weights_H1[i][BIAS_H1] += delta_H1[i];
    }
}

void error_weights_initilization(){
    int i, j;
    for(i=0; i<K; i++){
        for(j=0; j<=H2; j++){
            error_weights_O[i][j] = 0;
        }
    }
    for(i=0; i<H2; i++){
        for(j=0; j<=H1; j++){
            error_weights_H2[i][j] = 0;
        }
    }
    for(i=0; i<H1; i++){
        for(j=0; j<=D; j++){
            error_weights_H1[i][j] = 0;
        }
    }
}

void gradient_descent(float train_rate){
    int i, j;
    for(i=0; i<H1; i++){
        for(j=0; j<=D; j++){
            weights_H1[i][j] = weights_H1[i][j] - (train_rate * error_weights_H1[i][j]);
        }
    }
    for(i=0; i<H2; i++){
        for(j=0; j<=H1; j++){
            weights_H2[i][j] = weights_H2[i][j] - (train_rate * error_weights_H2[i][j]);
        }
    }
    for(i=0; i<K; i++){
        for(j=0; j<=H2; j++){
            weights_O[i][j] = weights_O[i][j] - (train_rate * error_weights_O[i][j]);
        }
    }
}

void training(int L, float e){
    int m, n, i, count_L = 1, count_epoch = 0;
    float prev_mse = 1, curr_mse = 1;
    float inner_sum, outter_sum;
    int* shuffled_indexes;

    while( (count_epoch < 500) || ( fabs(prev_mse - curr_mse) > e) ){
        error_weights_initilization();
        prev_mse = curr_mse;
        outter_sum = 0;
        shuffled_indexes = shuffling();
        for (n = 0; n < 3000; n++) {
            i = shuffled_indexes[n];
            forward_pass(X[i]);
            backprop(T[i],X[i]); // Inside backprop we update also the sum of weights error
            if(count_L == L){
                gradient_descent(0.001);
                error_weights_initilization();
                count_L=0;
            }

            inner_sum = 0;
            for(m = 0; m < K; m++){
                inner_sum = inner_sum + ( (T[i][m]-O[m]) * (T[i][m]-O[m]) );
            }
            outter_sum = outter_sum + ((inner_sum) / 2);
            count_L++;
        }
        curr_mse = outter_sum ;
        count_epoch++;

        printf("Epoch = %d\n",  count_epoch);
        printf("curr_mse = %f\n", curr_mse );
        printf("------------------------------------\n");

    }
}

float test(){
    //int c1, c2, c3;
    int n, i, index, correct, wrong;
    float max;
    FILE *corr_fp, *wrong_fp;

    if ((corr_fp = fopen("correct_predicts.txt", "w+")) == NULL){
        printf("Error! opening file");
        exit(1);
    }

    if ((wrong_fp = fopen("wrong_predicts.txt", "w+")) == NULL){
        printf("Error! opening file");
        exit(1);
    }

    correct = 0;
    wrong = 0;

    for(n = 0 ; n < 3000; n++){
        forward_pass(X[n]);
        max = O[0];
        index =0;
        for(i=0; i < K; i++){
            if(max < O[i]){
                max = O[i];
                index = i;
            }
        }

        if((int)T[n][index]){
            fprintf(corr_fp, "%f %f\n", X[n][0], X[n][1]);
            correct++;
        }else{
            fprintf(wrong_fp, "%f %f\n", X[n][0], X[n][1]);
            wrong++;
        }
    }

    fclose(corr_fp);
    fclose(wrong_fp);

    //--------------------------------------------------------------------------------
    FILE *gnuplot = popen("gnuplot", "w");
    fprintf(gnuplot, "plot 'correct_predicts.txt' w p pt \"+\" lc rgb \"blue\" ,\"wrong_predicts.txt\" w p pt \"x\" lc rgb \"red\" \n");
    fflush(gnuplot);
    scanf("%d\n", &i); // Use only for Visualization (for PAUSE)
    //--------------------------------------------------------------------------------

    //printf("correct == %d\nfalse == %d\n",correct, wrong);
    return (correct/(float)3000)*100;
}


int main(void){
    float rating;
    srand(time(0));
    read_from_file("train_set_1.txt");
    initialization(H1, D+1, weights_H1);
    initialization(H2, H1+1, weights_H2);
    initialization(K, H2+1, weights_O);
    training(1,0.001);

    read_from_file("test_set_1.txt");
    rating =  test();
    printf("%f %% \n",rating );

}
