/* Código hecho por: Daniel Marcial (@danielmarcial22)
   Mejorado por: ChatGPT
   Este código configura cualquier número de botones y potenciómetros.
   Los botones se conectan a partir del pin 2 digital, (2, 3, 4, 5...)
   Los potenciómetros se conectan a partir de A0, A1...
   Para usar el código sólo hay que modificar nBotones, notas[] y nPots. */

// Configuración de botones y potenciómetros
const uint8_t nBotones = 4; // Número de botones
const uint16_t calibracion = 80; // Este valor funciona como una especie de delay

// Inicialización de arrays
bool notaioff[nBotones] = {true, true, true, true};
uint16_t contador[nBotones] = {0, 0, 0, 0};

// El array notas[] debe tener un mismo número de elementos que el número de botones,
// pues cada nota corresponde a un botón.
const uint8_t notas[] = {34, 36, 39, 43}; // Estas son las notas que se dispararán los botones

const uint8_t nPots = 2; // Número de potenciómetros
int16_t lecturas[nPots] = {0, 0};
int16_t lecturasAnteriores[nPots] = {0, 0};

void setup() {
  Serial.begin(115200);
  configurarPines();
}

void loop() {
  manejarBotones();
  manejarPotenciometros();
}

// Configuración de los pines de los botones
void configurarPines() {
  for (uint8_t i = 0; i < nBotones; i++) {
    pinMode(i + 2, INPUT_PULLUP);
  }
}

// Manejo del estado de los botones y envío de notas MIDI
void manejarBotones() {
  for (uint8_t i = 0; i < nBotones; i++) {
    bool botonPresionado = digitalRead(i + 2) == LOW;
    if (botonPresionado) {
      if (contador[i] == 0 && notaioff[i]) {
        activarNota(i);
      }
    } else {
      if (contador[i] == 0 && !notaioff[i]) {
        desactivarNota(i);
      }
    }
    actualizarContador(i);
  }
}

// Activar nota MIDI
void activarNota(uint8_t i) {
  contador[i] = calibracion; // Reiniciar cuenta regresiva
  enviarNotaMIDI(144, notas[i], 100); // Enviar nota ON
  notaioff[i] = false; // Marcar nota como encendida
}

// Desactivar nota MIDI
void desactivarNota(uint8_t i) {
  contador[i] = calibracion; // Reiniciar cuenta regresiva
  enviarNotaMIDI(128, notas[i], 0); // Enviar nota OFF
  notaioff[i] = true; // Marcar nota como apagada
}

// Actualizar contador del botón
void actualizarContador(uint8_t i) {
  if (contador[i] > 0) {
    contador[i]--;
  }
}

// Manejo del estado de los potenciómetros y envío de valores MIDI
void manejarPotenciometros() {
  for (uint8_t k = 0; k < nPots; k++) {
    lecturas[k] = map(analogRead(k), 0, 1023, 0, 255); // Mapear lectura al rango de 0 a 255
    if (abs(lecturas[k] - lecturasAnteriores[k]) > 1) { // Verificar si hubo un cambio significativo
      enviarNotaMIDI(176, k + 30, lecturas[k] / 2); // Enviar valor del potenciómetro a partir del CC 30
      lecturasAnteriores[k] = lecturas[k];
    }
  }
}

// Función para enviar mensajes MIDI
void enviarNotaMIDI(uint8_t command, uint8_t note, uint8_t vel) {
  Serial.write(command);
  Serial.write(note);
  Serial.write(vel);
}
