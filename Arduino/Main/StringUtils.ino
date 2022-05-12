bool compareStrings(const char * str1, const char * str2) {
  return strcmp(str1, str2) == 0;
}

void stringCopy(const char * str1, const char * str2) {
  strcpy(str1, str2);
}

bool isStringEmpty(const char * str) {
  return str[0] == '\0';
}

int8_t getStringLength(const char * str) {
  return strlen(str);
}

char * shift_string_right(const char * str) {
  if (isStringEmpty(str))
    return '\0';

  int8_t str_len = getStringLength(str);
  //char * buf = (char *) malloc (str_len + 1);
  static char buf[100];

  for (int8_t i = 0, j = str_len - 1; i < str_len; i++, j++) {
    if (j >= str_len)
      j = 0;

    buf[i] = str[j];
  }

  buf[str_len] = '\0';
  return buf;
}

char * shift_string_left(const char * str) {
  if (isStringEmpty(str))
    return '\0';

  int8_t str_len = getStringLength(str);
  //char * buf = (char *) malloc (str_len + 1);
  static char buf[100];

  for (int8_t i = 0, j = 1; i < str_len; i++, j++) {
    if (j >= str_len)
      j = 0;

    buf[i] = str[j];
  }

  buf[str_len] = '\0';
  return buf;
}

bool argument_state;
bool command_state;
bool data_state;
bool params_state;

int8_t get_args_count(const char * str) {
  if (isStringEmpty(str))
    return 0;

  int8_t str_len = getStringLength(str);

  int8_t argsCount = 0;

  for (int8_t i = 0; i < str_len; i++) {
    if (str[i] == '\0') {
      break;
    }

    if (is_argument_closed() && is_command_closed() && is_data_closed() && are_params_closed()) {
      if (str[i] == '[') {
        open_argument();
      }

    } else if (is_argument_open() && is_command_closed() && is_data_closed() && are_params_closed()) {
      if (str[i] == '(') {
        open_command();
      } else if (str[i] == '{') {
        open_data();
      } else if (str[i] == '%') {
        open_params();
      } else if (str[i] == ']') {
        close_argument();
        argsCount++;
      }

    } else if (is_argument_open() && is_command_open() && is_data_closed() && are_params_closed()) {
      if (str[i] == ')') {
        close_command();
      }

    } else if (is_argument_open() && is_command_closed() && is_data_open() && are_params_closed()) {
      if (str[i] == '}') {
        close_data();
      }

    } else if (is_argument_open() && is_command_closed() && is_data_closed() && are_params_open()) {
      if (str[i] == '$') {
        close_params();
      }

    }
  }

  return argsCount;
}

char * get_argument(const char * str, const int8_t index) {
  if (isStringEmpty(str))
    return '\0';

  int8_t str_len = getStringLength(str);

  //char * buf = (char *) malloc (str_len + 1);
  static char buf[100];

  int8_t remaining_args = index;

  int8_t j = 0;

  for (int8_t i = 0; i < str_len; i++) {
    if (str[i] == '\0' || remaining_args < 0) {
      close_argument();
      break;
    }

    if (is_argument_closed() && is_command_closed() && is_data_closed() && are_params_closed()) {
      if (str[i] == '[') {
        open_argument();
        remaining_args--;
      }

    } else if (is_argument_open() && is_command_closed() && is_data_closed() && are_params_closed()) {
      if (remaining_args == 0 && str[i] != ']') {
        buf[j] = str[i];
        j++;
      } else if (str[i] == '(') {
        open_command();
      } else if (str[i] == '{') {
        open_data();
      } else if (str[i] == '%') {
        open_params();
      } else if (str[i] == ']') {
        close_argument();
      }

    } else if (is_argument_open() && is_command_open() && is_data_closed() && are_params_closed()) {
      if (str[i] == ')') {
        close_command();
      }

    } else if (is_argument_open() && is_command_closed() && is_data_open() && are_params_closed()) {
      if (str[i] == '}') {
        close_data();
      }

    } else if (is_argument_open() && is_command_closed() && is_data_closed() && are_params_open()) {
      if (str[i] == '$') {
        close_params();
      }

    }
  }

  buf[j] = '\0';

  return buf;
}

