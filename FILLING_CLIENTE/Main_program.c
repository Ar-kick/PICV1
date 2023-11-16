/*
 * File:   Main_programx.c
 * FILLING CLIENTE
 * Author: ARKICK
 *
 * Created on 13 de julio de 2023, 10:49 PM
 */

#include <xc.h>
#include <stdio.h>  //NECESARY TO USE sprintf() INSTRUCTION AND DISPLAY DATA ON LCD
#define _XTAL_FREQ 8000000L

#include "FUSES_CONF.h"
#include "kbd4x4.h"
#include "lcd.h"


///////////////////////////////////////////////////////////////////////////
//MACROS

#define DETECTOR PORTAbits.RA5  //INPUT bottle detector when detects is 0, otherwise 1
#define PLAY_STOP PORTBbits.RB0      //INPUT STOP BUTTON
#define PUMP LATCbits.LATC2      //OUTPUT PIN 17 PWM
#define BUZZER LATCbits.LATC0   //OUTPUT BUZZER
#define RELAY_PUMP LATBbits.LATB3   //OUTPUT FOR RELAY

///////////////////////////////////////////////////////////////////////////
//VARS & CONST

float PULSES_TO_REACH = 0;
float CAL = 3.448275862068966; //
float PULSE = 0; //STORES THE COUNT OF THE INTERRUPTION GENERATED BY THE FLOW SENSOR
float VALUE_IN_ML = 0;
int CURRENT_OPTION; // Control Variable
int PROFILE_NUM = 0;
int num2, num1;
char _CHAR2, _CHAR1 =0;
char SELECTED2 = 0;
char NAME1 [11] = {'X', '1', '0', '7', '8', '9', ' ', '0', 'X', '5', '5'};
char NAME2 [5] = {'N', 'N', 'H', 'M', 'N'};
char STRING_VAL_TYPED [5];
char STRING_VALUE_IN_ML [5];
const char SENSOR [8] = {0x4, 0xe, 0x4, 0x0, 0x4, 0xe, 0x4, 0x00};  // THERE IS BOTTLE LOGO

//////////////////////////////////////////////////////////////////////////
//INTERRUPTS CONFIG

void __interrupt() ISR_TIMER0(void) 
{

    if (INTCONbits.TMR0IF == 1) //si el flag se activa...
    {
        PULSE++; //EVERY OVERFLOW THIS COUNTER OF PULSES WILL INCREASE
        TMR0L = 255; //WILL STAR COUNTING FROM 255 WHEN REACHES 256(8BIT) WILL OVERFLOW AND CALL DE ISR FUNTION
        INTCONbits.TMR0IF = 0; //Limpiamos flag de interrupcion
    }

}

void TMR0_COUNT(void);
void WELCOME(void); //DECLARED FUNCTION
void USER_VALUE(void);
void FILL(void);


//************************************************************************//
////////////////////////////////////////////////////////////////////////////
//MAIN

