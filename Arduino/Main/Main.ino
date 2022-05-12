// include these librarbies :)
#include <LiquidCrystal.h>
#include <NoDelay.h>

#define BAUD_RATE           9600

//-------------------------------------------------

// Crea una instancia de la clase noDelay
// que determina si han transcurrido PERIODO ms
noDelay pausa(1000);
noDelay scroll_delay(500);

//-------------------------------------------------

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
#define REG_SEL                  13
#define ENABLE                   12
#define DB4                      5
#define DB5                      4
#define DB6                      3
#define DB7                      2

LiquidCrystal lcd(REG_SEL, ENABLE, DB4, DB5, DB6, DB7);

#define LCD_HORIZONTAL_SIZE      16
#define LCD_VERTICAL_SIZE        2

uint8_t cursor_x;
uint8_t cursor_y;

//-------------------------------------------------

#define PIN_ZUM                  9

//-------------------------------------------------

void setup() {
  Serial.begin(BAUD_RATE);

  pinMode(PIN_ZUM, OUTPUT);

  // set up the LCD's number of columns and rows:
  lcd.begin(LCD_HORIZONTAL_SIZE, LCD_VERTICAL_SIZE);

  lcd_print_string(F("Waiting for"), 0, 0);
  lcd_print_string(F("Python..."), 0, 1);
}

void loop() {
  run_arpy();
}

void run_arpy() {
  static int8_t function_to_call = -1;
  static int8_t arguments = 0;
  static int8_t current_arg = 1;

  static char * ser_input;

  static bool performing_tests = false;

  if (performing_tests) {
    //ser_input = "a[(PY_PRNT_CLSC)]b[(PY[_DSPL_TEXT){ye[{}%0,4,5,2$]]";
    //ser_input = "[(PY_PRNT_CLSC)][(PY_PRNT_TEXT){Alert, Mae!}%0,0$][(PY_PRNT_TEXT){Hi!}%0,1$]";
    //ser_input = "[(PY_PRNT_CLSC)][(PY_PRNT_TEXT){foo bar baz qux quux quuz}%0,0$][(PY_PRNT_TEXT){Hi!}%0,1$]";
    //ser_input = "[(PY_PRNT_CLSC)]";
    //ser_input = "[(PY_ALRM_HOUR){}%2$]";
    ser_input = "[(PY_PRNT_SCRL){foo bar baz}%0,5,0$]";

    debug_arpy_input(ser_input, 1);
    //execute_function(3, "foo bar baz qux quux quuz", "0", "4", "0");

  } else {
    static char ser_argument[100];
    static char ser_command[20];
    static char ser_data[100];
    static char ser_param1[5];
    static char ser_param2[5];
    static char ser_param3[5];

    if (arguments <= 0) {

      // Format: "[(COMMAND){text}%param1,param2,...$][..."
      ser_input = read_serial();

      if (!isStringEmpty(ser_input)) {
        current_arg = 1;
        arguments = get_args_count(ser_input);
      }

    } else {
      if (!isStringEmpty(ser_input)) {
        strcpy(ser_argument, get_argument(ser_input, current_arg));
        strcpy(ser_command, get_command(ser_argument));
        strcpy(ser_data, get_data(ser_argument));
        strcpy(ser_param1, get_params(ser_argument, 1));
        strcpy(ser_param2, get_params(ser_argument, 2));
        strcpy(ser_param3, get_params(ser_argument, 3));

        if (compareStrings(ser_command, "PY_PRNT_TEXT")) {
          lcd_print_string(ser_data, atoi(ser_param1), atoi(ser_param2));
        } else if (compareStrings(ser_command, "PY_PRNT_SCRL")) {
          function_to_call = 3;
        } else if (compareStrings(ser_command, "PY_PRNT_CLSC")) {
          lcd_clear_screen();
        } else if (compareStrings(ser_command, "PY_ALRM_PLAY")) {
          function_to_call = 2;
        } else if (compareStrings(ser_command, "PY_ALRM_STOP")) {
          // Apaga el zumbador
          noTone(PIN_ZUM);
          function_to_call = -1;
        } else if (compareStrings(ser_command, "PY_ALRM_HOUR")) {
          hour_clock(atoi(ser_param1));
        }
      }

      current_arg++;
      arguments--;
    }

    execute_function(function_to_call, ser_data, ser_param1, ser_param2, ser_param3);
  }
}

