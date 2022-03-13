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

static uint32_t alu_operation(const rv32esim_state_t* state, uint8_t funct3, uint8_t funct7, uint32_t a, uint32_t b);
static bool branch_operation(const rv32esim_state_t* state, uint8_t funct3, uint32_t rs1, uint32_t rs2);
static uint32_t mem_read(rv32esim_state_t* state, uint8_t funct3, uint32_t address);
static void mem_write(rv32esim_state_t* state, uint8_t funct3, uint32_t data, uint32_t address);
static bool callback_needed(const rv32esim_state_t* state, uint32_t address);

/* Function Implementations */

__attribute__ ((visibility ("hidden"))) rv32esim_return_code_t execute(rv32esim_state_t* state, const decoded_inst_t* decoded_inst) {
    switch (decoded_inst->opcode) {
        case OP: {
            rvlog(1, "pretty opcode = \"OP\"\n");

            uint32_t result = alu_operation(state, decoded_inst->funct3, decoded_inst->funct7, GET_X(decoded_inst->rs1), GET_X(decoded_inst->rs2));
            rvlog(2, "result (dec, hex, signed) = (%llu, 0x%llX, %lld)\n", result, result, result);

            if (decoded_inst->rd)
                state->x[decoded_inst->rd - 1] = result;

            state->pc += SEQ_PC_OFFSET();
            return SUCCESS;
        }
        case OP_IMM: {
            rvlog(1, "pretty opcode = \"OP-IMM\"\n");

            uint32_t result = alu_operation(state, decoded_inst->funct3, decoded_inst->funct7, GET_X(decoded_inst->rs1), decoded_inst->imm);
            rvlog(2, "result (dec, hex, signed) = (%llu, 0x%llX, %lld)\n", result, result, result);

            if (decoded_inst->rd)
                state->x[decoded_inst->rd - 1] = result;

            state->pc += SEQ_PC_OFFSET();
            return SUCCESS;
        }
        case LUI: {
            rvlog(1, "pretty opcode = \"LUI\"\n");

            uint32_t result = decoded_inst->imm;

            if (decoded_inst->rd)
                state->x[decoded_inst->rd - 1] = result;

            state->pc += SEQ_PC_OFFSET();
            return SUCCESS;
        }
        case AUIPC: {
            rvlog(1, "pretty opcode = \"AUIPC\"\n");

            uint32_t result = decoded_inst->imm + state->pc;
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

            uint32_t result = state->pc + SEQ_PC_OFFSET();
            rvlog(2, "link value = 0x%llx\n", result);

            if (decoded_inst->rd)
                state->x[decoded_inst->rd - 1] = result;

            state->pc += decoded_inst->imm;
            return SUCCESS;
        }
        case JALR: {
            rvlog(1, "pretty opcode = \"JALR\"\n");

            uint32_t result = state->pc + SEQ_PC_OFFSET();
            rvlog(2, "link value = 0x%llx\n", result);

            if (decoded_inst->rd)
                state->x[decoded_inst->rd - 1] = result;

            state->pc = GET_X(decoded_inst->rs1) + decoded_inst->imm;
            return SUCCESS;
        }
        case BRANCH: {
            rvlog(1, "pretty opcode = \"BRANCH\"\n");

            bool taken = branch_operation(state, decoded_inst->funct3, GET_X(decoded_inst->rs1), GET_X(decoded_inst->rs2));
            rvlog(2, "taken = %s\n", taken ? "Yes" : "No");

            state->pc += taken ? decoded_inst->imm : SEQ_PC_OFFSET();
            return SUCCESS;
        }
        case LOAD: {
            rvlog(1, "pretty opcode = \"LOAD\"\n");

            uint32_t address = GET_X(decoded_inst->rs1) + decoded_inst->imm;
            rvlog(2, "address = 0x%llX\n", address);
            uint32_t data = mem_read(state, decoded_inst->funct3, address);
            rvlog(2, "data = 0x%llX\n", data);

            if (decoded_inst->rd)
                state->x[decoded_inst->rd - 1] = data;

            state->pc += SEQ_PC_OFFSET();
            return SUCCESS;
        }
        case STORE: {
            rvlog(0, "STORE\"\n");

            uint32_t address = GET_X(decoded_inst->rs1) + decoded_inst->imm;
            rvlog(2, "address = 0x%llX\n", address);
            uint32_t data = GET_X(decoded_inst->rs2);
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

static uint32_t alu_operation(const rv32esim_state_t* state, uint8_t funct3, uint8_t funct7, uint32_t a, uint32_t b) {
    rvlog(2, "operand a (dec, hex, signed) = (%llu, 0x%llX, %lld)\n", a, a, a);
    rvlog(2, "operand b (dec, hex, signed) = (%llu, 0x%llX, %lld)\n", b, b, b);

    switch (funct3) {
        case 0b000:
            rvlog(2, "type = add/sub\n");
            return (funct7 & (1 << 5)) ? (a - b) : (a + b);
        case 0b001:
            rvlog(2, "type = sll\n");
            return a << (b & 0b11111);
        case 0b010:
            rvlog(2, "type = slt\n");
            return (((int32_t)a) < ((int32_t)b)) ? 1 : 0;//TODO ensure this is correct
        case 0b011:
            rvlog(2, "type = sltu\n");
            return (a < b) ? 1 : 0;
        case 0b100:
            rvlog(2, "type = xor\n");
            return a ^ b;
        case 0b101:
            rvlog(2, "type = srl/sra\n");
            return (funct7 & (1 << 5)) ? (((int32_t)a) >> (b & 0b11111)) : (a >> (b & 0b11111));//TODO ensure this is correct
        case 0b110:
            rvlog(2, "type = or\n");
            return a | b;
        case 0b111:
            rvlog(2, "type = and\n");
            return a & b;
        default:
            assert(false);
    }
}

static bool branch_operation(const rv32esim_state_t* state, uint8_t funct3, uint32_t rs1, uint32_t rs2) {
    rvlog(2, "rs1 value = %llu\n", rs1);
    rvlog(2, "rs2 value = %llu\n", rs2);

    switch (funct3) {
        case 0b000:
            rvlog(2, "type = BEQ\n");
            return rs1 == rs2;
        case 0b001:
            rvlog(2, "type = BNE\n");
            return rs1 != rs2;
        case 0b100:
            rvlog(2, "type = BLT\n");
            return ((int32_t)rs1) < ((int32_t)rs2);
        case 0b101:
            rvlog(2, "type = BGE\n");
            return ((int32_t)rs1) >= ((int32_t)rs2);
        case 0b110:
            rvlog(2, "type = BLTU\n");
            return rs1 < rs2;
        case 0b111:
            rvlog(2, "type = BGEU\n");
            return rs1 >= rs2;
        default://Undefined
            rvlog(2, "type = Undefined\n");
            return false;//TODO perhaps allow these to be handled as custom?
    }
}

static uint32_t mem_read(rv32esim_state_t* state, uint8_t funct3, uint32_t address) {
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

static void mem_write(rv32esim_state_t* state, uint8_t funct3, uint32_t data, uint32_t address) {
    if (callback_needed(state, address)) {
        switch (funct3) {
            case 0b000:
                if (state->mem_w_callback8)
                    state->mem_w_callback8(state, data, address);
                return;
            case 0b001:
                if (state->mem_w_callback16)
                    state->mem_w_callback16(state, data, address);
                return;
            case 0b010:
                if (state->mem_w_callback32)
                    state->mem_w_callback32(state, data, address);
                return;
            default:
                assert(false);
        }
    } else {
#if LITTLE_ENDIAN//Little endian host makes this easy+fast :)
    switch (funct3) {
        case 0b000:
            ((uint8_t*)state->mem)[address - state->mem_begin_address] = (uint8_t)data;
            return;
        case 0b001:
            ((uint16_t*)state->mem)[(address - state->mem_begin_address) / 2] = (uint16_t)data;
            return;
        case 0b010:
            ((uint32_t*)state->mem)[(address - state->mem_begin_address) / 4] = (uint32_t)data;
            return;
        default:
            assert(false);
    }
#else
#error TODO implement fetching if host is not known to be little endian
#endif
    }
}

static bool callback_needed(const rv32esim_state_t* state, uint32_t address) {
    return (address < state->mem_begin_address) || (address > (state->mem_begin_address + state->mem_len));
}
