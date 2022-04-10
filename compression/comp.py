from sys import argv
from PIL import Image

__version__ = '0.1'

class Colour:
    def __init__(self, r = 0, g = 0, b = 0):
        self.r = r
        self.g = g
        self.b = b

    def copy(self):
        return Colour(self.r, self.g, self.b)

    def key(self):
        return (self.r * 3 + self.g * 5 + self.b * 7) % 64
    
    def __sub__(self, other):
        return Colour(self.r - other.r, self.g - other.g, self.b - other.b)

    def __eq__(self, other):
        return (self.r == other.r) and (self.g == other.g) and (self.b == other.b)

    def __str__(self):
        return f"({self.r}, {self.g}, {self.b})"

help_commands = ["--help", "-h", "?", "help", "--version", "-v", "version"]
functions = ["buffer_to_qoi", "qoi_to_png", "png_to_qoi"]

# QOI byte headers
QOI_OP_RUN   = 0xc0
QOI_OP_INDEX = 0x00
QOI_OP_DIFF  = 0x40
QOI_OP_LUMA  = 0x80
QOI_OP_RGB   = 0xfe
QOI_OP_RGBA  = 0xff


def write32(f, value):
    f.write(((value & 0xff000000) >> 24).to_bytes(1, 'big'))
    f.write(((value & 0x00ff0000) >> 16).to_bytes(1, 'big'))
    f.write(((value & 0x0000ff00) >>  8).to_bytes(1, 'big'))
    f.write((value & 0x000000ff).to_bytes(1, 'big'))

def buffer_to_qoi(bufferfile_name, qoi_file_name):
    '''
    Converts a buffer file to a qoi file.
    Buffer file has the following format:
    width: on first line in integer form e.g. 2560
    height: on second line in integer form e.g. 1440
    a blank line
    Then a list of RGB values all in one line e.g.
    \\xff\\xff\\xff - represents a single white pixel (\xff\xff\xff in ascii)
    Should be 3 * width * height bytes.
    '''
    counter = 0
    with open(bufferfile_name, "rb") as file:
        for line in file:
            if counter == 0:
                width = int(line)
            elif counter == 1:
                height = int(line)
            elif counter == 3:
                index = 0
                array = [Colour() for _ in range(64)]
                previous = Colour()
                run_length = 0
                with open(qoi_file_name, "wb") as qoi:
                    # write file headers
                    write32(qoi, 0x716f6966)
                    write32(qoi, width)
                    write32(qoi, height)
                    qoi.write(b'\x03')
                    qoi.write(b'\x01')
                    while index < len(line)-2:
                        r = line[index]
                        g = line[index+1]
                        b = line[index+2]
                        current = Colour(r, g, b)
                        # run length encoding
                        if current == previous:
                            run_length += 1
                            if run_length == 62:
                                qoi.write((QOI_OP_RUN | (run_length-1)).to_bytes(1, 'big'))
                                previous = current
                                run_length = 0
                        else:
                            # write previous run
                            if run_length > 0:
                                qoi.write((QOI_OP_RUN | (run_length-1)).to_bytes(1, 'big'))
                                run_length = 0
                            # check if in lookup table
                            key = current.key()
                            if current == array[key]:
                                qoi.write((QOI_OP_INDEX | key).to_bytes(1, 'big'))
                            else:
                                # calculate differences
                                array[key] = current
                                diff = current - previous
                                dr_dg = diff.r - diff.g
                                db_dg = diff.b - diff.g
                                # small difference of -2 to 1 in each colour channel
                                if ((-2 <= diff.r <= 1) and (-2 <= diff.g <= 1) and (-2 <= diff.b <= 1)):
                                    qoi.write((QOI_OP_DIFF | ((diff.r + 2) << 4) | ((diff.g + 2) << 2) | (diff.b + 2)).to_bytes(1, 'big'))
                                # larger differences of -32 to 31 in green channel and -8 to 7 in red and blue channels
                                elif ((-32 <= diff.g <= 31) and (-8 <= dr_dg <= 7) and (-8 <= db_dg <= 7)):
                                    qoi.write((QOI_OP_LUMA | (diff.g + 32)).to_bytes(1, 'big'))
                                    qoi.write(((dr_dg + 8) << 4 | (db_dg + 8)).to_bytes(1, 'big'))
                                else:
                                    # no encoding possible for this colour
                                    qoi.write(QOI_OP_RGB.to_bytes(1, 'big'))
                                    qoi.write(current.r.to_bytes(1, 'big'))
                                    qoi.write(current.g.to_bytes(1, 'big'))
                                    qoi.write(current.b.to_bytes(1, 'big'))
                        previous = current
                        index += 3
            counter += 1


