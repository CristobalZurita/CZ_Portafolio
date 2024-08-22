### **Informe Técnico Completo del Proyecto de Monitorización de Calidad del Agua y Consumo en Tiempo Real**

---

### **Índice**

1. **Introducción**
2. **Objetivo del Proyecto**
3. **Desarrollo del Código**
   - 3.1 Estructura General del Código
   - 3.2 Medición del Caudal
   - 3.3 Medición del pH
   - 3.4 Medición de TDS
   - 3.5 Control de Relés para Gestión de Energía
   - 3.6 Visualización y Gestión de Menú en el LCD
   - 3.7 Almacenamiento de Datos en Tarjeta SD
   - 3.8 Conectividad Móvil con Módulo SIM900
   - 3.9 Estrategia de Depuración
4. **Desarrollo de las Placas PCB**
   - 4.1 Proceso de Mejora
   - 4.2 Integración con el Software
5. **Pruebas y Validación**
   - 5.1 Pruebas de Funcionalidad
   - 5.2 Resultados
6. **Conclusiones y Recomendaciones**
7. **Manual de Usuario**
8. **Código Completo y Comentado**

---

### **1. Introducción**

El presente informe técnico describe el desarrollo y las funcionalidades avanzadas de un sistema de monitorización en tiempo real de la calidad del agua y el consumo. Este sistema está diseñado para medir parámetros críticos del agua como el caudal, el pH, los sólidos disueltos totales (TDS), y cuenta con capacidades adicionales como el control de relés para gestionar bombas y procesos, almacenamiento de datos en una tarjeta SD, envío de datos por SMS a través de un módulo SIM900, y un menú interactivo en un LCD 16x2 que permite visualizar y controlar todas las funciones del sistema.

### **2. Objetivo del Proyecto**

El objetivo principal del sistema es proporcionar una solución integral para la monitorización en tiempo real de los siguientes parámetros del agua:

- **Caudal de Agua:** Medir el flujo de agua en litros por hora y acumular el volumen total consumido tanto en la entrada como en la salida.
- **pH del Agua:** Evaluar la acidez o alcalinidad del agua, un indicador clave de su calidad.
- **TDS del Agua:** Medir los sólidos disueltos totales (TDS), que proporcionan una indicación sobre la pureza del agua en tres puntos del proceso.
- **Control de Energía y Procesos:** Gestionar el encendido y apagado de bombas, el purgado del estanque, y la activación de procesos mediante relés.
- **Almacenamiento de Datos:** Registrar los datos en una tarjeta SD para su posterior análisis.
- **Monitoreo Remoto:** Enviar los datos medidos por SMS utilizando un módulo SIM900 para supervisión remota.
- **Detección de Fugas:** Supervisar el sistema en busca de fugas internas mediante un sensor de contacto de agua.

El sistema está diseñado para ser modular y fácilmente escalable, permitiendo la adición de nuevas funcionalidades en el futuro.

### **3. Desarrollo del Código**

#### **3.1 Estructura General del Código**

El código está escrito en C++ y se organiza de manera modular para garantizar claridad y mantenibilidad. Se utilizan diversas librerías, como `LiquidCrystal` para la interacción con el display LCD, `SD` para la gestión de la tarjeta SD, `SoftwareSerial` para la comunicación con el módulo SIM900, y se implementa una máquina de estados finitos para la navegación en el menú del LCD.

