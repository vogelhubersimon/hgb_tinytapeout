#define _POSIX_C_SOURCE 200809L

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "shader_sim.h"
static const char* pixel_shader = "src/shader.pix";
static const char* output_image = "./output.png";

static time_t last_time_file_changed = 0;

int update()
{
    struct stat file_stat = {0};
    int ret = lstat(pixel_shader, &file_stat);
    
    if (ret == -1)
    {
        fprintf(ERROR_STREAM, "Error: Could not find: %s'\n", pixel_shader);
    }

    const time_t file_changed = file_stat.st_mtime; 
    
    if (file_changed != last_time_file_changed)
    {
        fprintf(ERROR_STREAM, "Change happend!!!!\n");
        last_time_file_changed = file_changed;
        
        if (write_png_to_file(pixel_shader, output_image) == EXIT_FAILURE)
        {
            return EXIT_FAILURE;
        }
    } 
    
    sleep(1);

    return EXIT_SUCCESS;
}

int main() 
{

    do{}
    while(update() == EXIT_SUCCESS);
    
    return EXIT_SUCCESS;
}