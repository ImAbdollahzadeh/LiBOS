[Bits 16]
org 0x500

%define NULL_DESC 0x00
%define CODE_DESC 0x08
%define DATA_DESC 0x10
%define IMAGE_PMODE_BASE 0x100000 ; final kernel location in Protected Mode

[BITS 16]
entry_point:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ax, 0x9000       ; stack
    mov ss, ax
    mov sp, 0xFC00       ; stacksegment:stackpointer (linear): 9FC00h (below BIOS area)

A20:
    call EnableA20

;*******************************************************
;    Determine physical memory INT 0x15, eax = 0xE820
;    input: es:di (destination buffer)
;*******************************************************

Install_GDT:
    call InstallGDT
    mov si, msgGTD
    call print_string

    call EnterUnrealMode
    mov si, msgUnrealMode
    call print_string

Load_Root:
    mov si, msgLoadKernel
    call print_string

    call LoadRoot
    mov edi, IMAGE_PMODE_BASE
    mov si, ImageName

    call LoadFile       ; c.f. FAT12.inc

    test ax, ax
    je EnterProtectedMode
    mov si, msgFailure
    call print_string

;*******************************************************
;    Switch from Real Mode (RM) to Protected Mode (PM)
;*******************************************************
EnterProtectedMode:
    ; switch off floppy disk motor
    mov dx,0x3F2
    mov al,0x0C
    out dx,al
    mov si, msgFloppyMotorOff
    call print_string

    ; switch to PM
    mov si, msgSwitchToPM
    call print_string
    cli
    mov eax, cr0                       ; bit 0 in CR0 has to be set for entering PM
    or al, 1
    mov cr0, eax
    jmp DWORD CODE_DESC:ProtectedMode  ; far jump to code selector (cs = 0x08)

[Bits 32]
ProtectedMode:
    mov ax, DATA_DESC                  ; set data segments to data selector (0x10)
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov esp, 0x9000

;*******************************************************
;    Execute Kernel
;*******************************************************
EXECUTE:
    jmp DWORD CODE_DESC:IMAGE_PMODE_BASE
    cli
    hlt

;*******************************************************
;    calls, e.g. print_string
;*******************************************************
[BITS 16]
print_string:
    mov ah, 0x0E                    ; BIOS function 0x0E: teletype
    .loop:
        lodsb                       ; grab a byte from SI
        test al, al                 ; NULL?
        jz .done                    ; if zero: end loop
        int 0x10                    ; else: print character to screen
        jmp .loop
    .done:
        ret


;*******************************************************
;    Includes
;*******************************************************

InstallGDT:             ; load GDT into GDTR
    cli
    lgdt [gdt_limit_and_base]
    sti
    ret

EnterUnrealMode:
    cli
    mov     eax, cr0
    or      eax, 1
    mov     cr0, eax
    jmp dword 0x08:TempPM

[BITS 32]

TempPM:
    mov     ax, 0x10
    mov     fs, ax
    jmp dword 0x18:TempPM16

[BITS 16]

TempPM16:
    mov     eax, cr0
    and     eax, 0xFFFFFFFE
    mov     cr0, eax
    jmp dword 0x0000:Back2RM

Back2RM:
    sti
    ret

;******************************************************************************
; Global Descriptor Table (GDT) ;
;******************************************************************************

gdt_data:

NULL_Desc:              ; null descriptor (necessary)
    dd    0
    dd    0

CODE_Desc:
    dw    0xFFFF        ; segment length  bits 0-15 ("limit")
    dw    0             ; segment base    byte 0,1
    db    0             ; segment base    byte 2
    db    10011010b     ; access rights
    db    11001111b     ; bit 7-4: 4 flag bits:  granularity, default operation size bit,
                        ; 2 bits available for OS
                        ; bit 3-0: segment length bits 16-19
    db    0             ; segment base    byte 3

DATA_Desc:
    dw    0xFFFF        ; segment length  bits 0-15
    dw    0             ; segment base    byte 0,1
    db    0             ; segment base    byte 2
    db    10010010b     ; access rights
    db    11001111b     ; bit 7-4: 4 flag bits:  granularity,
                        ; big bit (0=USE16-Segm., 1=USE32-Segm.), 2 bits avail.
                        ; bit 3-0: segment length bits 16-19
    db    0             ; segment base    byte 3

CODE16_Desc:
    dw    0xFFFF
    dw    0
    db    0
    db    10011010b
    db    00001111b
    db    0

end_of_gdt:
gdt_limit_and_base:
    dw end_of_gdt - gdt_data - 1    ; limit (size/length of GDT)
    dd gdt_data

