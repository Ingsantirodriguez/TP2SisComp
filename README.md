# Trabajo Práctico 2
## Open Source Syndicate
![](http://www.clipartbest.com/cliparts/7ia/KKK/7iaKKKeyT.png "logo robado")

Este trabajo consiste en desarrollar un programa que llama a una API REST de criptomonedas para obtener la cotización de la misma en dólares, provee 3 únicamente:
* Bitcoin (BTC)
* Monero (XMR)
* Ethereum (ETH)

Luego de tomar el valor lo convierte a Pesos Argentinos y Euros con una función programada en assembler.

## Instalación

Se necesitan las siguientes dependencias:
* libcurl-devel.i686
* make

```
$ make all
```
## Uso

```
$ ./converter

Ingrese la criptomoneda:
 1. Bitcoin
 2. Monero
 3. Ethereum
 2
Valor de 1 XMR en USD = 161
Valor de 1 XMR en ARS = 34776
Valor de 1 XMR en EUR = 146
```