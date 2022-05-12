/**

*/
int8_t lcd_get_x() {
  return cursor_x;
}

/**

*/
int8_t lcd_get_y() {
  return cursor_y;
}

/**

*/
void lcd_set_x(const int8_t x) {
  cursor_x = x >= LCD_HORIZONTAL_SIZE ? LCD_HORIZONTAL_SIZE - 1 : x < 0 ? 0 : x;
}

/**

*/
void lcd_set_y(const int8_t y) {
  cursor_y = y >= LCD_VERTICAL_SIZE ? LCD_VERTICAL_SIZE - 1 : y < 0 ? 0 : y;
}

/**

*/
void lcd_set_cursor(const int8_t x, const int8_t y) {
  lcd_set_x(x);
  lcd_set_y(y);

  lcd.setCursor(lcd_get_x(), lcd_get_y());
}
