import pandas
from serial.serialwin32 import Serial
import yaml
from yaml.loader import FullLoader
from SerialLogger import read_header
import pandas as pd


def load_yaml_header(file_path):
    with open(file_path, "r") as stream:
        return read_header(stream)


path = "./test_results/Adafruit_Feather/all.log"
if __name__ == "__main__":
    header = load_yaml_header(path)
    data = pd.read_csv(path, sep=header["delimiter"], skiprows=(len(header) + 1))
    col_names = data.columns.values.tolist()
    data.sort_values(by=["AVG time [microseconds]"], inplace=True)
    print(data)
