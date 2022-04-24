from PIL import Image

img = Image.open("images/trioupped.png")
up = 3
a = (up-1) // 2
factor = 1/ (up * up)
pixels = img.load()

new_img = Image.new("RGB", (img.width // up, img.height // up))
new_pixels = new_img.load()

for y in range(0, img.height, up):
    for x in range(0, img.width, up):
        r = 0
        g = 0
        b = 0
        for dy in range(up):
            for dx in range(up):
                r += pixels[x+dx, y+dy][0] * factor
                g += pixels[x+dx, y+dy][1] * factor
                b += pixels[x+dx, y+dy][2] * factor
        new_pixels[x//up, y//up] = (int(r), int(g), int(b))

new_img.save("images/triodown.png")