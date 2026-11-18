# servo-pic-firmware

Firmware para **PIC16F877A**, desarrollado con CCS PIC C Compiler, que recibe un ángulo mediante UART y genera la señal PWM necesaria para controlar un servomotor. Es el componente embebido del proyecto de control de servomotores mediante visión artificial.

**Interfaz gráfica en Python:** [servo-vision-control](https://github.com/arturomeneses100/servo-vision-control)

---

## Funcionamiento

```text
PC (Python) ──"90\n"──► UART (RC7) ──► #int_RDA ──► Ángulo
                                                     │
                                                     ▼
                                      Pulso de 0.5–2.5 ms
                                      cada 20 ms (50 Hz)
                                                     │
                                                     ▼
                                             RB0 ──► Servo
```

- La **interrupción de recepción UART (`#int_RDA`)** acumula los dígitos recibidos y, al detectar un salto de línea (`\n`) o un retorno de carro (`\r`), convierte la cadena de texto en un valor numérico. Si el valor está entre 0 y 180, actualiza el ángulo del servomotor.
- El **ciclo principal** genera la señal de control del servomotor con un periodo de 20 ms (50 Hz) y un ancho de pulso proporcional al ángulo solicitado:

```text
pulso_us = PULSO_MIN + ángulo · (PULSO_MAX − PULSO_MIN) / 180
```

- La lectura de la variable ángulo (16 bits) se realiza con las interrupciones deshabilitadas para garantizar que la copia del dato sea atómica y evitar inconsistencias durante su actualización.

## Especificaciones

| Parámetro | Valor |
|---|---|
| Microcontrolador | PIC16F877A |
| Oscilador | Cristal de 20 MHz (`HS`) |
| Compilador | CCS PIC C Compiler |
| UART | 9600 baudios, 8N1, TX = RC6, RX = RC7 |
| Salida del servomotor | RB0 |
| Periodo de la señal | 20 ms (50 Hz) |
| Ancho de pulso | 500 µs (0°) a 2500 µs (180°), ajustable |

## Protocolo de comunicación

El ángulo se transmite mediante caracteres ASCII, utilizando números enteros en grados y terminando con un salto de línea.

Ejemplos:

```text
0\n     → 0°
90\n    → 90°
180\n   → 180°
```

Se ignoran los caracteres que no sean dígitos y los valores superiores a 180°.

## Conexiones

| PIC16F877A | Conexión |
|---|---|
| RC6 (TX) | RX del convertidor USB-serial |
| RC7 (RX) | TX del convertidor USB-serial |
| RB0 | Señal de control del servomotor |
| OSC1 / OSC2 | Cristal de 20 MHz + 2 × 22 pF |
| MCLR | Resistencia de 10 kΩ a 5 V |
| VDD / VSS | Alimentación de 5 V / GND |

**Importante:** alimenta el servomotor mediante una **fuente independiente de 5 V** y conecta las tierras de todos los dispositivos (PIC, servomotor y convertidor USB-serial).

Si el servomotor se alimenta desde el mismo regulador que el PIC, los picos de consumo pueden provocar caídas de tensión y reinicios del microcontrolador.

## Compilación y grabación

1. Abre el archivo `servo_pic.c` en CCS PIC C Compiler.
2. Compila el proyecto (F9) para generar el archivo `.hex`.
3. Graba el archivo `.hex` utilizando un PICkit 3/4 mediante MPLAB IPE o el programador que tengas disponible.
4. También puedes simular el funcionamiento en Proteus utilizando un módulo `COMPIM` o un terminal serial virtual.

## Ajustes y calibración

Puedes calibrar los siguientes parámetros según las características del servomotor utilizado:

```c
#define PULSO_MIN  500     // µs a 0° (típico: 500–1000)
#define PULSO_MAX  2500    // µs a 180° (típico: 2000–2500)
```

Para modificar la velocidad de comunicación, cambia el parámetro `baud=` en la directiva `#use rs232` y configura la misma velocidad en la interfaz gráfica desarrollada en Python.

## Limitaciones y mejoras propuestas

- **Generación de PWM:** actualmente, el pulso se genera mediante `delay_us`. Por lo tanto, una interrupción UART durante su generación puede introducir variaciones de algunos microsegundos en el ancho del pulso. Como mejora, se propone utilizar Timer1 o el módulo CCP en modo de comparación para obtener una temporización más precisa.
- **Siguiente etapa: implementación del PID en el PIC.** La propuesta consiste en que Python envíe únicamente el error calculado mediante visión artificial y que el microcontrolador se encargue de calcular el ángulo del servomotor. Esto permitirá comparar el control ejecutado en la PC con el control implementado directamente en el sistema embebido.

## Estructura del proyecto

```text
servo-pic-firmware/
├── servo_pic.c     # Firmware desarrollado en CCS PIC C Compiler
└── README.md       # Documentación del proyecto
```

## Autor

**Arturo Elfego Meneses Flores**  
Universidad Tecnológica de la Huasteca Hidalguense (UTHH)
