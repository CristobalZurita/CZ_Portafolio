// Cristóbal Zurita Quintanilla
// Desarrollador y Programador 
// 2018, Santiago, Chile.
// cristobal.z.q@gmail.com

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <SD.h>

// Inicialización del LCD con I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Dirección I2C, columnas, filas

// Definición de pines para sensores, relés y otros componentes
const int CD4051_S0 = 2;  // Pin de selección S0 para CD4051
const int CD4051_S1 = 3;  // Pin de selección S1 para CD4051
const int CD4051_S2 = 4;  // Pin de selección S2 para CD4051
const int CD4051_SIGNAL = A0;  // Pin de señal analógica del CD4051

const int FLOW_SENSOR_IN_PIN = 5;   // Sensor de flujo de entrada
const int FLOW_SENSOR_OUT_PIN = 6;  // Sensor de flujo de salida
const int LEAK_SENSOR_PIN = 7;      // Sensor de fugas

const int RELAY_POWER_PIN = 8;     // Relé de energía principal
const int RELAY_PURGE_PIN = 9;     // Relé de purga
const int RELAY_PUMP_IN_PIN = 10;  // Relé de bomba de entrada
const int RELAY_PUMP_OUT_PIN = 11; // Relé de bomba de salida

const int SIM900_RX_PIN = 12;  // Pin RX para SIM900
const int SIM900_TX_PIN = 13;  // Pin TX para SIM900

const int SD_CHIP_SELECT = A1;  // Pin de selección para la tarjeta SD

// Constantes y factores de calibración
const float LITERS_PER_PULSE = 1.0 / 450.0;
const int FLOW_CALIBRATION_FACTOR = 73;
const float TDS_FACTOR = 0.5;  // Factor de conversión para TDS
const float PH_OFFSET = 0.0;   // Offset de calibración para pH

// Estructura de datos para los sensores multiplexados
struct MultiplexedSensor {
  int channel;
  float value;
  const char* name;
};

MultiplexedSensor multiplexedSensors[] = {
  {0, 0.0, "TDS1"},
  {1, 0.0, "TDS2"},
  {2, 0.0, "TDS3"},
  {3, 0.0, "pH"},
  {4, 0.0, "Rain"}
};

// Estructura de datos para los relés
struct Relay {
  int pin;
  bool state;
  const char* name;
};

Relay relays[] = {
  {RELAY_POWER_PIN, false, "Power"},
  {RELAY_PURGE_PIN, false, "Purge"},
  {RELAY_PUMP_IN_PIN, false, "Pump In"},
  {RELAY_PUMP_OUT_PIN, false, "Pump Out"}
};

// Variables locales
volatile unsigned long flowPulsesIn = 0;
volatile unsigned long flowPulsesOut = 0;
unsigned long previousMillis = 0;
const unsigned long interval = 1000;  // Intervalo de medición en milisegundos

// Configuración de comunicación serial para el SIM900
SoftwareSerial sim900(SIM900_RX_PIN, SIM900_TX_PIN);

void setup() {
  // Inicialización de la comunicación serial
  Serial.begin(9600);
  
  // Inicialización del LCD
  lcd.init();
  lcd.backlight();
  lcd.print("Iniciando...");

  // Configuración de pines
  pinMode(CD4051_S0, OUTPUT);
  pinMode(CD4051_S1, OUTPUT);
  pinMode(CD4051_S2, OUTPUT);
  pinMode(LEAK_SENSOR_PIN, INPUT_PULLUP);
  
  for (auto &relay : relays) {
    pinMode(relay.pin, OUTPUT);
    digitalWrite(relay.pin, LOW);  // Inicialmente apagados
  }

  // Configuración de interrupciones para los sensores de flujo
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_IN_PIN), flowPulseIn, RISING);
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_OUT_PIN), flowPulseOut, RISING);

  // Inicialización del módulo SIM900
  sim900.begin(19200);
  delay(20000);  // Espera para que el módulo se conecte a la red
  sendATCommand("AT", 1000);
  sendATCommand("AT+CMGF=1", 1000);  // Configurar modo texto para SMS

  // Inicialización de la tarjeta SD
  if (!SD.begin(SD_CHIP_SELECT)) {
    Serial.println(F("Error al inicializar la tarjeta SD"));
    lcd.clear();
    lcd.print("Error SD");
  } else {
    Serial.println(F("Tarjeta SD inicializada correctamente"));
    lcd.clear();
    lcd.print("SD OK");
  }

  delay(2000);
  lcd.clear();
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Leer todos los sensores multiplexados
    for (auto &sensor : multiplexedSensors) {
      sensor.value = readMultiplexedSensor(sensor.channel);
    }

    // Calcular caudales
    float flowRateIn = calculateFlowRate(flowPulsesIn);
    float flowRateOut = calculateFlowRate(flowPulsesOut);

    // Mostrar datos en LCD
    displayData(flowRateIn, flowRateOut);

    // Registrar datos en la tarjeta SD
    logData(flowRateIn, flowRateOut);

    // Enviar datos por SMS (por ejemplo, cada hora)
    static unsigned long lastSMSTime = 0;
    if (currentMillis - lastSMSTime >= 3600000) {  // 3600000 ms = 1 hora
      sendDataSMS(flowRateIn, flowRateOut);
      lastSMSTime = currentMillis;
    }

    // Resetear contadores de pulsos
    flowPulsesIn = 0;
    flowPulsesOut = 0;
  }

  // Verificar fugas
  if (digitalRead(LEAK_SENSOR_PIN) == LOW) {
    handleLeak();
  }

  // Aquí se pueden agregar más lógicas de control, como activar/desactivar bombas basado en niveles de TDS, pH, etc.
}

