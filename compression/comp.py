

class Colour:
    def __init__(self, r = 0, g = 0, b = 0):
        self.r = r
        self.g = g
        self.b = b
    
    def __sub__(self, other):
        return Colour(self.r - other.r, self.g - other.g, self.b - other.b)

    def __eq__(self, other):
        return (self.r == other.r) and (self.g == other.g) and (self.b == other.b)

    def __str__(self):
        return "({}, {}, {})".format(self.r, self.g, self.b)

    def key(self):
        return self.r * 3 + self.g * 5 + self.b * 7

# power - result
# 0 - 1
# 1 - 2
# 2 - 4
# 3 - 8
# 4 - 16
# 5 - 32
# 6 - 64
# 7 - 128


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

counter = 0
with open("images/imgascii.ollo", "rb") as file:
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
            with open("compression/img.qoi", "wb") as qoi:
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
                    if current == previous:
                        run_length += 1
                        if run_length == 62 or index == len(line)-3:
                            qoi.write((QOI_OP_RUN | (run_length-1)).to_bytes(1, "big"))
                            previous = current
                            run_length = 0
                    else:
                        if run_length > 0:
                            qoi.write((QOI_OP_RUN | (run_length-1)).to_bytes(1, "big"))
                            run_length = 0
                        key = current.key() % 64
                        if current == array[key]:
                            qoi.write((QOI_OP_INDEX | key).to_bytes(1, "big"))
                        else:
                            array[key] = current
                            diff = current - previous
                            dr_dg = diff.r - diff.g
                            db_dg = diff.b - diff.g

                            if ((-2 <= diff.r <= 1) and (-2 <= diff.g <= 1) and (-2 <= diff.b <= 1)):
                                qoi.write((QOI_OP_DIFF | ((diff.r + 2) << 4) | ((diff.g + 2) << 2) | (diff.b + 2)).to_bytes(1, "big"))
                            elif ((-32 <= diff.g <= 31) and (-8 <= dr_dg <= 7) and (-8 <= db_dg <= 7)):
                                qoi.write((QOI_OP_LUMA | (diff.g + 32)).to_bytes(1, "big"))
                                qoi.write(((dr_dg + 8) << 4 | (db_dg + 8)).to_bytes(1, "big"))
                            else:
                                qoi.write(QOI_OP_RGB.to_bytes(1, 'big'))
                                qoi.write(current.r.to_bytes(1, 'big'))
                                qoi.write(current.g.to_bytes(1, 'big'))
                                qoi.write(current.b.to_bytes(1, 'big'))
                    previous = current
                    index += 3
        counter += 1

