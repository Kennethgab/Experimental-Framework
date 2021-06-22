from typing import Callable
import serial, time
from serial.serialutil import SerialException
import yaml
import os
import serial.tools.list_ports

from serial.serialwin32 import Serial
from yaml.loader import FullLoader


def header_dict(headers: list[str]):
    """turns a list of header lines contained as

    \---
    key: value
    key: value
    repeat ..

    \...

    into a dictionary.

    Args:
        headers (list[str]): list of lines

    Returns:
        dict(str) -> str : key-value pairs of any meta-information pertaining the logfile.
    """
    dict = {}
    for header in headers:
        h = header.replace(" ", "")
        h = h.split(":")
        dict[h[0]] = h[1]
    return dict


def load_yaml_header(file_path: str):
    """wrapper for read_head func to load a log file.

    Args:
        file_path (str): relative or absolute path to log file.

    Returns:
        dict[str] : key-value pairs
    """
    with open(file_path, "r") as stream:
        return read_header(stream)


def read_line(stream) -> str:
    """reads line from either a file stream or serial stream

    Args:
        stream (serial/filestream): Serial or opened file

    Returns:
        str: the read line.
    """
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
    """reads the header in a stream, which  is started by --- and closed by ...

    Args:
        stream (stream): stream of lines that is currently pointing at a header to be read in.

    Returns:
        dict: dictionary of header's key-value pairs.
        so for instance a header of

        \---
        "device" : "esp32"
        "delimiter" : ","

        \...

        would map to dict = {"device" : "esp32", "delimiter" : ","}
    """
    lines = []
    while True:
        line = read_line(stream)
        lines.append(line)
        if line == "...\n":
            break
    s = "".join(lines)
    return yaml.load(s, Loader=FullLoader)


def write_logfile(header: dict[str, str], content: list[str], result_dir: str):
    """writes the logfiles down in ./result_dir/device/log_type.log
    with headers and associated content.
    if device header is not set, put in unknown_device directory
    with unknown_log_{unix timestamp}

    Args:
        header (dict[str, str]): YML log header
        content (list[str]): all lines, excluding header
        result_dir (str): path to dir to store log
    """
    device = header.get("device", "unknown_device")
    log_type = header.get("log_type", f"unknown_log_{time.time()}")
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
    """encapsulates the logic for capturing serial communication with a device"""

    def __init__(
        self,
        serial: Serial,
        terminate: Callable[[str], bool],
        delimiter: str,
        result_dir: str,
    ):
        """constructor for seriallogger, takes in Serial stream for communication, the delimiter used,
        and a sentinel value for terminating logging. Stores all logged files in log_files.
        currently read log is stored in current_content and current_header.
        log_start bool controls whether or not serial communication should be captured,
        and is set to true by reading header preamble --- which indicates start of log.

        Args:
            serial (Serial): serial stream instance
            terminate (Callable[[str], bool]): condition to check for complete log termination.
            delimiter (str): delimiter in logfile
            result_dir (str): path to dir to store written logfiles.
        """
        self.terminate = terminate
        self.ser = serial
        self.current_content = []
        self.current_header = {}
        self.delimiter = delimiter
        self.log_files = []
        self.log_start = False
        self.result_dir = result_dir

    def save_log(self, create_files):
        """stores current log in log_files list, triggered
        when log is done/new header is being read.

        Args:
            create_files (boolean): true: create file
        """
        self.log_files.append([self.current_header, self.current_content])
        if create_files:
            write_logfile(self.current_header, self.current_content, self.result_dir)
        self.current_content = []
        self.current_header = {}

    def log_all(self, create_files=True):
        """Runs until it detects sentinel value > as start of line.
        Captures all serial communication line by line, and extracts logfile from it
        by associated header and content (columns/rows).
        stores logs in result_dir categorized by device name.

        create_files (bool): if true, store logs as files in result_dir/device/{log_type}.log

        """
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
                print("Done!")
                break
            elif self.log_start:
                self.current_content.append(line)

        """simple sentinel value check to terminate logging
        """


def term_func(line: str) -> bool:
    return line.startswith(">")


def write_logs(result_dir: str):
    """Uses a seriallogger instance to read all logs indefinely until sentinel value.
    automatically finds open Serial port and creates a stream.
    Has trivial inbuilt exception handling for devices that have two-stage uploads that causes serialexceptions.
    """
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
        logger = SerialLogger(ser, term_func, ",", result_dir)
        logger.log_all()
    except SerialException:  # on some boards the device will disconnect from USB when clicking reset button, prompting a serialexception.
        time.sleep(2)
        ser = serial.Serial(used_port, baudrate=115200)
        logger = SerialLogger(ser, term_func, ",", result_dir)
        logger.log_all()


if __name__ == "__main__":
    write_logs("test_results")
