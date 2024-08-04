
// base64.h by aciddev_
// 
// Instuctions:
// To use: Define BASE64_IMPL and include this header ONCE.
// In all other files just include it, without any definitions.

#ifndef BASE64_H_
#define BASE64_H_

// Header file

#define B64_ALPHABET_ERROR 0
#define B64_INVALID_LENGTH 1
#define B64_INVALID_CHARS 2

const char* b64_encode(const char* string);
// -> Encode a string with Base64

const char* b64_encode_url_safe(const char* string);
// -> Encode a string with Base64 with URL Safe alphabet

const char* b64_encode_alphabet(const char* string, const char* alphabet);
// -> Encode a string with Base64 with custom alphabet

const char* b64_decode(const char* string);
// -> Decode a string with Base64

const char* b64_decode_url_safe(const char* string);
// -> Decode a string with Base64 with URL Safe alphabet

const char* b64_decode_alphabet(const char* string, const char* alphabet);
// -> Decode a string with Base64 with custom alphabet

const char* b64_error();
// -> Return a string with error on error.


#ifdef BASE64_IMPL

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int B64_ERROR_CODE = -1;

#ifndef B64_MAX_STR_LENGTH
#define B64_MAX_STR_LENGTH 64000
#endif

const char* b64_encode_alphabet(const char* string, const char* alphabet) {
    if (strnlen(alphabet, 66) != 65) {
        B64_ERROR_CODE = B64_ALPHABET_ERROR;
        return 0;
    }
    
    int length = strnlen(string, B64_MAX_STR_LENGTH);
    
    int pad = 0;
    if (length % 3 == 1) pad = 2;
    if (length % 3 == 2) pad = 1;
    
    int til_pad = length / 3;
    
    int end_length = length % 3 == 0 ? length / 3 * 4 : length / 3 * 4 + 4;
    
    char* result = malloc(end_length+1);

    for (int i = 0; i < til_pad; ++i) {
        int str_pos = i*3;
        int end_pos = i*4;
        int first_sixtet = string[str_pos] >> 2;
        int second_sixtet = 
            ((string[str_pos] & 0b00000011) << 4) +
            (string[str_pos+1] >> 4);
        int third_sixtet = 
            ((string[str_pos+1] & 0b00001111) << 2) +
            (string[str_pos+2] >> 6);
        int fourth_sixtet = string[str_pos+2] & 0b00111111;
        
        result[end_pos] = alphabet[first_sixtet];
        result[end_pos+1] = alphabet[second_sixtet];
        result[end_pos+2] = alphabet[third_sixtet];
        result[end_pos+3] = alphabet[fourth_sixtet];
    }
    
    int end_sixtet = til_pad*3;
    int end_string = til_pad*4;
    
    if (pad == 1) {
        int first_sixtet = string[end_sixtet] >> 2;
        int second_sixtet =
            ((string[end_sixtet] & 0b00000011) << 4) +
            ((string[end_sixtet+1] & 0b11110000) >> 4);
        int third_sixtet = (string[end_sixtet+1] & 0b00001111) << 2;
        
        result[end_string] = alphabet[first_sixtet];
        result[end_string+1] = alphabet[second_sixtet];
        result[end_string+2] = alphabet[third_sixtet];
        result[end_string+3] = alphabet[64];
    } else if (pad == 2) {
        int first_sixtet = (string[end_sixtet] & 0b11111100) >> 2;
        int second_sixtet = (string[end_sixtet] & 0b00000011) << 4;
        
        result[end_string] = alphabet[first_sixtet];
        result[end_string+1] = alphabet[second_sixtet];
        result[end_string+2] = alphabet[64];
        result[end_string+3] = alphabet[64];
    }
    
    result[end_length] = '\0';

    return result;
}

