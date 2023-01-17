# Buffer Slicer

*Buffer Slicer* is a small header-only C++ library for cutting buffers into
slices of lengths that are constrained to be a multiple of a fixed block size.

Say we have some hardware that requests us to continually process a series of
incoming buffers. To process those buffers efficiently with SIMD
instructions, we would like the buffers to be 16-byte aligned (or 32-byte on AVX
platforms) and have a size that is a multiple of 16 bytes (or 32 bytes). If the
hardware doesn't guarantee this we can enforce it by adding an intermediate
layer that will slice all incoming buffers into a new set of buffers that
comply with the above-described requirements. Such an intermediate layer will
induce a small delay equal to the factor that we want the size of the buffers to
be divisible with. E.g. if we're processing floats and we want the size of the
sliced buffers to be a multiple of 4 floats, we'll introduce a 4 sample delay
into our chain.

## Usage

Here's how to use *Buffer Slicer*. Say we are in a context in which we are
required to process float buffers which lengths can vary between 1 and 1024
samples. Also, let us assume that we would like the sliced buffers to be 64-byte
aligned and have a size that is a multiple of 16 samples (corresponding to
64-bytes).

1. Create a `BufferSlicer` instance with 2 input, 2 output channels, and a block
size of 16:

    BufferSlicer<16, 2, 2> buffer_slicer;

2. Allocate your aligned buffers:
    
    float * const input_buffers [2] =
    {
        aligned_alloc(64, max_aligned_sample_count * sizeof(float)),
        aligned_alloc(64, max_aligned_sample_count * sizeof(float))
    };

3. Make sure that `max_aligned_sample_count` is a multiple of `64 / sizeof(float)`
greater than or equal to `max_block_count * block_size`, where `max_block_count`
is equal to `floor((max_unaligned_sample_count + block_size - 1) / block_size)`
and `max_unaligned_sample_count` is the maximum sample count reported by the
hardware.

In your callback, from the hardware, use the `BufferSlicer` to slice the
incoming buffers as follows:

    ...
    
    buffer_slicer.process
    (
        unaligned_input_buffers,
        unaligned_output_buffers,
        aligned_input_buffers,
        aligned_output_buffers,
        sample_count,
        [](float * const input_buffers [2], float * const output_buffers [2], int sample_count)
        {
            // Do your DSP with aligned buffers. The sample count in this inner
            // callback is guaranteed to be a multiple of 16 less than or equal
            // to `floor((1024 + 16 - 1) / 16) * 16` which is equal to `1024`.
            
            ...
        }
    );
    
    ...

## License

*BufferSlicer* uses the CC0 1.0 Universal license (see the [LICENSE](https://github.com/niswegmann/BufferSlicer/blob/main/LICENSE) file). Basically you can do whatever you want to do with the code.
