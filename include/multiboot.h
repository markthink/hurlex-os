#ifndef INCLUDE_MULTIBOOT_H_
#define INCLUDE_MULTIBOOT_H_

#include "types.h"

/*
启动后，在32位内核进入点，机器状态如下：
1. CS 指向基地址为 0x00000000 限长为4G-1 的代码段描述符
2. DS/SS/ES/FS/GS 指向基地址为 0x00000000 限长为4G-1的数据段描述符
3. A20 地址线已经打开
4. 页机制被禁止
5. 中断被禁止
6. EAX = 0x2BADB002
7. 系统信息和启动信息块的线性地址保存在 EBX 中
*/

typedef
struct multiboot_t
{
    uint32_t flags;//版本信息
    //BIOS 获知可用内存
    //低端内存首地址0 高端内存首地址1M
    //低端内存最大可能值是640KB
    //高端内存最大可能值是最大值减去1M

    uint32_t mem_lower;
    uint32_t mem_upper;

    uint32_t boot_device; //指出引导程序从哪个 BIOS磁盘设备载入的OS映象
    uint32_t cmdline;  //内核命令行
    uint32_t mods_count; //boot 模块列表
    uint32_t mods_addr;

    /*
    ELF格式内核映像的 section 头表、每项的大小，一共有几项以及作为名字索引的字符串表
    它们对应于可执行可连接格式 ELF 的 program 头中的 shdr_* 项 (shdr_num 等)
    所有的 section 都会被载入， ELF section 头的物理地址域指向所有的 section 在内存中的位置
    */
   uint32_t num;
   uint32_t size;
   uint32_t addr;
   uint32_t shndx;

   /*
   以下两项指出保存由 BIOS提供的内存分布的缓冲区的地址和长度
   mmap_addr 是缓冲区的地址， mmap_length 是缓冲区的总大小
   缓冲区由一个或多个下面的大小/结构对 mmap_entry_t (size 实际上是用来跳过下一个对的)组成
   */
  uint32_t mmap_length;
  uint32_t mmap_addr;

  uint32_t drives_length; //指出第一个驱动器结构的物理地址
  uint32_t drives_addr; //指出第一个驱动器这个结构的大小
  uint32_t config_table; //rom 配置表
  uint32_t boot_loader_name; //boot loader 的名字
  uint32_t apm_table; //APM表
  uint32_t vbe_control_info;
  uint32_t vbe_mode_info;
  uint32_t vbe_mode;
  uint32_t vbe_interface_seg;
  uint32_t vbe_interface_off;
  uint32_t vbe_interface_len;
} __attribute__((packed)) multiboot_t;


/*
size是相关结构的大小，单位是字节，它可能大于最小值20
base_addr_low 是启动地址的低 32 位，base_addr_high 是高32位，启动地址总共有 64位
length_low 是内存区域大小的低 32 位，length_hight 是内存区域大小的高 32 位，总共是64位
type 是相应地址区间的类型， 1代表可用 RAM，所有其它的值代表保留区域
*/

typedef
struct  mmap_entry_t
{
    uint32_t size;
    uint32_t base_addr_low;
    uint32_t base_addr_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t type;
} __attribute__((packed)) mmap_entry_t;



//声明全局的 multiboot_t * 指针
//内核未建立分页机制前暂存的指针
extern multiboot_t *mboot_ptr_tmp;
//内核页表建立后的指针
extern multiboot_t *glb_mboot_ptr;

#endif