def onetotwoD(index, width):
    # return the 2D coordinates of the index in a 1D array
    return index % width, index // width

def qoi_to_png(qoi_image_file, png_image_file):
    '''
    Converts a qoi file to a png file.
    .qoi file must be in .qoi format.
    .qoi format specification can be found here:
        https://qoiformat.org/qoi-specification.pdf
    .png must follow the specifications of the PNG standard.
    '''
    with open(qoi_image_file, "rb") as qoi:
        # qoi magic number
        if qoi.read(4) != b"qoif":
            raise Exception("Incorrect start to .qoi file format")
        # read width, height, colour channels and colour space from qoi file
        width = int.from_bytes(qoi.read(4), 'big')
        height = int.from_bytes(qoi.read(4), 'big')
        channels = int.from_bytes(qoi.read(1), 'big')
        colour_space = int.from_bytes(qoi.read(1), 'big')
        # create and load pixels of the png image
        img = Image.new("RGB", (width, height))
        pixels = img.load()
        # the pixel index of the RGB values
        index = 0
        previous = Colour()
        # pixel array lookup, acts like hash table
        # index of colour in array is (Red channel * 3 + G channel * 5 + Blue channel * 7) % 64
        array = [Colour() for _ in range(64)]
        byte = qoi.read(1)
        while byte:
            integer = int.from_bytes(byte, 'big')
            # new RGB value
            if integer == QOI_OP_RGB:
                r = int.from_bytes(qoi.read(1), 'big')
                g = int.from_bytes(qoi.read(1), 'big')
                b = int.from_bytes(qoi.read(1), 'big')
                previous = Colour(r, g, b)
                pixels[onetotwoD(index, width)] = (r, g, b, 255)
                index += 1
                key = previous.key()
                array[key] = previous.copy()
            # run length encoded RGB value
            elif integer & 0xc0 == QOI_OP_RUN:
                run_length = (integer & 0x3f) + 1
                for _ in range(run_length):
                    pixels[onetotwoD(index, width)] = (previous.r, previous.g, previous.b, 255)
                    index += 1
            # previously seen RGB value saved in array
            elif integer & 0xc0 == QOI_OP_INDEX:
                key = integer & 0x3f
                colour = array[key]
                pixels[onetotwoD(index, width)] = (colour.r, colour.g, colour.b, 255)
                previous = colour.copy()
                index += 1
            # difference of pixel RGB values
            elif integer & 0xc0 == QOI_OP_DIFF:
                dr = ((integer & 0x30) >> 4) - 2
                dg = ((integer & 0x0c) >> 2) - 2
                db = (integer & 0x03) - 2
                previous.r = (previous.r + dr) & 0xff
                previous.g = (previous.g + dg) & 0xff
                previous.b = (previous.b + db) & 0xff
                pixels[onetotwoD(index, width)] = (previous.r, previous.g, previous.b, 255)
                key = previous.key()
                array[key] = previous.copy()
                index += 1
            # 2 byte difference of pixel RGB values
            elif integer & 0xc0 == QOI_OP_LUMA:
                dg = (integer & 0x3f) - 32
                byte = qoi.read(1)
                integer = int.from_bytes(byte, 'big')
                dr_dg = ((integer & 0xf0) >> 4) - 8
                db_dg = (integer & 0x0f) - 8
                dr = dr_dg + dg
                db = db_dg + dg
                previous.r = (previous.r + dr) & 0xff
                previous.g = (previous.g + dg) & 0xff
                previous.b = (previous.b + db) & 0xff
                pixels[onetotwoD(index, width)] = (previous.r, previous.g, previous.b, 255)
                key = previous.key()
                array[key] = previous.copy()
                index += 1
            byte = qoi.read(1)
    img.save(png_image_file)


