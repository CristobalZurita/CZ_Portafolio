<h1 style="text-align: center;">Manual de Usuario Técnico para el Sistema de Purificación de Agua Aqua Andes</h1>

## 1. Introducción
Este documento técnico describe la instalación, configuración y operación del sistema de purificación de agua Aqua Andes. Este sistema está equipado con sensores de caudal, sensores de TDS (Total Dissolved Solids), un sensor de pH, un sensor de lluvia, un sensor de fugas, un multiplexor CD4051, un módulo de comunicación SIM900, un sistema de almacenamiento basado en tarjeta SD y una pantalla LCD 16x2. El sistema Aqua Andes está diseñado para monitorizar la calidad del agua en tiempo real, permitiendo el control y la gestión eficiente del flujo de agua, y asegurando la integridad del proceso de purificación.

## 2. Descripción del Sistema

### 2.1. Pantalla LCD 16x2
La pantalla LCD 16x2 es el principal dispositivo de salida del sistema Aqua Andes. Proporciona información en tiempo real sobre el caudal de entrada y salida, los valores de TDS y el pH del agua.

### 2.2. Módulo SIM900
El módulo SIM900 es un dispositivo de comunicación GSM/GPRS que permite enviar alertas SMS. Este módulo es crucial para las alertas de seguridad y el monitoreo remoto del sistema Aqua Andes.

### 2.3. Tarjeta SD
La tarjeta SD se utiliza para registrar y almacenar datos de operación del sistema Aqua Andes. Estos datos incluyen registros continuos de caudal, TDS, pH y nivel de lluvia, facilitando un análisis posterior y la trazabilidad del proceso.

### 2.4. Sensores

#### 2.4.1. Sensores de Caudal
Dos sensores de caudal se emplean para monitorizar el flujo de agua de entrada y salida, midiendo el volumen de agua en litros por hora. Esto es esencial para detectar pérdidas de agua y garantizar la eficiencia del sistema Aqua Andes.

#### 2.4.2. Sensores de TDS
El sistema Aqua Andes incluye tres sensores de TDS que miden la concentración de sólidos disueltos en el agua en diferentes etapas del proceso de purificación. Estos sensores están conectados al multiplexor CD4051.

#### 2.4.3. Sensor de pH
El sensor de pH mide la acidez o alcalinidad del agua, lo cual es crucial para asegurar que el agua purificada por Aqua Andes sea segura para su uso. Este sensor también está conectado al multiplexor CD4051.

#### 2.4.4. Sensor de Lluvia
El sensor de lluvia detecta la presencia de precipitaciones, lo que puede afectar la operación del sistema Aqua Andes. Este sensor está conectado al multiplexor CD4051.

#### 2.4.5. Sensor de Fugas
El sensor de fugas detecta la presencia de agua en lugares no deseados, activando una alerta que se comunica mediante el módulo SIM900.

### 2.5. Relés
Los relés son dispositivos electromecánicos que permiten controlar el encendido y apagado de varios componentes del sistema Aqua Andes:

- **Relé de Energía (Pin 8):** Controla el encendido y apagado del sistema.
- **Relé de Purga (Pin 9):** Controla el vaciado del tanque de agua.
- **Relé de Bomba de Entrada (Pin 10):** Controla la bomba que introduce agua en el sistema.
- **Relé de Bomba de Salida (Pin 11):** Controla la bomba que expulsa el agua purificada.

_[INSERTAR FOTO DE RELÉS CON CONEXIONES]_

## 3. Instalación del Sistema

### 3.1. Conexiones de Hardware

#### 3.1.1. Conexión del Multiplexor CD4051
Conecta los pines del multiplexor CD4051 a los siguientes pines del microcontrolador:

- **S0 (Pin 2)**
- **S1 (Pin 3)**
- **S2 (Pin 4)**
- **Señal (Pin A0)**

_[INSERTAR DIAGRAMA DE CONEXIÓN DEL CD4051]_

#### 3.1.2. Conexión de los Sensores de Caudal
Conecta el sensor de caudal de entrada al pin 5 y el sensor de caudal de salida al pin 6 del microcontrolador. Asegúrate de que los sensores estén instalados en las tuberías correspondientes y que no haya fugas en las conexiones.

_[INSERTAR DIAGRAMA DE CONEXIÓN DE SENSORES DE CAUDAL]_

#### 3.1.3. Conexión de los Sensores al CD4051
Conecta los sensores de TDS, pH y lluvia a las entradas del multiplexor CD4051. La salida común del multiplexor se conecta al pin A0 del microcontrolador.

_[INSERTAR DIAGRAMA DE CONEXIÓN DE SENSORES AL CD4051]_

#### 3.1.4. Conexión del Sensor de Fugas
Conecta el sensor de fugas al pin 7. Ubica el sensor en la parte inferior del sistema Aqua Andes, donde es más probable que se acumulen fugas.

_[INSERTAR DIAGRAMA DE CONEXIÓN DE SENSOR DE FUGAS]_

#### 3.1.5. Conexión de Relés
Conecta los relés a los pines 8, 9, 10 y 11, asegurándote de que las cargas controladas estén correctamente conectadas y que los relés puedan manejar la corriente necesaria para los dispositivos controlados.

_[INSERTAR DIAGRAMA DE CONEXIÓN DE RELÉS]_

#### 3.1.6. Conexión del Módulo SIM900
Conecta el módulo SIM900 a los pines 12 (RX) y 13 (TX). Inserta una tarjeta SIM válida en el módulo y asegúrate de que esté bien conectada a la red GSM.

_[INSERTAR DIAGRAMA DE CONEXIÓN DEL MÓDULO SIM900]_

