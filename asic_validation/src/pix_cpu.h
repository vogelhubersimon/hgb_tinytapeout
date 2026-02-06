// File: src/pix_cpu.h
// Description: Simple R2R CPU simulator for pixel processor

#ifndef PIX_CPU_H
#define PIX_CPU_H

#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define ERROR_STREAM stderr
#define OUTPUT_STREAM stdout
#define REG_COUNT  8        // Number of registers in CPU
#define REG_BIT_WIDTH 6     // Number of bits per register
#define INSTR_COUNT 20      // Number of instructions in program memory
#define LINE_BUFFER_SIZE 64 // Max line length in source file

// Representation of an instruction
// Data read from file gets parsed into this struct
typedef struct {
    char *identfier;      // string representation of instruciton
    uint8_t operand[3];   // up to 3 operands per instruciton
} instruction_t;

typedef instruction_t program_mem_t[INSTR_COUNT];

// CPU state
typedef struct {
    int program_counter;
    uint16_t x_coord;
    uint16_t y_coord;
    uint16_t time;
    program_mem_t program_mem;
    uint8_t register_file[REG_COUNT];
} cpu_t;

// Debug Functions
void dump_instruction (instruction_t *instr);
void dump_program_memory(program_mem_t const * const program_memory);
void dump_cpu(cpu_t const * const cpu);

// The fun stuff
void init_cpu(cpu_t * const cpu);
int parse_program(char const * const file_path, program_mem_t *program_memory);
int run_program(cpu_t *cpu, uint8_t *result_reg);

#endif//PIX_CPU_H