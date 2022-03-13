/* decode.c
 * By: John Jekel
 *
 * Code to decode RISC-V instructions
*/

/* Includes */

#include "decode.h"
#include "rv32esim.h"
#include "logging.h"
#include "common.h"
#include "cmake_config.h"

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

/* Static Function Declarations */

static void decode32(const rv32esim_state_t* state, decoded_inst_t* decoded_inst, uint32_t instruction);

#if EXTENSION_C
static void decode16(const rv32esim_state_t* state, decoded_inst_t* decoded_inst, uint16_t instruction);
#endif

/* Function Implementations */

__attribute__ ((visibility ("hidden"))) void decode(const rv32esim_state_t* state, decoded_inst_t* decoded_inst, uint32_t instruction) {
#if EXTENSION_C
    if ((instruction & 0b1) && (instruction & 0b10))//32 bit instruction
        decode32(state, decoded_inst, instruction);
    else
        decode16(state, decoded_inst, (uint16_t)instruction);
#else
    decode32(state, decoded_inst, instruction);
#endif

    rvlog(2, "funct3 = 0x%X", decoded_inst->funct3);
    rvlog(2, "funct7 = 0x%X", decoded_inst->funct7);
    rvlog(2, "opcode = 0x%X", decoded_inst->opcode);
    rvlog(2, "rd = x%u", decoded_inst->rd);
    rvlog(2, "rs1 = x%u", decoded_inst->rs1);
    rvlog(2, "rs2 = x%u", decoded_inst->rs2);
    rvlog(2, "imm = 0x%X", decoded_inst->imm);
#if EXTENSION_C
    rvlog(2, "compressed = %s", decoded_inst->compressed ? "Yes" : "No");
#endif
    rvlog(2, "invalid = %s", decoded_inst->invalid ? "Yes" : "No");
}

/* Static Function Implementations */

static void decode32(const rv32esim_state_t* state, decoded_inst_t* decoded_inst, uint32_t instruction) {
    decoded_inst->invalid = !instruction || (instruction == 0xFFFFFFFF);
    decoded_inst->opcode = (instruction >> 2) & 0b11111;
#if EXTENSION_C
    decoded_inst->compressed = false;
#endif

    switch (decoded_inst->opcode) {
        //R-type
        case OP:
            rvlog(2, "format = R");
            decoded_inst->funct7 = (instruction >> 25) & 0b1111111;
            decoded_inst->rs2 = (instruction >> 20) & 0b1111;
            decoded_inst->rs1 = (instruction >> 15) & 0b1111;
            decoded_inst->funct3 = (instruction >> 12) & 0b111;
            decoded_inst->rd = (instruction >> 7) & 0b1111;
            break;
        //I-type
        case LOAD:
        case OP_IMM:
        case JALR:
        case SYSTEM:
            rvlog(2, "format = I");
            decoded_inst->imm = SIGN_EXTEND_TO_32(instruction >> 20, 12);
            decoded_inst->rs1 = (instruction >> 15) & 0b1111;
            decoded_inst->funct3 = (instruction >> 12) & 0b111;
            decoded_inst->rd = (instruction >> 7) & 0b1111;
            decoded_inst->funct7 = (decoded_inst->funct3 == 0b101) ? instruction >> 25 : 0;//We need funct7 for immediate shifts, otherwise it should be 0 so that execute isn't confused
            break;
        //S-type
        case STORE:
            rvlog(2, "format = S");
            decoded_inst->imm = SIGN_EXTEND_TO_32(((instruction >> 20) & 0b111111100000) | ((instruction >> 7) & 0b11111), 12);
            decoded_inst->rs2 = (instruction >> 20) & 0b1111;
            decoded_inst->rs1 = (instruction >> 15) & 0b1111;
            decoded_inst->funct3 = (instruction >> 12) & 0b111;
            break;
        //B-type
        case BRANCH:
            rvlog(2, "format = B");
            decoded_inst->imm = SIGN_EXTEND_TO_32(((instruction >> 19) & 0b1000000000000) | ((instruction << 4) & 0b100000000000) | ((instruction >> 20) & 0b11111100000) | ((instruction >> 7) & 0b11110), 13);
            decoded_inst->rs2 = (instruction >> 20) & 0b1111;
            decoded_inst->rs1 = (instruction >> 15) & 0b1111;
            decoded_inst->funct3 = (instruction >> 12) & 0b111;
            break;
        //U-type
        case LUI:
        case AUIPC:
            rvlog(2, "format = U");
            decoded_inst->imm = instruction & 0b11111111111111111111000000000000;
            decoded_inst->rd = (instruction >> 7) & 0b1111;
            break;
        //J-type
        case JAL:
            rvlog(2, "format = J");
            decoded_inst->imm = SIGN_EXTEND_TO_32(((instruction >> 11) & 0b100000000000000000000) | (instruction & 0b11111111000000000000) | ((instruction >> 9) & 0b100000000000) | ((instruction >> 20) & 0b11111111110), 21);
            decoded_inst->rd = (instruction >> 7) & 0b1111;
            break;
        default:
            rvlog(2, "format = Unknown/Custom");
            //This is an unused/reserved/custom opcode
    }
}

#if EXTENSION_C
static void decode16(const rv32esim_state_t* state, decoded_inst_t* decoded_inst, uint16_t instruction) {
    decoded_inst->compressed = true;
    assert(false);//TODO implement
}
#endif
