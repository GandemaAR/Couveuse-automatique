/* Fichier de gestion du PWM du PIC16F877A */
#include <16F877A.h>
#device ADC=16
#fuses HS, NOWDT, NOPUT, NOPROTECT, NOLVP
#use delay(clock=16000000)

/* 
 * Gestion PWM selon datasheet PIC16F877A
 * CCP1 (RC2) en mode PWM
 */

/*************** Définitions des registres ***************/
#byte PR2 = 0x92
#byte T2CON = 0x12
#byte CCPR1L = 0x15
#byte CCP1CON = 0x17
#byte TRISC = 0x87
#byte TMR2 = 0x11
#byte PIR1 = 0x0C

/*************** Prototypes des Fonctions ***************/
void pwm_init(unsigned int16 pwm_frequency, unsigned int8 prescaler);
void pwm_set_duty_(unsigned int16 duty_value);
void pwm_set_duty_percent(unsigned int8 percent);
unsigned int16 calculate_pr2(unsigned int16 frequency, unsigned int8 prescaler);

/*************** Variables globales ***************/
unsigned int16 max_duty_value = 0;

/*************** Définitions Fonctions PWM ***************/

/*
 * Initialise le module PWM
 */
void pwm_init(unsigned int16 pwm_frequency, unsigned int8 prescaler) {
    unsigned int8 t2con_value;
    unsigned int16 pr2_value;
    
    // 1. Configurer le prescaler
    switch(prescaler) {
        case 1:
            t2con_value = 0b00000100;  // T2CKPS = 00
            break;
        case 4:
            t2con_value = 0b00000101;  // T2CKPS = 01
            break;
        case 16:
            t2con_value = 0b00000111;  // T2CKPS = 1x
            break;
        default:
            t2con_value = 0b00000101;  // Par défaut prescaler = 4
            prescaler = 4;
    }
    
    // 2. Calculer PR2
    pr2_value = calculate_pr2(pwm_frequency, prescaler);
    
    // 3. Désactiver le Timer2 pendant la configuration
    T2CON = 0;
    
    // 4. Configurer la broche RC2/CCP1 comme sortie
    output_bit(PIN_C2, 0);
    TRISC = 0b11111011;
    
    // 5. Écrire la valeur de PR2
    PR2 = (unsigned int8)pr2_value;
    
    // 6. Initialiser les registres PWM à 0%
    CCPR1L = 0;
    CCP1CON = 0b00001100;  // Mode PWM, bits DC1B = 00
    
    // 7. Calculer la valeur maximale du duty cycle
    max_duty_value = (pr2_value + 1) * 4;
    
    // 8. Activer le Timer2 avec le prescaler choisi
    T2CON = t2con_value | 0b00000100;  // TMR2ON = 1
    
    // 9. Attendre que le Timer2 déborde
    while(!(PIR1 & 0x02));  // Attendre TMR2IF = 1
    PIR1 &= ~0x02;          // Effacer le flag
}

/*
 * Calcule la valeur de PR2 pour une fréquence donnée
 */
unsigned int16 calculate_pr2(unsigned int16 frequency, unsigned int8 prescaler) {
    unsigned int32 pr2_calc;
    
    // Formule: PR2 = [Fosc / (4 * prescaler * Fpwm)] - 1
    pr2_calc = (16000000UL / (4UL * prescaler * frequency)) - 1;
    
    // Limiter à 255 (maximum pour un registre 8 bits)
    if(pr2_calc > 255) {
        pr2_calc = 255;
    }
    
    // Vérifier la valeur minimale
    if(pr2_calc < 1) {
        pr2_calc = 1;
    }
    
    return (unsigned int16)pr2_calc;
}

/*
 * Définit le duty cycle avec une valeur brute (0-1023)
 */
void pwm_set_duty_(unsigned int16 duty_value) {
    unsigned int8 dc1b;
    
    // Limiter la valeur au maximum permis
    if(duty_value > max_duty_value) {
        duty_value = max_duty_value;
    }
    
    // Limiter à 1023 (10 bits max)
    if(duty_value > 1023) {
        duty_value = 1023;
    }
    
    // Écrire les 8 bits MSB dans CCPR1L
    CCPR1L = (duty_value >> 2) & 0xFF;
    
    // Écrire les 2 bits LSB dans CCP1CON<5:4>
    dc1b = duty_value & 0x03;
    
    // Mettre à jour CCP1CON
    CCP1CON = 0x0C | (dc1b << 4);
}

/*
 * Définit le duty cycle en pourcentage (0-100%)
 */
void pwm_set_duty_percent(unsigned int8 percent) {
    unsigned int16 duty_val;
    
    // Limiter le pourcentage
    if(percent > 100) {
        percent = 100;
    }
    
    // Convertir pourcentage en valeur
    duty_val = ((unsigned int32)percent * max_duty_value) / 100;
    
    // Appliquer la valeur
    pwm_set_duty_(duty_val);
}