const char* b64_encode(const char* string) {
    return b64_encode_alphabet(string, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=");
}

const char* b64_encode_url_safe(const char* string) {
    return b64_encode_alphabet(string, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_=");
}

int _b64_string_pos(char charachter, const char* string) {
    int length = strlen(string);
    for (int i = 0; i < length; ++i) {
        if (string[i] == charachter) return i;
    }
    return -1;
}

int _b64_count(char charachter, const char* string) {
    int length = strlen(string);
    int result = 0;
    for (int i = 0; i < length; ++i) {
        if (string[i] == charachter) result++;
    }
    return result;
}

const char* b64_decode_alphabet(const char* string, const char* alphabet) {
    if (strnlen(alphabet, 66) != 65) {
        B64_ERROR_CODE = B64_ALPHABET_ERROR;
        return 0;
    }

    int length = strnlen(string, B64_MAX_STR_LENGTH);
    if (length % 4 != 0) {
        B64_ERROR_CODE = B64_INVALID_LENGTH;
        return 0;
    }

    int end_length = length/4*3 - _b64_count(alphabet[64], string);
    char* result = malloc(end_length+1);

    for (int i = 0; i < length/4-1; ++i) {
        int string_pos = i*4;
        int result_pos = i*3;
        
        int num1 = _b64_string_pos(string[string_pos], alphabet);
        int num2 = _b64_string_pos(string[string_pos+1], alphabet);
        int num3 = _b64_string_pos(string[string_pos+2], alphabet);
        int num4 = _b64_string_pos(string[string_pos+3], alphabet);

        if (num1 < 0 || num2 < 0 || num3 < 0 || num4 < 0) { B64_ERROR_CODE = B64_INVALID_CHARS; return 0; }

        result[result_pos+0] = (num1 << 2) + ((num2 & 0b110000) >> 4);
        result[result_pos+1] = ((num2 & 0b1111) << 4) + ((num3 & 0b111100) >> 2);
        result[result_pos+2] = ((num3 & 0b11) << 6) + num4;
    }

    int last_sixtet = (length/4-1)*3;
    int last_part = length-4;
    if (_b64_count(alphabet[64], string) == 2) {
        int num1 = _b64_string_pos(string[last_part], alphabet);
        int num2 = _b64_string_pos(string[last_part+1], alphabet);
        if (num1 < 0 || num2 < 0) { B64_ERROR_CODE = B64_INVALID_CHARS; return 0; }
        
        result[last_sixtet] = (num1 << 2) + ((num2 & 0b110000) >> 4);
    } else if (_b64_count(alphabet[64], string) == 1) {
        int num1 = _b64_string_pos(string[last_part], alphabet);
        int num2 = _b64_string_pos(string[last_part+1], alphabet);
        int num3 = _b64_string_pos(string[last_part+2], alphabet);
        if (num1 < 0 || num2 < 0 || num3 < 0) { B64_ERROR_CODE = B64_INVALID_CHARS; return 0; }
        
        result[last_sixtet] = (num1 << 2) + ((num2 & 0b110000) >> 4);
        result[last_sixtet+1] = ((num2 & 0b1111) << 4) + ((num3 & 0b111100) >> 2);
    } else {
        int num1 = _b64_string_pos(string[last_part], alphabet);
        int num2 = _b64_string_pos(string[last_part+1], alphabet);
        int num3 = _b64_string_pos(string[last_part+2], alphabet);
        int num4 = _b64_string_pos(string[last_part+3], alphabet);

        if (num1 < 0 || num2 < 0 || num3 < 0 || num4 < 0) { B64_ERROR_CODE = B64_INVALID_CHARS; return 0; }

        result[last_sixtet+0] = (num1 << 2) + ((num2 & 0b110000) >> 4);
        result[last_sixtet+1] = ((num2 & 0b1111) << 4) + ((num3 & 0b111100) >> 2);
        result[last_sixtet+2] = ((num3 & 0b11) << 6) + num4;
    }

    result[end_length] = '\0';

    return result;
}

const char* b64_decode(const char* string) {
    return b64_decode_alphabet(string, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=");
}

const char* b64_decode_url_safe(const char* string) {
    return b64_decode_alphabet(string, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_=");
}

const char* b64_error() {
    switch (B64_ERROR_CODE) {
        case B64_ALPHABET_ERROR: return "Length of alphabet is not 65";
        case B64_INVALID_LENGTH: return "Length of string should be divisible by 4";
        case B64_INVALID_CHARS: return "String has invalid charachters";
    }
    return "No error";
}

#endif // BASE64_IMPL

#endif // BASE64_H_