void main(void) 
{

    OSCCON = 0X72; //config for 8mhz internal oscilator
    
    //////////////////////////////////////////////////////////////////////////
    //INTERRUPTS CONFIG

    INTCONbits.GIE = 1; //Interurpcion global activada
    INTCONbits.T0IF = 0; //Limpiamos flag de interrupcion
    INTCONbits.TMR0IE = 1; //Habilitacio�n de interrupcion TMR0
    TRISAbits.TRISA4 = 1; //PIN A4 AS INPUT //FOR COUNTER MODE
    ADCON1bits.PCFG = 0X0F; // all ports as digital

    TRISAbits.TRISA5 = 1; //INPUT bottle detector when detects is 0, otherwise 1
    TRISBbits.TRISB0 = 1; //Input button STOP
    TRISBbits.TRISB3 = 0; //Output RELAY PUMP
    TRISCbits.TRISC2 = 0; //Output PUMP
    TRISEbits.TRISE2 = 0; //Output LED
    TRISCbits.TRISC0 = 0; //Output BUZZER
    LATB = 0;
    LATC = 0;
    LATE = 0;

    Keypad_Init(); //Initializes Keyboard
    Lcd_Init(); //Initializes lcd
    Lcd_CGRAM_Init();                   //Access to CGRAM (stores up to 8 customized characters)
    Lcd_CGRAM_CreateChar(0, SENSOR);  //Stores the character in position 2 of CGRAM
    Lcd_CGRAM_Close();                  //Exits from CGRAM function
    Lcd_Clear(); //Clears LCD
    __delay_ms(100);



    ///////////////////////////////////////////////////////////////////////////


    while (1) 
    {

        switch (CURRENT_OPTION) 
        {
            case 0:

                WELCOME();
                USER_VALUE();
                break;

            case 1:

                while (CURRENT_OPTION == 1)         //Controla la eleccion de seguir o regresar
                {
                    SELECTED2 = Keypad_Get_Char();  //reads the input of the keyboard
                    if (SELECTED2 == '*')           //if an * is pressed...
                    {
                        CURRENT_OPTION = 0;         //we exit from thi while because line 121 will be FALSE
                        
                    }

                    if (PLAY_STOP == 0 && DETECTOR == 0) //IF THERE IS BOTTLE AND WE PRESS START....
                    {
                        
                        CURRENT_OPTION = 2;
                        __delay_ms(700);
                    }
                    if (DETECTOR == 0)
                    {
                        Lcd_Set_Cursor(1, 16);
                        Lcd_Write_Char(0);  //DISPLAYS SENSOR LOGO
                    }
                    else
                    {
                        Lcd_Set_Cursor(1, 16);
                        Lcd_Write_String(" ");
                    }
                }
                break;

            case 2:
                FILL();
                Lcd_Init();
                while (CURRENT_OPTION == 2) 
                {
                    if (DETECTOR == 0) //IF IT DETECTS BOTTLE...
                    {
                        Lcd_Set_Cursor(1, 16);
                        Lcd_Write_Char(0);  //DISPLAYS SENSOR LOGO
                        Lcd_Set_Cursor(1, 1);
                        Lcd_Write_String("Retirar envase");
                        Lcd_Set_Cursor(2, 12);
                        Lcd_Write_String("Parar");
                    } 
                    else 
                    {
                        Lcd_Set_Cursor(2, 16);
                        Lcd_Write_String(" ");
                        Lcd_Set_Cursor(1, 1);
                        Lcd_Write_String("Ponga un envase ");
                        Lcd_Set_Cursor(2, 12);
                        Lcd_Write_String("Parar");
                        __delay_ms(400);
                        if (DETECTOR == 0)      //IF THEY PUT A NEW BOTTLE....
                        {
                            CURRENT_OPTION = 2;
                            break;
                        }
                    }

                    if (PLAY_STOP == 0) //if main button is pressed...
                    {
                        CURRENT_OPTION = 0; //we exit from thi while because line 121 will be FALSE
                        break;
                    }
                }
                break;

        }

    }

} //void main


//************************************************************************//
///////////////////////////////////////////////////////////////////////////
//FUNCTIONS

void TMR0_COUNT(void) //CONFIG FOR TIMERO 0 AS COUNTER FOR WATER FLOW PULSES
{ 

    T0CONbits.T08BIT = 1; //Modo 8 bits
    T0CONbits.T0CS = 1; //Modo CONTADOR
    T0CONbits.PSA = 1; //Prescaler No habilitado 
    T0CONbits.T0SE = 1; //Activacion por Flanco de bajada EN EL PIN ESPECIAL DE INTERRUPCION TOCKI (RA4)
    TMR0L = 255; //STARTS COUNTING FROM 255 AND WHEN REACHES 256(8BIT) WILL OVERFLOW
    T0CONbits.TMR0ON = 1; //Enciende TMR0

}

void WELCOME(void) 
{

    Lcd_Clear();
    Lcd_Set_Cursor(1, 4); //y,x
    for (int a = 0; a < 11; a++) {
        __delay_ms(80);
        Lcd_Write_Char(NAME1[a]); // Shows the pressed key
    }
    Lcd_Set_Cursor(2, 6); //y,x
    for (int a = 0; a < 5; a++) {
        __delay_ms(80);
        Lcd_Write_Char(NAME2[a]); // Shows the pressed key
    }
    BUZZER = 1;
    __delay_ms(50);
    BUZZER = 0;
    __delay_ms(100);
    BUZZER = 1;
    __delay_ms(50);
    BUZZER = 0;
    Lcd_Clear();

}

