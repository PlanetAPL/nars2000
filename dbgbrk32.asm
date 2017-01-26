; '$Header:   P:/PVCS/MAX/STATUTIL/DBGBRK.ASV   1.0   25 Sep 1996 10:14:50   BOB  $
        TITLE   DBGBRK.ASM

CPUFLAGS record  $ID:1,$VIP:1,$VIF:1,$ACHI:1,$VMHI:1,$RFHI:1, \
                 $R0:1,$NT:1,$IOPL:2,$OF:1,$DF:1,$IF:1,$TF:1,$SF:1,$ZF:1,$R1:1,$AF:1,$R2:1,$PF:1,$R3:1,$CF:1

        .386
        .model FLAT,C
        .code

        OPTION READONLY
        OPTION OLDSTRUCTS

NDPSW   record  $BUSY:1,$C3:1,$ST0:1,$ST1:1,$ST2:1,$C2:1,$C1:1,$C0:1,$IR:1, \
                $XRSV2:1,$XFPRC:1,$XFUNF:1,$XFOVF:1,$XFZDV:1,$XFDOP:1,$XFIOP:1

        public  iAsmAdd64
iAsmAdd64 proc                  ; Start iAsmAdd64 procedure

COMMENT|

Add two signed 64-bit numbers,
handling overflow.

On entry:

Stack as per ADD64_STR

On exit:

EAX     =       1 if successful
        =       0 if not

|

ADD64_STR struct

        dd      ?               ; Caller's EBP
        dd      ?               ; Caller's return EIP
ADD64_RES dd    ?               ; Ptr to result
ADD64_LFT dq    ?               ; Left arg
ADD64_RHT dq    ?               ; Right arg

ADD64_STR ends

        push    ebp             ; Prepare to address the stack
        mov     ebp,esp         ; Hello, Mr. Stack

        push    ebx             ; Save register

        finit                   ; Initialize the FPU

        fild    [ebp].ADD64_LFT ; Load the left arg
        fild    [ebp].ADD64_RHT ; ...      right ...
        faddp   st(1),st(0)     ; Add 'em
        fstsw   ax              ; Save the FPU Status Word from the FADDP

        mov     ebx,[ebp].ADD64_RES ; EBX => result
        fistp   qword ptr [ebx] ; Save in result

        mov     ebx,eax         ; Copy the FSTSW word from the FADDP
        fstsw   ax              ; Save the FPU Status Word from the FISTP
        or      eax,ebx         ; Merge the two FSTSW flags

        test    eax,(mask $XFPRC) OR (mask $XFOVF) OR (mask $XFIOP) ; Test the Precision Exception, Overflow, and Invalid Operation flags
        mov     eax,0           ; Clear the result
        setz    al              ; Return 0 or 1 from the <add>

        pop     ebx             ; Restore register

        pop     ebp             ; Restore

        ret                     ; Return to caller

iAsmAdd64 endp                  ; End iAsmAdd64 procedure

        public  iAsmSub64
iAsmSub64 proc                  ; Start iAsmSub64 procedure

COMMENT|

Subtract two signed 64-bit numbers,
handling overflow.

On entry:

Stack as per SUB64_STR

On exit:

EAX     =       1 if successful
        =       0 if not

|

SUB64_STR struct

        dd      ?               ; Caller's EBP
        dd      ?               ; Caller's return EIP
SUB64_RES dd    ?               ; Ptr to result
SUB64_LFT dq    ?               ; Left arg
SUB64_RHT dq    ?               ; Right arg

SUB64_STR ends

        push    ebp             ; Prepare to address the stack
        mov     ebp,esp         ; Hello, Mr. Stack

        push    ebx             ; Save register

        finit                   ; Initialize the FPU

        fild    [ebp].SUB64_LFT ; Load the left arg
        fild    [ebp].SUB64_RHT ; ...      right ...
        fsubp   st(1),st(0)     ; Subtract 'em
        fstsw   ax              ; Save the FPU Status Word from the FSUBP

        mov     ebx,[ebp].SUB64_RES ; EBX => result
        fistp   qword ptr [ebx] ; Save in result

        mov     ebx,eax         ; Copy the FSTSW word from the FSUBP
        fstsw   ax              ; Save the FPU Status Word from the FISTP
        or      eax,ebx         ; Merge the two FSTSW flags

        test    eax,(mask $XFPRC) OR (mask $XFOVF) OR (mask $XFIOP) ; Test the Precision Exception, Overflow, and Invalid Operation flags
        mov     eax,0           ; Clear the result
        setz    al              ; Return 0 or 1 from the <add>

        pop     ebx             ; Restore register

        pop     ebp             ; Restore

        ret                     ; Return to caller

