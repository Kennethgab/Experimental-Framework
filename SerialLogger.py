from typing import Callable
import serial, time
from serial.serialutil import SerialException
import yaml
import os
import serial.tools.list_ports

from serial.serialwin32 import Serial
from yaml.loader import FullLoader

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


def read_line(stream) -> bytes:
    line = "\n"
    try:
        line = stream.readline()
        if isinstance(line, bytes):
            return line.decode("utf-8")
        elif isinstance(line, str):
            return line
    except UnicodeDecodeError:  # some boards may print malign output at start
        return "\n"


def read_header(stream) -> dict:
    lines = []
    while True:
        line = read_line(stream)
        lines.append(line)
        if line == "...\n":
            break
    s = "".join(lines)
    return yaml.load(s, Loader=FullLoader)


def write_logfile(header: dict[str, str], content: list[str]):
    device = header["device"]
    log_type = header["log_type"]
    print(f"Writing log {log_type} for device {device}")
    dev = device.replace(" ", "_")
    if not os.path.exists(f"{result_dir}/{dev}"):
        os.makedirs(f"{result_dir}/{dev}")
    with open(f"{result_dir}/{dev}/{log_type}.log", "w") as f:
        f.write("---\n")
        f.write(yaml.dump(header, default_style='"'))
        f.write("...\n")
        f.writelines(content)
        f.close()


class SerialLogger:
    def __init__(
        self, serial: Serial, terminate: Callable[[str], bool], delimiter: str
    ):
        self.terminate = terminate
        self.ser = serial
        self.current_content = []
        self.current_header = {}
        self.delimiter = delimiter
        self.log_files = []
        self.log_start = False

    def save_log(self, create_files):
        self.log_files.append([self.current_header, self.current_content])
        if create_files:
            write_logfile(self.current_header, self.current_content)
        self.current_content = []
        self.current_header = {}

    def log_all(self, create_files=True):
        while True:
            line = read_line(self.ser)
            if line == "---\n" and len(self.current_content) == 0:
                print("Reading content in progress, this may take some time..")
                self.current_header = read_header(self.ser)
                self.log_start = True
            elif line == "---\n" and len(self.current_content) > 1:
                self.save_log(create_files)
                self.current_header = read_header(self.ser)
            elif term_func(line):
                self.save_log(create_files)
                break
            elif self.log_start:
                self.current_content.append(line)


def term_func(line: str) -> bool:
    return line.startswith(">")


def write_logs():
    # get all available ports
    ports = []
    for i in serial.tools.list_ports.comports():
        ports.append(str(i).split(" ")[0])

    used_port = ports[
        0
    ]  # use first available one, change if you have multiple or specify

    if not os.path.exists(result_dir):
        os.makedirs(result_dir)

    try:
        ser = serial.Serial(used_port, baudrate=115200)
        logger = SerialLogger(ser, term_func, ",")
        logger.log_all()
    except SerialException:  # on some boards the device will disconnect from USB when clicking reset button, prompting a serialexception.
        time.sleep(2)
        ser = serial.Serial(used_port, baudrate=115200)
        logger = SerialLogger(ser, term_func, ",")
        logger.log_all()


def analyze_logs():
    pass


result_dir = "./test_results"
if __name__ == "__main__":
    write_logs()
