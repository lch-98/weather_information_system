from PIL import Image

def png_to_rgb565_array(filename, size=(50, 50)):
    img = Image.open(filename).convert('RGB')
    img = img.resize(size)  # 이미지 크기 조절
    width, height = img.size
    rgb565_array = []

    for y in range(height):
        for x in range(width):
            r, g, b = img.getpixel((x, y))
            rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
            rgb565_array.append(rgb565)
    return rgb565_array

arr = png_to_rgb565_array('/home/lee/pico-freertos/src/picow_lcd/snow.png')

with open('output_array.c', 'w') as f:
    f.write(f'const uint16_t snow_bitmap[50 * 50] = {{\n')
    for i, val in enumerate(arr):
        if i % 50 == 0:
            f.write('\n    ')
        f.write(f'0x{val:04X}, ')
    f.write('\n};\n')