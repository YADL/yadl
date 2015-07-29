#include "parsing.h"

/*Function to implement md5.
Input:char *buffer
Output:char*
*/
DIGEST *str2md5(char *buffer)
{

        MD5_CTX c;
        DIGEST *digest = (unsigned char *)calloc(1, 16);

        if (buffer != NULL) {
                MD5_Init(&c);
                MD5_Update(&c, buffer, strlen(buffer));
                MD5_Final(digest, &c);
        }
        return digest;

}

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
 * Function to parse the md5  16 byte char to 32 byte hexadecimal.
 * Input:DIGEST -- 16 byte char.
 * Output:char *str -- md5 32 byte hexadecimal
 */
char *parse(DIGEST *digest)
{

        int n;
        char *out = (char *)calloc(1, 33);

        for (n = 0; n < 16; ++n) {
                snprintf(&(out[n*2]), 16*2, "%02x", (unsigned int)digest[n]);
        }
        return out;

}

/*
 * Function to unparse the md5 32 byte hexadecimal to 16 byte char.
 * Input:char *str -- md5 32 byte hexadecimal
 * Output:DIGEST -- 16 byte char.
 */
DIGEST *unparse(char *str)
{

        DIGEST *digest = (DIGEST *)calloc(1, 16);
        int index;

        for (index = 0; index < 32; index += 2) {
                digest[index / 2] = hex_to_int(str[index]) * 16 +
                hex_to_int(str[index + 1]);
        }
        return digest;

}

int main(void)
{

        DIGEST *digest          = (DIGEST *)calloc(1, 16);
        char buffer[100]        = "";
        char *buffer1           = NULL;
        int choice = 0;

        while (1) {
                printf("1.Parse\n2.Unparse\n");
                if (scanf("%d", &choice) <= 0)
                        goto out;
                switch (choice) {
                case 1:
                        printf("Enter string\n");
                        scanf("%s", buffer);
                        digest = str2md5(buffer);
                        buffer1 = parse(digest);
                        printf("%s\n", buffer1);
                        break;
                case 2:
                        if (buffer1 != NULL) {
                                digest = unparse(buffer1);
                                buffer1 = parse(digest);
                                printf("%s\n", buffer1);
                        } else {
                                printf("No string parsed");
                        }
                        break;
                default:
                        goto out;
                }
        }
out:
        return 0;

}

