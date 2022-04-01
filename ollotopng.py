import sys
from PIL import Image

'''
.ollo format
Blank Lines or Lines starting with # are ignored

Headers identified by # e.g.
# File Author: SaiMonYo
# Date Created: 31/03/2022
...

Next Two Lines indicate Width and Height in that order e.g.
1280
720

Rest of File is R G B values seperated by spaces e.g.
255 255 255
80 80 80
0 0 0
0 155 0
...


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

lines = []
size = []
counter = 0
with open(file_name) as file:
    for i, line in enumerate(file):
        if line != "" and line != "#" and line != "\n":
            if counter < 2:
                size.append(int(line))
            else:
                lines.append(line.replace("\n", "").split(" "))
            counter += 1

img = Image.new("RGB", ((size[0]), (size[1])))
pixels = img.load()
for i, (r, g, b) in enumerate(lines):
    x = i % size[0]
    y = i // size[0]
    pixels[x, y] = (int(float(r)), int(float(g)), int(float(b)), 255)
print(file_name_save)
img.save(file_name_save)
