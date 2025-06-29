## 🧵 Raspberry Pi Pico W + 실시간 기상청 Weather API

이 저장소는 **Raspberry Pi Pico W**에서 **기상청 Weather API**를 활용해 **실시간 기상 정보**를 받아와 LCD에 표시하는 1인 프로젝트입니다.

(`FreeRTOS, SPI, Http, Wifi 등`)의 기술을 **실습**해 볼 수 있습니다.
<br><br>

## 🎬 실행 결과 (블로그 영상)

[![실행 영상 썸네일](https://github.com/user-attachments/assets/1e8f200f-3686-469e-a427-e18fa90089ea)](https://blog.naver.com/dlcndgusgnss/223886415576)

> 📌 위 이미지를 클릭하면 블로그에 올린 실제 시연 영상으로 이동합니다.

<br>

## 🛠️ 개발 환경

- Ubuntu >= 20.04
- Raspberry Pi Pico W
- FreeRTOS Kernel
- Raspberry Pi Pico SDK
- CMake >= 3.13
- arm-none-eabi-gcc
- VScode
<br><br>

## 🧭 시스템 구성도

![다이어그램](https://github.com/user-attachments/assets/0e6f931c-bb53-48ee-bca4-3944df52af73)

> 📌 Raspberry Pi Pico W 기반 기상 정보 시스템의 전체 구조
<br>

## 🚀 빌드 및 실행 방법
```bash
# 1. 프로젝트 폴더 안에 Git clone (깃 복사) 및 서브 모듈 초기화 (pico-sdk, FreeRTOS-Kernel)
git clone https://github.com/lch-98/weather_information_system.git
cd weather_information_system
git submodule update --init --recursive
export PICO_SDK_PATH=$PWD/lib/pico-sdk

# 2. 빌드 디렉토리 생성
mkdir build
cd build

# 3. CMake 빌드 설정 (경로는 빌드 폴더에 위치)
cmake -DPICO_BOARD=pico_w ..

# 4. 컴파일 (경로는 빌드 폴더에 위치)
make

# 5. 생성된 .uf2 파일을 Pico W에 복사
```
