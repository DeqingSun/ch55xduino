/*
 created by Deqing Sun for use with CH55xduino
 need SDCC 13402 or higher version
 */

// dedicated function make main code less big because no xdata is needed in main
void USBSerial_print_i_func(__data long i);
void USBSerial_print_ib_func(__data long i, __xdata uint8_t base);
void USBSerial_print_u_func(__data unsigned long u);
void USBSerial_print_ub_func(__data unsigned long u, __xdata uint8_t base);
void USBSerial_print_s_func(char *__data s);
void USBSerial_print_sn_func(char *__data s, __xdata uint8_t size);
void USBSerial_print_f_func(__data float f);
void USBSerial_print_fd_func(__data float f, __xdata uint8_t digits);

void Serial0_print_i_func(__data long i);
void Serial0_print_ib_func(__data long i, __xdata uint8_t base);
void Serial0_print_u_func(__data unsigned long u);
void Serial0_print_ub_func(__data unsigned long u, __xdata uint8_t base);
void Serial0_print_s_func(char *__data s);
void Serial0_print_sn_func(char *__data s, __xdata uint8_t size);
void Serial0_print_f_func(__data float f);
void Serial0_print_fd_func(__data float f, __xdata uint8_t digits);

void Serial1_print_i_func(__data long i);
void Serial1_print_ib_func(__data long i, __xdata uint8_t base);
void Serial1_print_u_func(__data unsigned long u);
void Serial1_print_ub_func(__data unsigned long u, __xdata uint8_t base);
void Serial1_print_s_func(char *__data s);
void Serial1_print_sn_func(char *__data s, __xdata uint8_t size);
void Serial1_print_f_func(__data float f);
void Serial1_print_fd_func(__data float f, __xdata uint8_t digits);

void printNothing();

// https://stackoverflow.com/a/46222749/2561930
#define USBSerial_print(...) USB_SERIAL_SELECT(__VA_ARGS__)(__VA_ARGS__)
#define USB_SERIAL_SELECT(...)                                                 \
  CONCAT(USB_SERIAL_SELECT_, NARG(__VA_ARGS__))(__VA_ARGS__)
#define USB_SERIAL_SELECT_0() printNothing
#define USB_SERIAL_SELECT_1(_1)                                                \
  _Generic((_1),                                                               \
      char: USBSerial_write,                                                   \
      int: USBSerial_print_i_func,                                             \
      short: USBSerial_print_i_func,                                           \
      long: USBSerial_print_i_func,                                            \
      unsigned char: USBSerial_print_u_func,                                   \
      unsigned int: USBSerial_print_u_func,                                    \
      unsigned short: USBSerial_print_u_func,                                  \
      unsigned long: USBSerial_print_u_func,                                   \
      __code char *: USBSerial_print_s_func,                                   \
      __data char *: USBSerial_print_s_func,                                   \
      __xdata char *: USBSerial_print_s_func,                                  \
      float: USBSerial_print_f_func)
#define USB_SERIAL_SELECT_2(_1, _2)                                            \
  _Generic((_1),                                                               \
      float: _Generic((_2), default: USBSerial_print_fd_func),                 \
      __code char *: _Generic((_2), default: USBSerial_print_sn_func),         \
      __data char *: _Generic((_2), default: USBSerial_print_sn_func),         \
      __xdata char *: _Generic((_2), default: USBSerial_print_sn_func),        \
      char: _Generic((_2), default: USBSerial_print_ib_func),                  \
      int: _Generic((_2), default: USBSerial_print_ib_func),                   \
      short: _Generic((_2), default: USBSerial_print_ib_func),                 \
      long: _Generic((_2), default: USBSerial_print_ib_func),                  \
      unsigned char: _Generic((_2), default: USBSerial_print_ub_func),         \
      unsigned int: _Generic((_2), default: USBSerial_print_ub_func),          \
      unsigned short: _Generic((_2), default: USBSerial_print_ub_func),        \
      unsigned long: _Generic((_2), default: USBSerial_print_ub_func))

/**
 * Print a string to USB serial. This function is done with generic selection to
 * mimic overloading in Arduino C++. So you can put any type of data in the
 * argument list, and it will be printed as a string.
 */
#define USBSerial_println(...)                                                 \
  {                                                                            \
    USBSerial_print(__VA_ARGS__);                                              \
    Print_println(USBSerial_write);                                            \
  }

#define Serial0_print(...) SERIAL0_SELECT(__VA_ARGS__)(__VA_ARGS__)
#define SERIAL0_SELECT(...)                                                    \
  CONCAT(SERIAL0_SELECT_, NARG(__VA_ARGS__))(__VA_ARGS__)
#define SERIAL0_SELECT_0() printNothing
#define SERIAL0_SELECT_1(_1)                                                   \
  _Generic((_1),                                                               \
      char: Serial0_write,                                                     \
      int: Serial0_print_i_func,                                               \
      short: Serial0_print_i_func,                                             \
      long: Serial0_print_i_func,                                              \
      unsigned char: Serial0_print_u_func,                                     \
      unsigned int: Serial0_print_u_func,                                      \
      unsigned short: Serial0_print_u_func,                                    \
      unsigned long: Serial0_print_u_func,                                     \
      __code char *: Serial0_print_s_func,                                     \
      __data char *: Serial0_print_s_func,                                     \
      __xdata char *: Serial0_print_s_func,                                    \
      float: Serial0_print_f_func)
