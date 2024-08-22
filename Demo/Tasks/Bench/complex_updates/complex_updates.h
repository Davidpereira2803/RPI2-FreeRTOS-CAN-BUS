#ifndef COMPLEX_UPDATES_H
#define COMPLEX_UPDATES_H

#define N 16

// Extern declarations for global variables
extern float complex_updates_A[2 * N];
extern float complex_updates_B[2 * N];
extern float complex_updates_C[2 * N];
extern float complex_updates_D[2 * N];

// Function declarations
void complex_updates_pin_down(float *pa, float *pb, float *pc, float *pd);
void complex_updates_init(void);
void complex_updates_main(void);
int runmain(void);
void comrun( void *pParam);

#endif /* COMPLEX_UPDATES_H */
