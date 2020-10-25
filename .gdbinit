file hx_kernel
target remote :1234
b boot.s:25
b kern_entry
c