iAsmSub64 endp                  ; End iAsmSub64 procedure

        public  iAsmMul64
iAsmMul64 proc                  ; Start iAsmMul64 procedure

COMMENT|

Multiply two signed 64-bit numbers,
handling overflow.

On entry:

Stack as per MUL64_STR

On exit:

EAX     =       1 if successful
        =       0 if not

|

MUL64_STR struct

        dd      ?               ; Caller's EBP
        dd      ?               ; Caller's return EIP
MUL64_RES dd    ?               ; Ptr to result
MUL64_LFT dq    ?               ; Left arg
MUL64_RHT dq    ?               ; Right arg

MUL64_STR ends

        push    ebp             ; Prepare to address the stack
        mov     ebp,esp         ; Hello, Mr. Stack

        push    ebx             ; Save register

        finit                   ; Initialize the FPU

        fild    [ebp].MUL64_LFT ; Load the left arg
        fild    [ebp].MUL64_RHT ; ...      right ...
        fmulp   st(1),st(0)     ; Multiply 'em
        fstsw   ax              ; Save the FPU Status Word from the FMULP

        mov     ebx,[ebp].MUL64_RES ; EBX => result
        fistp   qword ptr [ebx] ; Save in result

        mov     ebx,eax         ; Copy the FSTSW word from the FMULP
        fstsw   ax              ; Save the FPU Status Word from the FISTP
        or      eax,ebx         ; Merge the two FSTSW flags

        test    eax,(mask $XFPRC) OR (mask $XFOVF) OR (mask $XFIOP) ; Test the Precision Exception, Overflow, and Invalid Operation flags
        mov     eax,0           ; Clear the result
        setz    al              ; Return 0 or 1 from the <mul>

        pop     ebx             ; Restore register

        pop     ebp             ; Restore register

        ret                     ; Return to caller

iAsmMul64 endp                  ; End iAsmMul64 procedure

        public  iAsmMulPiInt
iAsmMulPiInt proc               ; Start iAsmMulPiInt procedure

COMMENT|

Multiply a signed 64-bit number by Pi.

On entry:

Stack as per MULPI_STR

On exit:

EAX     =       clobbered

|

MULPI_STR struct

        dd      ?               ; Caller's EBP
        dd      ?               ; Caller's return EIP
MULPI_RES dd    ?               ; Ptr to result
MULPI_RHT dd    ?               ; Ptr to right arg

MULPI_STR ends

        push    ebp             ; Prepare to address the stack
        mov     ebp,esp         ; Hello, Mr. Stack

        finit                   ; Initialize the FPU

        fldpi                   ; Load Pi
        mov     eax,[ebp].MULPI_RHT ; EAX => integer right arg
        fild    qword ptr [eax] ; Load the integer right arg
        fmulp   st(1),st(0)     ; Multiply 'em
        mov     eax,[ebp].MUL64_RES ; EAX => result
        fstp    qword ptr [eax] ; Save in result

        pop     ebp             ; Restore

        ret                     ; Return to caller

iAsmMulPiInt endp               ; End iAsmMulPiInt procedure

        public  iAsmMulPiFlt
iAsmMulPiFlt proc               ; Start iAsmMulPiFlt procedure

COMMENT|

Multiply a signed 64-bit number by Pi.

On entry:

Stack as per MULPI_STR

On exit:

EAX     =       clobbered

|

        push    ebp             ; Prepare to address the stack
        mov     ebp,esp         ; Hello, Mr. Stack

        finit                   ; Initialize the FPU

        fldpi                   ; Load Pi
        mov     eax,[ebp].MULPI_RHT ; EAX => float right arg
        fld     qword ptr [eax] ; Load the float right arg
        fmulp   st(1),st(0)     ; Multiply 'em
        mov     eax,[ebp].MUL64_RES ; EAX => result
        fstp    qword ptr [eax] ; Save in result

        pop     ebp             ; Restore

        ret                     ; Return to caller

