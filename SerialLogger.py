import serial, time
import yaml
import os

from serial.serialwin32 import Serial

# turns a list ofaheader lines  contained in --- into dict key-value pairs
# ie
# ---
# device: esp32
# cols:
# ---
# for example : dict["device"] = "esp32"
# required headers: samples, device, log_type, cols
def header_dict(headers: list[str]):
    dict = {}
    for header in headers:
        h = header.replace(" ", "")
        h = h.split(":")
        dict[h[0]] = h[1]
    return dict


def write_logfile(header: dict[str, str], content: list[str]):
    device = header["device"]
    log_type = header["log_type"]
    print(f"Writing log {log_type} for device {device}")
    with open(f"{result_dir}/{device}_{log_type}.log", "w") as f:
        f.write("---\n")
        f.write(yaml.dump(header))
        f.write("...\n")
        f.writelines(content)
        f.close()


class SerialLogger:
    def __init__(self, serial: Serial):
        self.ser = serial
        self.current_content = []
        self.current_header = {}
        self.log_files = []
        self.log_start = False

    def read_line(self) -> str:
        return self.ser.readline().decode("utf-8")

    def read_header(self) -> str:
        lines = ["---\n"]
        while True:
            line = self.read_line()
            lines.append(line)
            if line == "...\n":
                break
        s = "".join(lines)
        return yaml.load(s)

    def save_log(self, create_files):
        self.log_files.append([self.current_header, self.current_content])
        if create_files:
            write_logfile(self.current_header, self.current_content)
        self.current_content = []
        self.current_header = {}

    def log_all(self, create_files=True):
        while True:
            line = self.read_line()
            if line == "---\n" and len(self.current_content) == 0:
                self.current_header = self.read_header()
                self.log_start = True
            elif line == "---\n" and len(self.current_content) > 1:
                self.save_log(create_files)
                self.current_header = self.read_header()
            elif line.startswith(">"):
                self.save_log(create_files)
                break
            elif self.log_start:
                self.current_content.append(line)


result_dir = "./test_results"
if __name__ == "__main__":

    if not os.path.exists(result_dir):
        os.makedirs(result_dir)

    ser = serial.Serial("COM3", baudrate=115200)
    logger = SerialLogger(ser)
    logger.log_all()
