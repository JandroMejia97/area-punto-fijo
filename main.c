#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define FALSE 0
#define TRUE 1
#define BASE_BINARIO 2
#define BASE_DECIMAL 10

// 2^17 = 131072
#define BITS_PARTE_DECIMAL_OPERANDOS 17
#define FACTOR_ESCALA_OPERANDOS ( 1U << BITS_PARTE_DECIMAL_OPERANDOS)
#define PARTE_DECIMAL_OPERANDOS_MASCARA (FACTOR_ESCALA_OPERANDOS - 1)

// 2^34 = 17179869184
#define BITS_PARTE_DECIMAL_RESULTADO 34
#define FACTOR_ESCALA_RESULTADO ( 1ULL << BITS_PARTE_DECIMAL_RESULTADO)
#define PARTE_DECIMAL_RESULTADO_MASCARA (FACTOR_ESCALA_RESULTADO - 1)

// RADIO_MINIMO = 2^-17
#define RADIO_MINIMO_PF (0.00001 * FACTOR_ESCALA_OPERANDOS)

// RADIO MAXIMO = sqrt(MAX_REPRE_64_BITS / PI)
#define RADIO_MAXIMO_PF (18487.36427 * FACTOR_ESCALA_OPERANDOS)

#define PI_PF (3.14159 * FACTOR_ESCALA_OPERANDOS)

unsigned int * rotarDerecha(unsigned int *numero, unsigned short base, unsigned int rotaciones);
unsigned int * rotarDerechaBin(unsigned int *numero, unsigned int rotaciones);
unsigned int * rotarIzquierda(unsigned int *numero, unsigned int base, unsigned int rotaciones);
unsigned int * rotarIzquierdaBin(unsigned int *numero, unsigned int rotaciones);

unsigned long long * rotarDerechaLong(unsigned long long *numero, unsigned short base, unsigned int rotaciones);
unsigned long long * rotarDerechaLongBin(unsigned long long *numero, unsigned int rotaciones);
unsigned long long * rotarIzquierdaLong(unsigned long long *numero, unsigned short base, unsigned int rotaciones);
unsigned long long * rotarIzquierdaLongBin(unsigned long long *numero, unsigned int rotaciones);

void mostrarMenu();
void lecturaDeDatos();
char * solicitarRadio(char *cadena);
unsigned int * convertirCadenaAPuntoFijo32Bits(char *cadena, unsigned int *entero);
int validarRadio(const char * cadena, unsigned int radio);
void dividirCadena(char *strEntrada, char *strParteEntera, char *strParteDecimal);
unsigned long long calcularArea(unsigned int radio);
double calcularAreaReal(char * cadena);
unsigned long long multiplicarU1617(unsigned int a, unsigned int b);
char * puntoFijoU1517AString(unsigned int numero);
char * puntoFijoU3034AString(unsigned long long numero);
void ejecutarPruebas();
void salir();
char * calcularErrorRelativo(char * strValorCalculado, double valorReal);

int main() {
    printf("LONG LONG %lu\n", sizeof (unsigned long long ));
    printf("INT %lu\n", sizeof (unsigned int ));
    mostrarMenu();
    return 0;
}

void mostrarMenu() {
    char opcion;
    printf("******************************************************\n");
    printf("***** CALCULADORA DE LA SUPERFICIE DE UN CIRCULO *****\n");
    printf("******************************************************\n\n");
    do {
        printf("MENU DE OPCIONES\n");
        printf("\t[A] Ingresar radio.\n");
        printf("\t[B] Ejecutar pruebas.\n");
        printf("\t[C] Salir.\n");
        printf("Ingresa alguna de las opciones anteriores: ");
        scanf(" %c", &opcion);
        switch (opcion) {
            case 'A':
            case 'a':
                lecturaDeDatos();
                break;
            case 'B':
            case 'b':
                ejecutarPruebas();
                break;
            case 'C':
            case 'c':
                salir();
                break;
            default:
                printf("\nLa opcion [%c] es invalida.\n", opcion);
                printf("Intenta nuevamente.\n\n");
        }
    } while (opcion != 'C' && opcion != 'c');
}

