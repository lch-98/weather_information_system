# 한글을 lcd에 띄위기 위해 get_bitmap()함수를 이용
# 해당 32바이트 한글 비트맵을 font5x7.h에 복사 붙여넣기 후 한글 사용
from PIL import Image, ImageFont, ImageDraw

def get_bitmap(char, font_path="/usr/share/fonts/truetype/nanum/NanumGothicCoding-Bold.ttf", size=16):
    font = ImageFont.truetype(font_path, size)
    image = Image.new("L", (size, size), 0)
    draw = ImageDraw.Draw(image)
    draw.text((0, 0), char, font=font, fill=255)

    # 흑백으로 변환
    threshold = 128
    image = image.point(lambda p: 255 if p > threshold else 0)

    bitmap = []
    for y in range(size):
        row = 0
        byte1 = 0
        byte2 = 0
        for x in range(8):
            if image.getpixel((x, y)) > 0:
                byte1 |= (1 << (7 - x))
        for x in range(8, 16):
            if image.getpixel((x, y)) > 0:
                byte2 |= (1 << (15 - x))
        bitmap.append(byte1)
        bitmap.append(byte2)

    return bitmap

# 문자 추출
bmp = get_bitmap("℃")

# 출력
for i in range(0, 32, 2):
    print(f"0x{bmp[i]:02X}, 0x{bmp[i+1]:02X},")
