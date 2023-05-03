#include "common.h"

int convertStringToInt(const std::string& str)
{
    int result = 0;
    int sign = 1;
    int i = 0;

    if (0 == str.length())
    {
        return 0;
    }

    if (str[0] == '-') // 处理负数
    {
        sign = -1;
        ++i;
    }

    for (; i < str.length(); ++i)
    {
        if (isdigit(str[i])) // 如果该字符是数字，则将其加入结果中
        {
            result *= 10;
            result += str[i] - '0';
        }
        else // 如果是其他字符（比如加减号），则跳过
        {
            continue;
        }
    }

    return result * sign;
}

void strToCharArray(const std::string &str, unsigned char *char_array, size_t array_size)
{
    std::stringstream ss(str);

    for (size_t i = 0; i < array_size; ++i)
    {
        int num;
        ss >> std::hex >> num;
        char_array[i] = (char)num;
        if (ss.peek() == ' ')
        {
            ss.ignore();
        }
    }
}

unsigned long long timestrToUint64(const std::string &time_str)
{
    try
    {
        double timestamp = std::stod(time_str);
        return static_cast<unsigned long long>(timestamp * 1000000);
    }
    catch(const std::exception& e)
    {
        logd("Exchange time string [%s] error:[%s]!", time_str.c_str(), e.what());
        return 0;
    }
}

unsigned int hexToUint(const std::string &hex_str)
{
    size_t idx = 0;
    if (hex_str.compare(0, 2, "0x") == 0)
    {
        idx = 2;
    }

    std::string num_str = hex_str.substr(idx);
    std::stringstream ss;
    ss << std::hex << num_str;
    unsigned int result;
    ss >> result;

    return result;
}

std::string bytesToHexString(const unsigned char *bytes, const int length)
{
    if (bytes == NULL)
    {
        return "";
    }

    std::string buff;
    const int len = length;
    for (int j = 0; j < len; j++)
    {
        int high = bytes[j] / 16, low = bytes[j] % 16;
        buff += (high < 10) ? ('0' + high) : ('a' + high - 10);
        buff += (low < 10) ? ('0' + low) : ('a' + low - 10);
        buff += " ";
    }

    return buff;
}

std::vector<std::string> split(const std::string &str, const std::string &delimiter)
{
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = str.find(delimiter);
    while (end != std::string::npos)
    {
        result.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    result.push_back(str.substr(start));
    return result;
}
