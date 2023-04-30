
#include "replay.h"

#include <time.h>

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
        std::cerr << e.what() << '\n';
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

FileCanchannel ::FileCanchannel(/* args */)
{
    m_state = std::make_shared<RunState_E>(RunState_stop);
    m_buffer = std::make_shared<LockedQueue<Canframe>>();
    m_file = std::make_shared<std::ifstream>();
}

FileCanchannel ::~FileCanchannel()
{
    *m_state = RunState_stop;
}

bool FileCanchannel::openFile(std::string filepath, std::function<void(Canframe *)> callback)
{
    if (NULL != m_file.get())   /* 已经打开一个文件了 */
    {
        m_file->close();
        *m_state = RunState_stop;
    }

    m_file->open(filepath, std::ios::in);
    if (!m_file->is_open())
    {
        m_file.reset();
        return false;
    }

    
    std::thread thd([&]() {
        std::shared_ptr<RunState_E> state = m_state;
        std::shared_ptr<std::ifstream> file = m_file;
        std::shared_ptr<LockedQueue<Canframe>> buffer = m_buffer;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        *m_state = RunState_run;
        while (RunState_stop < *state)
        {
            Canframe frame;
            std::string line = "";

            if (file->eof() || !file->good())
            {
                *state = RunState_stop;
                continue;
            }
            
            if (500 < buffer->size())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            std::getline(*file, line);
            if (file->fail())
            {
                std::cout << "Fail to read line." << std::endl;
                break;
            }

            if (!parseCanframe(line, frame))
            {
                try
                {
                    m_callback(NULL);
                }
                catch(const std::exception& e)
                {
                    // std::cerr << e.what() << '\n';
                }
                
                
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            buffer->push(frame);
        } 

        if (m_file->is_open())
        {
            m_file->close();
            *m_state = RunState_stop;
        }
    });

    thd.detach();
    m_callback = callback;

    return true;
}

bool FileCanchannel::replay()
{
    if (*m_state == RunState_stop)
    {
        return false;
    }

    Canframe *frame = new Canframe();

    if (m_buffer->pop(*frame))
    {
        m_callback(frame);
    }

    delete frame;
    return true;
}

bool FileCanchannel::parseCanframe(std::string &line, Canframe &frame)
{
    auto parts = split(line, ",");

    memset(frame.data, 0x00, sizeof(frame.data));
    try
    {
        frame.id = std::stoi(parts[0], nullptr);
        frame.dlc = std::stoi(parts[3], nullptr);
        frame.timestamp = timestrToUint64(parts[1]);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << " error:[" << line << "]" << std::endl;
        return false;
    }
    
    
    if (0 == frame.id || 0 == frame.timestamp || 0 == frame.dlc)
    {
        return false;
    }

    strToCharArray(parts[2], frame.data, frame.dlc);

    return true;
}

int main()
{
    static int count = 0;
    FileCanchannel chn;

    chn.openFile("./test.csv", [](Canframe *frame){
        static unsigned long long timestamp = 0;
        stringstream fmt;

        count++;

        if (NULL == frame)
        {
            return ;
        }

        if (0 == timestamp)
        {
            timestamp == frame->timestamp;
        }

        fmt << time(NULL) << "--";
        fmt << frame->timestamp / 1000000;
        fmt.precision(3);
        fmt << frame->timestamp % 1000 / 1000;
        fmt.precision(3);
        fmt << frame->timestamp % 1000000;
        fmt << " ["
            << "0x" << std::hex << frame->id << "] ";
        fmt << " " << bytesToHexString(frame->data, frame->dlc);

        // std::cout << fmt.str() << std::endl;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    while (chn.replay())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    std::cout << "read [" << count << "] frames" << std::endl;

    return 0;
}
