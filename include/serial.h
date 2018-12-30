#ifndef INCLUDE_SERIAL_H
#define INCLUDE_SERIAL_H

void serial_init(void (*char_callback) (char c));
void serial_send_char(char);

#endif
