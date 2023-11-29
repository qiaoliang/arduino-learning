# 简介

本项目是基于 ESP8266 开发板，使用 `ESP8266 Arduino Core` 库，并使用 `LittleFS` 库来存储数据。

# 硬件准备

## 1. ESP8266 开发板

### 1.1 ESP8266 开发板简介

ESP8266 是 Espressif Systems 公司开发的 Wi-Fi 模块，具有 4MB 的 Flash。

### 1.2 硬件连接

ESP8266 开发板与电脑通过 USB 连接。

# 软件准备

## 1. 下载并安装 `platformio`

打开终端，输入以下命令，下载并安装 `platformio`：

```
pip install -U platformio
```

## 2. 下载并安装 `ESP8266 Arduino Core`

打开终端，输入以下命令，下载并安装 `ESP8266 Arduino Core`：

```
platformio platform install espressif8266
```

# 项目的创建

## 1. 创建项目

打开终端，输入以下命令，创建项目：

```
platformio init --board uno
```

## 2. 项目结构

创建项目后，项目结构如下所示：

```
|-- platformio.ini
|-- src
|   |-- main.cpp
|-- data
|   └── wifi_config.json
```
其中，`platformio.ini` 是 `platformio` 的配置文件，`src/main.cpp` 是程序的入口文件，`data/wifi_config.json` 是 WiFi 配置文件。

# 程序的编写

## 1. main.cpp 的内容

在 `src/main.cpp` 中，我们编写如下程序：

```cpp
#include <Arduino.h>
#include <LittleFS.h>

void setup() {
    Serial.begin(115200);
    Serial.println("Hello World!");
}

void loop() {
}
```
在 `setup()` 函数中，我们使用 `Serial.begin()` 函数来设置串口通信的波特率为 115200，并打印 “Hello World!”。

在 `loop()` 函数中，我们什么都不做。

## 2. platformio.ini 的内容

在 `platformio.ini` 中，我们编写如下内容：

```ini
[env:uno]
platform = espressif8266
board = d1_mini
framework = arduino
board_build.filesystem = littlefs
;board_build.flash_mode = dio
lib_deps =
    bblanchon/ArduinoJson@^6.21.3

upload_protocol = esptool
```
其中，

`platform = espressif8266` 表示使用 `espressif8266` 平台，
`board = d1_mini` 表示使用 `d1_mini` 开发板，
`framework = arduino` 表示使用 `arduino` 框架，
`board_build.filesystem = littlefs` 表示使用 `littlefs` 文件系统，
`lib_deps = bblanchon/ArduinoJson@^6.21.3` 表示使用 `ArduinoJson` 库。

`upload_protocol = esptool` 表示使用 `esptool` 上传程序。

# 程序的编译和上传

在终端中，输入以下命令，编译并上传程序：

```
platformio run -t upload
```

# 程序的烧录

在终端中，输入以下命令，烧录程序：

```
platformio run -t uploadfs
```

# 程序的调试

烧录完成后，在终端中，输入以下命令，调试程序：

```
platformio device monitor -d
```

