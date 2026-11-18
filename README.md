# servo-pic-firmware

Firmware para **PIC16F877A** (CCS PIC C Compiler) que recibe un Ã¡ngulo por UART y genera la seÃ±al PWM de un servomotor. Es la parte embebida del proyecto de control de un servo con visiÃ³n artificial.

> Interfaz grÃ¡fica en Python: [servo-vision-control](https://github.com/arturomeneses100/servo-vision-control)

---

## Funcionamiento

```
PC (Python) â”€â”€"90\n"â”€â”€â–º UART (RC7) â”€â”€â–º #int_RDA â”€â”€â–º Ã¡ngulo â”€â”€â–º pulso 0.5â€“2.5 ms cada 20 ms â”€â”€â–º RB0 â”€â”€â–º Servo
```

- La **interrupciÃ³n de recepciÃ³n (`#int_RDA`)** acumula dÃ­gitos y, al llegar `\n` o `\r`, convierte el texto a nÃºmero. Si el valor estÃ¡ entre 0 y 180, actualiza el Ã¡ngulo.
- El **ciclo principal** genera la seÃ±al del servo: periodo de 20 ms (50 Hz) y ancho de pulso proporcional al Ã¡ngulo:

```
pulso_us = PULSO_MIN + Ã¡ngulo Â· (PULSO_MAX âˆ’ PULSO_MIN) / 180
```

- La lectura del Ã¡ngulo (16 bits) se hace con las interrupciones deshabilitadas para que la copia sea atÃ³mica.

## Especificaciones

| ParÃ¡metro | Valor |
|---|---|
| Microcontrolador | PIC16F877A |
| Oscilador | Cristal de 20 MHz (`HS`) |
| Compilador | CCS PIC C Compiler |
| UART | 9600 baudios, 8N1, TX = RC6, RX = RC7 |
| Salida del servo | RB0 |
| Periodo | 20 ms (50 Hz) |
| Pulso | 500 Âµs (0Â°) a 2500 Âµs (180Â°), ajustable |

## Protocolo

El Ã¡ngulo se envÃ­a en ASCII, en grados enteros, terminado en salto de lÃ­nea:

```
0\n     â†’  0Â°
90\n    â†’  90Â°
180\n   â†’  180Â°
```

Se ignoran los caracteres que no son dÃ­gitos y los valores mayores a 180.

## Conexiones

| PIC16F877A | ConexiÃ³n |
|---|---|
| RC6 (TX) | RX del convertidor USB-serial |
| RC7 (RX) | TX del convertidor USB-serial |
| RB0 | SeÃ±al del servo |
| OSC1 / OSC2 | Cristal de 20 MHz + 2 Ã— 22 pF |
| MCLR | 10 kÎ© a 5 V |
| VDD / VSS | 5 V / GND |

**Importante:** alimenta el servo con una **fuente de 5 V independiente** y une todas las tierras (PIC, servo y convertidor). Si el servo toma corriente del mismo regulador que el PIC, este puede reiniciarse.

## CompilaciÃ³n y grabaciÃ³n

1. Abre `servo_pic.c` en CCS PIC C Compiler.
2. Compila (F9) para generar el `.hex`.
3. Graba el `.hex` con PICkit 3/4 (MPLAB IPE) o con el programador que uses.
4. TambiÃ©n se puede simular en Proteus con un `COMPIM` o un terminal virtual.

## Ajustes

Calibra estas constantes segÃºn el modelo de servo:

```c
#define PULSO_MIN  500     // Âµs a 0Â°   (tÃ­pico 500â€“1000)
#define PULSO_MAX  2500    // Âµs a 180Â° (tÃ­pico 2000â€“2500)
```

Para cambiar la velocidad, modifica `baud=` en `#use rs232` y selecciona la misma en la interfaz de Python.

## Limitaciones y mejoras propuestas

- El pulso se genera con `delay_us`, asÃ­ que una interrupciÃ³n del UART durante el pulso introduce unos microsegundos de variaciÃ³n. Como mejora, el pulso se puede generar con Timer1 o el mÃ³dulo CCP en modo comparaciÃ³n.
- **Siguiente etapa:** llevar el PID al PIC. Python mandarÃ­a solo el error y el microcontrolador calcularÃ­a el Ã¡ngulo, lo que permite comparar el control en la PC con el control embebido.

## Estructura

```
servo-pic-firmware/
â”œâ”€â”€ servo_pic.c     # Firmware (CCS)
â””â”€â”€ README.md
```

## Autor

**Arturo Elfego Meneses Flores** â€” Universidad TecnolÃ³gica de la Huasteca Hidalguense (UTHH)
