*语言：[English](README.md)，[简体中文](README.zh-CN.md)*

# Ch55xduino: 支持ch55x系列芯片的小型Arduino

**快速上手Ch55x的路子，Fork至Sduino项目且基于ch554_sdcc project**

Ch55xduino 是一个可在低成本MCS51内核且支持USB的CH55X系列MCU上使用的类Arduino API。该项目试图消除配置编译环境的困难。用户可以简单地在Arduino IDE中写代码，点击一下按钮就可以烧录芯片让代码跑起来，不需要配置和疑惑。

CH551、CH552、CH554可能是工作于Arduino上需要最少部件的系统。最小系统仅需要1个芯片, 2个去耦电容和1个可选的上拉电阻，这些特性让他非常适合DIY项目。

![Script running gif](https://raw.githubusercontent.com/DeqingSun/ch55xduino/ch55xduino/docs/blinkLED.gif)

这个项目目前仍在进行中，支持Arduino的大部分功能(除了pulse, shift, tone)。请参考本仓库中的示例了解更多信息。

## 安装

通过Arduino开发板管理器自动IDE集成。这是目前推荐的安装方式。

打开 Arduino-IDE. 在 *文件->首选项*, *设置* 选项卡, 最下面 *附加开发板管理器网址* 中粘贴索引地址：

> https://raw.githubusercontent.com/DeqingSun/ch55xduino/ch55xduino/package_ch55xduino_mcs51_index.json

* 打开 *工具->开发板:...->开发板管理器*
* 在搜索框输入 'ch' 找到 Ch55xduino
* 点击列表条目
* 点击 *安装*

现在你可以在 *工具->开发板:...* 列表中找到 *CH55x Boards* 

* 从列表中选择 *CH552 Board* 
* 打开标准的Blink示例 *文件->示例->01. Basics->Blink*
* 在Blink修改灯的引脚，如果你的LED接在引脚 P3_0, 你要写引脚数字 30
* 点击✓ *验证* 编译
* 如果你的板子从未使用过ch55xduino，则需要先使ch55x芯片进入bootloader。先断开USB让芯片断电，然后在USB接口的DP引脚上连接上拉电阻(一般是用10K电阻连接DP引脚和5V电源，用一个按钮或相邻的焊盘控制通断)，连接USB上电即可进入bootloader，然后点击→ *上传*。如果是新的芯片则一上电就会进入bootloader，不需要上面的操作。
* 如果你使用过一次ch55xduino，并且你的代码没有破坏USB下载系统，那么你可以简单地按下Upload，Arduino和固件会自动让芯片进入bootloader。

### 不通过Github安装

如果你不能直接访问github，使用这个代理链接代替上面的URL。

> https://gh-proxy.deqing.workers.dev/raw.githubusercontent.com/DeqingSun/ch55xduino/playground/mirror/package_ch55xduino_mcs51_proxy_index.json

### USB和串口上传

Ch55xduino 支持USB和串口上传。如果CH55x芯片的USB接口与电脑相连，建议使用USB。

也可以通过CH55x芯片(CH551除外)的UART1串口上传。要使CH55x自动进入bootloader，可以将串口适配器的RTS或DTR用电容连接到CH55x的中断引脚上，并在中断处理程序中添加bootloader跳转代码。

如果你想要退出bootloader，你可以以57600波特率发送以下字节：57 AB A2 01 00 01 A4

### Windows驱动

从0.0.10开始，如果你的Windows自动安装了来自wch.cn的bootloader(4348,55e0)的驱动程序最好了。当前的上传工具可以使用默认的CH375驱动，和官方的[WCHISPTool](http://www.wch.cn/downloads/WCHISPTool_Setup_exe.html)共存。

如果你需要使用WinUSB或libusb-win32，这个工具仍然可以工作。

你可以为默认的CDC USB协议栈(1209,C550)使用USB串口(CDC)驱动程序。如果驱动程序没有自动安装，请使用 [Zadig](https://zadig.akeo.ie/) 来安装。

![Zadig CDC image](https://raw.githubusercontent.com/DeqingSun/ch55xduino/ch55xduino/docs/Zadig_CDC.png)

如果你试图模拟另一种类型的USB设备而不更改PID/VID，则可能需要在安装新驱动程序之前卸载该设备。

### Linux的权限

默认情况下，Linux不会为Arduino提供足够的权限来上传带有USB bootloader的代码。 复制文件 ```99-ch55xbl.rules``` 到 ```/etc/udev/rules.d/``` 然后重启电脑。不然的话上传工具可能找不到bootloader设备。

## 开发板参考

![Front image](https://raw.githubusercontent.com/DeqingSun/ch55xduino/ch55xduino/docs/simpleCH552Front.jpg)

![Back image](https://raw.githubusercontent.com/DeqingSun/ch55xduino/ch55xduino/docs/simpleCH552Back.jpg)

在“pcb”文件夹中有一个小的CH552开发板设计。用1.6mm板厚制作PCB对于一些USB插口来说可能太松，需要在USB接口后面添加一些胶带来增加厚度。

按钮的封装是为6毫米的按键设计的，但5毫米的按键也可以。

## 与常规Arduino的区别

### Pin的命名:

常规Arduino使用连续数字来编码AVR芯片上的引脚，但是MCS51内核使用数字作为端口名称，所以CH55xduino的引脚使用以下规则

```PortNumber*10+PinNumber```

例如： P1.1 是 11, P3.2 是 32.

### 模拟输入:

CH552有一个8位5通道的ADC，在引脚P1.1、P1.4、P1.5和P3.2上。因此输入范围是0~255，不是1023。

默认情况下，MCS51微控制器上的所有引脚都启用了内部上拉电阻。你可能需要使用 ```pinMode```将引脚设置为```INPUT```来禁用上拉电阻。

没有模拟引脚定义成如A0，需使用11、14、15或32作为模拟输入功能。

### 没有多态函数:

MCS51芯片没有免费的c++编译器，我们不能使用多态函数。但是，SDCC从13402的提交开始支持泛型选择，Ch55xduino从0.0.11开始支持泛型选择。

如果你使用的是高于0.0.11的版本，print函数可以根据参数的类型选择一个函数。

举个例子，如果你要用USB-CDC虚拟串口打印，可以这样做:

```
USBSerial_print(val);	//val: the value to print - any data type
USBSerial_print(val, format)	//specifies the number base (for integral data types) or number of decimal places (for floating point types)
USBSerial_print(charPointer, length)	//specifies the string length to be printed 
```

使用``` USBSerial_println ```也可以做到。如果要打印到Serial0或Serial1，只需使用``` Serial0_print ```或``` Serial1_print ```。

请注意，如果你传递的字符是单引号，例如```USBSerial_print(',');```，你会得到```44```，因为那个char被提升为int类型。你需要使用```USBSerial_print((char)',');```或```USBSerial_print(",");```。

它们定义在```genericPrintSelection.h```。

### 内存模式:

与包括AVR在内的大多数现代架构不同，MCS51有2个RAM区域，内部内存和外部内存。CH552内部内存只有256字节，外部内存有1024字节。

0.0.17版本开始，CH55xduino使用Large模式用于SDCC内存模型。默认情况下，Large模式将在外部RAM中分配所有变量。存储在内部RAM中的变量必须用```__data```关键字声明。

CH55xduino将堆栈放在内部RAM中，所以留给变量的空间不多。如果你的变量确实需要快速访问，在声明它时使用```__data```。 

对于默认的Arduino设置，为USB端点保留148字节，外部RAM将有876字节可用。

你可以通过打开与十六进制文件一起生成的map和mem文件来查看内存映射。

#### 使用SDCC时的常见陷阱：

请注意，当从中断服务程序调用某个函数时，它应该在#pragma NOOVERLAY之前(如果它不可重入)。这里需要特别注意的是，int(16位)和long(32位)整数除法、乘法和取模操作是使用ANSI-C开发的外部支持例程实现的，如果中断服务程序需要执行这些操作中的任何一个，那么支持程序(如以下章节所述)将不得不使用——stack-auto选项重新编译，源文件将需要使用——int-long-rent compiler选项编译。[src](http://fivedots.coe.psu.ac.th/~cj/masd/resources/sdcc-doc/SDCCUdoc-11.html)

使用SDCC时中断服务例程函数原型必须放置在包含main()的文件中，以便将中断向量放置在中断向量空间中。将函数原型放在头文件中是可以接受的，只要该头文件包含在包含main()的文件中。如果不这样做，SDCC将不会产生任何警告或错误，但是向量将不会在适当的位置，因此当中断发生时ISR将不会被执行。[src](https://www.silabs.com/community/mcu/8-bit/knowledge-base.entry.html/2007/11/16/common_pitfalls_when-E7zi)

### 复位引脚：

不像AVR芯片一样，当RST引脚为高时CH55x将复位。复位引脚可以配置为输入引脚。但是这样的配置需要修改芯片的配置字节，这需要一个外部工具[WCHISPTool](http://www.wch.cn/downloads/WCHISPTool_Setup_exe.html)来完成。

## 已知问题

.

## 包含库：

Arduino核心系统的大部分和一些Arduino库已经移植到c语法。生成的API仍然很接近c++版本，并且移植现有的应用程序并不困难。请参考库附带的示例。

#### 通信：

* SPI: 真硬件SPI达到12MHz.

* SoftI2C: Bit-Bang的软件I2C，可设置为任意2个引脚。

* WS2812: Bit-Bang的WS2812驱动，可设置为任意引脚。注意一些兼容的WS2812 LED有不同的时序可能需要一些调整。

#### 传感器：

* TouchKey: 内部6通道电容触摸模块，封装了自适应基线算法。

## Arduino世界的兼容性：

由于MCS51没有免费的c++编译器，因此不可能像STM32和ESP8266那样对整个环境实现完整的1:1端口。

这不是AVR的直接替代品，但多亏一些C预处理器的魔法使得编程API仍然非常非常相似，并且通常只需移动到类实例化语句的开始括号上，并将方法调用中的点替换为下划线就足够了。请查看[移植指南](https://tenbaht.github.io/sduino/api/migration/)获得概述。



## 支持系统:

Arduino IDE 版本 1.8.12 测试过, 但大多数 >=1.6.6 应该可以使用。

* Windows: 在 Windows 7 and XP 上测试过。

* MacOS: 在 10.14 上测试过。

* Linux:  在 Ubuntu 20.04 LTS 上测试过。