iAsmMulPiFlt endp               ; End iAsmMulPiFlt procedure

        public  iAsmCircle4Int
iAsmCircle4Int proc             ; Start iAsmCircle4Int procedure

COMMENT|

Compute (1 + R * 2) * 0.5

On entry:

Stack as per MULPI_STR

On exit:

EAX     =       clobbered

|

        push    ebp             ; Prepare to address the stack
        mov     ebp,esp         ; Hello, Mr. Stack

        finit                   ; Initialize the FPU

        mov     eax,[ebp].MULPI_RHT ; EAX => integer right arg
        fild    qword ptr [eax] ; Load the integer right arg
        fld     st(0)           ;   R       R
        fmulp   st(1),st(0)     ;   R*R
        fld1                    ;   1       R*R
        faddp   st(1),st(0)     ;   1+R*R
        fsqrt                   ;   (1+R*R)*0.5
        mov     eax,[ebp].MUL64_RES ; EAX => result
        fstp    qword ptr [eax] ; Save in result

        pop     ebp             ; Restore

        ret                     ; Return to caller

iAsmCircle4Int endp             ; End iAsmCircle4Int procedure

        public  iAsmCircle4Flt
iAsmCircle4Flt proc             ; Start iAsmCircle4Flt procedure

COMMENT|

Compute (1 + R * 2) * 0.5

On entry:

Stack as per MULPI_STR

On exit:

EAX     =       clobbered

|

        push    ebp             ; Prepare to address the stack
        mov     ebp,esp         ; Hello, Mr. Stack

        finit                   ; Initialize the FPU

        mov     eax,[ebp].MULPI_RHT ; EAX => float right arg
        fld     qword ptr [eax] ; Load the float right arg
        fld     st(0)           ;   R       R
        fmulp   st(1),st(0)     ;   R*R
        fld1                    ;   1       R*R
        faddp   st(1),st(0)     ;   1+R*R
        fsqrt                   ;   (1+R*R)*0.5
        mov     eax,[ebp].MUL64_RES ; EAX => result
        fstp    qword ptr [eax] ; Save in result

        pop     ebp             ; Restore

        ret                     ; Return to caller

iAsmCircle4Flt endp             ; End iAsmCircle4Flt procedure

        public  iAsmCircle3Int
iAsmCircle3Int proc             ; Start iAsmCircle3Int procedure

COMMENT|

Compute tan(R)

On entry:

Stack as per MULPI_STR

On exit:

EAX     =       clobbered

|

        push    ebp             ; Prepare to address the stack
        mov     ebp,esp         ; Hello, Mr. Stack

        finit                   ; Initialize the FPU

        mov     eax,[ebp].MULPI_RHT ; EAX => integer right arg
        fild    qword ptr [eax] ; Load the integer right arg
        fsincos                 ;   cos(R) sin(R)
        fdivp   st(1),st(0)     ;   tan(R)
        mov     eax,[ebp].MUL64_RES ; EAX => result
        fstp    qword ptr [eax] ; Save in result

        pop     ebp             ; Restore

        ret                     ; Return to caller

iAsmCircle3Int endp             ; End iAsmCircle3Int procedure

        public  iAsmCircle3Flt
iAsmCircle3Flt proc             ; Start iAsmCircle3Flt procedure

COMMENT|

Compute tan(R)

On entry:

Stack as per MULPI_STR

On exit:

EAX     =       clobbered

|

        push    ebp             ; Prepare to address the stack
        mov     ebp,esp         ; Hello, Mr. Stack

        finit                   ; Initialize the FPU

        mov     eax,[ebp].MULPI_RHT ; EAX => float right arg
        fld     qword ptr [eax] ; Load the float right arg
        fsincos                 ;   cos(R) sin(R)
        fdivp   st(1),st(0)     ;   tan(R)
        mov     eax,[ebp].MUL64_RES ; EAX => result
        fstp    qword ptr [eax] ; Save in result

        pop     ebp             ; Restore

        ret                     ; Return to caller

iAsmCircle3Flt endp             ; End iAsmCircle3Flt procedure

        public  iAsmCircle2Int
