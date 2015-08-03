#include "parsing.h"

/*Function to implement md5.
Input:char *buffer
Output:DIGEST
*/
DIGEST *str2md5(char *buffer)
{

        MD5_CTX c;
        DIGEST *digest = (DIGEST *)calloc(1, 16);

        if (buffer != NULL) {
                MD5_Init(&c);
                MD5_Update(&c, buffer, strlen(buffer));
                MD5_Final(digest, &c);
        }
        return digest;

}

/*Function to create hash using sha1
Input:char *buffer
Output:DIGEST
*/
DIGEST *sha1(char *buffer)
{

        DIGEST *digest = (DIGEST *)calloc(1, 20);
        SHA_CTX c;

        SHA1_Init(&c);
        SHA1_Update(&c, buffer, strlen(buffer));
        SHA1_Final(digest, &c);

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

int main(void)
{

        DIGEST *digest          = (DIGEST *)calloc(1, 16);
        char buffer[100]        = "";
        char hash[100]          = "";
        char *buffer1           = NULL;
        int choice = 0;
        int len = 0;

        while (1) {
                printf("1.Parse\n2.Unparse\n");
                if (scanf("%d", &choice) <= 0)
                        goto out;
                switch (choice) {
                case 1:
                        printf("Enter string\n");
                        scanf("%s", buffer);
                        printf("Enter hash\n");
                        scanf("%s", hash);
                        if (strcmp(hash, "md5") == 0) {
                                digest = str2md5(buffer);
                                len = MD5_DIGEST_LENGTH;
                        } else if (strcmp(hash, "sha1") == 0) {
                                digest = sha1(buffer);
                                len = SHA_DIGEST_LENGTH;
                        } else {
                                goto out;
                        }
                        buffer1 = parse(digest, len);
                        printf("%s\n", buffer1);
                        break;
                case 2:
                        if (buffer1 != NULL) {
                                digest = unparse(buffer1, len);
                                buffer1 = parse(digest, len);
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
