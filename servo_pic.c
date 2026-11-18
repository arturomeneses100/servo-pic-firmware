// Firmware PIC16F877A @ 20 MHz (CCS PIC C Compiler)
// Recibe por UART el ángulo en grados terminado en '\n' (ej. "90\n")
// y genera la señal del servo (periodo 20 ms, pulso 0.5–2.5 ms) en RB0.

#include <16F877A.h>
#fuses HS, NOWDT, NOPROTECT, NOLVP, PUT, BROWNOUT
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7, ERRORS)
#include <stdlib.h>

#define SERVO      PIN_B0
#define PULSO_MIN  500     // us a 0°   (ajustar según el servo: 500–1000)
#define PULSO_MAX  2500    // us a 180° (ajustar según el servo: 2000–2500)

int16 angulo = 90;
char  buffer[5];
int8  idx = 0;

#int_RDA
void serial_isr(void) {
   char c;
   int16 a;
   c = getc();
   if (c == '\n' || c == '\r') {
      if (idx > 0) {
         buffer[idx] = '\0';
         a = atol(buffer);
         if (a <= 180) angulo = a;
      }
      idx = 0;
   } else if (c >= '0' && c <= '9' && idx < 3) {
      buffer[idx++] = c;
   }
}

void main(void) {
   int16 a, pulso;
   output_low(SERVO);
   enable_interrupts(INT_RDA);
   enable_interrupts(GLOBAL);

   while (TRUE) {
      disable_interrupts(GLOBAL);   // copia atómica del valor de 16 bits
      a = angulo;
      enable_interrupts(GLOBAL);

      pulso = PULSO_MIN + (int32)a * (PULSO_MAX - PULSO_MIN) / 180;
      output_high(SERVO);
      delay_us(pulso);
      output_low(SERVO);
      delay_us(20000 - pulso);
   }
}