char * read_serial() {
  static char comando[200];

  // Si hay caracteres disponibles para lectura en el puerto serie
  if (Serial.available() > 0) {
    // Lee a lo mas 49 caracteres del puero serie o hasta que se
    // presione la tecla Enter y los guarda en el arreglo comando.
    int8_t n = Serial.readBytesUntil('\n', comando, 199);

    // Todas las cadenas en C/C++ terminan en el carácter de fin
    // de cadena, ‘\n’.
    comando[n] = '\0';

    Serial.println(comando);

    return comando;
  }

  return '\0';
}

void execute_function(const int8_t function, const char * data_, const char * param1_, const char * param2_, const char * param3_) {
  switch (function) {

    case -1:
      break;

    case 0:
      break;

    case 1:
      break;

    case 2:
      emiteTonos();
      break;

    case 3:
      static char previous_str[100];
      static char data_shift_left[100];

      if (scroll_delay.update()) {
        if (isStringEmpty(data_shift_left) || !compareStrings(previous_str, data_)) {
          strcpy(previous_str, data_);
          strcpy(data_shift_left, previous_str);
        } else {
          strcpy(data_shift_left, shift_string_left(data_shift_left));
        }

        lcd_print_string_scroll(data_shift_left, atoi(param1_), atoi(param2_), atoi(param3_));
      }
      break;

    // do nothing
    default:
      break;
  }
}

void debug_arpy_input(const char * ser_input, const int8_t current_arg) {
  if (isStringEmpty(ser_input))
    return;

  int8_t arguments = get_args_count(ser_input);
  static char ser_argument[100];
  static char ser_command[20];
  static char ser_data[100];
  static char ser_param1[5];
  static char ser_param2[5];
  static char ser_param3[5];

  strcpy(ser_argument, get_argument(ser_input, current_arg));
  strcpy(ser_command, get_command(ser_argument));
  strcpy(ser_data, get_data(ser_argument));
  strcpy(ser_param1, get_params(ser_argument, 1));
  strcpy(ser_param2, get_params(ser_argument, 2));
  strcpy(ser_param3, get_params(ser_argument, 3));

  static char ser_data_shift_right[100];
  if (isStringEmpty(ser_data_shift_right))
    strcpy(ser_data_shift_right, ser_data);

  strcpy(ser_data_shift_right, shift_string_right(ser_data_shift_right));

  static char ser_data_shift_left[100];
  if (isStringEmpty(ser_data_shift_left))
    strcpy(ser_data_shift_left, ser_data);

  strcpy(ser_data_shift_left, shift_string_left(ser_data_shift_left));

  Serial.print(F("Input: "));
  Serial.println(ser_input);

  Serial.print(F("Argument ("));
  Serial.print(current_arg);
  Serial.print(F("/"));
  Serial.print(arguments);
  Serial.print(F("): "));
  Serial.println(ser_argument);

  Serial.print(F("Command: "));
  Serial.println(ser_command);

  Serial.print(F("Data: "));
  Serial.println(ser_data);

  Serial.print(F("Shift data right: "));
  Serial.println(ser_data_shift_right);

  Serial.print(F("Shift data left: "));
  Serial.println(ser_data_shift_left);

  Serial.print(F("Param 1: "));
  Serial.println(ser_param1);

  Serial.print(F("Param 2: "));
  Serial.println(ser_param2);

  Serial.print(F("Param 3: "));
  Serial.println(ser_param3);

  Serial.println();
}
