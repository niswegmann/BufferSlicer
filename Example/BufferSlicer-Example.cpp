#include "BufferSlicer.hpp"

#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>

int main (int argc, const char * argv[])
{
    static constexpr int block_size = 4;
    
    BufferSlicer<block_size, 1, 1> buffer_slicer;
    
    float unaligned_input_buffer [1024], unaligned_output_buffer [1024];
    
    float aligned_input_buffer [256], aligned_output_buffer [256];
    
    for (int index = 0; index < 1024; index++)
    {
        unaligned_input_buffer[index] = (float) (index + 1);
    }
    
    // `unaligned_input_buffer` will now contain the samples [1, 2, 3, 4, ...].
    
    std::array<int, 10> const sample_counts = { 3, 15, 5, 32, 15, 1, 3, 51, 11, 18 };
    
    int offset = 0;
    
    for (auto const sample_count : sample_counts)
    {
        float * const unaligned_input_buffers [] = { unaligned_input_buffer + offset };
        float * const unaligned_output_buffers [] = { unaligned_output_buffer + offset };
        float * const aligned_input_buffers [] = { aligned_input_buffer };
        float * const aligned_output_buffers [] = { aligned_output_buffer };
        
        printf("caller - processing %d samples\n", sample_count);
        
        buffer_slicer.process
        (
            unaligned_input_buffers,
            unaligned_output_buffers,
            aligned_input_buffers,
            aligned_output_buffers,
            sample_count,
            [](float * const * aligned_input_buffers, float * const * aligned_output_buffers, int sample_count)
            {
                printf("  callee - processing %d samples\n", sample_count);
                
                for (int sample_index = 0; sample_index < sample_count; sample_index++)
                {
                    float const input_sample = aligned_input_buffers[0][sample_index];
                    
                    aligned_output_buffers[0][sample_index] = powf(input_sample, 2.0f);
                }
            }
        );
        
        offset += sample_count;
    }
    
    // `unaligned_output_buffer` will now contain the samples [1, 4, 9, 16, ...].
    
    printf("square root of each element in the unaligned output buffer:\n");
    
    for (int index = 0; index < offset; index++)
    {
        float const y = unaligned_output_buffer[index];
        
        printf("  %d, %f\n", index, sqrtf(y));
    }
    
    return EXIT_SUCCESS;
}
