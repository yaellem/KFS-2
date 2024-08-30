#include <stdint.h>

#define BASE_ADDR_LEN   32  // Base address of the segment described
#define LIMIT_LEN       20  // Size of the segment described
#define SEGM_TYPE_LEN   4   // Specify the segment type (code or data) 
#define DESC_TYPE_LEN   1   // Indicate if the the descriptor is a system (IDT, LDT, TSS) one or a code/data one
#define DPL_LEN         2   // Descripor privilege level indicates if the ring level of the segment
#define SPRESENT_LEN    1   // Indicates if the segment is present in memory
#define AVL_LEN         1   // Used by system software for its personnal use
#define CODE_SEG64_LEN  1   // Usd in 64bit mode to indicate if a code segment contains 64bit code or 32bit
#define DB_LEN          1   // For a code segment, it specifies the address and operand size. For a stack segment, it specifies the size of the stack pointer. For an expand down data segment, it specifies the upper bound of the segment (4Go or 64Ko)
#define GRAN_LEN        1   // It determines the scaling of the segment limit field. Wether it counts in byte or in 4Ko units. 

typedef struct  segment_desc {
    uint32_t    base_addr:BASE_ADDR_LEN;
    uint32_t    limit:LIMIT_LEN;
    uint8_t     stype:SEGM_TYPE_LEN;
    uint8_t     dtype:DESC_TYPE_LEN;
    uint8_t     dpl:DPL_LEN;
    uint8_t     spresent:SPRESENT_LEN;
    uint8_t     avl:AVL_LEN;
    uint8_t     code_seg64:CODE_SEG64_LEN;
    uint8_t     db:DB_LEN;
    uint8_t     granuarity:GRAN_LEN;
}               t_segment_desc;

// void create_segment_descriptor(uint32_t base_addr, uint32_t limit)
void create_segment_descriptor(t_segment_desc *descriptor)
{

}

void  create_segment_selector(uint16_t index, uint8_t table_id, uint8_t rpl)
{

}

void create_gdt()
{
// Null descriptor
// Kernel Code
// Kernel Data
// Task State segment: for kernel threads ?
// Place for other segment (LDTs for user, other TSS...)
}