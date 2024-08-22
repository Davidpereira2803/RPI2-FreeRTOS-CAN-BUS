#ifndef BINARYBLINK_H

void setgpio(uint32_t pin);

void decimaltobinaryblink(int decimalnum, int bits);

void decimaltobinaryblinktask(void *pParam);

void blink( int num);

#endif 

