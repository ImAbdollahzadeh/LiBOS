[Bits 16]
org 0x7C00                             ; start address of bootloader
jmp word entry_point                   ; jump to bootloader entry point

OperatingSystemName db "LiBOS   "      ;  8 byte
BytesPerSec         dw 512
SecPerClus          db 1
ReservedSec         dw 1
NumFATs             db 2
RootEntries         dw 224
TotSec              dw 2880
MediaType           db 0xF0
FATSize             dw 9
SecPerTrack         dw 18
NumHeads            dw 2
HiddenSec           dd 0
TotSec32            dd 0
DriveNum            db 0
Reserved            db 0
BootSig             db 0x29
VolumeSerialNum     dd 0xD00FC0DE
VolumeLabel         db "LiBOS      "   ; 11 byte
FileSys             db "FAT12   "      ;  8 byte


;******************************************************************************
;    bootloader entry point
;******************************************************************************
entry_point:
    xor     bx, bx                     ; set segment registers
    mov     ds, bx
    mov     es, bx

    mov     ax, 0x7C00                 ; set the stack
    mov     ss, ax                     ; this instruction disables interrupts
                                       ; for one instruction, thus no interrupt
                                       ; can occur when the stack is "unstable"
                                       ; (known ss, but unknown sp)
    xor     sp, sp                     ; interrupts are disabled here

    cld                                ; clear direction flag. We rely on this later.

    mov [DriveNum], dl                 ; store boot device

    mov ax, 0x1112
    int 0x10                           ; set 80x50 text mode and 8x8 font

    mov si, msgLoading
    call print_string

Load_Root_Directory_Table:
    ; compute size of root directory and store in "cx"
    mov ax, 0x20                              ; 32 byte directory entry
    mul WORD [RootEntries]                    ; total size of directory
    div WORD [BytesPerSec]                    ; sectors used by directory
    xchg cx, ax

    ; compute location of root directory and store in "ax"
    movzx ax, BYTE [NumFATs]                  ; number of FATs
    mul WORD [FATSize]                        ; sectors used by FATs
    add ax, WORD [ReservedSec]                ; adjust for bootsector
    mov WORD [datasector], ax                 ; base of root directory
    add WORD [datasector], cx

    ; read root directory into memory (7E00h)
    mov bx, 0x7E00                            ; copy root dir above bootcode
    call ReadSectors

;******************************************************************************
;    Find stage2 bootloader
;******************************************************************************
    ; browse root directory for binary image
    mov cx, WORD [RootEntries]                ; load loop counter
    mov di, 0x7E00                            ; locate first root entry
.LOOP:
        push cx
        push di
        mov cx, 11                            ; name has 11 characters
        mov si, ImageName                     ; look for this image name
        rep cmpsb                             ; test for entry match
        pop di
        pop cx
        je Load_FAT
        add di, 0x20                          ; queue next directory entry
        loop .LOOP
    jmp FAILURE

;******************************************************************************
;    Load File Allocation Table (FAT)
;******************************************************************************
Load_FAT:
    ; save starting cluster of boot image
    mov bp, WORD [di + 0x001A]               ; file's first cluster

    ; compute location of FAT and store in "cx"
    mov cx, WORD [ReservedSec]               ; adjust for bootsector

    ; compute size of FAT and store in "ax" (because we need ax for the mul instruction)
    movzx ax, BYTE [NumFATs]                 ; number of FATs
    mul WORD [FATSize]                       ; sectors used by FATs

    xchg cx, ax                              ; Swap cx (FAT location) and ax (Size of FAT)

    ; read FAT into memory (7E00h)
    mov bx, 0x7E00                           ; copy FAT above bootcode
    call ReadSectors

    ; read image file into memory (0500h)
    xor ax, ax
    mov bx, 0x0500                     ; destination for image
    mov es, ax                         ; destination for image

