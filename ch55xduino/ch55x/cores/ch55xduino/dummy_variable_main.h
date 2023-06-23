// add a reference to main to pull in main.c
void main(void);
void (*dummy_variable)() = main;
