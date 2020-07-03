#ifndef PAGING_H
#define PAGING_H

//TODO: something more portable
struct page_dir_ent;
typedef struct page_dir_ent * addr_space_t;

//TODO: something more portable
#define KERNEL_STACK_SIZE 4096
#define KERNEL_STACK_TOP (char *)0xffffffff
#define USER_STACK_TOP (KERNEL_STACK_TOP - KERNEL_STACK_SIZE)

void init_paging(void);

addr_space_t create_proc_addr_space(void);
void free_proc_addr_space(addr_space_t as);

// copy the modified parts of an address space
addr_space_t fork_proc_addr_space(addr_space_t as);

void use_addr_space(addr_space_t as);

// area must exist
void lock_page(addr_space_t as, void *addr);

#endif
