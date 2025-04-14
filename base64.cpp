#include "base64.h"
#include <ranges>
#include <algorithm>
#include <map>

#include <loglib/loglib.h>

namespace { // start of anonymous namespace

std::map<char, int> initialize_dict()
{
    std::map<char, int> dict;

    int i = 0;
    for (char c = 'A'; c <= 'Z'; ++c){
        dict[c] = i++;
    }

    for (char c = 'a'; c <= 'z'; ++c){
        dict[c] = i++;
    }

    for (char c = '0'; c <= '9'; ++c ){
        dict[c] = i++;
    }

    dict['+'] = i++;
    dict['/'] = i;

    dict['='] = 0;
    return dict;
}

std::map<int, char> reverse_dict(const std::map<char, int> &dict)
{
    std::map<int, char> rev_dict;
    for (auto i = dict.begin(); i != dict.end(); ++i){
        if (i->first == '=')
            continue;

        rev_dict[i->second] = i->first;
    }
    return rev_dict;
}

static std::map<char, int> base64_dict = initialize_dict();
static std::map<int, char> base64_dict_enc = reverse_dict(base64_dict);

bool inline verify_string_length(const std::string &string)
{
    return string.length() % 4 == 0;
}

bool inline valid_char(const char& c){
    return (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z') ||
           (c >= '0' && c <= '9') ||
           c == '+' || c == '/'   ||
           c == '=';
}

bool inline verify_string_content(const std::string &string)
{
    size_t valid_char_cnt = std::count_if(string.begin(), string.end(), valid_char);
    return valid_char_cnt == string.length();
}

bool inline verify_string_ending(const std::string &string)
{
    size_t number_of_equal_signs = std::count(string.begin(), string.end(), '=');

    switch(number_of_equal_signs){
    case 0:
        return true;
    case 1:
        return string.back() == '=';
    case 2:
        return string.back() == '=' &&
               string[string.length() - 2] == '=';
    default:
        return false;
    }
}

bool inline verify_string(const std::string &string)
{
    return verify_string_length(string) &&
           verify_string_content(string) &&
           verify_string_ending(string);
}

void inline pad_string(std::string& s){
    if (s.size() % 4 == 0)
        return;

    int pad_nr = 4 - (s.size() % 4);
    s.append(pad_nr, '=');
}

} // end of anonymous namespace



std::vector<uint8_t> base64::decode_base64(const std::string &string, bool strict)
{
    std::vector<uint8_t> dest;
    if (strict && !verify_string(string)){
        return dest;
    }

    std::string normalized_string;
    if (strict){
        normalized_string = string;
    } else {
        std::copy_if(string.begin(), string.end(), std::back_inserter(normalized_string), valid_char);
        pad_string(normalized_string);
    }

    dest.resize(normalized_string.size() / 4 * 3 + 1);
    size_t destcnt = 0;
    uint32_t temp;

    for (size_t idx = 0; idx < normalized_string.length();){
        temp = 0;

        for (uint8_t i = 0; i < 4; ++i, ++idx){
            temp <<= 6;
            temp |= base64_dict[normalized_string[idx]];
        }

        for (int i = 2; i >= 0; --i){
            dest[destcnt++] = (temp >> (i * 8)) & 0xff;
        }
    }

    dest.push_back(0); // null terminator, just in case.
    return dest;
}

std::string base64::encode_base64(const std::vector<uint8_t>& data)
{
    std::string ret;
    size_t idx = 0;
    uint32_t temp;

    while(data.size() - idx >= 3) {
        temp = 0;
        temp |= (data[idx++] << 16);
        temp |= (data[idx++] << 8);
        temp |= (data[idx++]);

        for (int i = 3; i >= 0; --i){
            int sixBits = (temp >> (i * 6)) & 0x3f;
            ret.append(&base64_dict_enc[sixBits]);
        }
    }

    temp = 0;
    if (data.size() - idx == 1) {
        temp = data[idx];

        ret.append(&base64_dict_enc[temp >> 2], 1);
        ret.append(&base64_dict_enc[(temp << 4) & 0x3f], 1);
    }

    if (data.size() - idx == 2) {
        temp = (data[idx++] << 10);
        temp |= (data[idx] << 2);
        for (ssize_t i = 2; i >= 0; --i){
            int sixBits = (temp >> (i * 6)) & 0x3f;
            ret.append(&base64_dict_enc[sixBits], 1);
        }
    }

    while (ret.length() % 4 != 0)
        ret.append("=");

    return ret;
}

std::string base64::encode_base64(const std::string &data)
{
    std::vector<uint8_t> vec {data.begin(), data.end()};
    return encode_base64(vec);
}
