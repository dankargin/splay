


__device__ void partition_by_bit(unsigned int* values, unsigned int bit);

__device__ void radix_sort(unsigned int* values)
{
    int  bit;
    for (bit = 0; bit < 32; ++bit)
    {
        partition_by_bit(values, bit);
        __syncthreads();
    }
}


template<class T>
__device__ T plus_scan(T* x)
{
    unsigned int i = threadIdx.x; 
    unsigned int n = blockDim.x;  
    unsigned int offset;          

    for (offset = 1; offset < n; offset *= 2) {
        T t;

        if (i >= offset)
            t = x[i - offset];

        __syncthreads();

        if (i >= offset)
            x[i] = t + x[i];      

        __syncthreads();
    }
    return x[i];
}



__device__ void partition_by_bit(unsigned int* values, unsigned int bit)
{
    unsigned int i = threadIdx.x;
    unsigned int size = blockDim.x;
    unsigned int x_i = values[i];          
    unsigned int p_i = (x_i >> bit) & 1;   

    
    
    values[i] = p_i;

    
    __syncthreads();

    unsigned int T_before = plus_scan(values);
    
    
    unsigned int T_total = values[size - 1];
    

    unsigned int F_total = size - T_total;
    


    __syncthreads();

    

    if (p_i)
        values[T_before - 1 + F_total] = x_i;
    else
        values[i - T_before] = x_i;
    


}




































































