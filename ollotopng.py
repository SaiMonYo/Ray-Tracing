import sys
from PIL import Image

'''
.ollo format
Blank Lines or Lines are ignored

First Two Lines indicate Width and Height in that order e.g.
1280
720

Rest of File is R G B values in one line, one after another etc.
RGBRGBRGB...
'''


if len(sys.argv) == 2:
    if ".ollo" not in sys.argv[1]:
        raise Exception("File must be in .ollo format")
    file_name = sys.argv[1]
    file_name_save = file_name[:file_name.index(".")] + ".png"
elif len(sys.argv) == 3:
    file_name = sys.argv[1]
    file_name_save = sys.argv[2]
    if ".png" not in file_name_save and ".jpg" not in file_name_save and ".jpeg" not in file_name_save:
        raise Exception("Saving file must be in .png/jpg/jpeg format")
else:
    raise Exception("Incorrect number of arguments")

values = []
size = []
counter = 0
with open(file_name, "rb") as file:
    for line in file:
        if counter == 0:
            size.append(int(line))
        elif counter == 1:
            size.append(int(line))
        else:
            for i, c in enumerate(line):
                values.append(c)
        counter += 1

img = Image.new("RGB", ((size[0]), (size[1])))
pixels = img.load()
i = 0
while i < len(values)-2:
    r = values[i]
    g = values[i+1]
    b = values[i+2]
    x = (i//3) % size[0]
    y = (i//3) // size[0]
    i += 3
    pixels[x, y] = (r, g, b, 255)
print(file_name_save)
img.save(file_name_save)