char * get_command(const char * str) {
  if (isStringEmpty(str)) {
    return '\0';
  }

  int8_t str_len = getStringLength(str);

  //char * buf = (char *) malloc (str_len + 1);
  static char buf[20];

  int8_t j = 0;

  for (int8_t i = 0; i < str_len; i++) {
    if (str[i] == '\0' || str[i] == ')') {
      close_command();
      break;
    }

    if (is_command_closed() && is_data_closed() && are_params_closed()) {
      if (str[i] == '(') {
        open_command();
      } else if (str[i] == '{') {
        open_data();
      } else if (str[i] == '%') {
        open_params();
      }

    } else if (is_command_open() && is_data_closed() && are_params_closed()) {
      buf[j] = str[i];
      j++;

    } else if (is_command_closed() && is_data_open() && are_params_closed()) {
      if (str[i] == '}') {
        close_data();
      }

    } else if (is_command_closed() && is_data_closed() && are_params_open()) {
      if (str[i] == '$') {
        close_params();
      }

    }
  }

  buf[j] = '\0';

  return buf;
}

char * get_data(const char * str) {
  if (isStringEmpty(str)) {
    return '\0';
  }

  int8_t str_len = getStringLength(str);

  //char * buf = (char *) malloc (str_len + 1);
  static char buf[100];

  int8_t j = 0;

  for (int8_t i = 0; i < str_len; i++) {
    if (str[i] == '\0' || str[i] == '}') {
      close_data();
      break;
    }

    if (is_command_closed() && is_data_closed() && are_params_closed()) {
      if (str[i] == '(') {
        open_command();
      } else if (str[i] == '{') {
        open_data();
      } else if (str[i] == '%') {
        open_params();
      }

    } else if (is_command_open() && is_data_closed() && are_params_closed()) {
      if (str[i] == ')') {
        close_command();
      }

    } else if (is_command_closed() && is_data_open() && are_params_closed()) {
      buf[j] = str[i];
      j++;

    } else if (is_command_closed() && is_data_closed() && are_params_open()) {
      if (str[i] == '$') {
        close_params();
      }

    }
  }

  buf[j] = '\0';

  return buf;
}

char * get_params(const char * str, const int8_t index) {
  if (isStringEmpty(str)) {
    return '\0';
  }

  int8_t str_len = getStringLength(str);

  //char * buf = (char *) malloc (str_len + 1);
  static char buf[5];

  int8_t j = 0;
  int8_t paramCount = index;

  for (int8_t i = 0; i < str_len; i++) {
    if (str[i] == '\0' || str[i] == '$' || paramCount < 0) {
      close_params();
      break;
    }

    if (is_command_closed() && is_data_closed() && are_params_closed()) {
      if (str[i] == '(') {
        open_command();
      } else if (str[i] == '{') {
        open_data();
      } else if (str[i] == '%') {
        open_params();
        paramCount--;
      }

    } else if (is_command_open() && is_data_closed() && are_params_closed()) {
      if (str[i] == ')') {
        close_command();
      }

    } else if (is_command_closed() && is_data_open() && are_params_closed()) {
      if (str[i] == '}') {
        close_data();
      }

    } else if (is_command_closed() && is_data_closed() && are_params_open()) {
      if (str[i] == ',') {
        paramCount--;
      } else if (paramCount == 0) {
        buf[j] = str[i];
        j++;
      }

    }
  }

  buf[j] = '\0';

  return buf;
}

void open_argument() {
  argument_state = true;
}

void close_argument() {
  argument_state = false;
}

bool is_argument_open() {
  return argument_state;
}

bool is_argument_closed() {
  return !argument_state;
}

void open_command() {
  command_state = true;
}

void close_command() {
  command_state = false;
}

bool is_command_open() {
  return command_state;
}

bool is_command_closed() {
  return !command_state;
}

void open_data() {
  data_state = true;
}

void close_data() {
  data_state = false;
}

bool is_data_open() {
  return data_state;
}

bool is_data_closed() {
  return !data_state;
}

void open_params() {
  params_state = true;
}

void close_params() {
  params_state = false;
}

bool are_params_open() {
  return params_state;
}

bool are_params_closed() {
  return !params_state;
}
