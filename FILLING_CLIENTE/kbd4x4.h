/* #include "kbd4x4.h"
*
* Creada por: Ing. Abiezer Hernandez O.
* Fecha de creacion: 02/12/2019
* Electronica y Circuitos
*
*/

#include <xc.h>
#define _XTAL_FREQ 8000000L

#define ROW0_PIN PORTDbits.RD7
#define ROW1_PIN PORTDbits.RD6
#define ROW2_PIN PORTDbits.RD5
#define ROW3_PIN PORTDbits.RD4
#define COL0_PIN PORTDbits.RD3
#define COL1_PIN PORTDbits.RD2
#define COL2_PIN PORTDbits.RD1
//#define COL3_PIN PORTDbits.RD0 //UNCOMENT TO WORK WITH 4X4 KEYPAD

#define ROW0_DIR TRISDbits.TRISD7   //ROWS AS OUTPUT
#define ROW1_DIR TRISDbits.TRISD6   //ROWS AS OUTPUT
#define ROW2_DIR TRISDbits.TRISD5   //ROWS AS OUTPUT
#define ROW3_DIR TRISDbits.TRISD4   //ROWS AS OUTPUT
#define COL0_DIR TRISDbits.TRISD3   //COLUMN AS INPUT
#define COL1_DIR TRISDbits.TRISD2   //COLUMN AS INPUT
#define COL2_DIR TRISDbits.TRISD1   //COLUMN AS INPUT
//#define COL3_DIR TRISDbits.TRISD0 //UNCOMENT TO WORK WITH 4X4 KEYPAD

//#define ROW0_PIN PORTAbits.RA0 
//#define ROW1_PIN PORTAbits.RA1 
//#define ROW2_PIN PORTAbits.RA2 
//#define ROW3_PIN PORTAbits.RA4 
//#define COL0_PIN PORTBbits.RB7
//#define COL1_PIN PORTBbits.RB6
//#define COL2_PIN PORTBbits.RB5
////#define COL3_PIN PORTBbits.RB4 //UNCOMENT TO WORK WITH 4X4 KEYPAD
//
//#define ROW0_DIR TRISAbits.TRISA0 //OUTPUT
//#define ROW1_DIR TRISAbits.TRISA1 //OUTPUT
//#define ROW2_DIR TRISAbits.TRISA2 //OUTPUT
//#define ROW3_DIR TRISAbits.TRISA4 //OUTPUT
//#define COL0_DIR TRISBbits.TRISB7 //INPUT COLUMN
//#define COL1_DIR TRISBbits.TRISB6 //INPUT COLUMN
//#define COL2_DIR TRISBbits.TRISB5 //INPUT COLUMN
////#define COL3_DIR TRISBbits.TRISB4 //INPUT COLUMN //UNCOMENT TO WORK WITH 4X4 KEYPAD

void Keypad_Init(void);
char Keypad_Get_Char(void);