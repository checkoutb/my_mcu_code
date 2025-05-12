# my_mcu_code

stm32f103c8t6

只是了解下，有些都没有写过代码

stm32, bilibili, 江协科技。  @Joplin/stm32

esp01s, @Joplin/IoT-Network/esp01s

FreeRTOS

HAL

lvgl

---

win10: keil5 community

linux没用过，因为烧录工具都是win的。 有个OpenOCD，但没试过。 买的是 PowerWriter 来烧录的，不知道 OpenOCD是否支持。 查了下，应该不支持。
还有，之前试过 `dnf install stlink`, `st-info --descr` 找不到 pwlink



---

my_include中 my开头的是自己照视频写的， 其他的都是直接复制的代码

启动stm32的库:  backup/stm32_include
- startup_stm32f10x_md.s
- system_stm32f10x.h
- system_stm32f10x.c
- stm32f10x.h
- stm32f10x_it.h
- stm32f10x_it.c
- stm32f10x_conf.h

---

stm32标准库：
/mnt/239G/z_my_backup/stm32/STM32F10x_StdPeriph_Lib_V3.6.0/Libraries/STM32F10x_StdPeriph_Driver/

---

keil5的core库
/run/media/asdf/SSD/w/sf/keil_pack/ARM/CMSIS/6.1.0/CMSIS/Core/Include/

core_cm3.h

---

keil5的工程中只需要添加.c
.h 是配置的include path

---

keil5 没有linux版，所以只复制了 main.c。

---

esp01s 有点坑。默认安装的是 1.7.4的， 网上的文档全是2.0开始的。 发那个 显示支持的命令的 命令，返回ERROR，应该是不支持这个命令。我都不知道 1.7.4能干什么。

然后更新固件，使用官方的github的 action中的固件，更新成功，但是 发AT命令给esp01s，没回应。
最后使用了 安信可 的 mqtt固件。 可以的。

---


