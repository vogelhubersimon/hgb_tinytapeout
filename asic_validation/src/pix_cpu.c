#include "pix_cpu.h"
#include <math.h>
#include <stdlib.h>
#include <sys/types.h>

// ------------------- //
// INTERNAL PROTOTYPES //
// ------------------- //
static int parse_instrucion (instruction_t *instr, char* line);
static int get_immediate    (char* token, uint8_t* res);
static int get_register     (char* token, uint8_t* res);
static int get_instruction  (instruction_t *instr, char* token);
static int execute_instr(cpu_t * const cpu);

// --------------- //
// DEBUG FUNCTIONS //
// --------------- //
void dump_instruction (instruction_t *instr)
{
    fprintf(OUTPUT_STREAM, "  %-9s   %3u  %3u  %3u\n",  
                instr->identfier,
                instr->operand[0],
                instr->operand[1],
                instr->operand[2]);
}

void dump_program_memory(program_mem_t const * const program_memory)
{
    fprintf(OUTPUT_STREAM, "\n=== PROGRAM MEMORY DUMP ===\n");
    fprintf(OUTPUT_STREAM, "PC   INSTRUCTION   OP0  OP1  OP2\n");
    fprintf(OUTPUT_STREAM, "---  -----------   ---  ---  ---\n");
    
    for (size_t i = 0; i < INSTR_COUNT; i++)
    {
        instruction_t instr = (*program_memory)[i];
        
        // Skip empty/uninitialized instructions
        if (instr.identfier == NULL)
        {
            continue;
        }

        fprintf(OUTPUT_STREAM, "%2zu: ", i);
        dump_instruction(&instr);
        
    }
}

void dump_cpu(cpu_t const * const cpu)
{
    fprintf(OUTPUT_STREAM, "\n=== CPU DUMP ===\n");
    fprintf(OUTPUT_STREAM, "PC   [%d]\n", cpu->program_counter);
    fprintf(OUTPUT_STREAM, "TIME [%d]\n", cpu->time);
    fprintf(OUTPUT_STREAM, "X    [%d]\n", cpu->x_coord);
    fprintf(OUTPUT_STREAM, "Y    [%d]\n", cpu->y_coord);
    
    fprintf(OUTPUT_STREAM, "\n=== REGISTERS ===\n");
    for (size_t i = 0; i < REG_COUNT; i++)
    {
        fprintf(OUTPUT_STREAM, "REG%zu [%d]\n", i, cpu->register_file[i]);
    }  
    dump_program_memory(&cpu->program_mem);
}

// -------------- //
// INIT FUNCTIONS //
// -------------- //

void init_instruction_mem(program_mem_t * instr_mem)
{
    for (size_t i = 0; i < INSTR_COUNT; i++)
    {
        (*instr_mem)[i].identfier = "NOOP";
        (*instr_mem)[i].operand[0] = 0;
        (*instr_mem)[i].operand[1] = 0;
        (*instr_mem)[i].operand[2] = 0;
    }
}

void init_cpu(cpu_t * const cpu)
{
    cpu->program_counter = 0;
    cpu->time = 0;
    cpu->x_coord = 0;
    cpu->y_coord = 0;
    for (size_t i = 0; i < REG_COUNT; i++)
    {
        cpu->register_file[i] = 0;
    } 
    init_instruction_mem(&cpu->program_mem);
}

// -------------- //
// INSTRUCTIONS   //
// -------------- //
static void noop_handler(cpu_t * const cpu)
{
    (void) cpu;
}

static void get_x_handler(cpu_t * const cpu)
{
    uint8_t rd = cpu->program_mem[cpu->program_counter].operand[0];
    cpu->register_file[rd] = cpu->x_coord;
}

static void get_y_handler(cpu_t * const cpu)
{
    uint8_t rd = cpu->program_mem[cpu->program_counter].operand[0];
    cpu->register_file[rd] = cpu->y_coord;
}

static void get_time_handler(cpu_t * const cpu)
{
    uint8_t rd = cpu->program_mem[cpu->program_counter].operand[0];
    cpu->register_file[rd] = cpu->time;
}

static void move_handler(cpu_t * const cpu)
{
    uint8_t rs = cpu->program_mem[cpu->program_counter].operand[0];
    uint8_t rd = cpu->program_mem[cpu->program_counter].operand[1];
    cpu->register_file[rd] = cpu->register_file[rs];
}

static void add_handler(cpu_t *cpu)
{
    uint8_t rs0 = cpu->program_mem[cpu->program_counter].operand[0];
    uint8_t rs1 = cpu->program_mem[cpu->program_counter].operand[1];
    uint8_t rd = cpu->program_mem[cpu->program_counter].operand[2];
    cpu->register_file[rd] = cpu->register_file[rs0] + cpu->register_file[rs1];  
}