float readMultiplexedSensor(int channel) {
  digitalWrite(CD4051_S0, channel & 0x01);
  digitalWrite(CD4051_S1, (channel >> 1) & 0x01);
  digitalWrite(CD4051_S2, (channel >> 2) & 0x01);
  delay(10);  // Pequeña pausa para estabilización
  int rawValue = analogRead(CD4051_SIGNAL);
  
  if (channel < 3) {  // TDS sensors
    return convertToTDS(rawValue);
  } else if (channel == 3) {  // pH sensor
    return convertToPH(rawValue);
  } else {  // Rain sensor
    return map(rawValue, 0, 1023, 0, 100);  // Map to 0-100%
  }
}

float convertToTDS(int rawValue) {
  float voltage = rawValue * (5.0 / 1024.0);
  return (133.42 * voltage * voltage * voltage - 255.86 * voltage * voltage + 857.39 * voltage) * TDS_FACTOR;
}

float convertToPH(int rawValue) {
  float voltage = rawValue * (5.0 / 1024.0);
  return 3.5 * voltage + PH_OFFSET;
}

float calculateFlowRate(unsigned long pulses) {
  return (pulses * 60.0 * 60.0 / FLOW_CALIBRATION_FACTOR) * LITERS_PER_PULSE;
}

void displayData(float flowRateIn, float flowRateOut) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("In:");
  lcd.print(flowRateIn, 1);
  lcd.print(" Out:");
  lcd.print(flowRateOut, 1);
  
  lcd.setCursor(0, 1);
  lcd.print("pH:");
  lcd.print(multiplexedSensors[3].value, 2);
  lcd.print(" TDS:");
  lcd.print(multiplexedSensors[0].value, 0);
}

void logData(float flowRateIn, float flowRateOut) {
  File dataFile = SD.open("waterlog.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.print(millis());
    dataFile.print(",");
    dataFile.print(flowRateIn);
    dataFile.print(",");
    dataFile.print(flowRateOut);
    for (auto &sensor : multiplexedSensors) {
      dataFile.print(",");
      dataFile.print(sensor.value);
    }
    dataFile.println();
    dataFile.close();
  } else {
    Serial.println(F("Error al abrir el archivo de registro"));
  }
}

void sendDataSMS(float flowRateIn, float flowRateOut) {
  sim900.println("AT+CMGS=\"+1234567890\"");  // Reemplazar con el número de destino
  delay(1000);
  sim900.print("Flow In: ");
  sim900.print(flowRateIn);
  sim900.print(" L/h, Out: ");
  sim900.print(flowRateOut);
  sim900.print(" L/h, pH: ");
  sim900.print(multiplexedSensors[3].value);
  sim900.print(", TDS: ");
  sim900.print(multiplexedSensors[0].value);
  sim900.write(26);  // Ctrl+Z para enviar
  delay(1000);
}

void handleLeak() {
  // Apagar todas las bombas
  digitalWrite(RELAY_PUMP_IN_PIN, LOW);
  digitalWrite(RELAY_PUMP_OUT_PIN, LOW);
  
  // Mostrar alerta en LCD
  lcd.clear();
  lcd.print("FUGA DETECTADA!");
  
  // Enviar alerta por SMS
  sendATCommand("AT+CMGS=\"+1234567890\"", 1000);  // Reemplazar con el número de destino
  sim900.println("ALERTA: Fuga de agua detectada!");
  sim900.write(26);
  
  // Esperar a que se resuelva la fuga (se podría implementar un botón de reinicio)
  while(digitalRead(LEAK_SENSOR_PIN) == LOW) {
    delay(1000);
  }
}

void sendATCommand(const char* command, int timeout) {
  sim900.println(command);
  long int time = millis();
  while((time+timeout) > millis()) {
    while(sim900.available()) {
      Serial.write(sim900.read());
    }
  }
}

void flowPulseIn() {
  flowPulsesIn++;
}

void flowPulseOut() {
  flowPulsesOut++;
}
   