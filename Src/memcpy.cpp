#include <cstddef>
#include <cstdint>
#include <cstring>

// 进阶版的 memcpy 实现，考虑了内存对齐、块复制和重叠问题，以提高效率和安全性。

// 使用块复制来提高效率,同时需要处理源和目标内存区域重叠问题
void* memcpy(void* dest,const void* src,size_t n){
    // 输入检查
    if(dest == nullptr || src == nullptr || n == 0){
        return dest;
    }
    
    // 转换为字节指针
    unsigned char* d = static_cast<unsigned char*>(dest);
    const unsigned char* s = static_cast<const unsigned char*>(src);
    
    // 检测内存重叠情况
    // 如果目标地址在源地址之后且有重叠，需要从后向前复制
    if(d > s && d < s + n){
        // 从后向前复制，避免覆盖未复制的数据
        d += n;
        s += n;
        
        // 先从后往前复制后缀未对齐的字节，结束时字节便对齐到块边界了。uintptr_t是 C/C++ 标准中定义的可以安全存储指针值的无符号整数类型。它能够确保任何有效的指针转换为这种类型再转换回来时，不会丢失信息。
        while(n > 0 && (reinterpret_cast<uintptr_t>(d) % sizeof(size_t)) != 0){
            *(--d) = *(--s);
            --n;
        }
        
        // 按块复制（每次复制一个 size_t 大小）
        size_t* d_block = reinterpret_cast<size_t*>(d);
        const size_t* s_block = reinterpret_cast<const size_t*>(s);
        while(n >= sizeof(size_t)){
            *(--d_block) = *(--s_block);
            n -= sizeof(size_t);
        }
        
        // 复制剩余的字节
        d = reinterpret_cast<unsigned char*>(d_block);
        s = reinterpret_cast<const unsigned char*>(s_block);
        while(n > 0){
            *(--d) = *(--s);
            --n;
        }
    }
    else{
        // 从前向后复制
        // 先按字节对齐到块边界
        while(n > 0 && (reinterpret_cast<uintptr_t>(d) % sizeof(size_t)) != 0){
            *d++ = *s++;
            --n;
        }
        
        // 按块复制（每次复制一个 size_t 大小，通常是 4 或 8 字节）
        size_t* d_block = reinterpret_cast<size_t*>(d);
        const size_t* s_block = reinterpret_cast<const size_t*>(s);
        while(n >= sizeof(size_t)){
            *d_block++ = *s_block++;
            n -= sizeof(size_t);
        }
        
        // 复制剩余的字节
        d = reinterpret_cast<unsigned char*>(d_block);
        s = reinterpret_cast<const unsigned char*>(s_block);
        while(n > 0){
            *d++ = *s++;
            --n;
        }
    }
    
    return dest;
}