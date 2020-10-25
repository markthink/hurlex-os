#include "console.h"
#include "debug.h"
#include "gdt.h"
#include "idt.h"
#include "pmm.h"
#include "timer.h"
#include "vmm.h"
#include "heap.h"
#include "sched.h"


//内核初始化函数
void kern_init();

//开启分页机制之后的 Multiboot 数据指针
multiboot_t *glb_mboot_ptr;

//开启分页机制之后的内核栈
char kern_stack[STACK_SIZE];

//内核栈的栈顶
uint32_t kern_stack_top;

//内核使用的临时页表和页目录
//该地址必须是页对齐的地址，内存 0 - 640KB 肯定是空闲的
__attribute__((section(".init.data"))) pgd_t *pgd_tmp = (pgd_t *)0x10000;
__attribute__((section(".init.data"))) pgd_t *pte_low = (pgd_t *)0x20000;
__attribute__((section(".init.data"))) pgd_t *pte_hign = (pgd_t *)0x30000;

//内核入口函数
__attribute__ ((section(".init.text"))) void kern_entry() {
  pgd_tmp[0] = (uint32_t)pte_low | PAGE_PRESENT | PAGE_WRITE;
  pgd_tmp[PGD_INDEX(PAGE_OFFSET)] = (uint32_t)pte_hign | PAGE_PRESENT | PAGE_WRITE;

  //映射内核虚拟地址 4MB 到物理地址的前 4MB
  int i;
  for (i = 0 ;i < 1024; i++){
    pte_low[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
  }

  //映射 0x00000000 - 0x00400000 的物理地址到虚拟地址 0xC0000000-0xC0400000
  for ( i = 0; i < 1024; i++){
    pte_hign[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
  }

  //设置时时页表
  asm volatile ("mov %0, %%cr3" : : "r" (pgd_tmp));

  uint32_t cr0;

  // 启用分页，将 cr0 寄存器的分页位置1就好
  asm volatile ("mov %%cr0, %0" : "=r" (cr0));
  cr0 |= 0x80000000;
  asm volatile ("mov %0, %%cr0" : : "r" (cr0));

  //切换内核栈
  uint32_t kern_stack_top = ((uint32_t)kern_stack + STACK_SIZE) & 0xFFFFFFF0;

  asm volatile ("mov %0, %%esp\n\t"
    "xor %%ebp, %%ebp" : : "r" (kern_stack_top));

  //更新全局 multiboot 指针
  glb_mboot_ptr = mboot_ptr_tmp + PAGE_OFFSET;

  // 调用内核初始化函数
  kern_init();
}

int flag = 0;
int thread(void *arg)
{
  while (1) {
    if (flag == 1) {
      printk_color(rc_black, rc_red, "B");
      flag = 0;
    }
  }
  return 0;
}

void kern_init()
{
  init_debug();
  init_gdt();
  init_idt();

  console_clear();
  // console_write_color("Hello, OS Kernel!\n", rc_black, rc_green);
  printk_color(rc_black, rc_green, "Hello, OS Kernel!\n");

  init_timer(200);
  // asm volatile ("sti");
  // print_cur_status();
  // panic("test");

  // asm volatile("int $0x3");
  // asm volatile("int $0x4");

  printk("kernel in memory start: 0x%08X\n", kern_start);
  printk("kernel in memory end: 0x%08X\n", kern_end);
  printk("kernel in memory used: %d KB\n\n", (kern_end - kern_start) / 1024);

  // show_memory_map();
  init_pmm();

  printk_color(rc_black, rc_red,
               "\nThe Count of Physical Memory Page is: %u\n\n",
               phy_page_count);

  // uint32_t allc_addr = NULL;
  // printk_color(rc_black, rc_light_brown, "Test Physical Memory Alloc: \n");

  // allc_addr = pmm_alloc_page();
  // printk_color(rc_black, rc_light_brown, "Alloc Physical Addr: 0x%08X\n",
  //              allc_addr);

  // allc_addr = pmm_alloc_page();
  // printk_color(rc_black, rc_light_brown, "Alloc Physical Addr: 0x%08X\n",
  //              allc_addr);

  // allc_addr = pmm_alloc_page();
  // printk_color(rc_black, rc_light_brown, "Alloc Physical Addr: 0x%08X\n",
  //              allc_addr);

  // allc_addr = pmm_alloc_page();
  // printk_color(rc_black, rc_light_brown, "Alloc Physical Addr: 0x%08X\n",
  //              allc_addr);

  init_vmm();
  init_heap();
  test_heap();
  init_sched();

  enable_intr();

  while (1) {
    if (flag == 0) {
      printk_color(rc_black, rc_red, "A");
      flag = 1;
    }
  }

  while (1)
  {
    asm volatile ("hlt");
  }
  
}