;******************************************************************************
;    Load stage2 bootloader
;******************************************************************************
Load_Image:
    mov ax, bp                         ; cluster to read
    movzx cx, BYTE [SecPerClus]        ; sectors to read
    call Convert_Cluster_to_LBA        ; convert cluster to LBA
    call ReadSectors

    ; compute next cluster
    mov cx, bp                         ; copy current cluster
    mov dx, bp                         ; copy current cluster
    shr dx, 1                          ; divide by two
    add cx, dx                         ; sum for (3/2)
    mov di, 0x7E00                     ; location of FAT in memory
    add di, cx                         ; index into FAT
    mov dx, WORD [di]                  ; read two bytes from FAT
    test ax, 1
    jnz .ODD_CLUSTER

.EVEN_CLUSTER:
    and dx, 0000111111111111b          ; take low twelve bits
    jmp .DONE

.ODD_CLUSTER:
    shr dx, 4                          ; take high twelve bits

.DONE:
    mov bp, dx                         ; store new cluster
    cmp dx, 0x0FF0                     ; test for EOF
    jb Load_Image

DONE:
    mov dl, [DriveNum]
    jmp 0x0000:0x0500

FAILURE:
    mov si, msgFailure
    call print_string
    mov ah, 0x00
    int 0x16                           ; wait for keypress
    int 0x19                           ; warm boot reset


;******************************************************************************
;    Convert CHS to LBA
;    LBA = (cluster - 2) * sectors per cluster
;******************************************************************************
Convert_Cluster_to_LBA:
    sub ax, 2                          ; zero base cluster number
    mul cx                             ; Number of sectors per cluster is stored in cx
    add ax, WORD [datasector]          ; base data sector
    ret

;******************************************************************************
;    Convert LBA to CHS
;    AX    LBA Address to convert
;
;    sector = (logical sector / sectors per track) + 1
;    head   = (logical sector / sectors per track) MOD number of heads
;    track  = logical sector / (sectors per track * number of heads)
;******************************************************************************
Convert_LBA_to_CHS:
    xor dx, dx                         ; prepare dx:ax for operation
    div WORD [SecPerTrack]             ; calculate
    inc dl                             ; adjust for sector 0
    mov BYTE [Sector], dl
    xor dx, dx                         ; prepare dx:ax for operation
    div WORD [NumHeads]                ; calculate
    mov BYTE [Head], dl
    mov BYTE [Cylinder], al
    ret

;******************************************************************************
;    Reads sectors
;    CX     Number of sectors to read
;    AX     Starting sector
;    ES:BX  Buffer to read to
;******************************************************************************
ReadSectors:
.NEXTSECTOR:
        mov di, 5                      ; five retries for error
        push ax
        push cx
        call Convert_LBA_to_CHS        ; convert starting sector from LBA to CHS
    .LOOP:
            mov  ax, 0x0201            ; BIOS read sector (CHS): ah = 0x02; read one sector: al = 0x01
            mov  ch, BYTE [Cylinder]   ; track/cylinder
            mov  cl, BYTE [Sector]     ; sector
            mov  dh, BYTE [Head]       ; head
            mov  dl, BYTE [DriveNum]   ; drive
            int  0x13
            jnc  .SUCCESS              ; check read error
            xor  ah, ah                ; INT 0x13, AH=0, DL=0 --> reset disk
            xor  dl, dl
            int  0x13
            dec  di                    ; decrement error counter
            jnz  .LOOP                 ; read again
        int  0x18
    .SUCCESS:
        mov ax, (0x0E << 8) | '*'      ; Prepare ax for printing a '*' on the screen
        int 0x10                       ; Print progress message
        pop  cx
        pop  ax
        add  bx, WORD [BytesPerSec]    ; queue next buffer
        inc  ax                        ; queue next sector
        loop .NEXTSECTOR               ; read next sector
    ret

;******************************************************************************
;    Print String
;    DS:SI  null-terminated string
;******************************************************************************
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

;******************************************************************************
;    Parameters
;******************************************************************************
Sector         db 0
Head           db 0
Cylinder       db 0
datasector     dw 0
ImageName      db "BOOT2   BIN"
msgLoading     db "Loading Second Stage Bootloader", 0x0D, 0x0A, 0
msgFailure     db 0x0D, 0x0A, "BOOT2.BIN missing", 0x0D, 0x0A, 0

TIMES 510-($-$$) db 0            ; fill bytes until boot signature
dw 0xAA55                        ; boot signature
