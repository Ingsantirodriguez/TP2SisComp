global convert          ; declara la función globalmente

convert:
    push ebp            ; guarda el valor del puntero en la pila
    mov ebp, esp        ; guarda el valor de esp en ebp

    ;Multiplico el valor en dólares por la tasa de cambio
    mov eax, [ebp+8]    ; mueve el primer parámetro en eax
    mov ebx, [ebp+12]   ; mueve el segundo parámetro en ebx
    imul eax, ebx       ; multiplica eax por ebx
                        ; el resultado se guarda en eax

    ;Divido el resultado obtenido por 100
    mov ecx, 100        ; mueve el número 100 a ecx (divisor)
    cdq                 ; extiende el tamaño de eax para que ocupe edx:eax
    idiv ecx            ; divide y almacena el cociente en eax

    pop ebp            ; restaura el valor de ebp
    ret                ; retorna el resultado en eax