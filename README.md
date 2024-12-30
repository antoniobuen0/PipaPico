# PipaPico

## Tabla de Contenidos
1. [Descripción](#descripción)
2. [Requisitos](#requisitos)
3. [Configuración](#configuración)
4. [Uso](#uso)
5. [Diseño Detallado](#diseño-detallado)
6. [Componentes](#componentes)
7. [Estructura del Código](#estructura-del-código)
8. [Códigos Explicados](#códigos-explicados)
9. [Contribuciones](#contribuciones)
10. [Licencia](#licencia)

## Descripción
🌟 PipaPico es un proyecto basado en la placa **RP2040-Zero** que implementa un dispensador de pipas de manera automática. Utiliza procesamiento de imágenes para detectar, contar y gestionar pipas en tiempo real. 🐦

## Requisitos

### Electrónicos
- 🖥️ **Placa RP2040-Zero**
- 📷 **Cámara OV7670** (sensor de imagen compatible)
- ⚙️ **Motor Paso a Paso 28BYJ-48** + Driver ULN2003
- 🛠️ **2 Servos SG90** (para trampilla y dispensador)
- 🎛️ **Motor de Vibración 1034**
- 🔋 **Fuente de Alimentación 5V, 2A**

### Materiales Estructurales
- 🧰 Guías de cajón
- 🛠️ Varilla roscada (M5 o M8)
- 🏗️ Perfiles de aluminio o madera
- 🪟 Bandeja de acrílico o metal
- 🔩 Tornillos y tuercas

### Software
- 💾 Pico SDK (Configuración en [Pico SDK](https://github.com/raspberrypi/pico-sdk))
- 🖥️ Herramientas de desarrollo CMake y GNU Make

## Configuración

1. **Clonar el Repositorio**:
    ```sh
    git clone https://github.com/antoniobuen0/PipaPico.git
    cd PipaPico
    ```

2. **Configurar el Entorno de Desarrollo**:
    🌐 Sigue las instrucciones oficiales del [Pico SDK](https://github.com/raspberrypi/pico-sdk) para configurar tu entorno. ⚙️

3. **Compilar el Proyecto**:
    ```sh
    mkdir build
    cd build
    cmake ..
    make
    ```

4. **Cargar el Firmware en la Placa**:
    🚀 Conecta la RP2040-Zero mediante USB y copia el archivo `.uf2` generado en la unidad que aparece.

## Uso

1. **Conectar los Componentes**:
    - ⚡ Conecta la cámara OV7670 y los motores siguiendo el esquema del repositorio.
    - 🔋 Alimenta la placa con la fuente de 5V.

2. **Abrir la Terminal Serial**:
    - 🖥️ Utiliza un programa como `minicom` o `PuTTY` para acceder al puerto serial (por ejemplo, `/dev/ttyACM0`) a **115200 baudios**.

3. **Observar el Resultado**:
    - 📊 El sistema mostrará el conteo de pipas, tiempo de procesamiento y estado de los motores.

## Diseño Detallado

### 1. Depósito Principal con Salida Reducida
- **Función**: 🛠️ Almacenar las pipas y limitar el flujo hacia la bandeja. 📦
- **Detalles**:
  - Salida ajustada para evitar que salgan más de las necesarias con la vibración.
  - Forma inclinada para facilitar el flujo.

### 2. Bandeja Compuerta
- **Función**: ⚖️ Chequear la cantidad de pipas y manejar excesos. ✂️
- **Detalles**:
  - Sensor para detectar la cantidad de pipas.
  - Trampilla basculante para devolver el exceso al depósito.

### 3. Ascensor con Tres Niveles
- **Niveles**:
  - **Nivel Depósito**: 🏗️ Llenado desde el depósito principal.
  - **Nivel Compuerta**: 🎛️ Chequeo y manejo de exceso.
  - **Nivel Superior**: 🎯 Entrega de pipas.

## Componentes

| **Categoría**          | **Componente**                                       | **Cantidad** |
|--------------------------|-----------------------------------------------------|--------------|
| **Controlador**          | RP2040-Zero                                        | 1            |
| **Motores y Actuadores** | Motor Paso a Paso 28BYJ-48 + ULN2003               | 1            |
|                          | Servo SG90                                         | 2            |
|                          | Motor de Vibración 1034                            | 1            |
| **Sensores**             | Cámara OV7670                                     | 1            |
| **Estructura Física**    | Guías de Cajón, Varilla Roscada, Perfiles y Bandejas | Variado      |
| **Alimentación**         | Fuente de Alimentación 5V 2A                       | 1            |

## Estructura del Código

- [`main.c`](https://github.com/antoniobuen0/PipaPico/blob/main/main.c): 🖥️ Lógica principal del dispensador. 💡
- [`config.h`](https://github.com/antoniobuen0/PipaPico/blob/main/config.h): 🛠️ Configuraciones globales del proyecto. ⚙️
- [`median_filter.h`](https://github.com/antoniobuen0/PipaPico/blob/main/median_filter.h): 📊 Implementación del filtro de mediana. ✂️
- [`detection.h`](https://github.com/antoniobuen0/PipaPico/blob/main/detection.h): 🔍 Detección de pipas. 🔬
- [`symmetry.h`](https://github.com/antoniobuen0/PipaPico/blob/main/symmetry.h): ⚖️ Cálculo de simetría para validar detecciones. 📏
- [`debug_comm.h`](https://github.com/antoniobuen0/PipaPico/blob/main/debug_comm.h): 💻 Comunicación serial para depuración. 🛰️

## Códigos Explicados

### `main.c`
Este archivo contiene el flujo principal del programa:
1. 🔌 Inicializa los periféricos (cámara, motores, UART).
2. 📸 Captura una imagen desde la cámara.
3. 🛠️ Procesa la imagen (filtro de mediana y umbralizado).
4. 📊 Detecta las pipas y evalúa su cantidad.
5. ⚙️ Controla los motores para gestionar las pipas.

### `median_filter.h`
📉 Implementa un filtro de mediana 3x3 para reducir el ruido en las imágenes capturadas. Es clave para mejorar la detección de bordes. 📉

### `detection.h`
🔍 Contiene las funciones para detectar pipas en la imagen procesada. Utiliza algoritmos como el etiquetado de componentes conectados (flood fill). 🔬

### `symmetry.h`
⚖️ Evalúa la simetría de las pipas detectadas para descartar falsos positivos o agrupaciones incorrectas. 📏

### `debug_comm.h`
🛰️ Proporciona herramientas para enviar datos de depuración a través del puerto serial, incluyendo datos de las pipas detectadas y tiempos de procesamiento. 🛰️

## Contribuciones

🌟 Las contribuciones son bienvenidas. Si deseas mejorar el proyecto o reportar errores, abre un issue o un pull request en el [repositorio](https://github.com/antoniobuen0/PipaPico). 🛠️

## Licencia

📜 Este proyecto está licenciado bajo la Licencia GPL-3.0. Consulta el archivo [`LICENSE`](https://github.com/antoniobuen0/PipaPico/blob/main/LICENSE) para más detalles. 📜

