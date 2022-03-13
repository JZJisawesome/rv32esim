/* execute.c
 * By: John Jekel
 *
 * Code to execute decoded RISC-V instructions
*/

/* Constants And Defines */

//TODO

/* Includes */

#include "execute.h"
#include "rv32esim.h"
#include "decode.h"
#include "logging.h"
#include "common.h"

#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

/* Types */

//TODO

/* Variables */

//TODO

/* Macros */

#define GET_X(index) (index ? state->x[index - 1] : 0)
#define SEQ_PC_OFFSET() (decoded_inst->compressed ? 2 : 4)

/* Static Function Declarations */

static uint32_t alu_operation(uint8_t funct3, uint8_t funct7, uint32_t a, uint32_t b);
static bool branch_operation(uint8_t funct3, uint32_t rs1, uint32_t rs2);
static uint32_t mem_read(const rv32esim_state_t* state, uint8_t funct3, uint32_t address);
static void mem_write(rv32esim_state_t* state, uint8_t funct3, uint32_t data, uint32_t address);
static bool callback_needed(const rv32esim_state_t* state, uint64_t address);

/* Function Implementations */

__attribute__ ((visibility ("hidden"))) rv32esim_return_code_t execute(rv32esim_state_t* state, const decoded_inst_t* decoded_inst) {
    switch (decoded_inst->opcode) {
        case OP: {
            rvlog(1, "pretty opcode = \"OP\"\n");

            uint64_t result = alu_operation(decoded_inst->funct3, decoded_inst->funct7, GET_X(decoded_inst->rs1), GET_X(decoded_inst->rs2));
            rvlog(2, "result (dec, hex, signed) = (%llu, 0x%llX, %lld)\n", result, result, result);

            if (decoded_inst->rd)
                state->x[decoded_inst->rd - 1] = result;

            state->pc += SEQ_PC_OFFSET();
            return SUCCESS;
        }
        case OP_IMM: {
            rvlog(1, "pretty opcode = \"OP-IMM\"\n");

            uint64_t result = alu_operation(decoded_inst->funct3, decoded_inst->funct7, GET_X(decoded_inst->rs1), decoded_inst->imm);
            rvlog(2, "result (dec, hex, signed) = (%llu, 0x%llX, %lld)\n", result, result, result);

            if (decoded_inst->rd)
                state->x[decoded_inst->rd - 1] = result;

            state->pc += SEQ_PC_OFFSET();
            return SUCCESS;
        }
        case LUI: {
            rvlog(1, "pretty opcode = \"LUI\"\n");

            uint64_t result = decoded_inst->imm;

            if (decoded_inst->rd)
                state->x[decoded_inst->rd - 1] = result;

            state->pc += SEQ_PC_OFFSET();
            return SUCCESS;
        }
        case AUIPC: {
            rvlog(1, "pretty opcode = \"AUIPC\"\n");

            uint64_t result = decoded_inst->imm + state->pc;
            rvlog(2, "result = 0x%llx\n", result);

            if (decoded_inst->rd)
                state->x[decoded_inst->rd - 1] = result;

            state->pc += SEQ_PC_OFFSET();
            return SUCCESS;
        }
        case MISC_MEM: {
            rvlog(1, "pretty opcode = \"MISC-MEM\"\n");
            state->pc += SEQ_PC_OFFSET();
            return SUCCESS;
        }
        case SYSTEM: {
            rvlog(1, "pretty opcode = \"SYSTEM\"\n");
            state->pc += SEQ_PC_OFFSET();
            if (decoded_inst->imm)//Lsb is set
                return EBREAK;
            else
                return ECALL;
        }
        case JAL: {
            rvlog(1, "pretty opcode = \"JAL\"\n");

            uint64_t result = state->pc + SEQ_PC_OFFSET();
            rvlog(2, "link value = 0x%llx\n", result);

            if (decoded_inst->rd)
                state->x[decoded_inst->rd - 1] = result;

            state->pc += decoded_inst->imm;
            return SUCCESS;
        }
        case JALR: {
            rvlog(1, "pretty opcode = \"JALR\"\n");

            uint64_t result = state->pc + SEQ_PC_OFFSET();
            rvlog(2, "link value = 0x%llx\n", result);

            if (decoded_inst->rd)
                state->x[decoded_inst->rd - 1] = result;

            state->pc = GET_X(decoded_inst->rs1) + decoded_inst->imm;
            return SUCCESS;
        }
        case BRANCH: {
            rvlog(1, "pretty opcode = \"BRANCH\"\n");

            bool taken = branch_operation(decoded_inst->funct3, GET_X(decoded_inst->rs1), GET_X(decoded_inst->rs2));
            rvlog(2, "taken = %s\n", taken ? "Yes" : "No");

            state->pc += taken ? decoded_inst->imm : SEQ_PC_OFFSET();
            return SUCCESS;
        }
        case LOAD: {
            rvlog(1, "pretty opcode = \"LOAD\"\n");

            uint64_t address = GET_X(decoded_inst->rs1) + decoded_inst->imm;
            rvlog(2, "address = 0x%llX\n", address);
            uint64_t data = mem_read(state, decoded_inst->funct3, address);
            rvlog(2, "data = 0x%llX\n", data);

            if (decoded_inst->rd)
                state->x[decoded_inst->rd - 1] = data;

            state->pc += SEQ_PC_OFFSET();
            return SUCCESS;
        }
        case STORE: {
            rvlog(0, "STORE\"\n");

            uint64_t address = GET_X(decoded_inst->rs1) + decoded_inst->imm;
            rvlog(2, "address = 0x%llX\n", address);
            uint64_t data = GET_X(decoded_inst->rs2);
            rvlog(2, "data = 0x%llX\n", data);

            mem_write(state, decoded_inst->funct3, data, address);
            state->pc += SEQ_PC_OFFSET();
            return SUCCESS;
        }
        default: {
            rvlog(0, "Custom/Unknown\"\n");
            return CUSTOM_OPCODE;
        }
    }
}

