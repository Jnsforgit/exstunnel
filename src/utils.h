#ifndef UTILS_H
#define UTILS_H

namespace exstunnel
{
    class utils
    {
    public:

        static int addFdFlag(int fd, int flag);
        static std::string format(const char *fmt, ...);
        static int convertStringToInt(const std::string &str);
        static unsigned int hexToUint(const std::string &hex_str);
        static unsigned long long timestrToUint64(const std::string &time_str);
        static std::string bytesToHexString(const unsigned char *bytes, const int length);
        static std::vector<std::string> split(const std::string &str, const std::string &delimiter);
        static void strToCharArray(const std::string &str, unsigned char *char_array, size_t array_size);
    };

    class Noncopyable
    {
    protected:
        Noncopyable() = default;
        virtual ~Noncopyable() = default;

        Noncopyable(const Noncopyable &) = delete;
        Noncopyable &operator=(const Noncopyable &) = delete;
    };
}

#endif
