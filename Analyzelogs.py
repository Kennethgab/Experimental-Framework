from pandas.core.frame import DataFrame
from serial.serialwin32 import Serial
import yaml
from yaml.loader import FullLoader
from SerialLogger import load_yaml_header
import pandas as pd
import glob
from typing import TypeVar


def categorize_logfiles(dir_path) -> dict[str, pd.DataFrame]:
    """This function takes a folder containing results for a device,
    and creates a Pandas dataframe from each logfile in the folder,
    categorizing them in a dictionary.
    use "device" header in logfile to infer device
    for example:
        dir_path = "./xor_256.log",
    Args:
        dir_path ([str]): path to folder containing log files for a specific device.

    Returns:
        dict[str, pd.DataFrame]: the dict contains all primitive results which can be accessed as dataframes by dict[primitive]
        where "primitive" is the log_type header, also part of filename.
    """
    device_dict = {}
    file_paths = glob.glob(f"{dir_path}/*.log")
    for fp in file_paths:
        primitive = fp.split(".log")[0].split("\\")[-1]
        device_dict[primitive] = load_log(fp)
    return device_dict


def extract_avg(dict, primitives, col="time [microseconds]"):
    """Takes a device dict containing log dataframes, iterates through a list of database primitives,
    and extracts the average (in seconds) from specified column. Creates a list/column of averages for a specific device
    by given primitives.

    Args:
        dict (str, pd.DataFrame): Device dict where primitive str maps to dataframe result
        primitives (list(str)): list of primitives where average execution time will be extracted
        col (str): name of column that contains avg execution time, defaults to "time [microseconds]"

    Returns:
        list(int): Average execution time of given primitives listed in given order, to be used as column in a dataframe.
    """
    avgs = []
    for primitive in primitives:
        try:
            avg = round(dict[primitive][col].mean()) * 1e-6
            avgs.append(avg)  # convert microseconds to seconds
        except KeyError:
            avg = None
            avgs.append(avg)
    return avgs


def calculate_cost(df, scheme_prims, device):
    """takes a list of tuples of form ("primitive index", amount)
    and looks up the AVG execution time table to calculate scheme cost lis by given device name.

    Args:
        df (pd.DataFrame): AVG execution time of primitives dataframe
        scheme_prims (list([str,int])): list of tuples to calculate cost in form [("SHA256", 2), .. (index, amount)]
        device (str): device column to use for primitive lookup

    Returns:
        list(float): list of the calculated cost for each primitive that can be summed to equal the scheme execution time.
    """
    costs = []
    for prim_pair in scheme_prims:
        primitive = prim_pair[0]
        multi = prim_pair[1]
        cost = df.at[primitive, device] * multi
        costs.append(cost)
    return costs


def calculate_all(df, schemes, device):
    """Goes through a takes a list of list of tuples and calculates scheme cost using calculate_cost function then
    summing the returned list.


    Args:
        df (pd.DataFrame): dataframe of average execution time
        schemes (list[str,int]): list of list of tuples to calculate cost in form [("SHA256", 2), .. (index, amount)]
        device (str): which device column to use in the dataframe

    Returns:
        list(int) : column of theoretical calculated execution time for the schemes in given order
    """
    scheme_costs = []
    for scheme_prims in schemes:
        scheme_cost = round(sum(calculate_cost(df, scheme_prims, device)), 6)
        scheme_costs.append(scheme_cost)
    return scheme_costs


def load_log(path: str) -> pd.DataFrame:
    """loads a .log file by extracting the YML header,
    currently only using delimiter as metainfo and adds rest of headers to dataframe attributes.

    Args:
        path ([type]): [description]

    Returns:
        pd.DataFrame: [description]
    """
    header = load_yaml_header(path)
    df = pd.read_csv(path, sep=header["delimiter"], skiprows=(len(header) + 1))
    for key in header:
        df.attrs[key] = header[key]
    return df


def percentage_difference(a, b):
    """calculate percentage difference"""
    delta = abs(a - b)
    return round((delta / (a + b)) * 100, 2)


def percentage_error(a, b):
    """calculate percentage error"""
    delta = abs(a - b)
    return round((delta / abs(b)) * 100, 2)


def percentage_change(a, b):
    """calculate percentage change"""
    delta = a - b
    return round((delta / abs(b)) * 100, 2)


if __name__ == "__main__":
    path = "./test_results/Adafruit_Feather/all.log"
    header = load_yaml_header(path)
    data = pd.read_csv(path, sep=header["delimiter"], skiprows=(len(header) + 1))
    col_names = data.columns.values.tolist()
    data.sort_values(by=["AVG time [microseconds]"], inplace=True)
    print(data)
