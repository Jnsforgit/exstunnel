
#include "replay.h"

#include <time.h>

FileCanchannel ::FileCanchannel(/* args */)
{
    m_state = std::make_shared<RunState_E>(RunState_stop);
    m_buffer = std::make_shared<LockedQueue<Canframe>>();
    m_file = std::make_shared<FILE *>();
}

FileCanchannel ::~FileCanchannel()
{
    *m_state = RunState_stop;
}

bool FileCanchannel::openFile(std::string filepath, std::function<void(Canframe *)> callback)
{
    FILE *fp = NULL;

    fp = fopen(filepath.c_str(), "r");
    if (NULL == fp)
    {
        return false;
    }

    if (NULL != m_file.get()) /* 已经打开一个文件了 */
    {
        fclose((FILE *)m_file.get());
        *m_state = RunState_stop;
        m_file.reset();
    }

    *m_file = fp;

    std::thread thd([&]() {
        std::shared_ptr<RunState_E> state = m_state;
        FilePtr file = m_file;
        std::shared_ptr<LockedQueue<Canframe>> buffer = m_buffer;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        *m_state = RunState_run;
        while (RunState_stop < *state)
        {
            Canframe frame;
            std::string line = "";

            if (0 == feof((FILE *)m_file.get()))
            {
                *state = RunState_stop;
                continue;
            }
            
            if (500 < buffer->size())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            line = readLine(m_file);
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

        if (NULL != m_file.get())
        {
            fclose((FILE *)m_file.get());
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

std::string FileCanchannel::readLine(FilePtr file)
{
    std::string line;
    char *buf = NULL;
    size_t buf_size = 0;
    for (;;)
    {
        int ch = fgetc((FILE *)file.get());
        switch (ch)
        {
            case EOF:
            {
                if (line.empty())
                {
                    return "";
                }
                return line;
            }
            case '\n':
            {
                return line;
            }
            default:
            {
                if (buf_size == 0 || (line.size() + 1 >= buf_size))
                {
                    buf_size = line.size() + 128;
                    buf = (char *)std::realloc(buf, buf_size);
                    if (!buf)
                    {
                        throw std::runtime_error("Memory allocation failed!");
                    }
                }
                buf[line.size()] = ch;
                line.append(buf + line.size(), 1);
                break;
            }
        }
    }
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
    catch (const std::exception &e)
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