void USER_VALUE(void) 
{

    PROFILE_NUM = 0;
    Lcd_Set_Cursor(2, 1);
    Lcd_Write_String("Limpiar:  Inicio");
    Lcd_Set_Cursor(1, 1);
    Lcd_Write_String("Perfil: #00");
    Lcd_Set_Cursor(1, 10);
    Lcd_Blink();
    _CHAR2 = 0;
    _CHAR1 = 0;

    while (_CHAR2 == 0) //TENS
    {
        _CHAR2 = Keypad_Get_Char();
        if (_CHAR2) // VERIFIES IF CHAR2 HAS A VALUE ALREADY
        {
            if (_CHAR2 == '*' || _CHAR2 == '#')
                _CHAR2 = '0';
            Lcd_Write_Char(_CHAR2);
            num2 = _CHAR2 - '0'; //Converts ASCII to decimal
        }
        if(PLAY_STOP == 0) //IF START IS PRESSED.. MEANS THAT WE WANT CLEANNING PROCESS
        {
            num2 = 0;
            break;
        }
    }
    while (_CHAR1 == 0) //UNITS
    {
        _CHAR1 = Keypad_Get_Char();
        if (_CHAR1) // VERIFIES IF CHAR1 HAS A VALUE ALREADY
        {
            if (_CHAR1 == '*' || _CHAR1 == '#')
                _CHAR1 = '0';
            Lcd_Write_Char(_CHAR1);
            Lcd_NoBlink();
            num1 = _CHAR1 - '0';
        }
        if(PLAY_STOP == 0) //IF START IS PRESSED.. MEANS THAT WE WANT CLEANNING PROCESS
        {
            num1 = 0;
            break;
        }         
    }
    PROFILE_NUM = num2 * 10 + num1; //Converts the value to decimals TENS + UNITS
    __delay_ms(500);
    
    switch (PROFILE_NUM)
    {
        case 0 :
           
            __delay_ms(400);
            Lcd_Clear();
            Lcd_Set_Cursor(2, 1);
            Lcd_Write_String("Mantenga iniciar");
            while (PLAY_STOP == 1); //While the button is 1, does nothing
            __delay_ms(200);        //Delay for push noise
            while (PLAY_STOP == 0) //While button is 0, activates the pump
            {
                Lcd_Set_Cursor(1,1);
                Lcd_Write_String("Limpiando..");
                
                RELAY_PUMP = 1;
            }
            RELAY_PUMP = 0;
            CURRENT_OPTION = 0;
            break;
        
        case 1 :
            Lcd_Clear();
            Lcd_Set_Cursor(1, 1);
            Lcd_Write_String("Perf.01= 1500ml");
            PROFILE_NUM = 1500;
            Lcd_Set_Cursor(2, 1);
            Lcd_Write_String("Atras *");
            Lcd_Set_Cursor(2, 10);
            Lcd_Write_String("Iniciar");
            CURRENT_OPTION = 1;
            break;
            
        case 2 :
            Lcd_Clear();
            Lcd_Set_Cursor(1, 1);
            Lcd_Write_String("Perf.02= 1000ml");
            PROFILE_NUM = 1000;
            Lcd_Set_Cursor(2, 1);
            Lcd_Write_String("Atras *");
            Lcd_Set_Cursor(2, 10);
            Lcd_Write_String("Iniciar");
            CURRENT_OPTION = 1;
            break;
            
        case 3:
            Lcd_Clear();
            Lcd_Set_Cursor(1, 1);
            Lcd_Write_String("Perf.03= 500ml");
            PROFILE_NUM = 500;
            Lcd_Set_Cursor(2, 1);
            Lcd_Write_String("Atras *");
            Lcd_Set_Cursor(2, 10);
            Lcd_Write_String("Iniciar");
            CURRENT_OPTION = 1;
            break;
            
        case 4:
            Lcd_Clear();
            Lcd_Set_Cursor(1, 1);
            Lcd_Write_String("Perf.04= 250ml");
            PROFILE_NUM = 250;
            Lcd_Set_Cursor(2, 1);
            Lcd_Write_String("Atras *");
            Lcd_Set_Cursor(2, 10);
            Lcd_Write_String("Iniciar");
            CURRENT_OPTION = 1;
            break;
            
        case 5:
            Lcd_Clear();
            Lcd_Set_Cursor(1, 1);
            Lcd_Write_String("Perf.05= 100ml ");
            PROFILE_NUM = 100;
            Lcd_Set_Cursor(2, 1);
            Lcd_Write_String("Atras *");
            Lcd_Set_Cursor(2, 10);
            Lcd_Write_String("Iniciar");
            CURRENT_OPTION = 1;
            break;
            
        default:
            Lcd_Clear();
            Lcd_Set_Cursor(1, 1);
            Lcd_Write_String("  No encontrado ");
            Lcd_Set_Cursor(2, 1);
            Lcd_Write_String("Atras *");
    }
    //IF THERE IS VALUE, CURRENT OPTION IS 0 AND WE CONTINUE
    //if (PROFILE_NUM == 0) CURRENT_OPTION = 0; 

}

