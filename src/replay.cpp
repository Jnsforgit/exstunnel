
#include "replay.h"

#include <time.h>

FileCanchannel ::FileCanchannel(/* args */)
{
    m_buffer = std::make_shared<LockedQueue<Canframe>>();
    m_file = std::make_shared<ReplayFile_T>();
}

FileCanchannel ::~FileCanchannel()
{
    m_file->state = RunState_stop;
}

bool FileCanchannel::openFile(std::string filepath, std::function<void(Canframe *)> callback)
{
    FILE *fp = NULL;

    fp = fopen(filepath.c_str(), "r");
    if (NULL == fp)
    {
        return false;
    }

    if (NULL != m_file->fptr) /* 已经打开一个文件了 */
    {
        logd("Close last opened file!");
        fclose(m_file->fptr);
        m_file->state = RunState_stop;
        m_file.reset();
    }

    fseek(fp, 0, SEEK_SET);
    m_file->fptr = fp;
    std::thread thd([&]() {
        std::shared_ptr<ReplayFile_T> file = m_file;
        std::shared_ptr<LockedQueue<Canframe>> buffer = m_buffer;

        m_file->state  = RunState_run;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        logi("Start file read thread ...");
        while (RunState_stop < m_file->state)
        {
            Canframe frame;
            std::string line = "";

            if (NULL != m_file->fptr && feof(m_file->fptr))
            {
                logd("Read file eof!");
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                file->state = RunState_stop;
                continue;
            }

            if (500 < buffer->size())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            line = readLine(file);
            if (!parseCanframe(line, frame))
            {
                try
                {
                    m_callback(NULL);
                }
                catch(const std::exception& e)
                {
                    logd("Run file replay callback exception catched:[%s]!", e.what());
                }
                
                
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            else
            {
                logd("Parse can frame from line error:[%s]!", line.c_str());
            }

            buffer->push(frame);
        } 

        if (!file)
        {
            fclose(file->fptr);
            file->state = RunState_stop;
            logi("Exiting file replay thread.");
        } 
    });

    thd.detach();
    m_callback = callback;

    return true;
}

bool FileCanchannel::replay()
{
    if (m_file->state == RunState_stop)
    {
        logi("File replay thread is not running.");
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

std::string FileCanchannel::readLine(std::shared_ptr<ReplayFile_T> &file)
{
    std::string line;
    char *buf = NULL;
    size_t buf_size = 0;
    for (;;)
    {
        int ch = fgetc(file->fptr);
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
        logd("Exchange line [%s] error:[%s]!", line.c_str(), e.what());
        return false;
    }

    if (0 == frame.id || 0 == frame.timestamp || 0 == frame.dlc)
    {
        logd("Bad can frame params:[%d:%d:%d]!", frame.id, frame.timestamp, frame.dlc);
        return false;
    }

    strToCharArray(parts[2], frame.data, frame.dlc);

    return true;
}
