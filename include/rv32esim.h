/* rv32esim
 * By: John Jekel
 *
 * RISC-V Architecture Simulator (single-hart, little endian, RV32ICE)
*/

#ifndef RVSIM32IE_H
#define RVSIM32IE_H

/* Includes */

#include <stdint.h>
#include <stdbool.h>

/* Constants And Defines */

//TODO

/* Macros */

//TODO

/* Types */

//TODO decide what to do with callbacks vs return codes

typedef enum {SUCCESS, CUSTOM_OPCODE, ECALL, EBREAK, INVALID} rv32esim_return_code_t;//TODO others/error codes/success

typedef struct rv32esim_state_t {
    /* Things that are okay to modify */
    uint32_t x[15];//[0] is not stored since it is always 0
    uint32_t pc;

    //Called if memory that is not mapped below is accessed
    //If null, will be interpreted the same as if returning false
    uint32_t (*mem_r_callback8)(struct rv32esim_state_t* state, uint32_t address);
    void (*mem_w_callback8)(struct rv32esim_state_t* state, uint8_t data, uint32_t address);
    uint32_t (*mem_r_callback16)(struct rv32esim_state_t* state, uint64_t address);
    void (*mem_w_callback16)(struct rv32esim_state_t* state, uint16_t data, uint32_t address);
    uint32_t (*mem_r_callback32)(struct rv32esim_state_t* state, uint64_t address);
    void (*mem_w_callback32)(struct rv32esim_state_t* state, uint32_t data, uint32_t address);
    void* p_flex;//Can point to/be used for whatever for callbacks to access

    //TODO callbacks for traps, or just use rvsim_return_code_t

    uint32_t mem_begin_address;//In the simulated cpu's address space
    uint32_t mem_len;//Length of memory (also of memory in simulated cpu's address space)
    void* mem;//NOTE if the host does not support unaligned accesses, this must be alligned to 4 bytes

    bool logging_enabled;//Ignored if disabled in CMAKE
    uint64_t log_when_ninsts_ge;
    uint64_t ninsts_executed;//Used during logging, may also be of interest externally

    /* Things for rvsim internal use */
} rv32esim_state_t;

/* Function Declarations */

///Initializes state, allocating memory contiguously starting from the hart's address 0, with PC set to zero, ninsts_executed set to 0, logging disabled, and callbacks set to NULL. All other members undefined
void rv32esim_init_state(rv32esim_state_t* state, uint32_t mem_len);
///Frees resources in state
void rv32esim_free_state(rv32esim_state_t* state);

rv32esim_return_code_t rv32esim_tick(rv32esim_state_t* state);

uint8_t rv32esim_version_major();
uint8_t rv32esim_version_minor();
const char* rv32esim_version_string();

#endif//RVSIM32IE_H