void FILL(void) 
{

    Lcd_Clear();
    __delay_ms(100);
    Lcd_Set_Cursor(1, 1);
    Lcd_Write_String("Llenando     ml");
    Lcd_Set_Cursor(2, 12);
    Lcd_Write_String("Salir");
    Lcd_Set_Cursor(2, 7); //y,x
    sprintf(STRING_VAL_TYPED, "%u", PROFILE_NUM); //CONVERTS INT VAL_TYPED INTO CHAR STRING_VAL_TYPED
    Lcd_Set_Cursor(1, 10);
    Lcd_Write_String(STRING_VAL_TYPED); //DISPLAYS THE INT CONVERTED INTO STRING
 
    Lcd_Set_Cursor(2, 1);
    Lcd_Write_String("     ml ");
    BUZZER = 1;                          //TURNS ON BUZZER
    __delay_ms(100);
    BUZZER = 0;
    PULSES_TO_REACH = (PROFILE_NUM / CAL);   //# TYPED ON KEYBOARD EX: 100 ml *.36 = 36 PULSES TO COUNT
    PULSE = 0;                           //SETS THE COUNTER FOR THE INTERRUPTION TO 0
    VALUE_IN_ML = 0;
    TMR0_COUNT();                        //INITIALIZES THE FLOW PULSE DTECTION
    RELAY_PUMP = 1;                     //TURNS ON PUMP

    while (PULSE <= PULSES_TO_REACH) //DO THIS WHILE INTERRUPT COUNTER IS SMALLER TAH VAL_SOLICITADO
    {
        VALUE_IN_ML = (PULSE * CAL);
        sprintf(STRING_VALUE_IN_ML, "%0.1f", VALUE_IN_ML); // CONVERTS FROM INT PULSE TO STRING PUL
        Lcd_Set_Cursor(2, 1);
        Lcd_Write_String(STRING_VALUE_IN_ML);         //DISPLAYS THE INT CONVERTED INTO STRING

        if (DETECTOR == 0)  //If bottle detection; puts sensor character
        {
            Lcd_Set_Cursor(1, 16);
            Lcd_Write_Char(0);
        }
        
        if (PLAY_STOP == 0)    //IF FILLING IS INTERRUPTED
        {
            RELAY_PUMP = 0;
            BUZZER = 1;
            __delay_ms(600);
            BUZZER = 0;
            PULSE = 0;
            CURRENT_OPTION = 0;
            break;
        }
    }
    if (PULSE >= PULSES_TO_REACH) //When the pulses reach the QUANTITY desred, it stops  
    {
        RELAY_PUMP = 0;
        PULSE = 0;
        BUZZER = 1;
        __delay_ms(100);
        BUZZER = 0;
        CURRENT_OPTION = 2;
        __delay_ms(1000);   //1 SEC BEFORE THE LCD IS ERASED
    }

}
/*Si quieres encontrar un n�mero que, al multiplicarse por 0.36, resulte en 1, 
 * simplemente debes dividir 1 / 0.42 = 2.3809	 26
 * 
 * 26 PULSES FOR 100ML 
 * 26/100 = 0.26
 * 1/.26 = 3.8461

 * 76 PULSES FOR 100ML 
 * 76/100 = 0.76
 * 1/.76 = 1.315789473684211
 * 
 * 28 PULSES FOR 100ML
 * 28/100 = 0.28
 * 1/.28 = 3.571428571428571 
 * 
 * 29 PULSES FOR 100ML
 * 29/100 = 0.29
 * 1/.29 = 3.448275862068966
 */


 