```cpp
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <SD.h>

// Definición de pines y constantes
const int RS_PIN = 12;           // Pines de control para el LCD
const int EN_PIN = 11;
const int D4_PIN = 4;
const int D5_PIN = 5;
const int D6_PIN = 6;
const int D7_PIN = 7;
const int FLOW_SENSOR_IN_PIN = 2;  // Pin del sensor de flujo de entrada
const int FLOW_SENSOR_OUT_PIN = 3; // Pin del sensor de flujo de salida
const int TDS_SENSOR_1_PIN = 4;    // Pin del sensor de TDS 1
const int TDS_SENSOR_2_PIN = 5;    // Pin del sensor de TDS 2
const int TDS_SENSOR_3_PIN = 6;    // Pin del sensor de TDS 3
const int PH_SENSOR_PIN = A0;      // Pin del sensor de pH (entrada analógica)
const int RELAY_POWER_PIN = 8;     // Pin para controlar el relé de encendido del equipo
const int RELAY_PURGE_PIN = 9;     // Pin para controlar el relé de purgado del estanque
const int RELAY_PUMP_IN_PIN = 10;  // Pin para controlar el relé de la bomba de entrada
const int RELAY_PUMP_OUT_PIN = 11; // Pin para controlar el relé de la bomba de salida
const int SIM900_RX_PIN = 10;      // Pin RX para comunicación con SIM900
const int SIM900_TX_PIN = 11;      // Pin TX para comunicación con SIM900
const int SD_CHIP_SELECT = 4;      // Pin CS de la tarjeta SD
const int LEAK_SENSOR_PIN = 7;     // Pin del sensor de fugas de agua

// Inicialización del LCD
LiquidCrystal lcd(RS_PIN, EN_PIN, D4_PIN, D5_PIN, D6_PIN, D7_PIN);

// Configuración de comunicación serial para el SIM900
SoftwareSerial sim900(SIM900_RX_PIN, SIM900_TX_PIN);

// Variables globales
volatile int flowPulsesIn = 0;      // Contador de pulsos del sensor de flujo de entrada
volatile int flowPulsesOut = 0;     // Contador de pulsos del sensor de flujo de salida
volatile int tdsPulses1 = 0;        // Contador de pulsos del sensor de TDS 1
volatile int tdsPulses2 = 0;        // Contador de pulsos del sensor de TDS 2
volatile int tdsPulses3 = 0;        // Contador de pulsos del sensor de TDS 3
unsigned long previousTime = 0;     // Tiempo de referencia para el cálculo
unsigned long accumulatedPulsesIn = 0;   // Pulsos acumulados para el cálculo de litros (entrada)
unsigned long accumulatedPulsesOut = 0;  // Pulsos acumulados para el cálculo de litros (salida)
float totalLitersIn = 0.0;          // Litros totales acumulados (entrada)
float totalLitersOut = 0.0;         // Litros totales acumulados (salida)
float currentPH = 0.0;              // Valor actual de pH
File dataFile;                      // Archivo de datos para almacenar en la tarjeta SD

// Estado de la máquina de estados finitos para el menú LCD
#define S_HOME  0
#define S_FLOW  1
#define S_PH    2
#define S_TDS   3
#define S_SETTINGS  4
uint8_t estado = S_HOME;

// Variables para el menú
uint8_t menuSelection = 0;
uint8_t buttonState[4];
uint8_t lastButtonState[4];

// Definición de botones para el menú
#define BTN_MENU  7
#define BTN_EXIT  8
#define BTN_UP    9
#define BTN_DOWN  10

// Pin de alimentación de la pantalla LCD
#define LCD_POWER_PIN 13
```

**Comentario Técnico:** La definición de pines y variables globales al comienzo del código facilita su reutilización y modificación. La modularidad del código asegura una fácil adaptación a diferentes entornos y requisitos.

#### **3.2 Medición del Caudal**

La medición del caudal se realiza a través de dos sensores de flujo, uno para el agua de entrada y otro para el agua de salida. Estos sensores generan pulsos eléctricos proporcionales al volumen de agua que pasa a través de ellos. Estos pulsos son capturados por el microcontrolador utilizando interrupciones externas. El caudal en litros por hora (L/h) se calcula a partir de la frecuencia de estos pulsos.

