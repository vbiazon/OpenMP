// calcula aproximacoes da integral de e^(-x^2) no intervalo de 0 a 1.

#include <iostream>
#include <math.h>
#include <omp.h>
using namespace std;

float Funcao(float x) //calcula o valor da funcao no ponto x
{
	return exp(-pow(x,2));
}

float Derivada2(float x) //calcula o valor da segunda derivada da funcao no ponto x
{
	return exp(-pow(x, 2))*(4*pow(x,2)-2);
}

float Derivada4(float x) //calcula o valor da quarta derivada da funcao no ponto x
{
	return 4* exp(-pow(x, 2)) * (4 * pow(x, 4) - 12 * pow(x, 2) + 3);
}

float Retangular(float a, float b) //aproximacao pelo metodo retangular (ponto médio)
{
	return (b - a) * Funcao((a + b) / 2);
}

float RetangularErro(float a, float b) //erro estimado da aproximacao pelo metodo retangular (ponto médio)
{
	return -((pow((b - a), 3) / 24) * Derivada2(a + ((b - a) / 2)));
}

float Trapezio(float a, float b) //aproximacao pelo metodo dos trapezios
{
	return (b - a) * ((Funcao(a) + Funcao(b)) / 2);
}

float TrapezioErro(float a, float b) //erro estimado da aproximacao pelo metodo dos trapezios
{
	return -((pow((b - a), 3) / 12) * Derivada2(a + ((b - a) / 2)));
}

float Simpson(float a, float b) //aproximacao pelo metodo de Simpson
{
	return (b - a) * ((Funcao(a) + (4 * Funcao((a + b) / 2)) + Funcao(b)) / 6);
}

float SimpsonErro(float a, float b) //erro estimado da aproximacao pelo metodo de Simpson
{
	return -((pow((b - a), 5) / 2880) * Derivada4(a + ((b - a) / 2)));
}

float QuadraturaAdaptativa(float a, float b, float erro) { //aproximacao pelo metodo da quadratura adaptativa
	float Q = Simpson(a, b); //calcula a aproximacao pelo metodo de Simpson
	float erroQ = SimpsonErro(a, b); //calcula o erro da aproximacao

	if (abs(erroQ) > erro) //Verifica se o erro atual da aproximacao é menor que o erro desejado, se for maior executa
	{
		float pm = (a + b) / 2; //encontra ponto medio entre os dois intervalos de integracao
		float Q1 = QuadraturaAdaptativa(a, pm, erro); //calcula aproximacao do primeiro intervalor até o ponto medio
		float Q2 = QuadraturaAdaptativa(pm, b, erro); //calcula aproximacao do ponto medio até o segundo intervalo
		Q = Q1 + Q2; //soma aproximacoes parciais
	}
	return Q; //retorna valor da aproximacao
}

float IntegralNumericaIterativa (float a, float b, float erro){
    float n = 2;
    float intervalo = 0;  
    float totalIntegral = 0;
    float totalerro = 0;
    do {
        intervalo = (a + b)/n;
        totalerro = 0;
        totalIntegral = 0;
        for (int contador = 0; contador < n; contador++){
        float a1 = contador * intervalo;
        float b1 = (contador + 1) * intervalo;
        totalIntegral += Simpson(a1, b1); //calcula a aproximacao pelo metodo de Simpson
        totalerro += SimpsonErro(a1, b1); //calcula o erro da aproximacao
        
        }
        cout << "Integral 1 thread: " << totalIntegral << " erro: " << totalerro << " numero de divisoes: " << n << endl;
         n *= 2;
    }
    while (abs(totalerro) > erro);
}
int tid;
float IntegralNumericaIterativaOMP (float a, float b, float erro){
    int n = 2048;
    float intervalo = 0;  
    float totalIntegral = 0;
    float totalerro = 0;
    int contador = 0;
    
    do {
        // n *= 2;
        intervalo = (a + b)/n;
        totalerro = 0;
        totalIntegral = 0;
        #pragma omp parallel for shared(contador) private(tid)
        for (contador = 0; contador < n; contador++){
        tid = omp_get_thread_num();
        float a1 = contador * intervalo;
        float b1 = (contador + 1) * intervalo; 
        //printf("TID: %d Contador: %d a1: %f b1: %f \n", tid, contador, a1, b1);
        
        totalIntegral += Simpson(a1, b1); //calcula a aproximacao pelo metodo de Simpson
        totalerro += SimpsonErro(a1, b1); //calcula o erro da aproximacao
        
        }
               
    }
    while (false);//(abs(totalerro) > erro);
    cout << "Integral OpenMP: " << totalIntegral << " erro: " << totalerro << " numero de divisoes: " << n << endl;
}

int main()
{
	float a = 0; //delimita o intervalo
	float b = 1;

	float erro = 0.0000000000000001; //determina o erro de aproximacao desejado para uso na quadratura adaptativa

	float metodo1 = Retangular(a, b); //realiza calculo das aproximacoes
	float metodo2 = Trapezio(a, b);
	float metodo3 = Simpson(a, b);

	float erro1 = RetangularErro(a, b); //realiza calculo dos erros aproxiamdos
	float erro2 = TrapezioErro(a, b);
	float erro3 = SimpsonErro(a, b);

	float QA = QuadraturaAdaptativa(a, b, erro); //calcula integral pelo metodo da quadratura adaptativa

    //configura o OMP
    int nprocs = omp_get_num_procs();
    omp_set_num_threads(nprocs);

	//mostra valores no prompt

	cout << "Para a funcao e^(-x^2) o calculos resultam em: " << endl;

	cout << "Metodo 1(retangular): " << metodo1 << endl;
	cout << "Erro 1: " << erro1 << endl;
	cout << "Metodo 2(trapezios): " << metodo2 << endl;
	cout << "Erro 2: " << erro2 << endl;
	cout << "Metodo 3(Simpson): " << metodo3 << endl;
	cout << "Erro 3: " << erro3 << endl;

	cout << "Metodo 4(quadratura adaptativa) :" << QA << endl;

    IntegralNumericaIterativa(a, b, erro);
    IntegralNumericaIterativaOMP(a, b, erro);
    cout << "Numero de threads: " << nprocs << endl;
	cin.get();
}