/*
 *  Generador de Secuencia de Fibonacci usando hilos
 *  El hilo principal crea un hilo trabajador que calcula la secuencia
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Estructura para pasar datos al hilo trabajador
// Contiene el puntero al arreglo compartido y el numero de elementos
typedef struct {
    long long* array;      // Puntero al arreglo donde se guardaran los numeros
    int n;                  // Numero de elementos de Fibonacci a generar
} fibonacci_args_t;

// Declaraciones de funciones
void* fibonacci_worker(void* arg);

int main(int argc, char **argv)
{
    int n;
    long long* fib_array;
    pthread_t worker_thread;
    fibonacci_args_t args;
    int i;
    
    // Verificar que se paso el argumento con el numero de elementos
    if (argc < 2) {
        printf("Uso: %s <numero_de_elementos>\n", argv[0]);
        printf("Ejemplo: %s 10\n", argv[0]);
        return 1;
    }
    
    // Leer el numero de elementos desde la linea de comandos
    n = atoi(argv[1]);
    
    // Validar que el numero sea positivo
    if (n <= 0) {
        printf("Error: El numero de elementos debe ser mayor que 0\n");
        return 1;
    }
    
    // Reservar memoria dinamica para el arreglo compartido
    // El arreglo tendra n elementos de tipo long long
    fib_array = (long long*)malloc(n * sizeof(long long));
    
    // Verificar que se pudo asignar la memoria
    if (fib_array == NULL) {
        printf("Error: No se pudo asignar memoria para el arreglo\n");
        return 1;
    }
    
    // Preparar los argumentos para el hilo trabajador
    args.array = fib_array;
    args.n = n;
    
    // Crear el hilo trabajador
    // Le pasamos la funcion fibonacci_worker y los argumentos
    if (pthread_create(&worker_thread, NULL, fibonacci_worker, &args) != 0) {
        printf("Error: No se pudo crear el hilo trabajador\n");
        free(fib_array);
        return 1;
    }
    
    // Bloquearse y esperar a que el hilo trabajador termine
    // Esto es importante para asegurar que todos los numeros esten calculados
    // antes de intentar imprimirlos
    pthread_join(worker_thread, NULL);
    
    // Una vez que el hilo trabajador termino, imprimir la secuencia
    printf("Secuencia de Fibonacci (primeros %d elementos):\n", n);
    for (i = 0; i < n; i++) {
        printf("%lld", fib_array[i]);
        if (i < n - 1) {
            printf(" ");
        }
    }
    printf("\n");
    
    // Liberar la memoria que reservamos
    free(fib_array);
    
    return 0;
}

// Funcion que ejecuta el hilo trabajador
// Calcula los numeros de Fibonacci y los almacena en el arreglo compartido
void* fibonacci_worker(void* arg)
{
    // Convertir el argumento generico a nuestra estructura
    fibonacci_args_t* args = (fibonacci_args_t*)arg;
    
    // Obtener el puntero al arreglo y el numero de elementos
    long long* array = args->array;
    int n = args->n;
    
    int i;
    
    // Los primeros dos numeros de Fibonacci son siempre 0 y 1
    if (n >= 1) {
        array[0] = 0;
    }
    if (n >= 2) {
        array[1] = 1;
    }
    
    // Calcular el resto de los numeros
    // Cada numero es la suma de los dos anteriores
    for (i = 2; i < n; i++) {
        array[i] = array[i-1] + array[i-2];
    }
    
    // Terminar el hilo
    pthread_exit(NULL);
}

