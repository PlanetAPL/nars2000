; '$Header:   P:/PVCS/MAX/STATUTIL/DBGBRK.ASV   1.0   25 Sep 1996 10:14:50   BOB  $
        TITLE   DBGBRK.ASM

CPUFLAGS record  $ID:1,$VIP:1,$VIF:1,$ACHI:1,$VMHI:1,$RFHI:1, \
                 $R0:1,$NT:1,$IOPL:2,$OF:1,$DF:1,$IF:1,$TF:1,$SF:1,$ZF:1,$R1:1,$AF:1,$R2:1,$PF:1,$R3:1,$CF:1

ifdef IS_32
        .386
        .model FLAT,C
        .code

        OPTION READONLY
        OPTION OLDSTRUCTS

NDPSW   record  $BUSY:1,$C3:1,$ST0:1,$ST1:1,$ST2:1,$C2:1,$C1:1,$C0:1,$IR:1, \
                $XRSV2:1,$XFPRC:1,$XFUNF:1,$XFOVF:1,$XFZDV:1,$XFDOP:1,$XFIOP:1

        public  DbgBrk
DbgBrk  proc    near            ; Start DbgBrk procedure

COMMENT|

Signal a debug breakpoint

|

DB_STR  struc

        dd      ?               ; Caller's EBP
DB_FL   dd      ?               ; ...      EFL

DB_STR  ends

        pushfd                  ; Save the flags
        push    ebp             ; Prepare to address the stack
        mov     ebp,esp         ; Hello, Mr. Stack
        or      [ebp].DB_FL,mask $TF ; TF=1
        pop     ebp             ; Restore
        popfd                   ; Put TF into effect

        ret                     ; Return to caller

        assume  ds:nothing,es:nothing,fs:nothing,gs:nothing,ss:nothing

DbgBrk  endp                    ; End DbgBrk procedure

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

        finit                   ; Initialize the FPU

        fild    [ebp].ADD64_LFT ; Load the left arg
        fild    [ebp].ADD64_RHT ; ...      right ...
        faddp   st(1),st(0)     ; Add 'em

        mov     eax,[ebp].ADD64_RES ; EAX => result
        fistp   qword ptr [eax] ; Save in result

        fstsw   ax              ; Save the FPU Status Word

        test    ax,mask $XFIOP  ; Test the Invalid Operation flag
        mov     eax,0           ; Clear the result
        setz    al              ; Return 0 or 1 from the <add>

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

        finit                   ; Initialize the FPU

        fild    [ebp].SUB64_LFT ; Load the left arg
        fild    [ebp].SUB64_RHT ; ...      right ...
        fsubp   st(1),st(0)     ; Subtract 'em

        mov     eax,[ebp].SUB64_RES ; EAX => result
        fistp   qword ptr [eax] ; Save in result

        fstsw   ax              ; Save the FPU Status Word

        test    ax,mask $XFIOP  ; Test the Invalid Operation flag
        mov     eax,0           ; Clear the result
        setz    al              ; Return 0 or 1 from the <add>

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

        finit                   ; Initialize the FPU

        fild    [ebp].MUL64_LFT ; Load the left arg
        fild    [ebp].MUL64_RHT ; ...      right ...
        fmulp   st(1),st(0)     ; Multiply 'em

        mov     eax,[ebp].MUL64_RES ; EAX => result
        fistp   qword ptr [eax] ; Save in result

        fstsw   ax              ; Save the FPU Status Word

        test    ax,mask $XFIOP  ; Test the Invalid Operation flag
        mov     eax,0           ; Clear the result
        setz    al              ; Return 0 or 1 from the <add>

        pop     ebp             ; Restore

        ret                     ; Return to caller

iAsmMul64 endp                  ; End iAsmMul64 procedure
else
ifdef IS_64
        .code

        public  DbgBrk
DbgBrk  proc                    ; Start DbgBrk procedure

COMMENT|

Signal a debug breakpoint

|

DB_STR  struct

        dq      ?               ; Caller's RBP
DB_FL   dq      ?               ; ...      RFL

DB_STR  ends

        pushf                   ; Save the flags
        push    Rbp             ; Prepare to address the stack
        mov     Rbp,Rsp         ; Hello, Mr. Stack
        or      DB_STR.DB_FL[Rbp],mask $TF ; TF=1
        pop     Rbp             ; Restore
        popf                    ; Put TF into effect

        ret                     ; Return to caller

DbgBrk  endp                    ; End DbgBrk procedure

        public  iAsmAdd64
iAsmAdd64 proc                  ; Start iAsmAdd64 procedure

COMMENT|

Add two signed 64-bit numbers,
handling overflow.

On entry:

Rdx     =       left arg
R8      =       right arg
Rcx     =       ptr to result

On exit:

Rax     =       1 if successful
        =       0 if not

|

        mov     Rax,Rdx         ; Copy left arg
        add     Rax,R8          ; Plus the right arg
                                ;   setting OF on error
        mov     qword ptr [Rcx],Rax ; Save in the result
        mov     Rax,0           ; Clear the result
        setno   al              ; Return 0 or 1 from the <add>

        ret                     ; Return to caller

iAsmAdd64 endp                  ; End iAsmAdd64 procedure

        public  iAsmSub64
iAsmSub64 proc                  ; Start iAsmSub64 procedure

COMMENT|

Subtract two signed 64-bit numbers,
handling overflow.

On entry:

Rdx     =       left arg
R8      =       right arg
Rcx     =       ptr to result

On exit:

Rax     =       1 if successful
        =       0 if not

|

        mov     Rax,Rdx         ; Copy left arg
        sub     Rax,R8          ; Less the right arg
                                ;   setting OF on error
        mov     qword ptr [Rcx],Rax ; Save in the result
        mov     Rax,0           ; Clear the result
        setno   al              ; Return 0 or 1 from the <sub>

        ret                     ; Return to caller

iAsmSub64 endp                  ; End iAsmSub64 procedure

        public  iAsmMul64
iAsmMul64 proc                  ; Start iAsmMul64 procedure

COMMENT|

Multiply two signed 64-bit numbers,
handling overflow.

On entry:

Rdx     =       left arg
R8      =       right arg
Rcx     =       ptr to result

On exit:

Rax     =       1 if successful
        =       0 if not

|

        push    Rdx             ; Save register clobbered by <imul>

        mov     Rax,Rdx         ; Copy left arg
        imul    R8              ; Times the right arg
                                ;   producing Rdx:Rax
                                ;   and setting OF and CF on error
        mov     qword ptr [Rcx],Rax ; Save in the result
        mov     Rax,0           ; Clear the result
        setno   al              ; Return 0 or 1 from the <imul>

        pop     Rdx             ; Restore

        ret                     ; Return to caller

iAsmMul64 endp                  ; End iAsmMul64 procedure
else
echo Need code for this architecture
.err
endif
endif
        end
