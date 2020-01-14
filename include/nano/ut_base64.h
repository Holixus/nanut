#ifndef NANUT_BASE64_H
#define NANUT_BASE64_H

size_t b64_decode(unsigned char *out, size_t size, const char *str);
size_t b64_decoded_len(size_t size);
size_t b64_encode(char *out, size_t osize, const unsigned char *in, size_t size);
size_t b64_encoded_len(size_t size);

#endif