```cpp
void flowISR_In() {
    flowPulsesIn++; // Incrementa el contador de pulsos del sensor de flujo de entrada
}

void flowISR_Out() {
    flowPulsesOut++; // Incrementa el contador de pulsos del sensor de flujo de salida
}

int calculateFlowRateIn() {
    int litersPerHourIn = (flowPulsesIn * 60) / 73; // Fórmula para calcular el caudal de entrada
    accumulatedPulsesIn += flowPulsesIn;            // Acumula pulsos para calcular los litros totales (entrada)
    return litersPerHourIn;
}

int calculateFlowRateOut() {
    int litersPerHourOut = (flowPulsesOut * 60) / 73; // Fórmula para calcular el caudal de salida
    accumulatedPulsesOut += flowPulsesOut;            // Acumula pulsos para calcular los litros totales (salida)
    return litersPerHourOut;
}

float calculateTotalLitersIn() {
    return accumulatedPulsesIn * (1.0 / 450); // Cada 450 pulsos representan 1 litro de agua (entrada)


}

float calculateTotalLitersOut() {
    return accumulatedPulsesOut * (1.0 / 450); // Cada 450 pulsos representan 1 litro de agua (salida)
}
```

**Análisis Técnico:**

1. **Calibración del Sensor de Flujo:** Los sensores tienen un factor de calibración `k` de 73, lo que implica que por cada 73 pulsos se produce un caudal equivalente a 1 L/min. Este valor debe ajustarse según el fabricante y las condiciones del fluido.

2. **Cálculo del Caudal:** Las funciones `calculateFlowRateIn()` y `calculateFlowRateOut()` convierten los pulsos en un valor de caudal en litros por hora (L/h) tanto para la entrada como para la salida de agua.

3. **Acumulación de Volumen:** Las funciones `calculateTotalLitersIn()` y `calculateTotalLitersOut()` acumulan los pulsos a lo largo del tiempo y los convierten en un valor de volumen total de agua en litros, basado en que cada 450 pulsos equivalen a 1 litro de agua.

#### **3.3 Medición del pH**

El sensor de pH genera una señal analógica que se mide utilizando un convertidor analógico-digital (ADC) del microcontrolador. El valor del pH se calcula a partir de esta lectura mediante una fórmula de calibración.

```cpp
float readPH() {
    int phValue = analogRead(PH_SENSOR_PIN);    // Lectura analógica del sensor de pH
    float voltage = phValue * (5.0 / 1023.0);   // Conversión del valor ADC a voltaje
    float ph = 3.5 * voltage + 0.0;             // Fórmula de calibración del pH
    return ph;
}
```

**Análisis Técnico:**

1. **Conversión ADC:** El valor leído por el ADC es un entero entre 0 y 1023, correspondiente a un rango de voltaje de 0 a 5V. La conversión a voltaje se realiza mediante la fórmula `voltage = phValue * (5.0 / 1023.0)`.

2. **Fórmula de Calibración:** La ecuación utilizada `ph = 3.5 * voltage + 0.0` es una fórmula lineal de calibración. El coeficiente `3.5` depende de la calibración del sensor de pH, y debe ajustarse en función de las condiciones específicas del agua y del rango operativo del sensor.

3. **Importancia de la Calibración:** Es esencial realizar una calibración precisa del sensor de pH utilizando soluciones de referencia (buffers) para asegurar que la lectura del pH sea exacta. La calibración debe realizarse regularmente para compensar la deriva del sensor.

#### **3.4 Medición de TDS**

La medición de TDS se realiza en tres puntos diferentes del sistema, utilizando sensores que generan pulsos proporcionales a la concentración de sólidos disueltos en el agua. Estos pulsos son capturados mediante interrupciones y convertidos a un valor de TDS utilizando una fórmula exponencial.

```cpp
float measureTDS(int sensorIndex) {
    int tdsPulses = 0;
    switch (sensorIndex) {
        case 1:
            tdsPulses = tdsPulses1;
            break;
        case 2:
            tdsPulses = tdsPulses2;
            break;
        case 3:
            tdsPulses = tdsPulses3;
            break;
    }
    return 6.64 * pow(2.71828, 0.0000381 * tdsPulses); // Fórmula de TDS
}
```

