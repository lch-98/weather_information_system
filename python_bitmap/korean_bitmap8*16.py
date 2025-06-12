from PIL import Image, ImageFont, ImageDraw

def get_bitmap_8x16(char, font_path="/usr/share/fonts/truetype/nanum/NanumGothicCoding-Bold.ttf", size=16):
    font = ImageFont.truetype(font_path, size)
    image = Image.new("L", (size, size), 0)
    draw = ImageDraw.Draw(image)
    draw.text((0, 0), char, font=font, fill=255)

    # 흑백 변환
    threshold = 128
    image = image.point(lambda p: 255 if p > threshold else 0)

    bitmap = []
    for y in range(size):
        byte = 0
        for x in range(8):  # 8픽셀만 사용
            if image.getpixel((x, y)) > 0:
                byte |= (1 << (7 - x))  # 상위 8비트에만 데이터 채우기
        bitmap.append(byte)

    return bitmap

# 문자 추출
bmp = get_bitmap_8x16('5')

# 출력: 1바이트씩, 16줄 = 총 16바이트
for b in bmp:
    print(f"0x{b:02X},")
