
#include <stdlib.h>
#include <string.h>

#include <string>
#include <fstream>
#include <queue>
#include <mutex>
#include <chrono>
#include <thread>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>

using namespace std;

template <typename T>
class LockedQueue
{
public:
    void push(const T &item)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push(item);
    }

    bool pop(T &item)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_queue.empty())
        {
            return false;
        }
        item = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

    bool empty() const
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    unsigned int size()
    {
        return m_queue.size();
    }

private:
    std::queue<T> m_queue;
    mutable std::mutex m_mutex;
};

struct Canframe
{
    unsigned int id;
    unsigned int dlc;
    unsigned char data[8];
    unsigned long long timestamp;
};

typedef enum tagRunState
{
    RunState_stop = 0,
    RunState_pause,
    RunState_run
} RunState_E;

class FileCanchannel
{

public:
    FileCanchannel(/* args */);
    ~FileCanchannel();

    bool openFile(std::string filepath, std::function<void(Canframe *)> callback);
    bool replay();

private:
    bool parseCanframe(std::string &line, Canframe &frame);

    /* data */
    std::shared_ptr<RunState_E> m_state;
    std::shared_ptr<FILE *> m_file;
    std::shared_ptr<LockedQueue<Canframe>> m_buffer;
    std::function<void(Canframe *)> m_callback;
};