iAsmCircle2Int proc             ; Start iAsmCircle2Int procedure

COMMENT|

Compute cos(R)

On entry:

Stack as per MULPI_STR

On exit:

EAX     =       clobbered

|

        push    ebp             ; Prepare to address the stack
        mov     ebp,esp         ; Hello, Mr. Stack

        finit                   ; Initialize the FPU

        mov     eax,[ebp].MULPI_RHT ; EAX => integer right arg
        fild    qword ptr [eax] ; Load the integer right arg
        fcos                    ;   cos(R)
        mov     eax,[ebp].MUL64_RES ; EAX => result
        fstp    qword ptr [eax] ; Save in result

        pop     ebp             ; Restore

        ret                     ; Return to caller

iAsmCircle2Int endp             ; End iAsmCircle2Int procedure

        public  iAsmCircle2Flt
iAsmCircle2Flt proc             ; Start iAsmCircle2Flt procedure

COMMENT|

Compute cos(R)

On entry:

Stack as per MULPI_STR

On exit:

EAX     =       clobbered

|

        push    ebp             ; Prepare to address the stack
        mov     ebp,esp         ; Hello, Mr. Stack

        finit                   ; Initialize the FPU

        mov     eax,[ebp].MULPI_RHT ; EAX => float right arg
        fld     qword ptr [eax] ; Load the float right arg
        fcos                    ;   cos(R)
        mov     eax,[ebp].MUL64_RES ; EAX => result
        fstp    qword ptr [eax] ; Save in result

        pop     ebp             ; Restore

        ret                     ; Return to caller

iAsmCircle2Flt endp             ; End iAsmCircle2Flt procedure

        public  iAsmCircle1Int
iAsmCircle1Int proc             ; Start iAsmCircle1Int procedure

COMMENT|

Compute sin(R)

On entry:

Stack as per MULPI_STR

On exit:

EAX     =       clobbered

|

        push    ebp             ; Prepare to address the stack
        mov     ebp,esp         ; Hello, Mr. Stack

        finit                   ; Initialize the FPU

        mov     eax,[ebp].MULPI_RHT ; EAX => integer right arg
        fild    qword ptr [eax] ; Load the integer right arg
        fsin                    ;   sin(R)
        mov     eax,[ebp].MUL64_RES ; EAX => result
        fstp    qword ptr [eax] ; Save in result

        pop     ebp             ; Restore

        ret                     ; Return to caller

iAsmCircle1Int endp             ; End iAsmCircle1Int procedure

        public  iAsmCircle1Flt
iAsmCircle1Flt proc             ; Start iAsmCircle1Flt procedure

COMMENT|

Compute sin(R)

On entry:

Stack as per MULPI_STR

On exit:

EAX     =       clobbered

|

        push    ebp             ; Prepare to address the stack
        mov     ebp,esp         ; Hello, Mr. Stack

        finit                   ; Initialize the FPU

        mov     eax,[ebp].MULPI_RHT ; EAX => float right arg
        fld     qword ptr [eax] ; Load the float right arg
        fsin                    ;   sin(R)
        mov     eax,[ebp].MUL64_RES ; EAX => result
        fstp    qword ptr [eax] ; Save in result

        pop     ebp             ; Restore

        ret                     ; Return to caller

iAsmCircle1Flt endp             ; End iAsmCircle1Flt procedure

        public  iAsmCircle0Int
iAsmCircle0Int proc             ; Start iAsmCircle0Int procedure

COMMENT|

Compute (1 - R * 2) * 0.5

On entry:

Stack as per MULPI_STR

On exit:

EAX     =       clobbered

|

        push    ebp             ; Prepare to address the stack
        mov     ebp,esp         ; Hello, Mr. Stack

        finit                   ; Initialize the FPU

        mov     eax,[ebp].MULPI_RHT ; EAX => integer right arg
        fild    qword ptr [eax] ; Load the integer right arg
        fld     st(0)           ;   R       R
        fmulp   st(1),st(0)     ;   R*R
        fchs                    ;   -R*R
        fld1                    ;   1       -R*R
        faddp   st(1),st(0)     ;   1-R*R
        fsqrt                   ;   (1-R*R)*0.5
        mov     eax,[ebp].MUL64_RES ; EAX => result
        fstp    qword ptr [eax] ; Save in result

        pop     ebp             ; Restore

        ret                     ; Return to caller

