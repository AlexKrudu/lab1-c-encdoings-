#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
typedef unsigned char uchar;

int main(int argc, char *argv[]) {
    FILE *in, *out;
    int temp1, temp2, size = 0, size_out = 0;
    unsigned __int32 res2;
    long res;
    in = fopen(argv[1], "rb");
    uchar u16le[2] = {-1, -2};
    uchar u16be[2] = {-2, -1};
    uchar utf8[3] = {-17, -69, -65};
    uchar utf32le[4] = {-1, -2, 0, 0};
    uchar utf32be[4] = {0, 0, -2, -1};

    fseek(in, 0, SEEK_END);
    size_t len = ftell(in);
    if (len == 4294967295){
        printf("There is no such file or folder: %s", argv[1]);
        exit(2);
    }
    uchar *buff_in = (uchar *) malloc(sizeof(uchar) * len * 4);
    uchar *buff_cur = (uchar *) malloc(sizeof(uchar) * len * 4);
    uchar *buff_out = (uchar *) malloc(sizeof(uchar) * len * 4);
    if (buff_in == NULL) {
        printf("Memory allocation error");
        exit(1);
    }
    rewind(in);
    size_t result = fread(buff_in, sizeof(uchar), len, in);
    fclose(in);
    if (result != len) {
        printf("File reading error");
        exit(2);
    } else if (!memcmp(utf32be, buff_in, 4)) {
        for (int i = 4; i < len; i++) {
            buff_cur[i - 4] = buff_in[i];
            size++;
        }
    } else if (!memcmp(utf32le, buff_in, 4)) {
        for (int i = 4; i < len; i += 4) {
            buff_cur[size] = buff_in[i + 3];
            buff_cur[size + 1] = buff_in[i + 2];
            buff_cur[size + 2] = buff_in[i + 1];
            buff_cur[size + 3] = buff_in[i];
            size += 4;
        }
    } else if (!memcmp(u16le, buff_in, 2)) {
        for (int i = 2; i < len; i += 4) {
            temp1 = ((buff_in[i + 1] << 8) + buff_in[i]);
            if (temp1 >= -10240 && temp1 <= -9217) {
                temp1 = (temp1 + 10240) * 1024;
                temp2 = (((buff_in[i + 3] << 8) + buff_in[i + 2]) + 9216);
            } else {
                buff_cur[size] = 0;
                buff_cur[size + 1] = 0;
                buff_cur[size + 2] = (uchar) ((temp1 >> 8) & -1);
                buff_cur[size + 3] = (uchar) ((temp1) & -1);
                size += 4;
                i -= 2;
                continue;
            }
            res = temp1 + temp2 + 65536;
            buff_cur[size] = (uchar) (res >> 24);
            buff_cur[size + 1] = (uchar) ((res >> 16) & -1);
            buff_cur[size + 2] = (uchar) ((res >> 8) & -1);
            buff_cur[size + 3] = (uchar) ((res) & -1);
            size += 4;
        }
    } else if (!memcmp(u16be, buff_in, 2)) {
        for (int i = 2; i < len; i += 4) {
            temp1 = ((buff_in[i] << 8) + buff_in[i + 1]);
            if (temp1 >= -10240 && temp1 <= -9217) {
                temp1 = (temp1 + 10240) * 1024;
                temp2 = (((buff_in[i + 2] << 8) + buff_in[i + 3]) + 9216);
            } else {
                buff_cur[size] = 0;
                buff_cur[size + 1] = 0;
                buff_cur[size + 2] = (uchar) ((temp1 >> 8) & -1);
                buff_cur[size + 3] = (uchar) ((temp1) & -1);
                size += 4;
                i -= 2;
                continue;
            }
            res = temp1 + temp2 + 65536;
            buff_cur[size] = (uchar) (res >> 24);
            buff_cur[size + 1] = (uchar) ((res >> 16) & -1);
            buff_cur[size + 2] = (uchar) ((res >> 8) & -1);
            buff_cur[size + 3] = (uchar) ((res) & -1);
            size += 4;
        }
    } else if (!memcmp(utf8, buff_in, 3)) {
        for (int i = 3; i < len; i++) {
            if ((buff_in[i] & 128) == 0) {
                buff_cur[size + 3] = buff_in[i];
                buff_cur[size + 2] = 0;
                buff_cur[size + 1] = 0;
                buff_cur[size] = 0;
                size += 4;
            } else if ((buff_in[i] & 224) == 192) {
                buff_cur[size + 3] = (uchar) (((buff_in[i + 1] & 63) + (buff_in[i] << 6)) & 255);
                buff_cur[size + 2] = (uchar) ((buff_in[i] & 28) >> 2);
                buff_cur[size + 1] = 0;
                buff_cur[size] = 0;
                i++;
                size += 4;
            } else if ((buff_in[i] & 240) == 224) {
                buff_cur[size + 3] = (uchar) (((buff_in[i + 2] & 63) + (buff_in[i + 1] << 6)) & 255);
                buff_cur[size + 2] = (uchar) (((buff_in[i + 1] & 60) >> 2) + ((buff_in[i] & 15) << 4));
                buff_cur[size + 1] = 0;
                buff_cur[size] = 0;
                i += 2;
                size += 4;
            } else if ((buff_in[i] & 248) == 240) {
                buff_cur[size + 3] = (uchar) (((buff_in[i + 3] & 63) + (buff_in[i + 2] << 6)) & 255);
                buff_cur[size + 2] = (uchar) (((buff_in[i + 2] & 60) >> 2) + ((buff_in[i + 1] & 15) << 4));
                buff_cur[size + 1] = (uchar) (((buff_in[i + 1] & 63) >> 4) + ((buff_in[i] & 7) << 2));
                buff_cur[size] = 0;
                i += 3;
                size += 4;
            }
        }
    } else {
        for (int i = 0; i < len; i++) {
            if ((buff_in[i] & 128) == 0) {
                buff_cur[size + 3] = buff_in[i];
                buff_cur[size + 2] = 0;
                buff_cur[size + 1] = 0;
                buff_cur[size] = 0;
                size += 4;
            } else if ((buff_in[i] & 224) == 192) {
                if ((buff_in[i + 1] & 192) != 128 || i + 1 >= len){
                    goto invalid;
                }
                buff_cur[size + 3] = (uchar) (((buff_in[i + 1] & 63) + (buff_in[i] << 6)) & 255);
                buff_cur[size + 2] = (uchar) ((buff_in[i] & 28) >> 2);
                buff_cur[size + 1] = 0;
                buff_cur[size] = 0;
                i++;
                size += 4;
            } else if ((buff_in[i] & 240) == 224) {
                if ((buff_in[i + 1] & 192) != 128 || (buff_in[i + 2] & 192) != 128 || i + 2 >= len){
                    goto invalid;
                }
                buff_cur[size + 3] = (uchar) (((buff_in[i + 2] & 63) + (buff_in[i + 1] << 6)) & 255);
                buff_cur[size + 2] = (uchar) (((buff_in[i + 1] & 60) >> 2) + ((buff_in[i] & 15) << 4));
                buff_cur[size + 1] = 0;
                buff_cur[size] = 0;
                i += 2;
                size += 4;
            } else if ((buff_in[i] & 248) == 240) {
                if ((buff_in[i + 1] & 192) != 128 || (buff_in[i + 2] & 192) != 128 || (buff_in[i + 3] & 192) != 128  || i + 3 >= len ){
                    goto invalid;
                }
                buff_cur[size + 3] = (uchar) (((buff_in[i + 3] & 63) + (buff_in[i + 2] << 6)) & 255);
                buff_cur[size + 2] = (uchar) (((buff_in[i + 2] & 60) >> 2) + ((buff_in[i + 1] & 15) << 4));
                buff_cur[size + 1] = (uchar) (((buff_in[i + 1] & 63) >> 4) + ((buff_in[i] & 7) << 2));
                buff_cur[size] = 0;
                i += 3;
                size += 4;
            }else {
                invalid:
                temp1 = buff_in[i] - 128 + 56448;
                buff_cur[size + 2] = temp1 >> 8;
                buff_cur[size + 3] = temp1 & 255;
                buff_cur[size + 1] = 0;
                buff_cur[size] = 0;
                size += 4;
            }
        }
    }
    /*if (!isdigit((int)argv[3])){
        printf("Expected number at 3rd argument.");
        exit(1);
    }*/
    int c = strtol(argv[3], NULL, 10);
    if (c > 5 || c < 0){
        printf("Expected 3rd agument to be in range[0..5]");
        exit(1);
    }
    out = fopen(argv[2], "wb");
    switch (c) {
        case 0: {//utf-8(no BOM)
            for (int i = 0; i < size; i+= 4){
                res2 = (buff_cur[i] << 24) + (buff_cur[i + 1] << 16) + (buff_cur[i + 2] << 8) + buff_cur[i + 3];
                if (res2 < 128){
                    buff_out[size_out] =(uchar) (res2);
                    size_out++;
                }
                else if (res2 < 2048){
                    buff_out[size_out] = (uchar)(192 + ((res2 & 4032) >> 6));
                    buff_out[size_out + 1] = (uchar)((res2 & 63) + 128);
                    size_out+= 2;
                }
                else if (res2 >= 56448 && res2 <= 56575){
                    res2 -= 56448;
                    res2 += 128;
                    buff_out[size_out] = (uchar) (res2);
                    size_out++;
                }
                else if (res2 < 65536){
                    buff_out[size_out] = (uchar)(224 + ((res2 & 258048) >> 12));
                    buff_out[size_out + 1] = (uchar)((((res2 & 4032)) >> 6) + 128);
                    buff_out[size_out + 2] = (uchar)((res2 & 63) + 128);
                    size_out += 3;
                }
                else if (res2 < 1114111) {
                    buff_out[size_out] = (uchar) (240 + ((res2 & 16515072) >> 18));
                    buff_out[size_out + 1] = (uchar) (((res2 & 258048) >> 12) + 128);
                    buff_out[size_out + 2] = (uchar) ((((res2 & 4032)) >> 6) + 128);
                    buff_out[size_out + 3] = (uchar) ((res2 & 63) + 128);
                    size_out += 4;
                }

            }
            fwrite(buff_out, 1, size_out, out);
            break;
        };
        case 1: { //utf-8 (BOM)
            fwrite(utf8, 1, 3, out);
            for (int i = 0; i < size; i+= 4){
                res2 = (buff_cur[i] << 24) + (buff_cur[i + 1] << 16) + (buff_cur[i + 2] << 8) + buff_cur[i + 3];
                if (res2 < 128){
                    buff_out[size_out] =(uchar) (res2);
                    size_out++;
                }
                else if (res2 < 2048){
                    buff_out[size_out] = (uchar)(192 + ((res2 & 4032) >> 6));
                    buff_out[size_out + 1] = (uchar)((res2 & 63) + 128);
                    size_out+= 2;
                }
                else if (res2 >= 56448 && res2 <= 56575){
                    res2 -= 56448;
                    res2 += 128;
                    buff_out[size_out] = (uchar) (res2);
                    size_out++;
                }
                else if (res2 < 65536){
                    buff_out[size_out] = (uchar)(224 + ((res2 & 258048) >> 12));
                    buff_out[size_out + 1] = (uchar)((((res2 & 4032)) >> 6) + 128);
                    buff_out[size_out + 2] = (uchar)((res2 & 63) + 128);
                    size_out += 3;
                }
                else if (res2 < 1114111){
                    buff_out[size_out] = (uchar)(240 + ((res2 & 16515072) >> 18));
                    buff_out[size_out + 1] = (uchar) (((res2 & 258048) >> 12) + 128);
                    buff_out[size_out + 2] = (uchar)((((res2 & 4032)) >> 6) + 128);
                    buff_out[size_out + 3] = (uchar)((res2 & 63) + 128);
                    size_out += 4;
                }
            }
            fwrite(buff_out, 1, size_out, out);
            break;
        };
        case 2: { //utf-16(le)
            fwrite(u16le, 1, 2, out);
            for (int i = 0; i < size; i++) {
                res = (buff_cur[i] << 24) + (buff_cur[i + 1] << 16) + (buff_cur[i + 2] << 8) + buff_cur[i + 3];
                if (res > 65535){
                    res -= 65536;
                    temp1 = (res >> 10) - 10240;
                    temp2 =  (res & 1023) - 9216;
                    buff_out[size_out] = (uchar) (temp1 & 255);
                    buff_out[size_out + 1] =(uchar) (temp1 >> 8);
                    buff_out[size_out + 2] = (uchar) (temp2 & 255);
                    buff_out[size_out + 3] =(uchar) (temp2 >> 8);
                    size_out += 4;
                    i += 3;
                    continue;
                }
                buff_out[size_out] = (uchar) (res & 255);
                buff_out[size_out + 1] =(uchar) (res >> 8);
                size_out += 2;
                i += 3;
            }
            fwrite(buff_out, 1, size_out, out);
            break;
        };
        case 3: { //utf-16(be)
            fwrite(u16be, 1, 2, out);
            for (int i = 0; i < size; i++) {
                res = (buff_cur[i] << 24) + (buff_cur[i + 1] << 16) + (buff_cur[i + 2] << 8) + buff_cur[i + 3];
                if (res > 65535){
                    res -= 65536;
                    temp1 = (res >> 10) - 10240;
                    temp2 =  (res & 1023) - 9216;
                    buff_out[size_out + 1] = (uchar) (temp1 & 255);
                    buff_out[size_out] =(uchar) (temp1 >> 8);
                    buff_out[size_out + 3] = (uchar) (temp2 & 255);
                    buff_out[size_out + 2] =(uchar) (temp2 >> 8);
                    size_out += 4;
                    i += 3;
                    continue;
                }
                buff_out[size_out + 1] = (uchar) (res & 255);
                buff_out[size_out] =(uchar) (res >> 8);
                size_out += 2;
                i += 3;
            }
            fwrite(buff_out, 1, size_out, out);
            break;
        };
        case 4: { //utf-32(le)
            fwrite(utf32le, 1, 4, out);
            for (int i = 0; i < size; i += 4) {
                buff_out[size_out] = buff_cur[i + 3];
                buff_out[size_out + 1] = buff_cur[i + 2];
                buff_out[size_out + 2] = buff_cur[i + 1];
                buff_out[size_out + 3] = buff_cur[i];
                size_out += 4;
            }
            fwrite(buff_out, 1, size_out, out);
            break;
        };
        case 5: { //utf-32(be)
            fwrite(utf32be, 1, 4, out);
            fwrite(buff_cur, 1, size, out);
            break;
        }
        default:
            fclose(out);
    }
    return 0;
}
