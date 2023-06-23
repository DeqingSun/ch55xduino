// SDCC doesn't support weak attribute. But function in sketch can override
// function in library. Arduino compiles core as an archive and linked with the
// sketch
void GPIOInterrupt(void) __interrupt {
  // weak
}