def png_to_qoi(png_file_name, qoi_file_name):
    image = Image.open(png_file_name)
    pixels = image.load()
    width = image.size[0]
    height = image.size[1]
    with open(qoi_file_name, "wb") as qoi:
        array = [Colour() for _ in range(64)]
        previous = Colour()
        run_length = 0
        # write file headers
        write32(qoi, 0x716f6966)
        write32(qoi, width)
        write32(qoi, height)
        qoi.write(b'\x03')
        qoi.write(b'\x01')
        for y in range(height):
            for x in range(width):
                r, g, b = pixels[x, y]
                current = Colour(r, g, b)
                # run length encoding
                if current == previous:
                    run_length += 1
                    if run_length == 62:
                        qoi.write((QOI_OP_RUN | (run_length-1)).to_bytes(1, 'big'))
                        previous = current
                        run_length = 0
                else:
                    # write previous run
                    if run_length > 0:
                        qoi.write((QOI_OP_RUN | (run_length-1)).to_bytes(1, 'big'))
                        run_length = 0
                    # check if in lookup table
                    key = current.key()
                    if current == array[key]:
                        qoi.write((QOI_OP_INDEX | key).to_bytes(1, 'big'))
                    else:
                        # calculate differences
                        array[key] = current
                        diff = current - previous
                        dr_dg = diff.r - diff.g
                        db_dg = diff.b - diff.g
                        # small difference of -2 to 1 in each colour channel
                        if ((-2 <= diff.r <= 1) and (-2 <= diff.g <= 1) and (-2 <= diff.b <= 1)):
                            qoi.write((QOI_OP_DIFF | ((diff.r + 2) << 4) | ((diff.g + 2) << 2) | (diff.b + 2)).to_bytes(1, 'big'))
                        # larger differences of -32 to 31 in green channel and -8 to 7 in red and blue channels
                        elif ((-32 <= diff.g <= 31) and (-8 <= dr_dg <= 7) and (-8 <= db_dg <= 7)):
                            qoi.write((QOI_OP_LUMA | (diff.g + 32)).to_bytes(1, 'big'))
                            qoi.write(((dr_dg + 8) << 4 | (db_dg + 8)).to_bytes(1, 'big'))
                        else:
                            # no encoding possible for this colour
                            qoi.write(QOI_OP_RGB.to_bytes(1, 'big'))
                            qoi.write(current.r.to_bytes(1, 'big'))
                            qoi.write(current.g.to_bytes(1, 'big'))
                            qoi.write(current.b.to_bytes(1, 'big'))
                previous = current


def _help():
    print("Usages: ")
    print("       python3 comp.py <qoi_image_file> <png_image_file>     - convert qoi image to png")
    print("       python3 comp.py <qoi_image_file>                      - convert qoi image to png of same name")
    print("       python3 comp.py <png_image_file> <qoi_image_file>     - convert png image to qoi")
    print("       python3 comp.py <qoi_image_file>                      - convert png image to qoi of same name")
    print("       python3 comp.py <buffer_image_file> <qoi_image_file>  - convert buffer image to qoi")
    print("       python3 comp.py <buffer_image_file>                   - convert buffer image to qoi of same name")
    print("")
    print("To get specific help on a function (from list below) use: python3 comp.py --help <function_name>")
    print("       png_to_qoi")
    print("       qoi_to_png")
    print("       buffer_to_qoi")

def main():
    if len(argv) == 3:
        if argv[1] not in help_commands:
            if argv[1].endswith(".qoi"):
                qoi_to_png(argv[1], argv[2])
            elif argv[1].endswith(".png"):
                png_to_qoi(argv[1], argv[2])
            elif argv[2].endswith(".qoi"):
                buffer_to_qoi(argv[1], argv[2])
            exit(0)
        else:
            if argv[2] == "png_to_qoi":
                print(png_to_qoi.__doc__)
            elif argv[2] == "qoi_to_png":
                print(qoi_to_png.__doc__)
            elif argv[2] == "buffer_to_qoi":
                print(buffer_to_qoi.__doc__)
            exit(0)
    if len(argv) == 2 and argv[1] not in help_commands:
        if argv[1].endswith(".qoi"):
            qoi_to_png(argv[1], argv[1][:-4] + ".png")
        elif argv[1].endswith(".png"):
            png_to_qoi(argv[1], argv[1][:-4] + ".qoi")
        elif "." in argv[1]:
            buffer_to_qoi(argv[1], argv[1][:-4] + ".qoi")
        exit(0)
    if argv[1] == "--help":
        _help()
    elif argv[1] == "--version":
        print(f"    version: {__version__}")
        print("    Currently only supports non-alpha channel files (To be implemented soon)")
    exit(0)

if __name__ == "__main__":
    main()