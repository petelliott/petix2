#ifndef PAGING_H
#define PAGING_H


//TODO: something more portable
struct page_dir_ent;
typedef struct page_dir_ent * addr_space_t;

void init_paging(void);

addr_space_t create_proc_addr_space(void);
void free_proc_addr_space(addr_space_t as);

// copy the modified parts of an address space
addr_space_t fork_proc_addr_space(addr_space_t as);

void use_addr_space(addr_space_t as);

#endif
