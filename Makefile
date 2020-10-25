#!Makefile
# [Make 命令教程](http://www.ruanyifeng.com/blog/2015/02/make.html)
# [Grub Multiboot规范](https://www.cnblogs.com/chio/archive/2008/01/01/1022430.html)
# clang-format -style=llvm -dump-config > .clang-format
C_SOURCES = $(shell find . -name "*.c")
C_OBJECTS = $(patsubst %.c, %.o, $(C_SOURCES))
S_SOURCES = $(shell find . -name "*.s")
S_OBJECTS = $(patsubst %.s, %.o, $(S_SOURCES))


CC = gcc
LD = ld
ASM = nasm
# -c 编译、汇编到目标代码，不进行链接
# -Wall 参数启用所有警告
# -m32 是生成32位代码
# -ggdb -gstabs+ 添加相关的调试信息
# -nostdinc 不包含C语言的标准库头文件
# -fno_pic  不要使用与位置无关的代码
# -fno-builtin 要求 gcc 不主动使用自己的内建函数，除非显式声明
# -fno-stack-protector 不使用栈保护等检测
C_FLAGS = -c -Wall -m32 -ggdb -gstabs+ -nostdinc -fno-pic -fno-builtin -fno-stack-protector -fgnu89-inline -I include
# -nostdlib 不链接C语言的标准库
LD_FLAGS = -T scripts/kernel.ld -m elf_i386 -nostdlib
# -g 生成调试信息
# -F stabs 选择调试格式 stabs是调试的信息，存储在符号表中。 名称来自 symbol table entries
# 调试信息包含了源文件的行号，类型变量，函数，以及他们的参数及作用域等
ASM_FLAGS = -f elf -g -F stabs

RM = rm -rf 

all: $(S_OBJECTS) $(C_OBJECTS) link update_image

.c.o:
	@echo 编译代码文件 $< ...
	$(CC) $(C_FLAGS) $< -o $@

.s.o:
	@echo 编译汇编文件 $< ...
	$(ASM) $(ASM_FLAGS) $<

link:
	@echo 链接内核文件...
	$(LD) $(LD_FLAGS) $(S_OBJECTS) $(C_OBJECTS) -o hx_kernel

.PHONY:clean
clean:
	$(RM) $(S_OBJECTS) $(C_OBJECTS) hx_kernel

.PHONY:update_image
update_image:
	sudo mount floppy.img /mnt/kernel
	sudo cp hx_kernel /mnt/kernel/hx_kernel
	sleep 1
	sudo umount /mnt/kernel

.PHONY:bochs
bochs:
	bochs -f bochsrc.txt
