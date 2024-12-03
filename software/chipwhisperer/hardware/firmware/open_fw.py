from importlib import resources
import io

curpath = resources.files("chipwhisperer").joinpath("hardware/firmware")

def res_file_path(dev_name, file):
    return curpath.joinpath(dev_name).joinpath(file)

def bit_raw(dev_name):
    with open(res_file_path(dev_name, "bitstream.bit"), "rb") as f:
        return f.read()

def bit_zip(dev_name, filelike=True):
    with open(res_file_path(dev_name, "bitstream.zip"), "rb") as f:
        data = f.read()
        if filelike:
            data = io.BytesIO(data)
        return data
    pass

def mcufw(dev_name, filelike=True):
    with open(res_file_path(dev_name, "mcufw.bin"), "rb") as f:
        data = f.read()
        if filelike:
            data = io.BytesIO(data)
        return data
    pass

def fwver(dev_name):
    with open(res_file_path(dev_name, "version.txt"), "r") as f:
        lines = f.readlines()
        major = lines[0].split(" ")[-1].strip()
        minor = lines[1].split(" ")[-1].strip()
        debug = lines[2].split(" ")[-1].strip()
        return "{}.{}.{}".format(major, minor, debug)
    pass

def registers(dev_name, filelike=True):
    with open(res_file_path(dev_name, "registers.v"), "rb") as f:
        data = f.read()
        if filelike:
            data = io.BytesIO(data)
        return data
    pass

def getsome_generator(dev_name):
    def getsome(item, filelike=True):
        with open(res_file_path(dev_name, item), "rb") as f:
            data = f.read()
            if filelike:
                data = io.BytesIO(data)
            return data
    return getsome

def _extract_file(getsome, item):
    data = getsome(item).read()
    with open(item, "wb") as f:
        f.write(data)