void lecturaDeDatos() {
    char *cadena = (char*) malloc(11*sizeof(char));
    char *cadenaAreaCalculada = NULL;
    unsigned int *radioPF = (unsigned int*) malloc(sizeof(unsigned int));
    unsigned long long areaCalculada;
    do {
        solicitarRadio(cadena);
        convertirCadenaAPuntoFijo32Bits(cadena, radioPF);
    } while(!validarRadio(cadena, *radioPF));
    printf("\n");
    areaCalculada = calcularArea(*radioPF);
    double areaReal = calcularAreaReal(cadena);
    cadenaAreaCalculada = puntoFijoU3034AString(areaCalculada);
    printf("RADIO: %s SUPERFICIE: %s ERROR DEL CALCULO: %s %%\n", puntoFijoU1517AString(*radioPF), cadenaAreaCalculada , calcularErrorRelativo(cadenaAreaCalculada, areaReal));
}

char * solicitarRadio(char * cadena) {
    printf("Ingrese el valor del radio (Puedes usar punto o coma, como separador decimal): ");
    scanf(" %s", cadena);
    return cadena;
}

unsigned int * convertirCadenaAPuntoFijo32Bits(char *cadena, unsigned int *entero) {
    char *strParteEntera = (char*)malloc(5*sizeof(char));
    char *strParteDecimal = (char*)malloc(5*sizeof(char));

    dividirCadena(cadena, strParteEntera, strParteDecimal);

    unsigned int parteEntera = (atoi(strParteEntera) << BITS_PARTE_DECIMAL_OPERANDOS);
    unsigned int parteDecimal = (atoi(strParteDecimal) << BITS_PARTE_DECIMAL_OPERANDOS);

    // MULTIPLICACIONES SUCESIVAS DE POTENCIAS DE 2
    // rotarDerechaBin(&parteEntera, BITS_PARTE_DECIMAL_OPERANDOS);
    // rotarDerechaBin(&parteDecimal, BITS_PARTE_DECIMAL_OPERANDOS);

    unsigned int divisorDecimal = 1;
    // OBTIENE LA POTENCIA DE 10 POR LA QUE SE DEBE DIVIDIR LA PARTE DECIMAL
    rotarDerecha(&divisorDecimal, BASE_DECIMAL, strlen(strParteDecimal));

    if (parteDecimal > 0) {
        parteDecimal /= divisorDecimal;
    }

    // DIRECTAMENTE
    *entero = parteEntera + parteDecimal;
    return entero;
}


void dividirCadena(char *strEntrada, char *strParteEntera, char *strParteDecimal) {
    int i=0;
    while (strEntrada[i] != '\0' && strEntrada[i] != ',' && strEntrada[i] != '.') {
        i++;
    }
    if (strEntrada[i] == '\0') {
        strEntrada[i + 1] = '0';
    }
    strncpy(strParteEntera, strEntrada, i * sizeof(char));
    strcpy(strParteDecimal, (strEntrada + i + 1));
}

int validarRadio(const char * cadena, unsigned int radio) {
    if (radio < RADIO_MINIMO_PF || radio > RADIO_MAXIMO_PF) {
        printf("El valor \"%s\" no es válido. Tome en cuenta qué:\n", cadena);
        printf("\t1. El radio debe ser mayor o igual a ");
        puntoFijoU1517AString(RADIO_MINIMO_PF);
        printf("\n");
        printf("\t2. El radio debe ser menor o igual a ");
        puntoFijoU1517AString(RADIO_MAXIMO_PF);
        printf("\n");
        return FALSE;
    } else {
        return TRUE;
    }
}

unsigned long long calcularArea(unsigned int radioPF) {
    // PI * RADIO
    unsigned int piXRadio = multiplicarU1617(PI_PF, radioPF) / FACTOR_ESCALA_OPERANDOS;
    // PI * RADIO * RADIO
    unsigned long long resultado = multiplicarU1617(piXRadio, radioPF);
    return resultado;
}

double calcularAreaReal(char * cadena) {
    char *ptr;
    double radio = strtod(cadena, &ptr);
    return 3.14159 * radio * radio;
}

unsigned long long multiplicarU1617(unsigned int a, unsigned int b) {
    return (unsigned long long) a * b;
}

char * puntoFijoU1517AString(unsigned int numero) {
    unsigned int parteEntera = numero >> BITS_PARTE_DECIMAL_OPERANDOS;
    numero &= PARTE_DECIMAL_OPERANDOS_MASCARA;
    rotarDerecha(&numero, BASE_DECIMAL, 5);
    unsigned int parteDecimal = (unsigned int) numero / FACTOR_ESCALA_OPERANDOS;
    char *outputString = (char*)malloc(11 * sizeof(char));
    sprintf(outputString, "%d.%05d", parteEntera, parteDecimal);
    return outputString;
}