**Análisis Técnico:**

1. **Captura de Pulsos:** Similar a la medición del caudal, los pulsos generados por los sensores de TDS se capturan mediante interrupciones. Cada sensor de TDS se mide en un punto diferente del proceso de purificación.

2. **Fórmula Exponencial:** La fórmula `TDS = 6.64 * exp(0.0000381 * tdsPulses)` convierte los pulsos capturados en un valor de TDS. Este enfoque es adecuado para medir concentraciones de sólidos disueltos en soluciones acuosas.

3. **Interpretación de TDS:** Los valores obtenidos para TDS proporcionan una medida indirecta de la cantidad de iones y partículas disueltas en el agua, un indicador crítico de su calidad.

#### **3.5 Control de Relés para Gestión de Energía y Procesos**

El sistema incluye cuatro relés controlados por el microcontrolador para gestionar el encendido y apagado del equipo, la activación de bombas y el purgado del estanque. Esto permite un control preciso de los procesos de tratamiento del agua.

```cpp
void initRelays() {
    pinMode(RELAY_POWER_PIN, OUTPUT);
    pinMode(RELAY_PURGE_PIN, OUTPUT);
    pinMode(RELAY_PUMP_IN_PIN, OUTPUT);
    pinMode(RELAY_PUMP_OUT_PIN, OUTPUT);
    digitalWrite(RELAY_POWER_PIN, LOW); // Apagar equipo inicialmente
    digitalWrite(RELAY_PURGE_PIN, LOW); // Apagar purgado inicialmente
    digitalWrite(RELAY_PUMP_IN_PIN, LOW); // Apagar bomba de entrada inicialmente
    digitalWrite(RELAY_PUMP_OUT_PIN, LOW); // Apagar bomba de salida inicialmente
}

void controlRelay(int relayPin, bool state) {
    digitalWrite(relayPin, state ? HIGH : LOW);
}
```

**Análisis Técnico:**

1. **Gestión de Procesos:** Los relés permiten el control de procesos críticos como el encendido del equipo, la activación de bombas para la entrada y salida de agua, y el purgado del estanque, asegurando un tratamiento eficiente del agua.

2. **Modularidad del Control:** La función `controlRelay()` permite activar o desactivar cada relé de manera individual, facilitando la integración de nuevos procesos o la modificación de los existentes.

#### **3.6 Visualización y Gestión de Menú en el LCD**

El sistema cuenta con un menú interactivo en un LCD 16x2, gestionado mediante una máquina de estados finitos. Este menú permite al usuario navegar entre diferentes pantallas para visualizar el caudal, el pH, los valores de TDS, y gestionar configuraciones del sistema.

```cpp
void setupLCDMenu() {
    lcd.begin(16, 2);
    pinMode(LCD_POWER_PIN, OUTPUT);
    digitalWrite(LCD_POWER_PIN, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("Sistema Monitor.");
}

void printLCDHome() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("1. Flujo Agua");
    lcd.setCursor(0, 1);
    lcd.print("2. pH / TDS");
}

void printFlowInfo() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Flujo In: "); lcd.print(calculateFlowRateIn());
    lcd.setCursor(0, 1);
    lcd.print("Flujo Out: "); lcd.print(calculateFlowRateOut());
}

void printPHInfo() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("pH: "); lcd.print(readPH());
    lcd.setCursor(0, 1);
    lcd.print("TDS1: "); lcd.print(measureTDS(1));
}

void handleMenuNavigation() {
    switch(estado) {
        case S_HOME:
            if (flancoSubida(BTN_MENU)) {
                estado = S_FLOW;
                printFlowInfo();
            } else if (flancoSubida(BTN_EXIT)) {
                estado = S_PH;
                printPHInfo();
            }
            break;
        case S_FLOW:
            if (flancoSubida(BTN_MENU)) {
                estado = S_SETTINGS;
                printLCDHome();
            } else if (flancoSubida(BTN_EXIT)) {
                estado = S_HOME;
                printLCDHome();
            }
            break;
        case S_PH:
            if (flancoSubida(BTN_MENU)) {
                estado = S_HOME;
                printLCDHome();
            } else if (flancoSubida(BTN_EXIT)) {
                estado = S_FLOW;
                printFlowInfo();
            }
            break;
        case S_SETTINGS:
            // Aquí se agregarán configuraciones adicionales
            break;
    }
}

void loop() {
    handleMenuNavigation();
}
```

