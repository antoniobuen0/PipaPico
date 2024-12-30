# PipaPico

Dispensador de pipas

## Descripción

PipaPico es un proyecto basado en la placa RP2040 que implementa un dispensador de pipas. Utiliza procesamiento de imágenes para detectar y contar pipas en tiempo real.

## Requisitos

- Placa RP2040
- Sensor de imagen compatible
- Conexión UART para depuración
- Biblioteca Pico SDK

## Configuración

1. Clona el repositorio:
    ```sh
    git clone https://github.com/antoniobuen0/PipaPico.git
    cd PipaPico
    ```

2. Configura el entorno de desarrollo para RP2040 siguiendo las instrucciones del [Pico SDK](https://github.com/raspberrypi/pico-sdk).

3. Compila el proyecto:
    ```sh
    mkdir build
    cd build
    cmake ..
    make
    ```

4. Carga el firmware en la placa RP2040.

## Uso

1. Conecta el sensor de imagen a la placa RP2040.
2. Conecta la placa RP2040 a tu computadora mediante USB.
3. Abre una terminal serial en `/dev/ttyACM0` (o el puerto correspondiente) a 115200 baudios.
4. Observa la salida en la terminal para ver el conteo de pipas y el tiempo de procesamiento.

## Diseño Detallado

### 1. Depósito Principal con Salida Reducida
- **Función:** Almacenar las semillas y limitar el flujo hacia la bandeja compuerta.
- **Características:**
  - Salida con abertura reducida para minimizar el riesgo de que la vibración empuje más semillas de las necesarias.
  - Vibración activa solo cuando se requiere llenar la bandeja.
  - Forma inclinada para facilitar el flujo hacia la abertura.

### 2. Bandeja Compuerta
- **Función:** Realiza el chequeo inicial y permite manejar excesos devolviendo semillas al depósito.
- **Características:**
  - Detector de cantidad: Un sensor visual o infrarrojo detecta si hay 1 semilla, más de una o ninguna.
  - Trampilla basculante:
    - Si hay exceso, la bandeja se inclina para devolver las semillas al depósito.
    - Al volver a su posición original, la trampilla se une al depósito principal sin dejar espacio para derrames.
  - Retroalimentación al sistema:
    - Si no llegó ninguna semilla, activa la vibración nuevamente.

### 3. Ascensor con Tres Niveles
- **Niveles:**
  1. Nivel Depósito:
    - El ascensor en esta posición permite que las semillas caigan hacia la bandeja compuerta.
  2. Nivel Compuerta:
    - Se realiza el chequeo de cantidad y el manejo de excesos.
  3. Nivel Superior:
    - Si hay 1 semilla, el ascensor la eleva y la libera al exterior.
- **Mecanismo de Elevación:**
  - Motor paso a paso o husillo para precisión en la posición.
  - Estructura sólida para manejar cargas ligeras como semillas.

### Lógica del Proceso
1. **Inicialización:**
  - El sistema comienza en el nivel depósito y activa la vibración para llenar la bandeja compuerta.
  - La bandeja compuerta chequea el contenido con el detector.
2. **Chequeo de Cantidad:**
  - Nada detectado: Se activa nuevamente la vibración.
  - Exceso detectado: La bandeja se inclina como trampilla, devolviendo el contenido al depósito.
  - Una semilla detectada: El ascensor se mueve al nivel superior.
3. **Entrega:**
  - En el nivel superior, la semilla se libera al exterior.
  - El ascensor regresa al nivel depósito para repetir el proceso.

## Componentes

| Categoría           | Componente                        |
|---------------------|-----------------------------------|
| **Controlador**     | RP2040-Zero                      |
| **Motores y Servos**| Motor Paso a Paso 28BYJ-48 + ULN2003, Servo SG90 |
| **Actuadores**      | Motor de Vibración 1034           |
| **Sensores**        | OV7670 (Cámara)                   |
| **Estructura Física**| Guías de Cajón, Varilla Roscada (M5 o M8), Perfiles de Aluminio o Madera, Bandeja de Acrílico o Metal, Tornillos y Tuercas |
| **Alimentación**    | Fuente de Alimentación 5V 2A      |
| **Accesorios**      | Acoplador Flexible, Protoboard o PCB Personalizada, Cables Dupont |

## Estructura del Código

- `main.c`: Contiene la lógica principal del programa.
- `config.h`: Configuraciones del proyecto.
- `types.h`: Definiciones de tipos utilizados en el proyecto.
- `median_filter.h`: Implementación del filtro de mediana.
- `detection.h`: Funciones para la detección de pipas.
- `symmetry.h`: Funciones relacionadas con la simetría (si aplica).
- `debug_comm.h`: Comunicación para depuración.

## Contribuciones

Las contribuciones son bienvenidas. Por favor, abre un issue o un pull request para discutir cualquier cambio.

## Licencia

Este proyecto está licenciado bajo la Licencia GPL-3.0. Consulta el archivo `LICENSE` para más detalles.

