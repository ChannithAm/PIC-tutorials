// PIC16F1509 Configuration Bit Settings
#include <xc.h>
// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection Bits (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = ON        // Internal/External Switchover Mode (Internal/External Switchover Mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)
// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)

#define STRLEN 12

volatile unsigned char t;
volatile unsigned char rcindex;
volatile unsigned char rcbuf[STRLEN];

void USART_init(void)
{
    TXSTAbits.TXEN = 1;     // enable transmitter
    TXSTAbits.BRGH = 1;     // high baud rate mode
    RCSTAbits.CREN = 1;     // enable continous receiving

    // configure I/O pins
    ANSELBbits.ANSB5 = 0;   // RX input type is digital
    TRISBbits.TRISB5 = 1;     // RX pin is input
    TRISBbits.TRISB7 = 1;     // TX pin is input (automatically configured)

    SPBRGL = 25;            // set baud rate to 9600 baud (4MHz/(16*baudrate))-1

    PIE1bits.RCIE = 1;      // enable USART receive interrupt
    RCSTAbits.SPEN = 1;     // enable USART
}

void USART_putc(unsigned char c)
{
    while (!TXSTAbits.TRMT); // wait until transmit shift register is empty
    TXREG = c;               // write character to TXREG and start transmission
}

void USART_puts(unsigned char *s)
{
    while (*s)
    {
        USART_putc(*s);     // send character pointed to by s
        s++;                // increase pointer location to the next character
    }
}

int main(void)
{
    OSCCONbits.IRCF = 0x0D; // INTOSC frequency 4MHz

    USART_init();

    USART_puts("Init complete!\n");

    INTCONbits.PEIE = 1;    // enable peripheral interrupts
    INTCONbits.GIE = 1;     // enable interrupts

    while(1)
    {

    }

    return 0;
}

void interrupt ISR(void)
{
    if (PIR1bits.RCIF)  // check if receive interrupt has fired
    {
        t = RCREG;      // read received character to buffer

        // check if received character is not new line character
        // and that maximum string length has not been reached
        if ( (t != '\n') && (rcindex < STRLEN) )
        {
            rcbuf[rcindex] = t; // append received character to string
            rcindex++;          // increment string index
        }
        else
        {
            rcindex = 0;        // reset string index
            USART_puts(rcbuf);  // echo received string
        }

        PIR1bits.RCIF = 0;      // reset receive interrupt flag
    }
}