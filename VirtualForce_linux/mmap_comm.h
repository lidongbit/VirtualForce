#ifndef __MMAP_COMM_H__
#define __MMAP_COMM_H__

typedef struct
{
    void  *comm_ptr;
    void  *para_ptr;
    void  *cmd_ptr;
    void  *cmd_bak_ptr;
    void  *ins_ptr;
    void  *state_ptr;
    void  *res_ptr;

    void *monitor_ptr;
    void *virtual_ptr;
}MMAP_COMM_PTR_t;

extern void mmap_init(void);
extern void mmap_get_comm_ptr(MMAP_COMM_PTR_t **ptr);
extern void mmap_close(void);
#endif
