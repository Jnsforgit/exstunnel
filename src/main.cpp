#include "common.h"
#include "replay.h"

using namespace exstunnel;

int main()
{
    static int count = 0;
    bool ret = false;
    std::string filepath("./test.csv");
    FileCanchannel chn;

    logi("Start programing ...");
    ret = chn.openFile(filepath, [](Canframe *frame){
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
        fmt << " " << utils::bytesToHexString(frame->data, frame->dlc);

        // std::cout << fmt.str() << std::endl;
    });

    if (ret)
    {
        logi("Open file [%s] success!", filepath.c_str());
    }
    else
    {
        logi("Open file [%s] failed!", filepath.c_str());
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    while (chn.replay())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    std::cout << "read [" << count << "] frames" << std::endl;

    return 0;
}
