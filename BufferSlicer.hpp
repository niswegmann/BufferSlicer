#ifndef BUFFER_SLICER_HPP
#define BUFFER_SLICER_HPP

#include <algorithm>
#include <cstring>

template <int block_size, int input_channel_count, int output_channel_count>
struct BufferSlicer
{
    template <typename Callback> void process
        (
            float * const * const unaligned_input_buffers,
            float * const * const unaligned_output_buffers,
            float * const * const aligned_input_buffers,
            float * const * const aligned_output_buffers,
            int const unaligned_sample_count,
            Callback && callback
        )
    {
        int unaligned_sample_index = 0, leftover_index = m_leftover_index;
        
        if (0 < unaligned_sample_count) // Handle any left-over samples.
        {
            // Copy as many left-over samples as possible until we have a block.
            int const copy_sample_count =
                std::min
                (
                    block_size - leftover_index,
                    unaligned_sample_count
                );
            
            for (int buf_index = 0; buf_index < output_channel_count; buf_index += 1)
            {
                std::memcpy
                (
                    unaligned_output_buffers[buf_index] + unaligned_sample_index,
                    m_leftover_buffers[buf_index] + leftover_index,
                    sizeof(float) * copy_sample_count
                );
            }
            
            for (int buf_index = 0; buf_index < input_channel_count; buf_index += 1)
            {
                std::memcpy
                (
                    m_leftover_buffers[buf_index] + leftover_index,
                    unaligned_input_buffers[buf_index] + unaligned_sample_index,
                    sizeof(float) * copy_sample_count
                );
            }
            
            unaligned_sample_index += copy_sample_count;
            leftover_index += copy_sample_count;
        }
        
        if (leftover_index == block_size) // If we have a block, process it.
        {
            // Process as many blocks as possible.
            int const extra_blocks_count =
                (unaligned_sample_count - unaligned_sample_index) / block_size;
            
            for (int buf_index = 0; buf_index < input_channel_count; buf_index += 1)
            {
                std::memcpy
                (
                    aligned_input_buffers[buf_index],
                    m_leftover_buffers[buf_index],
                    sizeof(float) * block_size
                );
                
                std::memcpy
                (
                    aligned_input_buffers[buf_index] + block_size,
                    unaligned_input_buffers[buf_index] + unaligned_sample_index,
                    sizeof(float) * extra_blocks_count * block_size
                );
            }
            
            callback
            (
                aligned_input_buffers,
                aligned_output_buffers,
                (1 + extra_blocks_count) * block_size
            );
            
            for (int buf_index = 0; buf_index < output_channel_count; buf_index += 1)
            {
                std::memcpy
                (
                    unaligned_output_buffers[buf_index] + unaligned_sample_index,
                    aligned_output_buffers[buf_index],
                    sizeof(float) * extra_blocks_count * block_size
                );
                
                std::memcpy
                (
                    m_leftover_buffers[buf_index],
                    aligned_output_buffers[buf_index] + extra_blocks_count * block_size,
                    sizeof(float) * block_size
                );
            }
            
            unaligned_sample_index += extra_blocks_count * block_size;
            leftover_index = 0;
        }
        
        if (unaligned_sample_index < unaligned_sample_count) // Handle any left-over samples.
        {
            int const copy_samples_count = unaligned_sample_count - unaligned_sample_index;
            
            for (int buf_index = 0; buf_index < output_channel_count; buf_index += 1)
            {
                std::memcpy
                (
                    unaligned_output_buffers[buf_index] + unaligned_sample_index,
                    m_leftover_buffers[buf_index] + leftover_index,
                    sizeof(float) * copy_samples_count
                );
            }
            
            for (int buf_index = 0; buf_index < input_channel_count; buf_index += 1)
            {
                std::memcpy
                (
                    m_leftover_buffers[buf_index] + leftover_index,
                    unaligned_input_buffers[buf_index] + unaligned_sample_index,
                    sizeof(float) * copy_samples_count
                );
            }
            
            unaligned_sample_index += copy_samples_count;
            leftover_index += copy_samples_count;
        }
        
        m_leftover_index = leftover_index;
    }
    
  private:
    
    /*
        Left-over buffers structure:
     
                      leftover_index
                            |
        0   1   2  ...      v
        +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
        |t+0|t+1|t+2|...|   |t+0|t+1|t+2|...|   |   |   |   |   |   |   |
        +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
         \--------v--------/ \--------------------v--------------------/
                  |                               |
        leftover input samples         leftover output samples
    */
    
    static constexpr int k_leftover_buffer_count =
        std::max(input_channel_count, output_channel_count);
    
    float m_leftover_buffers [k_leftover_buffer_count][block_size] {};
    
    int m_leftover_index {};
};

#endif /* BUFFER_SLICER_HPP */