static void sub_handler(cpu_t *cpu)
{
    uint8_t rs0 = cpu->program_mem[cpu->program_counter].operand[0];
    uint8_t rs1 = cpu->program_mem[cpu->program_counter].operand[1];
    uint8_t rd = cpu->program_mem[cpu->program_counter].operand[2];
    cpu->register_file[rd] = cpu->register_file[rs0] - cpu->register_file[rs1]; 
}

static void sl_handler(cpu_t *cpu)
{
    uint8_t rs0 = cpu->program_mem[cpu->program_counter].operand[0];
    uint8_t rs1 = cpu->program_mem[cpu->program_counter].operand[1];
    uint8_t rd = cpu->program_mem[cpu->program_counter].operand[2]; 
    cpu->register_file[rd] = cpu->register_file[rs0] << cpu->register_file[rs1]; 
}

static void sr_handler(cpu_t *cpu)
{
    uint8_t rs0 = cpu->program_mem[cpu->program_counter].operand[0];
    uint8_t rs1 = cpu->program_mem[cpu->program_counter].operand[1];
    uint8_t rd = cpu->program_mem[cpu->program_counter].operand[2]; 
    cpu->register_file[rd] = cpu->register_file[rs0] >> cpu->register_file[rs1]; 
}

static void rand_handler(cpu_t *cpu)
{
    // TODO: Implement random number generator
    uint8_t rd = cpu->program_mem[cpu->program_counter].operand[0];
    cpu->register_file[rd] = rand() % 256;
}

static void and_handler(cpu_t *cpu)
{
    uint8_t rs0 = cpu->program_mem[cpu->program_counter].operand[0];
    uint8_t rs1 = cpu->program_mem[cpu->program_counter].operand[1];
    uint8_t rd = cpu->program_mem[cpu->program_counter].operand[2];
    cpu->register_file[rd] = cpu->register_file[rs0] & cpu->register_file[rs1];
}

static void nand_handler(cpu_t *cpu)
{
    uint8_t rs0 = cpu->program_mem[cpu->program_counter].operand[0];
    uint8_t rs1 = cpu->program_mem[cpu->program_counter].operand[1];
    uint8_t rd = cpu->program_mem[cpu->program_counter].operand[2];
    cpu->register_file[rd] = ~(cpu->register_file[rs0] & cpu->register_file[rs1]);
}

static void or_handler(cpu_t *cpu)
{
    uint8_t rs0 = cpu->program_mem[cpu->program_counter].operand[0];
    uint8_t rs1 = cpu->program_mem[cpu->program_counter].operand[1];
    uint8_t rd = cpu->program_mem[cpu->program_counter].operand[2];
    cpu->register_file[rd] = cpu->register_file[rs0] | cpu->register_file[rs1];
}

static void nor_handler(cpu_t *cpu)
{
    uint8_t rs0 = cpu->program_mem[cpu->program_counter].operand[0];
    uint8_t rs1 = cpu->program_mem[cpu->program_counter].operand[1];
    uint8_t rd = cpu->program_mem[cpu->program_counter].operand[2];
    cpu->register_file[rd] = ~(cpu->register_file[rs0] | cpu->register_file[rs1]);
}

static void xor_handler(cpu_t *cpu)
{
    uint8_t rs0 = cpu->program_mem[cpu->program_counter].operand[0];
    uint8_t rs1 = cpu->program_mem[cpu->program_counter].operand[1];
    uint8_t rd = cpu->program_mem[cpu->program_counter].operand[2];
    cpu->register_file[rd] = cpu->register_file[rs0] ^ cpu->register_file[rs1];
}

static void set_handler(cpu_t *cpu)
{
    uint8_t rd = cpu->program_mem[cpu->program_counter].operand[0];
    uint8_t imm = cpu->program_mem[cpu->program_counter].operand[1];
    cpu->register_file[rd] = imm;
}

typedef struct 
{
    char* identfier;
    void (*instr_handler)(cpu_t*);
} instr_lookup_t;

// INSTRUCTIONS lookup table (internal)
static const instr_lookup_t instr_table[] = {
    // Does nothing
    {"NOOP", noop_handler},
    
    // GETX RD
    {"GETX", get_x_handler},

    // GETY RD 
    {"GETY", get_y_handler},

    // GETTIME RD
    {"GETTIME", get_time_handler},

    // MOV RS RD
    {"MOV", move_handler},

    // ADD RS0 RS1 RD
    {"ADD", add_handler},

    // SUB RS0 RS1 RD
    {"SUB", sub_handler},

    // SL RS0 RS1 RD
    {"SL", sl_handler},

    // SR RS0 RS1 RD
    {"SR", sr_handler},

    // RAND RD
    {"RAND", rand_handler},

    // AND RS0 RS1 RD
    {"AND", and_handler},

    // NAND RS0 RS1 RD
    {"NAND", nand_handler},

    // OR RS0 RS1 RD
    {"OR", or_handler},

    // NOR RS0 RS1 RD
    {"NOR", nor_handler},

    // XOR RS0 RS1 RD
    {"XOR", xor_handler},

    // SET RD IMM
    {"SET", set_handler},
};


