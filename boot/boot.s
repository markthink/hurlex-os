; ------------------------------------------------------
MBOOT_HEADER_MAGIC  equ     0x1BADB002
MBOOT_PAGE_ALIGN    equ     1 << 0 ; 0 号位表示所有的引导模块按页 4kb 边界对齐
MBOOT_MEM_INFO      equ     1 << 1 ; 1 号位通过 Multiboot 信息结构的 mem_* 域包括可用内存的信息

; 定义 Multiboot 标记
MBOOT_HEADER_FLAGS  equ     MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
; 域 checksum 是一个 32 位的无符号值
MBOOT_CHECKSUM      equ     - (MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

; 符合 Multiboot 规范的 OS 映象需要这样一个 magic Multiboot Header 
; ------------------------------------------------------
[BITS 32]
section .text ;代码从这里开始
; 在代码段的起始位置设置符合 Multiboot 规范的标记
dd MBOOT_HEADER_MAGIC
dd MBOOT_HEADER_FLAGS
dd MBOOT_CHECKSUM

[GLOBAL start]                  ;内核代码入口，此处提供该声明给 ld 链接器
[GLOBAL glb_mboot_ptr]          ;全局的 struct multiboot * 变量
[EXTERN kern_entry]             ;声明内核 C 代码的入口函数

start:
    cli
    
    mov [glb_mboot_ptr], ebx    ; 将 ebx 中存储的指针存入 glb_mboot_ptr 变量
    mov esp, STACK_TOP          ; 设置内核栈地址，按照 multiboot 规范，当需要使用堆栈时，OS 映象必须自己创建一个
    and esp, 0FFFFFFF0H         ; 栈地址按照 16 字节对齐
    mov ebp, 0                  ; 帧指针修改为 0

    call kern_entry             ; 调用内核入口函数
stop:
    hlt
    jmp stop

; ------------------------------------------------------
section .bss                    ; 未初始化的数据段从这里开始
stack:
    resb    32768               ; 这里作为内核栈

glb_mboot_ptr:                  ; 全局的 multiboot 结构体指针
    resb 4
STACK_TOP   equ     $-stack-1   ; 内核栈顶，$ 符指代是当前地址
; ------------------------------------------------------