char * puntoFijoU3034AString(unsigned long long numero) {
    unsigned long parteEntera = numero >> BITS_PARTE_DECIMAL_RESULTADO;
    numero &= PARTE_DECIMAL_RESULTADO_MASCARA;
    rotarDerechaLong(&numero, BASE_DECIMAL, 10);
    unsigned long long parteDecimal = (unsigned long long) numero / FACTOR_ESCALA_RESULTADO;
    char *outputString = (char*)malloc(21 * sizeof(char));
    sprintf(outputString, "%lu.%010llu", parteEntera, parteDecimal);
    return outputString;
}

void ejecutarPruebas() {
    const int cantFilas = 8, cantColumnas = 11;
    char cadenas[8][11] = {
            "0.01563",
            "0.0625",
            "12.125",
            "100.0",
            "256.03125",
            "1000.0",
            "10000.0",
            "15000.0"
    };
    printf("\nA CONTINUACION SE EJECUTARA UN CONJUNTO DE PRUEBAS: \n");
    for (int i = 0; i < cantFilas; i++) {
        char *cadenaAreaCalculada = NULL;
        unsigned int *radioPF = (unsigned int*) malloc(sizeof(unsigned int));
        unsigned long long areaCalculada;
        convertirCadenaAPuntoFijo32Bits(cadenas[i], radioPF);
        areaCalculada = calcularArea(*radioPF);
        double areaReal = calcularAreaReal(cadenas[i]);
        cadenaAreaCalculada = puntoFijoU3034AString(areaCalculada);
        printf("%d. RADIO: %s SUPERFICIE: %s ERROR DEL CALCULO: %s %%\n", i + 1, puntoFijoU1517AString(*radioPF), cadenaAreaCalculada , calcularErrorRelativo(cadenaAreaCalculada, areaReal));
    }
}

void salir() {
    printf("\n**********************************\n");
    printf("***** EL PROGRAMA SE CERRARA *****\n");
    printf("************ GRACIAS *************\n");
    printf("**********************************\n");
}

char * calcularErrorRelativo(char * strValorCalculado, double valorReal) {
    char *ptr;
    double valorCalculado = strtod(strValorCalculado, &ptr);
    double error = fabs(valorCalculado - valorReal) * 100 / valorReal;
    char *outputString = (char*)malloc(21 * sizeof(char));
    sprintf(outputString, "%.5f", error);
    return outputString;
}

unsigned int * rotarDerecha(unsigned int *numero, unsigned short base, unsigned int rotaciones) {
    while(rotaciones > 0) {
        *numero = *numero * base;
        rotaciones--;
    }
    return numero;
}

unsigned int *  rotarDerechaBin(unsigned int *numero, unsigned int rotaciones) {
    return rotarDerecha(numero, BASE_BINARIO, rotaciones);
}

unsigned int *  rotarIzquierda(unsigned int *numero, unsigned int base, unsigned int rotaciones) {
    while (rotaciones > 0) {
        *numero = *numero / base;
        rotaciones--;
    }
    return numero;
}

unsigned int *  rotarIzquierdaBin(unsigned int *numero, unsigned int rotaciones) {
    return rotarIzquierda(numero, BASE_BINARIO, rotaciones);
}

unsigned long long * rotarDerechaLong(unsigned long long *numero, unsigned short base, unsigned int rotaciones) {
    while(rotaciones > 0) {
        *numero = *numero * base;
        rotaciones--;
    }
    return numero;
}
unsigned long long * rotarDerechaLongBin(unsigned long long *numero, unsigned int rotaciones) {
    return rotarDerechaLong(numero, BASE_BINARIO, rotaciones);
}

unsigned long long * rotarIzquierdaLong(unsigned long long *numero, unsigned short base, unsigned int rotaciones) {
    while(rotaciones > 0) {
        *numero = *numero / base;
        rotaciones--;
    }
    return numero;
}
unsigned long long * rotarIzquierdaLongBin(unsigned long long *numero, unsigned int rotaciones) {
    return rotarIzquierdaLong(numero, BASE_BINARIO, rotaciones);
}