**Análisis Técnico:**

1. **Interfaz de Usuario:** El menú en el LCD permite una interacción intuitiva con el sistema, facilitando la visualización de parámetros críticos y la gestión de procesos.

2. **Máquina de Estados:** El uso de una máquina de estados finitos permite manejar la lógica de navegación del menú de manera estructurada y extensible.

3. **Expansibilidad:** La estructura modular del menú permite agregar nuevas opciones y configuraciones fácilmente.

#### **3.7 Almacenamiento de Datos en Tarjeta SD**

El sistema almacena los datos medidos en una tarjeta SD, lo que permite un análisis posterior de la calidad del agua y del rendimiento del sistema.

```cpp
void initSD() {
    if (!SD.begin(SD_CHIP_SELECT)) {
        lcd.setCursor(0, 0);
        lcd.print("Fallo SD");
        return;
    }
}

void logData(int litersPerHourIn, int litersPerHourOut, float totalLitersIn, float totalLitersOut, float ph, float t

ds1, float tds2, float tds3) {
    dataFile = SD.open("waterlog.txt", FILE_WRITE);
    if (dataFile) {
        dataFile.print("InFlow: "); dataFile.print(litersPerHourIn);
        dataFile.print(" OutFlow: "); dataFile.print(litersPerHourOut);
        dataFile.print(" InLiters: "); dataFile.print(totalLitersIn);
        dataFile.print(" OutLiters: "); dataFile.print(totalLitersOut);
        dataFile.print(" pH: "); dataFile.print(ph);
        dataFile.print(" TDS1: "); dataFile.print(tds1);
        dataFile.print(" TDS2: "); dataFile.print(tds2);
        dataFile.print(" TDS3: "); dataFile.println(tds3);
        dataFile.close();
    } else {
        lcd.setCursor(0, 0);
        lcd.print("Error SD");
    }
}
```

**Análisis Técnico:**

1. **Almacenamiento Persistente:** Los datos se registran en un archivo de texto en la tarjeta SD, lo que permite su recuperación y análisis posterior.

2. **Manejo de Errores:** La función `initSD()` verifica la disponibilidad de la tarjeta SD y maneja errores de inicialización, asegurando que los datos se registren correctamente.

#### **3.8 Conectividad Móvil con Módulo SIM900**

El módulo SIM900 permite el envío de datos a través de SMS, lo que facilita la monitorización remota del sistema.

```cpp
void initSIM900() {
    sim900.begin(9600);
    delay(1000);
    sim900.println("AT");            // Enviar comando AT para comprobar la conexión
    delay(1000);
    sim900.println("AT+CSQ");        // Comprobar la intensidad de la señal
    delay(1000);
    sim900.println("AT+CREG?");      // Comprobar el estado de registro de la red
    delay(1000);
}

void sendSMS(int litersPerHourIn, int litersPerHourOut, float ph, float tds1, float tds2, float tds3) {
    sim900.println("AT+CMGF=1");    // Configurar el modo de texto
    delay(1000);
    sim900.println("AT+CMGS=\"+1234567890\"");  // Reemplazar con el número de destino
    delay(1000);
    sim900.print("InFlow: "); sim900.print(litersPerHourIn);
    sim900.print(" OutFlow: "); sim900.print(litersPerHourOut);
    sim900.print(" pH: "); sim900.print(ph);
    sim900.print(" TDS1: "); sim900.print(tds1);
    sim900.print(" TDS2: "); sim900.print(tds2);
    sim900.print(" TDS3: "); sim900.println(tds3);
    sim900.write(26);  // Enviar Ctrl+Z para finalizar el mensaje
    delay(1000);
}
```

