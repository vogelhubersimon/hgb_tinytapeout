#ifndef SHADER_SIM_H
#define SHADER_SIM_H

#define SCREEN_WIDTH 64 
#define SCREEN_HEIGHT 40
#define PIXEL_SCALE 10

#define STB_IMAGE_IMPLEMENTATION
#include "../third_party/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../third_party/stb_image_write.h"


#include "pix_cpu.h"
#include "pix_color.h"

int write_png_to_file(char const * const path_to_shader, 
                      char const * const path_to_target_png)
{   
    cpu_t cpu;
    init_cpu(&cpu);
    
    if (parse_program(path_to_shader, &cpu.program_mem) == EXIT_FAILURE)
    {
        fprintf(ERROR_STREAM, "Error: Could not parse program file \"%s\"\n", path_to_shader);
        return -1;
    }
    
    // Original small buffer
    unsigned char image_data[SCREEN_HEIGHT][SCREEN_WIDTH][3];
    
    for (cpu.y_coord = 0; cpu.y_coord < SCREEN_HEIGHT; cpu.y_coord++)
    {
        for (cpu.x_coord = 0; cpu.x_coord < SCREEN_WIDTH; cpu.x_coord++)
        {
            cpu.time = 0;
            RGB222Color col;
            
            if (run_program(&cpu, &col) == EXIT_FAILURE)
            {
                fprintf(ERROR_STREAM, "Error: Could not run program\n");
                return -1;
            }
            
            unsigned char r2 = (col >> 4) & 0x3;
            unsigned char g2 = (col >> 2) & 0x3;
            unsigned char b2 = (col >> 0) & 0x3;
            
            image_data[cpu.y_coord][cpu.x_coord][0] = r2 * 85;
            image_data[cpu.y_coord][cpu.x_coord][1] = g2 * 85;
            image_data[cpu.y_coord][cpu.x_coord][2] = b2 * 85;
        }
    }
    
    // Scaled output buffer
    int scaled_width = SCREEN_WIDTH * PIXEL_SCALE;
    int scaled_height = SCREEN_HEIGHT * PIXEL_SCALE;
    unsigned char* scaled_data = (unsigned char*)malloc(scaled_width * scaled_height * 3);
    
    if (!scaled_data)
    {
        fprintf(ERROR_STREAM, "Error: Could not allocate scaled buffer\n");
        return -1;
    }
    
    // Simple nearest-neighbor scaling
    for (int y = 0; y < scaled_height; y++)
    {
        for (int x = 0; x < scaled_width; x++)
        {
            // Map scaled coordinates back to original
            int src_y = y / PIXEL_SCALE;
            int src_x = x / PIXEL_SCALE;
            
            // Copy pixel
            int dst_idx = (y * scaled_width + x) * 3;
            scaled_data[dst_idx + 0] = image_data[src_y][src_x][0];
            scaled_data[dst_idx + 1] = image_data[src_y][src_x][1];
            scaled_data[dst_idx + 2] = image_data[src_y][src_x][2];
        }
    }
    
    // Write the scaled image
    if (!stbi_write_png(path_to_target_png, scaled_width, scaled_height, 3, 
                        scaled_data, scaled_width * 3))
    {
        fprintf(ERROR_STREAM, "Error: Printing image was not successful\n");
        free(scaled_data);
        return -1;
    }
    
    free(scaled_data);
    return EXIT_SUCCESS;
}

#endif//SHADER_SIM_H