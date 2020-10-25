#ifndef INCLUDE_VARGS_H_

#define INCLUDE_VARGS_H_
// 参考资源 https://www.coder.work/article/6937614
// typedef __builtin_va_list va_list;

// #define va_start(ap, last) (__builtin_va_start(ap, last))
// #define va_arg(ap, type) (__builtin_va_arg(ap, type))
// #define va_end(ap)

// 下面是简化版本的定义
// 事实上出于x86压栈元素长度的限制和优化的考虑，小于等于4字节的类型统一扩展到4字节压栈。
// 大于4字节小于等于8字节的类型统一以8字节压栈（另外32位压栈指令的操作数只能是16位或者32位的)
#define va_list char*

#define va_start(p, first) (p = (va_list)&first + sizeof(first))
#define va_arg(p, next) (*(next*)((p += sizeof(next)) - sizeof(next)))
#define va_end(p) (p = (va_list)NULL)

#endif