iAsmCircle0Int endp             ; End iAsmCircle0Int procedure

        public  iAsmCircle0Flt
iAsmCircle0Flt proc             ; Start iAsmCircle0Flt procedure

COMMENT|

Compute (1 - R * 2) * 0.5

On entry:

Stack as per MULPI_STR

On exit:

EAX     =       clobbered

|

        push    ebp             ; Prepare to address the stack
        mov     ebp,esp         ; Hello, Mr. Stack

        finit                   ; Initialize the FPU

        mov     eax,[ebp].MULPI_RHT ; EAX => float right arg
        fld     qword ptr [eax] ; Load the float right arg
        fld     st(0)           ;   R       R
        fmulp   st(1),st(0)     ;   R*R
        fchs                    ;   -R*R
        fld1                    ;   1       -R*R
        faddp   st(1),st(0)     ;   1-R*R
        fsqrt                   ;   (1-R*R)*0.5
        mov     eax,[ebp].MUL64_RES ; EAX => result
        fstp    qword ptr [eax] ; Save in result

        pop     ebp             ; Restore

        ret                     ; Return to caller

iAsmCircle0Flt endp             ; End iAsmCircle0Flt procedure

        public  iAsmCircleN4Int
iAsmCircleN4Int proc             ; Start iAsmCircleN4Int procedure

COMMENT|

Compute (R + 1) × sqrt ((R - 1) / (R + 1))
a.k.a.  ((-1) + R * 2) * 0.5

On entry:

Stack as per MULPI_STR

On exit:

EAX     =       clobbered

|

        push    ebp             ; Prepare to address the stack
        mov     ebp,esp         ; Hello, Mr. Stack

        finit                   ; Initialize the FPU

        mov     eax,[ebp].MULPI_RHT ; EAX => integer right arg
        fild    qword ptr [eax] ; Load the integer right arg

        fld     st(0)           ;   R           R
        fld1                    ;   1           R      R
        faddp   st(1),st(0)     ; R+1           R
        fld1                    ;   1           R+1    R
        fsubp   st(2),st(0)     ; R-1           R+1
        fld     st(1)           ; R+1           R-1    R+1
        fdivp   st(1),st(0)     ; (R-1)/(R+1)   R+1
        fsqrt                   ; sqrt(...)     R+1
        fmulp   st(1),st(0)     ; sqrt(...) * (R+1)

        mov     eax,[ebp].MUL64_RES ; EAX => result
        fstp    qword ptr [eax] ; Save in result

        pop     ebp             ; Restore

        ret                     ; Return to caller

iAsmCircleN4Int endp             ; End iAsmCircleN4Int procedure

        public  iAsmCircleN4Flt
iAsmCircleN4Flt proc             ; Start iAsmCircleN4Flt procedure

COMMENT|

Compute (R + 1) × sqrt ((R - 1) / (R + 1))
a.k.a.  ((-1) + R * 2) * 0.5

On entry:

Stack as per MULPI_STR

On exit:

EAX     =       clobbered

|

        push    ebp             ; Prepare to address the stack
        mov     ebp,esp         ; Hello, Mr. Stack

        finit                   ; Initialize the FPU

        mov     eax,[ebp].MULPI_RHT ; EAX => float right arg
        fld     qword ptr [eax] ; Load the float right arg

        fld     st(0)           ;   R           R
        fld1                    ;   1           R      R
        faddp   st(1),st(0)     ; R+1           R
        fld1                    ;   1           R+1    R
        fsubp   st(2),st(0)     ; R-1           R+1
        fld     st(1)           ; R+1           R-1    R+1
        fdivp   st(1),st(0)     ; (R-1)/(R+1)   R+1
        fsqrt                   ; sqrt(...)     R+1
        fmulp   st(1),st(0)     ; sqrt(...) * (R+1)

        mov     eax,[ebp].MUL64_RES ; EAX => result
        fstp    qword ptr [eax] ; Save in result

        pop     ebp             ; Restore

        ret                     ; Return to caller

iAsmCircleN4Flt endp             ; End iAsmCircleN4Flt procedure

        end