// --------------- //
// Simulator Logic //
// --------------- //

int parse_program(char const * const file_path, program_mem_t *program_memory)
{
    if (file_path == NULL || *file_path == '\0')
    {
        fprintf(ERROR_STREAM, "Invalid Path provided: \"%s\"\n", file_path);
        return -1;
    }

    FILE* source_file = fopen(file_path, "r");
    
    if (source_file == NULL)
    {
        fprintf(ERROR_STREAM, "Could not read file: \"%s\"\n", file_path);
        return -1;
    }
    
    char line_buffer[LINE_BUFFER_SIZE];
    size_t line_counter = 0;
    
    while (fgets(line_buffer, LINE_BUFFER_SIZE, source_file) != NULL && line_counter < INSTR_COUNT)
    {
        line_buffer[strcspn(line_buffer, "\n")] = '\0';

        if (parse_instrucion(&(*program_memory)[line_counter], line_buffer) != 0)
        {
            fprintf(ERROR_STREAM, "%s - Error: Could not parse line\n\t%zu: %s\n", file_path, line_counter, line_buffer);
            fclose(source_file);
            return -1;
        }
        
        line_counter++; 
    }

    fclose(source_file);
    return 0;
}

int run_program(cpu_t *cpu, uint8_t *result_reg)
{
    while(cpu->program_counter < INSTR_COUNT)
    {
        if (execute_instr(cpu) != 0)
        {
            return -1;
        }
    }
    
    if (result_reg != NULL)
    {
        *result_reg = cpu->register_file[0];
    }

    // Reset PC and registers after program execution
    cpu->program_counter = 0;
    for (size_t i = 0; i < REG_COUNT; i++)
    {
        cpu->register_file[i] = 0;
    }

    return 0;
}

// ------------------ //
// INTERNAL FUNCTIONS //
// ------------------ //
static int execute_instr(cpu_t * const cpu)
{
    char const * const instr = cpu->program_mem[cpu->program_counter].identfier;

    if (instr == NULL)
    {
        return -1;
    }

    for (size_t i = 0; i < sizeof(instr_table) / sizeof(instr_lookup_t); i++)
    {
        if (strcmp(instr_table[i].identfier, instr) == 0)
        {
            instr_table[i].instr_handler(cpu);
            cpu->program_counter++;

            // whacky tabaky
            // (because we have no custom bitwidth types)
            for (size_t reg_idx = 0; reg_idx < REG_COUNT; reg_idx++)
            {
                cpu->register_file[reg_idx] = cpu->register_file[reg_idx] % (uint8_t)pow(2, REG_BIT_WIDTH);
            }
            
            return 0;
        }
    }
    
    return -1;
}

static int get_instruction (instruction_t *instr, char* token)
{
    for (size_t i = 0; i < sizeof(instr_table) / sizeof(instr_lookup_t); i++)
    {
        if (strcmp(instr_table[i].identfier, token) == 0)
        {
            instr->identfier = instr_table[i].identfier;
            return 0;
        }
    }

    return -1;
}

static int get_register (char* token, uint8_t* res)
{
    if (token == NULL || *token != 'R')
    {
        return -1;
    }

    token++;
    
    if (!isalnum(*token))
    {
        return -1;
    }
    
    *res = *token - '0';
    return 0;
}

static int get_immediate (char* token, uint8_t* res)
{
    if (token == NULL)
    {
        return -1;
    }

    char* endptr = NULL;
    long operand = strtol(token, &endptr, 10);

    if (endptr == token || operand < 0 || operand > UINT8_MAX)
    {
        return -1;
    }

    *res = (uint8_t)operand;
    return 0;
}

static int parse_instrucion (instruction_t *instr, char* line)
{
    if (line == NULL)
    {
        return -1;
    }

    char* token = strtok(line, " ");

    if (token == NULL)
    {
        return -1;
    }

    if (get_instruction(instr, token) != 0)
    {
        fprintf(ERROR_STREAM, "Error: Could not parse instruction \"%s\"\n", token);
        return -1;
    }

    size_t operand_counter = 0;
    char* next_token = strtok(NULL, " "); 

    while (next_token != NULL)
    {
        uint8_t reg = 0;
        uint8_t imm = 0;
        
        if (get_register(next_token, &reg) == 0)
        {
            instr->operand[operand_counter] = reg;
        }
        else if (get_immediate(next_token, &imm) == 0)
        {
            instr->operand[operand_counter] = (uint8_t) imm;
        }
        else 
        {
            return -1;
        }

        next_token = strtok(NULL, " ");
        operand_counter++;
    }

    return 0;
}