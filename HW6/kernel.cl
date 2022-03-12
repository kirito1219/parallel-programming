__kernel void convolution(
    __global const float* input, 
    __constant const float* filter,
    __global  float* output,
    int height,
    int width,
    int filterWidth) 
{
    int halffilterSize = filterWidth / 2;
    int index = get_global_id(0);
    int x = index % width;
    int y = index / width;
    
    if(y * width + x < height * width){
        float sum = 0;
        for (int k = -halffilterSize; k <= halffilterSize; k++)
        {
            for (int l = -halffilterSize; l <= halffilterSize; l++)
            {
                if (y + k >= 0 && y + k < height &&
                    x + l >= 0 && x + l < width)
                {
                    sum += input[(y + k) * width + x + l] *
                        filter[(k + halffilterSize) * filterWidth +
                                l + halffilterSize];
                }
            }
        }
        output[y * width + x] = sum;
    }
}
