#ifndef WiringDigitalFast_h
#define WiringDigitalFast_h

// To stringify the result of expansion of a macro argument, two levels of
// macros must be used

#define XpinModeFast(__PORT, __PIN, __MODE)                                    \
  (__MODE == INPUT                                                             \
       ? (P##__PORT##_MOD_OC &= ~(1 << __PIN),                                 \
          P##__PORT##_DIR_PU &= ~(1 << __PIN))                                 \
       : (__MODE == INPUT_PULLUP                                               \
              ? (P##__PORT##_MOD_OC |= (1 << __PIN),                           \
                 P##__PORT##_DIR_PU |= (1 << __PIN))                           \
              : (__MODE == OUTPUT                                              \
                     ? (P##__PORT##_MOD_OC &= ~(1 << __PIN),                   \
                        P##__PORT##_DIR_PU |= (1 << __PIN))                    \
                     : (__MODE == OUTPUT_OD                                    \
                            ? (P##__PORT##_MOD_OC |= (1 << __PIN),             \
                               P##__PORT##_DIR_PU &= ~(1 << __PIN))            \
                            : (0)))))

#define pinModeFast(__PORT, __PIN, __MODE) XpinModeFast(__PORT, __PIN, __MODE)

#define XdigitalWriteFast(__PORT, __PIN, __LEVEL) P##__PORT##_##__PIN = __LEVEL

#define digitalWriteFast(__PORT, __PIN, __LEVEL)                               \
  XdigitalWriteFast(__PORT, __PIN, __LEVEL)

#define XdigitalReadFast(__PORT, __PIN) P##__PORT##_##__PIN

#define digitalReadFast(__PORT, __PIN) XdigitalReadFast(__PORT, __PIN)

#endif
