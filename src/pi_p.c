/*
 *  Calculo de Pi usando integracion numerica
 *  Version paralela - multiples hilos usando Pthreads
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

// Estructura para pasar informacion a cada hilo
// Cada hilo necesita saber que rango de iteraciones debe calcular
typedef struct {
    int start;              // Indice donde empieza el rango de este hilo
    int end;                // Indice donde termina el rango de este hilo (no incluido)
    int n;                  // Numero total de intervalos
    double partial_sum;     // Suma parcial que calcula este hilo
} thread_args_t;

// Declaraciones de funciones
double CalcPi(int n, int num_threads);
double GetTime(void);
double f(double a);
void* thread_calc_pi(void* arg);

int main(int argc, char **argv)
{
    // Numero de intervalos para la integracion
    int n = 2000000000;
    
    // Numero de hilos a usar, por defecto 1
    int num_threads = 1;
    
    // Valor real de pi para comparar el error
    const double fPi25DT = 3.141592653589793238462643;
    
    // Variables para almacenar el resultado y medir tiempo
    double fPi;
    double fTimeStart, fTimeEnd;
    
    // Leer el numero de hilos desde los argumentos de la linea de comandos
    // El primer argumento es el numero de hilos
    if (argc >= 2) {
        num_threads = atoi(argv[1]);
        if (num_threads <= 0) {
            printf("Error: El numero de hilos debe ser mayor que 0\n");
            return 1;
        }
    }
    
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

    // Aqui se hace el calculo principal de pi usando multiples hilos
    fPi = CalcPi(n, num_threads);

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

// Funcion que ejecuta cada hilo para calcular su parte del trabajo
// Cada hilo calcula la suma parcial de un rango de iteraciones
void* thread_calc_pi(void* arg)
{
    // Convertir el argumento generico a nuestra estructura
    thread_args_t* args = (thread_args_t*)arg;
    
    // Altura de cada rectangulo (igual para todos los hilos)
    const double fH = 1.0 / (double)args->n;
    
    // Variable local para acumular la suma parcial de este hilo
    // Es local para evitar problemas de sincronizacion
    double fSum = 0.0;
    
    // Variable temporal para el valor de x en cada iteracion
    double fX;
    
    // Contador del bucle
    int i;
    
    // Bucle que recorre solo el rango asignado a este hilo
    // El hilo calcula desde args->start hasta args->end
    for (i = args->start; i < args->end; i++)
    {
        // Calcular el punto medio del intervalo i
        fX = fH * ((double)i + 0.5);
        
        // Evaluar la funcion en ese punto y sumar al total parcial
        fSum += f(fX);
    }
    
    // Guardar el resultado parcial en la estructura
    // Esto permite que el hilo principal recupere el resultado
    args->partial_sum = fSum;
    
    // Terminar el hilo
    pthread_exit(NULL);
}

// Funcion paralela que calcula pi usando multiples hilos
// Divide el trabajo entre varios hilos y luego combina los resultados
double CalcPi(int n, int num_threads)
{
    // Altura de cada rectangulo
    const double fH = 1.0 / (double)n;
    
    // Arreglo para almacenar los identificadores de los hilos
    pthread_t* threads;
    
    // Arreglo para almacenar los argumentos de cada hilo
    thread_args_t* thread_args;
    
    // Variables de control
    int i;
    int chunk_size;
    
    // Variable para acumular la suma total de todos los hilos
    double total_sum = 0.0;
    
    // Reservar memoria para los identificadores de hilos
    threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    
    // Reservar memoria para los argumentos de cada hilo
    thread_args = (thread_args_t*)malloc(num_threads * sizeof(thread_args_t));
    
    // Verificar que se pudo asignar la memoria
    if (threads == NULL || thread_args == NULL) {
        printf("Error: No se pudo asignar memoria\n");
        exit(1);
    }
    
    // Calcular cuantas iteraciones le tocan a cada hilo
    // Dividimos el trabajo total entre el numero de hilos
    chunk_size = n / num_threads;
    
    // Crear cada hilo y asignarle su rango de trabajo
    for (i = 0; i < num_threads; i++) {
        // Calcular donde empieza el rango de este hilo
        thread_args[i].start = i * chunk_size;
        
        // Calcular donde termina el rango de este hilo
        // El ultimo hilo se queda con las iteraciones sobrantes
        thread_args[i].end = (i == num_threads - 1) ? n : (i + 1) * chunk_size;
        
        // Pasar el numero total de intervalos
        thread_args[i].n = n;
        
        // Inicializar la suma parcial en cero
        thread_args[i].partial_sum = 0.0;
        
        // Crear el hilo y darle la funcion thread_calc_pi para que ejecute
        // Le pasamos los argumentos de este hilo
        if (pthread_create(&threads[i], NULL, thread_calc_pi, &thread_args[i]) != 0) {
            printf("Error: No se pudo crear el hilo %d\n", i);
            exit(1);
        }
    }
    
    // Esperar a que todos los hilos terminen su trabajo
    // Esto es importante para asegurar que todos terminaron antes de combinar resultados
    for (i = 0; i < num_threads; i++) {
        // pthread_join espera a que el hilo termine
        pthread_join(threads[i], NULL);
        
        // Una vez que el hilo termino, sumar su resultado parcial al total
        total_sum += thread_args[i].partial_sum;
    }
    
    // Liberar la memoria que reservamos
    free(threads);
    free(thread_args);
    
    // Multiplicar la suma total por el ancho de cada rectangulo
    // Esto da la aproximacion final de la integral que es pi
    return fH * total_sum;
}
