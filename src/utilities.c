static UCHAR hex2bin (UCHAR c)
{
    if (c >= 0x30 && c <= 0x39) return (c - 0x30);
    if (c >= 0x41 && c <= 0x46) return (c - 0x41 + 10);
    if (c >= 0x61 && c <= 0x66) return (c - 0x61 + 10);
    return 0;
}
/* --------------------------------------------------------------------------- */
static int urldecodeBuf (PUCHAR out , PUCHAR in , int inLen)
{
    PUCHAR outBegin = out;
    PUCHAR end = in + inLen;
    for (;in < end; in ++) {
        // % escape?
        if (*in == 0x25) {
            UCHAR c = hex2bin(*(++in)) * 16+
                      hex2bin(*(++in));
            *(out ++) = c;
        } else {
            *(out ++) = *in;
        }

    }
    return out - outBegin;
}