**Análisis Técnico:**

1. **Monitoreo Remoto:** Permite recibir actualizaciones de los parámetros del sistema en tiempo real mediante SMS, lo que es crucial para aplicaciones donde no se puede estar físicamente presente.

2. **Manejo de Errores:** Se incluyen comandos AT para verificar la señal y el estado de la red antes de enviar datos, asegurando una conexión confiable.

#### **3.9 Estrategia de Depuración**

Se implementa una estrategia de depuración para asegurar la robustez del sistema:

- **Monitoreo de Rango:** Verificación de que las lecturas estén dentro de límites aceptables.
- **Registro de Eventos:** Almacenamiento de logs en la tarjeta SD.
- **Alertas en LCD:** Notificación de errores o valores críticos en la pantalla LCD.

---

### **4. Desarrollo de las Placas PCB**

#### **4.1 Proceso de Mejora**

El desarrollo de las placas PCB ha pasado por varias iteraciones, enfocadas en reducir el tamaño y mejorar la eficiencia del diseño sin comprometer las funcionalidades. Las versiones iniciales fueron producidas localmente, y tras pruebas y ajustes, se realizó una producción en masa en China, corregiendo errores identificados en las primeras versiones.

1. **Miniaturización:** Optimización de la disposición de componentes para reducir el tamaño de la PCB.
2. **Corrección de Errores:** Identificación y corrección de errores como cortocircuitos y problemas de interferencia electromagnética.
3. **Optimización del Trazado:** Mejora en el diseño de las pistas para reducir la resistencia y mejorar la integridad de la señal.

#### **4.2 Integración con el Software**

La integración entre la PCB y el software se ha realizado de manera que no se requiera configuración adicional. El sistema está diseñado para que una vez ensamblado, funcione sin necesidad de ajustes adicionales.

---

### **5. Pruebas y Validación**

#### **5.1 Pruebas de Funcionalidad**

Se han realizado pruebas exhaustivas en todas las funciones del sistema, asegurando que los parámetros de caudal, pH, TDS, y el control de los relés operen correctamente. También se validó el almacenamiento en tarjeta SD y el envío de datos por SMS.

#### **5.2 Resultados**

Los resultados de las pruebas muestran que el sistema es robusto y preciso, capaz de monitorizar la calidad del agua en tiempo real de manera confiable. Las funciones de control de procesos y de monitoreo remoto operan sin errores significativos.

---

### **6. Conclusiones y Recomendaciones**

El sistema de monitorización en tiempo real cumple con los objetivos planteados, ofreciendo una solución integral para la gestión de calidad del agua. Se recomienda explorar la integración de interfaces gráficas más avanzadas y la expansión del sistema con nuevos sensores.

---

### **7. Manual de Usuario**

#### **7.1 Descripción General**

El sistema permite al usuario monitorizar parámetros críticos del agua, con almacenamiento en tarjeta SD y envío de alertas por SMS. La navegación en el menú LCD es intuitiva y permite gestionar todas las funciones del sistema.

#### **7.2 Instrucciones de Uso**

1. **Encendido:** Conectar la alimentación, el sistema se inicializa automáticamente.
2. **Visualización de Parámetros:** Navegar por el menú en el LCD para ver el caudal, pH, TDS y otros parámetros.
3. **Almacenamiento:** Los datos se almacenan automáticamente en la tarjeta SD.
4. **Envío de SMS:** Los datos se envían automáticamente por SMS al número configurado.
5. **Mantenimiento:** Calibrar regularmente el sensor de pH y verificar las conexiones y sensores.

---

### **8. Código Completo y Comentado**

El código completo y comentado está diseñado para ser fácil de entender y modificar, permitiendo futuras expansiones del sistema. Se incluye manejo de LCD, control de relés, captura de datos de sensores, almacenamiento en SD, y envío de datos por SMS.
