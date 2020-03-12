#include "setting_hardaware/setting.h"
#include <stdlib.h>
#include "stdio.h"
#include "string.h"

void open_isr(){
    TRISB = 0xff;
    INTCONbits.INT0F = 0;
    INTCONbits.INT0E = 1;
    INTCONbits.GIE = 1;
    INTCON2 = 0x00;
    LATC = 0x00;
    TRISAbits.RA4 = 1;
    RCONbits.IPEN = 1;
    ADCON1 = 0x0f;
}
void open_PWM(){
    PR2 = 65535;
    CCPR2 = 30000;
    TMR2 = 0;
    TRISCbits.TRISC1 = 0;
    T2CON = 0x04;
    CCP2CON = 0x0C;
}
void setting_config(){
    TRISCbits.TRISC2 = 0; /* configure CCP1 pin for output */
    T3CON = 0xC9; /* turn on TMR3 in 16-bit mode, TMR3
    & TMR4 as base timer for all CCP
    modules */
    CCP1CON = 0x09; /* configure CCP1 pin set high initially and pull
    low
    on match */
    CCPR1 = TMR3 + 20; /* start CCP1 compare operation with 1600
    cycles delay */
    PIR1bits.CCP1IF = 0;
    IPR1bits.CCP1IP = 1; /* set CCP1 interrupt to high priority */
    PIE1bits.CCP1IE = 1; /* enable CCP1 interrupt */
}

int control_flag;

void main(void) 
{
    //SYSTEM_Initialize();
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 0; 
    open_isr();
    open_PWM();
    setting_config();
    TRISD = 0x00;
    LATB = 0x00;
    LATD = 0x00;
    PORTB = 0x00;
    while(1){
        if(control_flag < 0x0800){
            CCP2CON = 0x0C;
        }
        else{
            CCP2CON = 0x00;
        }
        if(control_flag < 0x0D00){
            LATD = 0xff;
        }
        else{
            LATD = 0x00;
        }
    }
    while(1){
        LATD = 0x00;
        CCP2CON = 0x00;
    }
    return;
    
}

// old version: 
// void interrupt high_priority Hi_ISR(void)
void __interrupt(high_priority) Hi_ISR(void)
{
    if(PIR1bits.CCP1IF == 1){
        PIR1bits.CCP1IF = 0;
        while(PORTBbits.RB5 == 0);
        control_flag = TMR3;
        while(PORTBbits.RB5 == 1);
        control_flag = TMR3 - control_flag; 
        CCP1CON = 0x09;
        CCPR1 = TMR3 + 20;
    }
    return;
}