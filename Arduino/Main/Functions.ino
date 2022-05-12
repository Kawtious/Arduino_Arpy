const int16_t tonos[] = {261, 494};

#define tono_h                     261

#define nTonos                   sizeof(tonos) / sizeof(int16_t)

// Estado del zumbador. LOW = apagado, HIGH = encendido
int estadoZum = HIGH;



/*
  Cada vez que se invoca a esta funcion, la funcion emite un
  tono diferente o apaga el zumbador.
*/
void emiteTonos() {
  // Las variables estaticas conservan el valor entre llamadas
  // a la funcion
  static int tono = 0;

  if (pausa.update()) {
    if (estadoZum == HIGH) {

      // Genera un tono de tono KHz
      tone(PIN_ZUM, tonos[tono]);

      if (tono < nTonos - 1) {
        // Establece el siguiente tono
        tono++;
      } else {
        // Si ya se terminó la secuencia de tonos
        tono = 0;
        estadoZum = LOW;
      }

    } else {
      // Apaga el zumbador
      noTone(PIN_ZUM);
      estadoZum = HIGH;
    }
  }
}

/*
  Cada vez que se invoca a esta funcion, la funcion emite un
  tono diferente o apaga el zumbador.
*/
void hour_clock(const int8_t param) {
  int8_t i = 0;

  while (i <= param) {
    if (estadoZum == HIGH) {
      // Genera un tono de tono KHz
      tone(PIN_ZUM, tono_h);

      if (pausa.update()) {
        estadoZum = LOW;
      }

    } else {
      // Apaga el zumbador
      noTone(PIN_ZUM);
      estadoZum = HIGH;
      delay(100);
      i++;
    }
  }

  estadoZum = HIGH;
  // Apaga el zumbador
  noTone(PIN_ZUM);
}
