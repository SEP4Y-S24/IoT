
#include <stdint.h>

void cryptorator_init(uint8_t *givenkey);
void cryptorator_encrypt(char **message);
void cryptorator_decrypt(char *message);
void cryptorator_generate_iv(char *iv);
