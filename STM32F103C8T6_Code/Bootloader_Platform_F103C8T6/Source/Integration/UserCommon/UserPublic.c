
#include "UserPublic.h"



/**
 * @brief 高速内存拷贝（优先按32位字拷贝）
 * @param dest 目标地址（uint8_t*）
 * @param src  源地址（uint8_t*）
 * @param n    要拷贝的字节数
 * @note  适用于Cortex-M3及以上（允许非对齐访问），
 *        但为最高效率，会尽量使内部32位拷贝的地址对齐。
 */
void fast_memcpy(uint8 *dest, const uint8 *src, uint16 n)
{
    // 1. 按字节拷贝，直到目标地址对齐到4字节边界
    while (n > 0 && ((uint32)dest & 0x03))
    {
        *dest++ = *src++;
        n--;
    }

    // 2. 按32位字块拷贝（目标地址已对齐）
    uint32 *dest32 = (uint32*)dest;
    const uint32 *src32 = (const uint32*)src;

    // 只要剩余字节数 >= 4，就按字拷贝
    while (n >= 4)
    {
        *dest32++ = *src32++;
        n -= 4;
    }

    // 3. 处理末尾不足4个字节的部分
    dest = (uint8*)dest32;
    src  = (const uint8*)src32;
    while (n--)
    {
        *dest++ = *src++;
    }
}


