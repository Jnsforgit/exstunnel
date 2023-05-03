#include "common.h"
#include "replay.h"

void initLogger()
{
    /* close printf buffer */
    setbuf(stdout, NULL);
    /* initialize EasyLogger */
    elog_init();
    /* set EasyLogger log format */
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
    /* start EasyLogger */
    elog_start();
}

int main()
{
    static int count = 0;
    bool ret = false;
    std::string filepath("./test.csv");
    FileCanchannel chn;

    initLogger();

    log_i("Start programing ...");
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
        fmt << " " << bytesToHexString(frame->data, frame->dlc);

        // std::cout << fmt.str() << std::endl;
    });

    if (ret)
    {
        log_i("Open file [%s] success!", filepath.c_str());
    }
    else
    {
        log_i("Open file [%s] failed!", filepath.c_str());
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    while (chn.replay())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    std::cout << "read [" << count << "] frames" << std::endl;

    return 0;
}
