#include "parsing.h"

/*
 * Function to convert hexadecimal digit to integer.
 * Input:char digit
 * Output:int hex_digit
 */
int hex_to_int(char digit)
{

        int hex_digit;

        if (digit >= '0' && digit <= '9') {
                hex_digit = digit - '0';
        } else if (digit >= 'a' && digit <= 'f') {
                hex_digit = digit - 'a' + 10;
        } else if (digit >= 'A' && digit <= 'F') {
                hex_digit = digit - 'A' + 10;
        } else {
                hex_digit = 0;
        }
        return hex_digit;

}

/*
 * Function to parse the hash byte char to hexadecimal hash value.
 * Input:DIGEST -- byte char of hash.
 * Output:char *str -- Hexadecimal hash value
 */
char *parse(DIGEST *digest, int len)
{

        int n;
        char *out = (char *)calloc(1, len * 2 + 1);

        for (n = 0; n < len; ++n) {
                snprintf(&(out[n*2]), 16 * 2, "%02x", (unsigned int)digest[n]);
        }
        return out;

}

/*
 * Function to unparse the hexadecimal to byte char.
 * Input:char *str -- Hexadecimal hash value
 * Output:DIGEST -- byte char of hash.
 */
DIGEST *unparse(char *str, int len)
{

        DIGEST *digest = (DIGEST *)calloc(1, len);
        int index;

        for (index = 0; index < len * 2; index += 2) {
                digest[index / 2] = hex_to_int(str[index]) * 16 +
                hex_to_int(str[index + 1]);
        }
        return digest;

}
