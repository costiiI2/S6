/*****************************************************************************************
 *
 * File                 : 7seg_alphanum.c
 * Author               : Cecchet Costantino
 * Date                 : 6.10.2023
 *
 *****************************************************************************************
 * Brief: Values for the 7-segment display
 *
 *****************************************************************************************
 * Modifications :
 * Ver    Date        Student      Comments
 * 1.0    6.10.2023   CCO          Initial version.
 *****************************************************************************************/

#define BLANK 0x00
#define ZERO 0x3f
#define ONE 0x06
#define TWO 0x5b
#define THREE 0x4f
#define FOUR 0x66
#define FIVE 0x6d
#define SIX 0x7d
#define SEVEN 0x07
#define EIGHT 0x7f
#define NINE 0x6f
#define A 0x77
#define B 0x7c
#define C 0x39
#define D 0x5e
#define E 0x79
#define F 0x71
#define G 0x3d
#define H 0x76
#define I 0x06
#define J 0x1e
#define K 0x76
#define L 0x38
#define M 0x15
#define N 0x37
#define O 0x3f
#define P 0x73
#define Q 0x67
#define R 0x50
#define S 0x6d
#define T 0x78
#define U 0x3e
#define V 0x1c
#define Y 0x6e
#define Z 0x5b

static const char hexa[16] = {ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, A, B, C, D, E, F};