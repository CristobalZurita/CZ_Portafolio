#include <LiquidCrystal.h>

// Definición de pines para el LCD
const int RS_PIN = 12;
const int EN_PIN = 11;
const int D4_PIN = 4;
const int D5_PIN = 5;
const int D6_PIN = 6;
const int D7_PIN = 7;

// Definición de pines para los sensores
const int FLOW_SENSOR_PIN = 2;
const int TDS_SENSOR_PIN = 3;
const int PH_SENSOR_PIN = A0; // Sensor de pH conectado a una entrada analógica

// Constantes para cálculos
const float E_CONSTANT = 2.71828;
const float LITERS_PER_PULSE = 1.0 / 450;
const int FLOW_CALIBRATION_FACTOR = 73;

// Variables globales
volatile int flowPulses = 0;
volatile int tdsPulses = 0;
unsigned long previousTime = 0;
unsigned long accumulatedPulses = 0;
float totalLiters = 0.0;
float currentPH = 0.0;

// Inicialización del LCD
LiquidCrystal lcd(RS_PIN, EN_PIN, D4_PIN, D5_PIN, D6_PIN, D7_PIN);

// Función de interrupción para el sensor de flujo
void flowISR() {
    flowPulses++;
}

// Función de interrupción para el sensor de TDS
void tdsISR() {
    tdsPulses++;
}

// Configuración inicial
void setup() {
    // Configuración de pines
    pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
    pinMode(TDS_SENSOR_PIN, INPUT_PULLUP);
    pinMode(PH_SENSOR_PIN, INPUT);

    // Inicialización del LCD
    lcd.begin(16, 2);

    // Habilitación de interrupciones
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowISR, RISING);

    // Guardar el tiempo inicial
    previousTime = millis();
}

// Bucle principal
void loop() {
    // Verificar si ha pasado un segundo
    if (millis() - previousTime >= 1000) {
        previousTime = millis();

        // Calcular caudal y litros acumulados
        int litersPerHour = calculateFlowRate();
        totalLiters = calculateTotalLiters();

        // Medir pH
        currentPH = readPH();

        // Medir TDS
        float tdsValue = measureTDS();

        // Mostrar resultados en el LCD
        displayResults(litersPerHour, totalLiters, currentPH, tdsValue);

        // Resetear contadores para la siguiente medición
        resetCounters();
    }
}

// Función para calcular el caudal en litros por hora
int calculateFlowRate() {
    int litersPerHour = (flowPulses * 60) / FLOW_CALIBRATION_FACTOR;
    accumulatedPulses += flowPulses;
    return litersPerHour;
}

// Función para calcular los litros totales acumulados
float calculateTotalLiters() {
    return accumulatedPulses * LITERS_PER_PULSE;
}

// Función para leer el valor de pH
float readPH() {
    int phValue = analogRead(PH_SENSOR_PIN);
    // Convertir la lectura del ADC a un valor de pH (requiere calibración específica)
    float voltage = phValue * (5.0 / 1023.0);
    float ph = 3.5 * voltage + 0.0; // Ejemplo de ecuación de calibración
    return ph;
}

// Función para medir TDS
float measureTDS() {
    tdsPulses = 0;
    attachInterrupt(digitalPinToInterrupt(TDS_SENSOR_PIN), tdsISR, RISING);
    delay(1000); // Esperar un segundo para contar los pulsos
    detachInterrupt(TDS_SENSOR_PIN);
    return 6.64 * pow(E_CONSTANT, 0.0000381 * tdsPulses);
}

// Función para mostrar los resultados en el LCD
void displayResults(int litersPerHour, float totalLiters, float ph, float tds) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("L/H: ");
    lcd.print(litersPerHour);
    lcd.print(" L: ");
    lcd.print(totalLiters);
    
    lcd.setCursor(0, 1);
    lcd.print("pH: ");
    lcd.print(ph);
    lcd.print(" TDS: ");
    lcd.print(tds);
}

// Función para resetear los contadores de pulsos
void resetCounters() {
    flowPulses = 0;
    tdsPulses = 0;
}