#### 3.1.7. Conexión de la Tarjeta SD
Inserta la tarjeta SD en su módulo correspondiente y conéctalo al pin A1 para la lectura y escritura de datos. Verifica que la tarjeta SD esté formateada correctamente y que tenga suficiente espacio libre.

_[INSERTAR DIAGRAMA DE CONEXIÓN DE LA TARJETA SD]_

### 3.2. Configuración del Software

- **Instalación del Entorno de Desarrollo (IDE):**
  - Descarga e instala el Arduino IDE desde arduino.cc.
  - Conecta el microcontrolador al computador mediante un cable USB.

- **Carga del Código:**
  - Abre el archivo de código fuente en el Arduino IDE.
  - Verifica que el código esté libre de errores seleccionando "Verify/Compile".
  - Carga el código al microcontrolador utilizando la opción "Upload".

- **Configuración de la Tarjeta SIM:**
  - Asegúrate de que la tarjeta SIM insertada en el módulo SIM900 esté activada y tenga saldo suficiente.
  - Modifica el número de teléfono de destino en el código fuente para recibir las alertas SMS.

_[INSERTAR CAPTURA DE PANTALLA DEL ARDUINO IDE CON EL CÓDIGO CARGADO]_

_[INSERTAR FOTO DE LA CONFIGURACIÓN DEL MÓDULO SIM900]_

## 4. Operación del Sistema

### 4.1. Inicio del Sistema

- **Encendido:**  
  Activa el relé de energía (Pin 8) para encender el sistema Aqua Andes. La pantalla LCD mostrará los valores iniciales.

_[INSERTAR FOTO DE PANTALLA LCD AL INICIO]_

- **Visualización de Datos:**  
  La pantalla LCD mostrará en tiempo real los valores de caudal, pH y TDS. Los valores se actualizan cada segundo.

_[INSERTAR FOTO DE DATOS EN LA PANTALLA LCD]_

- **Almacenamiento de Datos:**  
  Los datos se almacenan automáticamente en la tarjeta SD. Puedes retirar la tarjeta SD para revisar los registros en un computador.

_[INSERTAR FOTO DE LA LECTURA DE DATOS DESDE LA TARJETA SD]_

### 4.2. Envío de Alertas SMS

- **Configuración de Envío:**  
  El sistema Aqua Andes enviará automáticamente un SMS con los datos de operación al número configurado en caso de detección de anormalidades o en intervalos regulares.

_[INSERTAR FOTO DE SMS RECIBIDO EN EL TELÉFONO]_

- **Alerta de Fuga:**  
  Si el sensor de fugas detecta agua en áreas no deseadas, se enviará un SMS de alerta al número predefinido.

_[INSERTAR FOTO DE SMS DE ALERTA DE FUGA]_

### 4.3. Control de Relés

- **Control de la Bomba de Entrada y Salida:**  
  Los relés correspondientes controlan la activación de las bombas de entrada y salida de agua. Estos relés se activan automáticamente según las necesidades del sistema Aqua Andes.

_[INSERTAR FOTO DE CONTROL DE RELÉS]_

- **Purga del Estanque:**  
  El relé de purga (Pin 9) controla el vaciado del estanque de agua cuando se requiere. Esto puede realizarse de forma manual o automática.

_[INSERTAR FOTO DE ACCIÓN DE PURGA]_

## 5. Mantenimiento del Sistema

### 5.1. Inspección Regular

- **Verificación de Sensores:**  
  Inspecciona regularmente los sensores de caudal, TDS, pH y lluvia para asegurarte de que estén limpios y funcionando correctamente.

_[INSERTAR FOTO DE INSPECCIÓN DE SENSORES]_

- **Limpieza del Sistema:**  
  Realiza una limpieza completa del sistema Aqua Andes cada tres meses para evitar acumulación de sedimentos.

_[INSERTAR FOTO DE LIMPIEZA DEL SISTEMA]_

### 5.2. Sustitución de Componentes

- **Reemplazo de Sensores:**  
  Los sensores deben reemplazarse periódicamente según las recomendaciones del fabricante o si muestran signos de desgaste o lecturas inconsistentes.

_[INSERTAR FOTO DE REEMPLAZO DE SENSORES]_

- **Actualización de la Tarjeta SD:**  
  Sustituye la tarjeta SD si está llena o presenta errores de lectura/escritura. Formatea la nueva tarjeta antes de su uso.

_[INSERTAR FOTO DE TARJETA SD NUEVA]_

## 6. Solución de Problemas

### 6.1. Diagnóstico de Errores Comunes

- **Pantalla LCD Sin Datos:**  
  Verifica la conexión de los pines del LCD y asegura que el sistema esté encendido.

_[INSERTAR DIAGRAMA DE VERIFICACIÓN DE CONEXIONES DEL LCD]_

- **No se Guardan Datos en la Tarjeta SD:**  
  Asegúrate de que la tarjeta SD esté correctamente insertada y no esté protegida contra escritura.

_[INSERTAR FOTO DE VERIFICACIÓN DE TARJETA SD]_

- **No se Envía SMS:**  
  Verifica que el módulo SIM900 esté conectado correctamente y que la tarjeta SIM tenga señal y saldo.

_[INSERTAR FOTO DE DIAGNÓSTICO DEL MÓDULO SIM900]_

### 6.2. Verificación de Conexiones

- **Comprobación de Conexiones Eléctricas:**  
  Revisa todas las conexiones con un multímetro para asegurarte de que no haya cortocircuitos o conexiones sueltas.

_[INSERTAR FOTO DEL USO DE MULTÍMETRO PARA COMPROBACIÓN]_

- **Revisión del Código:**  
  Revisa el código fuente para asegurarte de que no haya errores de programación. Vuelve a cargar el código si es necesario.

_[INSERTAR FOTO DE REVISIÓN DEL CÓDIGO]_
