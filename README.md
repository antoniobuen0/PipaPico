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
🌟 PipaPico es un módulo de detección visual basado en la placa **RP2040-Zero** que forma parte de un sistema dispensador de pipas. Su función principal es detectar si hay exactamente una pipa en la imagen capturada, permitiendo así el control preciso del dispensador. 🔍

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
  - Aquí entra en juego la cámara para detectar la cantidad de pipas.
  - Trampilla basculante para devolver el exceso al depósito.

### 3. Ascensor con Tres Niveles
- **Niveles**:
  - **Nivel Depósito**: 🏗️ Llenado desde el depósito principal.
  - **Nivel Compuerta**: 🎛️ Chequeo y manejo de exceso.
  - **Nivel Superior**: 🎯 Entrega de pipas.

## Integración con el Dispensador

### Flujo de Control
1. 📸 **Detección con la Cámara**
   - Captura de imagen mediante OV7670
   - Procesamiento y filtrado
   - Conteo de pipas mediante flood fill

2. 🔄 **Lógica de Control**
   - Si detecta 1 pipa → Activa servo para dispensar
   - Si detecta 0 pipas → Activa vibración
   - Si detecta >1 pipas → Abre trampilla de retorno

3. ⚙️ **Actuadores**
   - Servo principal (dispensador)
   - Motor de vibración
   - Servo de trampilla


### Diagrama de Estados
```
[Captura] → [Procesado] → [Detección] → [Decisión]
                                          │
                    ┌────────────────┬────┴────────┐
                    ↓                ↓             ↓
              [0 pipas]        [1 pipa]     [>1 pipas]
                    ↓                ↓             ↓
             [Vibración]      [Servo ON]    [Trampilla]
```

## Componentes

| **Categoría**          | **Componente**                                       | **Cantidad** |
|------------------------|-------------------------------------------------------|--------------|
| **Control Principal**  | RP2040-Zero                                          | 1            |
| **Sensores**           | Cámara OV7670                                        | 1            |
| **Actuadores**         | Servo SG90 (Dispensador)                             | 1            |
|                        | Servo SG90 (Trampilla)                               | 1            |
|                        | Motor de Vibración                                   | 1            |
| **Estructura**         | Guías y Soportes                                     | Varios       |

## Estructura del Código

- El **núcleo** de la lógica se encuentra en archivos `.c`, mientras que los headers (`.h`) declaran prototipos y estructuras.  
- A continuación se describen los archivos `.c` principales y su responsabilidad:

### `main.c`
- **Flujo Principal** del programa:  
  1. Inicializa periféricos (cámara, UART, etc.).  
  2. Captura una imagen desde la cámara (y genera XCLK si procede).  
  3. Aplica filtrados / umbralizados básicos.  
  4. Llama a la detección de pipas.  
  5. Activa los actuadores (vibración, servos) según el número de pipas detectadas.

### `median_filter.c`
- Implementa el **filtro de mediana 3×3**, clave para reducir ruido (“salt-and-pepper noise”) y mejorar la nitidez antes de la detección.  
- Usa un **histograma deslizante** para actualizar eficientemente la mediana en cada píxel.

### `detection.c`
- **Detección de pipas**:  
  - Realiza el etiquetado de componentes conectados (flood fill) para contar cuántos objetos blancos hay en la imagen binaria.  
  - Calcula área, bounding box, centro de masa, etc.  
  - Devuelve el conteo `numPipas`.

### `symmetry.c`
- Complementa la detección calculando **simetría** (por ejemplo, puntaje de cuán “simétrica” es la pipa).  
- Puede descartar objetos ambiguos o filtrar regiones que no cumplan cierto nivel de simetría.

### `debug_comm.c`
- Maneja la **comunicación de depuración** por UART.  
  - Envía estructuras con la imagen procesada o con el número de pipas detectadas.  
  - Útil para monitorear datos en una PC o sistema externo.

> *Nota*: Los headers `.h` (como `median_filter.h`, `detection.h`, etc.) simplemente **declaran** las funciones y estructuras que se usan en los `.c`. Revisarlos puede ser útil si necesitas entender la API, pero la **implementación real** está en los `.c`.

## Códigos Explicados

En esta sección destacamos la **lógica** de cada `.c`:

### `main.c`
1. 🔌 **Inicializa** periféricos (p.ej. `initCamera()`, UART, etc.).
2. 📸 **Captura** la imagen desde la cámara (función `captureImage()`).
3. 🛠️ **Procesa** la imagen (opcional: llama a `optimizedMedianFilter()`, hace un umbralizado).
4. 📊 **Detecta** las pipas (llamando a `detectPipas()`).
5. ⚙️ **Controla** motores (servo, vibración) en función de `numPipas`.

### `median_filter.c`
- **Función principal**: `optimizedMedianFilter()`
  - Calcula la mediana en vecindarios 3×3 usando una estructura de histograma deslizante.
  - Sobrescribe el `frameBuffer` (o un buffer temporal) con el resultado filtrado.

### `detection.c`
- **Función clave**: `detectPipas(int* numPipas)`
  - Recorre la imagen binaria (0/255).
  - Usa un flood fill con stack para agrupar píxeles contiguos en blanco.
  - Extrae área y bounding box de cada objeto.
  - Incrementa `(*numPipas)` al encontrar regiones válidas.

### `symmetry.c`
- **Función**: `calculateSymmetry(...)`
  - Opcional, evalúa la mitad izquierda vs. derecha (o arriba vs. abajo) de la región para un “score” de simetría.
  - Ayuda a descartar formas que no sean pipas reales.

### `debug_comm.c`
- **Envía** un `DebugFrame` o datos de telemetría:
  - Estructura empaquetada (timestamp, número de pipas, etc.) por UART.
  - Permite monitorear el comportamiento del sistema en una PC.

## Contribuciones

🌟 Las contribuciones son bienvenidas. Si deseas mejorar el proyecto o reportar errores, abre un issue o un pull request en el [repositorio](https://github.com/antoniobuen0/PipaPico). 🛠️

## Licencia

📜 Este proyecto está licenciado bajo la Licencia GPL-3.0. Consulta el archivo [`LICENSE`](https://github.com/antoniobuen0/PipaPico/blob/main/LICENSE) para más detalles. 📜