/* Static Function Implementations */

__attribute__ ((visibility ("hidden"))) static uint32_t alu_operation(uint8_t funct3, uint8_t funct7, uint32_t a, uint32_t b) {

}

__attribute__ ((visibility ("hidden"))) static bool branch_operation(uint8_t funct3, uint32_t rs1, uint32_t rs2) {

}

__attribute__ ((visibility ("hidden"))) static uint32_t mem_read(const rv32esim_state_t* state, uint8_t funct3, uint32_t address) {
    if (callback_needed(state, address)) {
        switch (funct3) {
            case 0b000:
                return SIGN_EXTEND_TO_32(state->mem_r_callback8 ? state->mem_r_callback8(state, address) : 0, 8);
            case 0b001:
                return SIGN_EXTEND_TO_32(state->mem_r_callback16 ? state->mem_r_callback16(state, address) : 0, 16);
            case 0b010:
                return state->mem_r_callback32 ? state->mem_r_callback32(state, address) : 0;
            case 0b100:
                return state->mem_r_callback8 ? state->mem_r_callback8(state, address) : 0;
            case 0b101:
                return state->mem_r_callback16 ? state->mem_r_callback16(state, address) : 0;
            default:
                assert(false);
        }
    } else {
#if LITTLE_ENDIAN//Little endian host makes this easy+fast :)
        switch (funct3) {
            case 0b000:
                return SIGN_EXTEND_TO_32(((uint8_t*)state->mem)[address - state->mem_begin_address], 8);
            case 0b001:
                return SIGN_EXTEND_TO_32(((uint16_t*)state->mem)[(address - state->mem_begin_address) / 2], 16);
            case 0b010:
                return ((uint32_t*)state->mem)[(address - state->mem_begin_address) / 4];
            case 0b100:
                return ((uint8_t*)state->mem)[address - state->mem_begin_address];
            case 0b101:
                return ((uint16_t*)state->mem)[(address - state->mem_begin_address) / 2];
            default:
                assert(false);
        }
#else
#error TODO implement fetching if host is not known to be little endian
#endif
    }
}

__attribute__ ((visibility ("hidden"))) static void mem_write(rv32esim_state_t* state, uint8_t funct3, uint32_t data, uint32_t address) {

}

__attribute__ ((visibility ("hidden"))) static bool callback_needed(const rv32esim_state_t* state, uint64_t address) {

}
