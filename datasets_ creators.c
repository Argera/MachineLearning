#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


typedef struct Data{
    float x1;
    float x2;
    char* label;
}Data;

// Σ1
Data train_set_1[3000];
Data test_set_1[3000];

// Σ2
Data data_set_2[600];

float randomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

char* labeling(float x1, float x2) {
    float a, b, c, d;
    a = (x1-1)*(x1-1) + (x2-1)*(x2-1);
    b = (x1+1)*(x1+1) + (x2+1)*(x2+1);
    c = (x1+1)*(x1+1) + (x2-1)*(x2-1);
    d = (x1-1)*(x1-1) + (x2+1)*(x2+1);

    if( (a <= 0.49) || (b <= 0.49) ){
        return "c2";
    }else if( (c <= 0.49) || (d <= 0.49) ){
        return "c3";
    }else{
        return "c1";
    }
}


void generator_S1(){
    int i;
    for(i=0; i<3000; i++){
        // Initialize train_set_1
        train_set_1[i].x1 = randomFloat(-2,2);
        train_set_1[i].x2 = randomFloat(-2,2);
        train_set_1[i].label = labeling(train_set_1[i].x1,train_set_1[i].x2);
        // Initialize test_set_1
        test_set_1[i].x1 = randomFloat(-2,2);
        test_set_1[i].x2 = randomFloat(-2,2);
        test_set_1[i].label = labeling(test_set_1[i].x1,test_set_1[i].x2);
    }
}

void generator_S2(){
    int i=0;
    // Circle with R=0.3, C=(0,0)
    while(i<100){
        data_set_2[i].x1 = randomFloat(-0.3,0.3);
        data_set_2[i].x2 = randomFloat(-0.3,0.3);
        if (data_set_2[i].x1*data_set_2[i].x1 + data_set_2[i].x2*data_set_2[i].x2 <= 0.09){
            i++;
        }
    }
    // Squares
    for (i = 0; i < 100; i++)
    {
        // Square [-1.1,-0.5] x [0.5,1.1]
        data_set_2[i+100].x1 = randomFloat(-1.1,-0.5);
        data_set_2[i+100].x2 = randomFloat(0.5,1.1);
        // Square [-1.1,-0.5] x [-1.1,-0.5]
        data_set_2[i+200].x1 = randomFloat(-1.1,-0.5);
        data_set_2[i+200].x2 = randomFloat(-1.1,-0.5);
        // Square [0.5,1.1] x [-1.1,-0.5]
        data_set_2[i+300].x1 = randomFloat(0.5,1.1);
        data_set_2[i+300].x2 = randomFloat(-1.1,-0.5);
        // Square [0.5,1.1] x [0.5,1.1]
        data_set_2[i+400].x1 = randomFloat(0.5,1.1);
        data_set_2[i+400].x2 = randomFloat(0.5,1.1);
        // Square [-1,1] x [-1,1]
        data_set_2[i+500].x1 = randomFloat(-1,1);
        data_set_2[i+500].x2 = randomFloat(-1,1);

    }

    /* ------Visualization of data_set_2---------
    FILE *gnuplot = popen("gnuplot", "w");
    fprintf(gnuplot, "plot '-' w p pt 3\n");
    for (i = 0; i < 600; i++)
        fprintf(gnuplot, "%g %g\n", data_set_2[i].x1, data_set_2[i].x2);
    fprintf(gnuplot, "e\n");
    fflush(gnuplot);
    -----------------------------------------------*/

}

void put_noise(){
    int i;
    float random;
    for(i=0; i<3000; i++){
        if (strcmp(train_set_1[i].label,"c1")){
            random = ((float) rand()) / (float) RAND_MAX;
            if (random <= 0.1){
                train_set_1[i].label = "c1";
            }
        }
    }
}

int write_to_file(Data* dataset, char* filename, int size){
    FILE *fp;
    int i;
    if ((fp = fopen(filename, "w+")) == NULL){
        printf("Error! opening file");
        exit(1);
    }

    for(i=0; i<size ; i++){
        fprintf(fp, "%f %f %s\n", dataset[i].x1, dataset[i].x2, dataset[i].label);
    }

    fclose(fp);
    return 0;
}

int main() {
    //int x;  // Use only for Visualization
    srand(time(0));
    generator_S1();
    put_noise();
    generator_S2();
    write_to_file(test_set_1,"test_set_1.txt", 3000);
    write_to_file(train_set_1,"train_set_1.txt", 3000);
    write_to_file(data_set_2,"data_set_2.txt", 600);
    //scanf("%d\n", &x); // Use only for Visualization (for PAUSE)
    return 0;
}