EnableA20:
    in  al, 0x92      ; switch A20 gate via fast A20 port 92
    cmp al, 0xff      ; if 0xFF, nothing's implemented on this port
    je .no_fast_A20

    or  al, 2         ; set A20_Gate_Bit (bit 1)
    and al, ~1        ; clear INIT_NOW bit (don't reset pc...)
    out 0x92, al
    ret

.no_fast_A20:         ; no fast shortcut -> use the slow kbc...
    call empty_8042

    mov al, 0xD1      ; kbc command: write to output port
    out 0x64, al
    call empty_8042

    mov al, 0xDF      ; writing this to kbc output port enables A20
    out 0x60, al
    call empty_8042
    ret

;******************************************************************************
;   calls
;******************************************************************************

empty_8042:
    call Waitingloop
    in al, 0x64
    cmp al, 0xff      ; ... no real kbc at all?
    je .done
    test al, 1        ; something in input buffer?
    jz .no_output
    call Waitingloop
    in al, 0x60       ; yes: read buffer
    jmp empty_8042    ; and try again
.no_output:
    test al, 2        ; command buffer empty?
    jnz empty_8042    ; no: we can't send anything new till it's empty
.done:
    ret

Waitingloop:
    mov bx,0xFFFF
.loop_start:
    dec bx
    jnz .loop_start
    ret


	datasector  dw 0x0000

Sector      db 0x00
Head        db 0x00
Cylinder    db 0x00

; Stage 1 bootloader resides at 0x7C00, including FAT Boot Parameter Block
; To avoid having duplicate values stored somewhere and to ensure that the
; actual structure of the filesystem is taken into account, we use Stage 1's
; values. Thus, following values are addresses.
%define BytesPerSec 0x7C00 + 11
%define SecPerClus  0x7C00 + 13
%define ReservedSec 0x7C00 + 14
%define NumFATs     0x7C00 + 16
%define RootEntries 0x7C00 + 17
%define FATSize     0x7C00 + 22
%define SecPerTrack 0x7C00 + 24
%define NumHeads    0x7C00 + 26
%define DriveNum    0x7C00 + 36

;******************************************************************************
; Convert CHS to LBA
; LBA = (cluster - 2) * sectors per cluster
;******************************************************************************

Convert_Cluster_to_LBA:
    sub ax, 2                                  ; zero base cluster number
    mul cx
    add ax, WORD [datasector]                  ; base data sector
    ret

;******************************************************************************
; Convert LBA to CHS
; AX      LBA Address to convert
;
; sector          = (logical sector / sectors per track) + 1
; head            = (logical sector / sectors per track) MOD number of heads
; cylinder/track  = logical sector / (sectors per track * number of heads)
;
;******************************************************************************

Convert_LBA_to_CHS:
    xor dx, dx                              ; prepare dx:ax for operation
    div WORD [SecPerTrack]                  ; calculate
    inc dl                                  ; adjust for sector 0
    mov BYTE [Sector], dl
    xor dx, dx                              ; prepare dx:ax for operation
    div WORD [NumHeads]                     ; calculate
    mov BYTE [Head], dl
    mov BYTE [Cylinder], al
    ret

;******************************************************************************
; Read sectors
; CX      Number of sectors to read
; AX      Starting sector
; FS:EDI  Buffer to read to
; Invalidates: BX, DX, BP
;******************************************************************************

ReadSectors:
    push    es
    mov     dx, 0x0F00                          ; enough space for up to 4096 byte sectors
    mov     es, dx
.MAIN:
        mov     bp, 0x0005                      ; five retries for error
        push    ax
        push    cx
        call    Convert_LBA_to_CHS              ; convert sector to CHS
    .SECTORLOOP:
            mov     ax, 0x0201                  ; BIOS read sector: ah = 0x02; read one sector: al = 0x01
            xor     bx, bx
            mov     ch, BYTE [Cylinder]         ; track
            mov     cl, BYTE [Sector]           ; sector
            mov     dh, BYTE [Head]             ; head
            mov     dl, BYTE [DriveNum]         ; drive
            int     0x13                        ; invoke BIOS
            jnc     .SUCCESS                    ; test for read error. If not, leave loop
            xor     ah, ah                      ; ERROR: Reset disk (AH=0, DL=0)
            xor     dl, dl
            int     0x13                        ; invoke BIOS
            dec     bp                          ; decrement error counter
            jnz     .SECTORLOOP                 ; attempt to read again
        int     0x18                            ; Indicate BOOT ERROR.
    .SUCCESS:
        mov     cx, WORD [BytesPerSec]
        shr     cx, 2
        xor     si, si
        .COPY_TO_DEST:                          ; copy to destination buffer
            es lodsd                            ; Load dword from es:esi and increment esi
            mov     [fs:edi], eax
            add     edi, 4
            loop    .COPY_TO_DEST
        pop     cx
        pop     ax
        inc     ax                              ; queue next sector
        loop    .MAIN                           ; read next sector
    pop     es
    ret

;******************************************************************************
; Defines
;******************************************************************************

%define ROOT_OFFSET  0x02e00
%define FAT_DEST     0x02c00
%define FAT_SEG      0x002c0
%define ROOT_DEST    0x02e00
%define ROOT_SEG     0x002e0

;******************************************************************************
; LoadRoot
; Transfers Root Directory Table into memory at ROOT_DEST
;******************************************************************************

LoadRoot:
    pusha

    ; compute size of root directory and store in "cx"
    mov     ax, 32                              ; 32 byte directory entry
    mul     WORD [RootEntries]                  ; total size of directory
    div     WORD [BytesPerSec]                  ; sectors used by directory
    mov     cx, ax                              ; store in CX

    ; compute location of root directory and store in "ax"
    movzx   ax, BYTE [NumFATs]                  ; number of FATs
    mul     WORD [FATSize]                      ; sectors used by FATs
    add     ax, WORD [ReservedSec]
    mov     WORD [datasector], ax               ; base of root directory
    add     WORD [datasector], cx

    ; read root directory into ROOT_DEST
    mov     edi, ROOT_DEST
    call    ReadSectors                         ; read directory table
    popa
    ret

;******************************************************************************
; LoadFAT
; Loads FAT table into memory at FAT_DEST
; ES:DI Root Directory Table
;******************************************************************************

LoadFAT:
    pusha

    ; compute size of FAT and store it into CX
    movzx ax, BYTE [NumFATs]                    ; number of FATs
    mul WORD [FATSize]                          ; sectors used by FATs
    mov cx, ax

    ; compute location of FAT and store it into AX
    mov ax, WORD [ReservedSec]

    ; read FAT into memory
    mov edi, FAT_DEST
    call ReadSectors
    popa
    ret

;******************************************************************************
; FindFile
; Search for filename in root table
; DS:SI  File name
; AX     Return value: File index number in directory table (error: -1)
;******************************************************************************

FindFile:
    push cx                     ; store registers

    ; browse root directory for binary image
    mov cx, WORD [RootEntries]  ; load loop counter
    mov di, ROOT_OFFSET         ; locate first root entry at 1 MB mark

.LOOP2:
    push cx
    mov cx, 11                  ; eleven character name
    push si                     ; image name is in SI
    push di
    repe cmpsb                  ; test for entry match
    pop di
    pop si
    je .Found
    pop cx
    add di, 32                  ; queue next directory entry
    loop .LOOP2

.NotFound:
    pop cx                      ; restore registers and return
    mov ax, -1                  ; set error code
    ret

.Found:
    pop ax                      ; return value into AX contains entry of file
    pop cx                      ; restore registers and return
    ret

;******************************************************************************
; LoadFile
; ES:SI   File to load
; FS:EDI  Buffer to load file to
; AX      Return value: success 0, error -1
;******************************************************************************

LoadFile:
    push edi

.FIND_FILE:
    call FindFile                   ; find our file. ES:SI contains our filename
    cmp ax, -1                      ; Error if ax == -1
    jne .LOAD_IMAGE_PRE

    pop edi
    ret

.LOAD_IMAGE_PRE:
    sub edi, ROOT_OFFSET

    ; get starting cluster
    mov dx, word ROOT_SEG           ; root segment loc
    mov es, dx
    mov bp, WORD [es:di + 0x001A]   ; DI points to file entry in root directory table. Reference the table... Store result in bp
    call LoadFAT

.LOAD_IMAGE:
    ; load the cluster
    mov ax, bp                      ; cluster to read
    movzx cx, BYTE [SecPerClus]
    call Convert_Cluster_to_LBA

    pop edi
    push bp
    call ReadSectors
    pop bp

    push edi
    mov ax, FAT_SEG                 ; start reading from fat
    mov es, ax

    ; get next cluster
    mov bx, bp                      ; copy current cluster
    mov dx, bp                      ; copy current cluster
    shr dx, 1                       ; divide by two
    add bx, dx                      ; sum for (3/2)
    mov dx, WORD [es:bx]            ; bx: location of fat in memory
    test bp, 1                      ; test for odd or even cluster
    jnz .ODD_CLUSTER

.EVEN_CLUSTER:
    and dx, 0000111111111111b       ; take low 12 bits
    jmp .DONE

.ODD_CLUSTER:
    shr dx, 4                       ; take high 12 bits

.DONE:
    mov bp, dx
    cmp dx, 0x0ff0                  ; test for EOF
    jb .LOAD_IMAGE

.SUCCESS:
    pop edi
    xor ax, ax                      ; No error.
    ret

;*******************************************************
;    Data Section
;*******************************************************
ImageName         db "KERNEL  BIN"
msgBootLoaderName db "PrettyBL", 0
msgGTD            db 0x0D, 0x0A, 0x0A, "GTD installed", 0
msgUnrealMode     db 0x0D, 0x0A, "Unreal Mode entered", 0
msgLoadKernel     db 0x0D, 0x0A, "Loading Kernel...", 0
msgFloppyMotorOff db 0x0D, 0x0A, "Floppy Disk Motor switched off", 0
msgSwitchToPM     db 0x0D, 0x0A, "Switching to Protected Mode (PM)...", 0
msgFailure        db 0x0D, 0x0A, "KERNEL.BIN missing (Fatal Error)", 0