#define SERIAL0_SELECT_2(_1, _2)                                               \
  _Generic((_1),                                                               \
      float: _Generic((_2), default: Serial0_print_fd_func),                   \
      __code char *: _Generic((_2), default: Serial0_print_sn_func),           \
      __data char *: _Generic((_2), default: Serial0_print_sn_func),           \
      __xdata char *: _Generic((_2), default: Serial0_print_sn_func),          \
      char: _Generic((_2), default: Serial0_print_ib_func),                    \
      int: _Generic((_2), default: Serial0_print_ib_func),                     \
      short: _Generic((_2), default: Serial0_print_ib_func),                   \
      long: _Generic((_2), default: Serial0_print_ib_func),                    \
      unsigned char: _Generic((_2), default: Serial0_print_ub_func),           \
      unsigned int: _Generic((_2), default: Serial0_print_ub_func),            \
      unsigned short: _Generic((_2), default: Serial0_print_ub_func),          \
      unsigned long: _Generic((_2), default: Serial0_print_ub_func))
#define Serial0_println(...)                                                   \
  {                                                                            \
    Serial0_print(__VA_ARGS__);                                                \
    Print_println(Serial0_write);                                              \
  }

#define Serial1_print(...) SERIAL1_SELECT(__VA_ARGS__)(__VA_ARGS__)
#define SERIAL1_SELECT(...)                                                    \
  CONCAT(SERIAL1_SELECT_, NARG(__VA_ARGS__))(__VA_ARGS__)
#define SERIAL1_SELECT_0() printNothing
#define SERIAL1_SELECT_1(_1)                                                   \
  _Generic((_1),                                                               \
      char: Serial1_write,                                                     \
      int: Serial1_print_i_func,                                               \
      short: Serial1_print_i_func,                                             \
      long: Serial1_print_i_func,                                              \
      unsigned char: Serial1_print_u_func,                                     \
      unsigned int: Serial1_print_u_func,                                      \
      unsigned short: Serial1_print_u_func,                                    \
      unsigned long: Serial1_print_u_func,                                     \
      __code char *: Serial1_print_s_func,                                     \
      __data char *: Serial1_print_s_func,                                     \
      __xdata char *: Serial1_print_s_func,                                    \
      float: Serial1_print_f_func)
#define SERIAL1_SELECT_2(_1, _2)                                               \
  _Generic((_1),                                                               \
      float: _Generic((_2), default: Serial1_print_fd_func),                   \
      __code char *: _Generic((_2), default: Serial1_print_sn_func),           \
      __data char *: _Generic((_2), default: Serial1_print_sn_func),           \
      __xdata char *: _Generic((_2), default: Serial1_print_sn_func),          \
      char: _Generic((_2), default: Serial1_print_ib_func),                    \
      int: _Generic((_2), default: Serial1_print_ib_func),                     \
      short: _Generic((_2), default: Serial1_print_ib_func),                   \
      long: _Generic((_2), default: Serial1_print_ib_func),                    \
      unsigned char: _Generic((_2), default: Serial1_print_ub_func),           \
      unsigned int: _Generic((_2), default: Serial1_print_ub_func),            \
      unsigned short: _Generic((_2), default: Serial1_print_ub_func),          \
      unsigned long: _Generic((_2), default: Serial1_print_ub_func))
#define Serial1_println(...)                                                   \
  {                                                                            \
    Serial1_print(__VA_ARGS__);                                                \
    Print_println(Serial1_write);                                              \
  }

#define CONCAT(X, Y) CONCAT_(X, Y)
#define CONCAT_(X, Y) X##Y

#define ARGN(...) ARGN_(__VA_ARGS__)
#define ARGN_(_0, _1, _2, N, ...) N

#define NARG(...) ARGN(__VA_ARGS__ COMMA(__VA_ARGS__) 3, 2, 1, 0)
#define HAS_COMMA(...) ARGN(__VA_ARGS__, 1, 1, 0)

#define SET_COMMA(...) ,

#define COMMA(...)                                                             \
  SELECT_COMMA(HAS_COMMA(__VA_ARGS__), HAS_COMMA(__VA_ARGS__()),               \
               HAS_COMMA(SET_COMMA __VA_ARGS__),                               \
               HAS_COMMA(SET_COMMA __VA_ARGS__()))

#define SELECT_COMMA(_0, _1, _2, _3) SELECT_COMMA_(_0, _1, _2, _3)
#define SELECT_COMMA_(_0, _1, _2, _3) COMMA_##_0##_1##_2##_3

#define COMMA_0000 ,
#define COMMA_0001
#define COMMA_0010 ,
#define COMMA_0011 ,
#define COMMA_0100 ,
#define COMMA_0101 ,
#define COMMA_0110 ,
#define COMMA_0111 ,
#define COMMA_1000 ,
#define COMMA_1001 ,
#define COMMA_1010 ,
#define COMMA_1011 ,
#define COMMA_1100 ,
#define COMMA_1101 ,
#define COMMA_1110 ,
#define COMMA_1111 ,
