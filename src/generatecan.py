import can
import csv
import random
import time

def generate_can_message(can_id):
    data = [random.randint(0, 255) for _ in range(random.randint(0, 8))]
    return can.Message(arbitration_id=can_id, data=data, timestamp=time.time())

def main():
    # 打开要写入的 CSV 文件
    filename = "test.csv"
    with open(filename, "w", newline="") as f:
        writer = csv.writer(f)
        # 写入表头行
        writer.writerow(["can_id", "timestamp", "data", "dlc"])
        # 循环发送 CAN 报文，并将其写入 CSV 文件
        try:
            while True:
                    can_id = random.randint(0, 0x7FF)
                    message = generate_can_message(can_id)

                    # 将 CAN 报文转为 CSV 行
                    dataline = ''
                    for byte in message.data:
                        if 0 != len(dataline):
                            dataline += ' '
                        dataline += '%02x'%(byte)

                    row = [message.arbitration_id, message.timestamp]
                    row += [dataline]
                    row += [message.dlc]
                    writer.writerow(row)
                    print("[{:.6f}] Message sent on : {}".format(message.timestamp, message))
                    time.sleep(0.1)
        except KeyboardInterrupt:
            # 在用户按下 Ctrl-C 退出程序时关闭 CAN总线
            f.close()

if __name__ == "__main__":
    main()