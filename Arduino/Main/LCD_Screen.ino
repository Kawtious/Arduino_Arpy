void lcd_print_string(const char * str, const int8_t x, const int8_t y) {
  lcd_set_cursor(x, y);

  lcd.print(str);
  lcd_set_cursor(x, y);
}

void lcd_print_string(const __FlashStringHelper * str, const int8_t x, const int8_t y) {
  lcd_set_cursor(x, y);

  lcd.print(str);
  lcd_set_cursor(x, y);
}

void lcd_print_string_scroll(const char * str, const int8_t limitBegin, const int8_t limitEnd, const int8_t y) {
  if (check_horizontal_limit(limitBegin, limitEnd)) {
    lcd_set_cursor(limitBegin, y);

    static char buf[100];
    strcpy(buf, str);

    buf[limitEnd - limitBegin + 1] = '\0';

    lcd.print(buf);
    lcd_set_cursor(limitBegin, y);
  }
}

void lcd_print_char(char msg, const int8_t x, const int8_t y) {
  lcd_set_cursor(x, y);

  if (msg == '\0') {
    msg = '<';
  }

  lcd.print(msg);
  lcd_set_cursor(x, y);
}

void lcd_clear_screen(const int8_t limitBegin, const int8_t limitEnd, const int8_t y) {
  if (check_horizontal_limit_forloop(limitBegin, limitEnd)) {
    lcd_set_y(y);

    for (int8_t i = limitBegin; i < limitEnd; i++) {
      lcd_print_char(' ', i, lcd_get_y());
    }
  }
}

void lcd_clear_screen(const int8_t limitBegin, const int8_t limitEnd, const int8_t limitBeginY, const int8_t limitEndY) {
  if (check_horizontal_limit_forloop(limitBegin, limitEnd) && check_vertical_limit_forloop(limitBeginY, limitEndY)) {
    for (int8_t i = limitBeginY; i < limitEndY; i++) {
      for (int8_t j = limitBegin; j < limitEnd; j++) {
        lcd_print_char(' ', j, i);
      }
    }
  }
}

void lcd_clear_screen() {
  for (int8_t i = 0; i < LCD_VERTICAL_SIZE; i++) {
    for (int8_t j = 0; j < LCD_HORIZONTAL_SIZE; j++) {
      lcd_print_char(' ', j, i);
    }
  }
}

bool check_horizontal_limit(const int8_t limitBegin, const int8_t limitEnd) {
  return limitBegin < limitEnd && limitBegin >= 0 && limitEnd < LCD_HORIZONTAL_SIZE;
}

bool check_vertical_limit(const int8_t startY, const int8_t endY) {
  return startY < endY && startY >= 0 && endY < LCD_VERTICAL_SIZE;
}

bool check_horizontal_limit_forloop(const int8_t limitBegin, const int8_t limitEnd) {
  return limitBegin < limitEnd && limitBegin >= 0 && limitEnd <= LCD_HORIZONTAL_SIZE;
}

bool check_vertical_limit_forloop(const int8_t startY, const int8_t endY) {
  return startY < endY && startY >= 0 && endY <= LCD_VERTICAL_SIZE;
}
