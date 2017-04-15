#ifndef PCB_H
#define PCB_H

// max num of file descriptors is 8 per task
#define MAX_DESC            8
#define END_OF_KERNEL_PAGE  0x800000
#define PCB_PLUS_STACK      8192
#define FOUR_MB             0x400000
#define EIGHT_KB            0x2000

#include "file_system.h"
#include "rtc.h"
#include "terminal.h"

// sets flag field of file descriptors
typedef enum {NOT_USED, IN_USE} status_t;

typedef struct f_ops_table {
    int32_t (*open)(const uint8_t* filename);
    int32_t (*read)(int32_t fd, void *buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const void *buf, int32_t nbytes);
    int32_t (*close)(int32_t fd);
} f_ops_table;

typedef struct fdesc {
    f_ops_table file_ops;               // pointer to file operations jump table
    uint32_t inode;                     // inode number
    uint32_t file_pos;                  // current position in file
    status_t flags;                     // status of file at the moment
} fdesc_t;

// PCB below; any additional structs should be defined above this point
// Forward declaration so that a pcb can contain another pcb
typedef struct pcb_t pcb_t;
struct pcb_t {
    // I feel like there's a bunch of stuff missing here... but idk what.
    fdesc_t io_files[MAX_DESC];         // file descriptor array
    int fd_status;                      // bitmap of which fds are occupied
    uint32_t gpr[8];                    // eax ebx ecx edx ebp esi edi esp (order subject to change)
    uint32_t* page_dir;                 // pointer to process's page directory
    pcb_t* parent_task;                 // pointer to parent task's PCB
};


pcb_t * init_pcb();
int32_t open_file_desc(pcb_t *blk, f_ops_table file_op, uint32_t inode_num);
int32_t close_file_desc(pcb_t *blk, uint32_t fd_id);

pcb_t *find_empty_pcb(void);
void done_with_pcb(pcb_t* old_pcb);
int find_empty_fd(void);
void done_with_fd(int fd);

extern pcb_t *cur_pcb;
extern uint32_t pcb_status;

#endif /* PCB_H */
