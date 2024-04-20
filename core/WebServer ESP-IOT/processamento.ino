//#include <iostream>
#include <cmath>
#include <bits/stdc++.h>
using namespace std;

// importar os dados 

// sinal = [] dados importados; 

// cálculo da frequencia mediana 



// calculo do valor rms 
 
float rmsValue(int sinal[], int n)
{
    int square = 0;
    float mean = 0.0, rms = 0.0;
 
    // Calculate square.
    for (int i = 0; i < n; i++) {
        square += pow(sinal[i], 2);
    }
 
    // Calculate Mean.
    mean = (square / (float)(n));
 
    // Calculate Root.
    rms = sqrt(mean);
 
    return rms;
}
 

int main()
{
    //int sinal[] = { 10, 4, 6, 8 };
    int n = sizeof(sinal) / sizeof(sinal[0]);
 
    cout << rmsValue(sinal, n);
 
    return 0;
}