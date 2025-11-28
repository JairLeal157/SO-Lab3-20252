/*
 *  Calculo de Pi usando integracion numerica
 *  Version serial - un solo hilo
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>

// Declaraciones de funciones
double CalcPi(int n);
double GetTime(void);
double f(double a);

int main(int argc, char **argv)
{
    // Numero de intervalos para la integracion
    // Entre mas intervalos, mas preciso el resultado pero mas lento
    int n = 2000000000;
    
    // Valor real de pi para comparar el error
    const double fPi25DT = 3.141592653589793238462643;
    
    // Variables para almacenar el resultado y medir tiempo
    double fPi;
    double fTimeStart, fTimeEnd;
    
    // Si se define READ_INPUT, el programa pide el numero de intervalos
#ifdef READ_INPUT  
    printf("Enter the number of intervals: ");
    scanf("%d",&n);
#endif

    // Validar que el numero de intervalos sea valido
    if (n <= 0 || n > 2147483647 ) 
    {
        printf("\ngiven value has to be between 0 and 2147483647\n");
        return 1;
    }
    
    // Tomar el tiempo antes de empezar el calculo
    fTimeStart = GetTime();

    // Aqui se hace el calculo principal de pi
    fPi = CalcPi(n);

    // Tomar el tiempo despues de terminar el calculo
    fTimeEnd = GetTime();
    
    // Mostrar el resultado aproximado de pi y el error respecto al valor real
    printf("\npi is approximately = %.20f \nError               = %.20f\n",
           fPi, fabs(fPi - fPi25DT));
    
    // Mostrar cuanto tiempo tardo el calculo
    printf("Time elapsed: %.6f seconds\n", fTimeEnd - fTimeStart);

    return 0;
}

// Funcion para obtener el tiempo actual del sistema
// Retorna el tiempo en segundos con precision de microsegundos
double GetTime(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

// Funcion matematica que se integra para calcular pi
// Esta es la funcion 4/(1+x^2) que al integrarse de 0 a 1 da pi
double f(double a)
{
    return (4.0 / (1.0 + a*a));
}

// Funcion principal que calcula pi usando integracion numerica
// Divide el intervalo de 0 a 1 en n rectangulos y suma sus areas
double CalcPi(int n)
{
    // Altura de cada rectangulo (ancho del intervalo dividido entre n)
    const double fH = 1.0 / (double) n;
    
    // Variable para acumular la suma de todas las areas
    double fSum = 0.0;
    
    // Variable temporal para el valor de x en cada iteracion
    double fX;
    
    // Contador del bucle
    int i;

    // Bucle principal que recorre todos los intervalos
    // Para cada intervalo calcula el area de un rectangulo y la suma
    for (i = 0; i < n; i += 1)
    {
        // Calcular el punto medio del intervalo i
        // El punto medio esta en (i + 0.5) * fH
        fX = fH * ((double)i + 0.5);
        
        // Evaluar la funcion en ese punto y sumar al total
        // Esto calcula el area del rectangulo en ese intervalo
        fSum += f(fX);
    }
    
    // Multiplicar la suma por el ancho de cada rectangulo para obtener el area total
    // Esta area total es la aproximacion de la integral que da pi
    return fH * fSum;
}
