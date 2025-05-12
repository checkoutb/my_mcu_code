#include "stm32f10x.h"                  // Device header

#include "OLED.h"
#include "Serial.h"
#include "my_delay.h"


// 波特率要 115200.. 直接复制的代码，里面是9600. 连不了。。
// 为了不影响 库的代码，直接复制了init

// 能收到信息，但是怎么是个 16进制的A?  应该Ok 啊
// 0xA 是 换行键  似乎是 \r ?
// 难道是delay的问题? 串口是异步的， 对面可不管我 是不是停了。
// 不，还是 0xA

// AT 返回 0a
// AT+GMR 返回 4b 0a 33 0a   。。 K \r 3 \r    ..wtf?
// AT+CMD?   0a
// ATE1    0a
// AT+CWMODE?   0a 0a
// AT+CWSTATE? 0a

// 如果目标 AP 的 SSID 是 "abc"，密码是 "0123456789"，则命令是：
//AT+CWJAP="ChineNet-xxxx","0123456789"    // 6e   n    可以，登路由器，可以看到 esp连上了。

// AT+CWJAP   0A   连过一次后，不需要 wifi名 密码， 也可以连。 路由器可以看到它


// 还有，不知道为什么下面 一个 AF_PP 一个 IPU。  改成 都是 AF_PP， oled没有第三行，多reset几次，会出现 FF 96 这种 非ascii码。

// ..似乎是打印的问题。

// 1. 串口异步，所以 原先的库肯定不行。 只能 在 中断处理中 保存数据。  原先的库 ，等到去访问的时候，只剩最后一个  \r 了。
// 2. 应该是 我忘记 clear OLED 导致 第二遍的时候 显示不正确?  。我已经改成 char了， 暂时没有 第二遍。
//      应该也不是， 因为 我第一次成功是 使用了OLED_ShowHexNum(3, 1, arr[i++], 16);   而不是 二个位置。
// 还有就是 这个 库的 OLED 打印O 似乎有问题。 我的问题，我从 0-15， 应该是 1-16
// 1-16 一下子ok 了， 我还在想 第一个char为什么是T 是什么意思?  1-16后 就是 AT 。。 回显了。
// 但是我只发了一个 AT 没让它联网啊， 自动连了。。 登录 路由器，确实 自动连接了。


// 挺奇怪的， 切断电源 的重启 它会输出3次，但是 我这里应该是一次啊。 就是 AT  ok ;  wifi connected ;  got ip  3个屏幕。
// 按 开发板的 reboot， 只有一个 AT  ok
// 但是之前 是 3个语句 一个屏幕。


void Serial_Init_A(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   // AF_PP 不能用，不知道为什么
    // 参考手册(中文) 8.1.3 : 所有端口都有外部中断能力。为了使用外部中断线，端口必须配置成输入模式 ?? 但是还是很迷糊
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
    // USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
}

uint16_t arr[100] = {0};
int idx = 0;
void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		uint16_t Serial_RxData = USART_ReceiveData(USART1);
        // OLED_ShowHexNum(3, 1, Serial_RxData, 2);
        if (idx < 100)   // 需要一个 reset 功能，读完数据 把 idx=0
            arr[idx++] = Serial_RxData;
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}


void execute_at_command_op(char* cmd, char* op) {
    idx = 0;
    OLED_ShowString(1, 1, op);
    Serial_SendString(cmd);
    
    // 如何等待 esp01s 的返回? 网络可能耗时几秒钟。
    int wt = 1;
    while (idx == 0) {
        delay_ms(200);
        if (wt == 1) {
            OLED_ShowChar(1, 15, '.');
            wt = 0;
        } else {
            OLED_ShowChar(1, 15, ' ');
            wt = 1;
        }
    }
    
    for (int i = 0; i < idx; ) {
        
        OLED_Clear();
        OLED_ShowString(1, 1, op);
        for (int k = 2; k <= 4; ++k) {
            for (int j = 1; j <= 16 && i < idx; ++j, ++i) {
                if (arr[i] >= 32 && arr[i] <= 126)
                    OLED_ShowChar(k, j, (char) arr[i]);
                else
                    OLED_ShowChar(k, j, '`');
            }
        }
        delay_ms(5000);
    }
}



void execute_at_command_op_delay_ms(char* cmd, char* op, int delayms) {
    idx = 0;
    OLED_ShowString(1, 1, op);
    Serial_SendString(cmd);
    
    // 如何等待 esp01s 的返回? 网络可能耗时几秒钟。
    int wt = 1;
    while (idx == 0) {
        delay_ms(200);
        if (wt == 1) {
            OLED_ShowChar(1, 15, '.');
            wt = 0;
        } else {
            OLED_ShowChar(1, 15, ' ');
            wt = 1;
        }
    }
    
    for (int i = 0; i < idx; ) {
        
        OLED_Clear();
        OLED_ShowString(1, 1, op);
        for (int k = 2; k <= 4; ++k) {
            for (int j = 1; j <= 16 && i < idx; ++j, ++i) {
                if (arr[i] >= 32 && arr[i] <= 126)
                    OLED_ShowChar(k, j, (char) arr[i]);
                else
                    OLED_ShowChar(k, j, '`');
            }
        }
        delay_ms(delayms);
    }
}

void execute_at_command(char* cmd) {
    idx = 0;
    Serial_SendString(cmd);
    
    // 如何等待 esp01s 的返回? 网络可能耗时几秒钟。
    int wt = 1;
    while (idx == 0) {
        delay_ms(200);
        if (wt == 1) {
            OLED_ShowChar(1, 15, '.');
            wt = 0;
        } else {
            OLED_ShowChar(1, 15, ' ');
            wt = 1;
        }
    }
    
    for (int i = 0; i < idx; ) {
        
        OLED_Clear();
        for (int k = 2; k <= 4; ++k) {
            for (int j = 1; j <= 16 && i < idx; ++j, ++i) {
                if (arr[i] >= 32 && arr[i] <= 126)
                    OLED_ShowChar(k, j, (char) arr[i]);
                else
                    OLED_ShowChar(k, j, '`');
            }
        }
        delay_ms(5000);
    }
}


// 买了一个烧录器+esp01s，所以先试下新的esp01s
int main() {
    OLED_Init();
    Serial_Init_A();
    
    // OLED_ShowString(1, 1, "Net");
    
    // OLED_ShowString(1, 5, "op1");
    execute_at_command_op("ATE0\r\n", "ate0");   // 每次都需要。不然就回显了。
    // execute_at_command("AT+GMR\r\n");
    
    
    // 烧录个最新的试试。
    
    // 只烧录了一个 安信可的 mqtt。。官方的 不行。 烧录成功 但是 没有任何响应。
    
    // 准备试下 mqtt发送数据，接收数据
    
    
    // https://www.emqx.com/zh/mqtt/public-mqtt5-broker
//Broker:
//broker.emqx.io
//TCP 端口:
//1883
//WebSocket 端口:
//8083
//SSL/TLS 端口:
//8883
//WebSocket Secure 端口:
//8084
//QUIC 端口:
//14567
//CA 证书文件:
//broker.emqx.io-ca.crt

// mqtt 有库。

    // OLED_ShowString(1, 5, "net");
    // 连wifi，ping mqtt server
    
    // execute_at_command_op("AT+CWMODE=1\r\n", "cwm");
    // execute_at_command("AT+CWJAP=\"ChinaNet-XXX\",\"qwert\"\r\n");  // 只有第一次需要，因为默认自动连接
    
    // execute_at_command("AT+PING=\"broker.emqx.io\"\r\n");    // cmd 也ping不了
    
    // OLED_ShowString(1, 5, "snd");
    
    execute_at_command_op("AT+PING=\"www.baidu.com\"\r\n", "pBaiDu"); 
    
    // AT+MQTTUSERCFG=0,1,"22033-ddd-33-s","aa","pwd",0,0,""
    execute_at_command_op("AT+MQTTUSERCFG=0,1,\"22033-ddd-33-s\",\"aa\",\"pwd\",0,0,\"\"\r\n", "mcfg");
    
    
    // AT+MQTTCONN=0,"broker.emqx.io",1883,0
    // wocao, 忘了 \r\n， 搞了半天。
    execute_at_command_op("AT+MQTTCONN=0,\"broker.emqx.io\",1883,0\r\n", "mCon");
//    execute_at_command_op("AT+MQTTCONN=0,\"44.232.241.40\",1883,0\r\n", "mCon2");
    
    
    
    
    // ...这个是订阅，所以必须 先订阅，然后发送，才会看到数据。
    // 而且 用 安可信串口助手 发送消息后， 串口助手可以 收到 刚发送的消息， 然后 浏览器的 在线 mqtt客户端 可以也看到 消息， 虽然 mqtt客户端连的是 8404.  
    // AT+MQTTSUB=0,"topic_tEsT_eeFF_44_33",0
    execute_at_command_op("AT+MQTTSUB=0,\"topic_tEsT_eeFF_44_33\",0\r\n", "mRcv");
    
    
    
    // AT+MQTTPUB=0,"topic_tEsT_eeFF_44_33","hello world!",0,0
    execute_at_command_op("AT+MQTTPUB=0,\"topic_tEsT_eeFF_44_33\",\"he--ccckkkkbb!\",0,0\r\n", "mPub");
    
    
    // OLED_ShowString(1, 5, "rcv");
    

    
    OLED_ShowString(1, 14, "end");
    while (1) { }
}

int main_001() {
    OLED_Init();
    Serial_Init_A();
    
    OLED_ShowString(1, 1, "Net");
    
    OLED_ShowString(1, 5, "op1");
    execute_at_command("ATE0\r\n");   // 每次都需要。不然就回显了。
    
    OLED_ShowString(1, 5, "op11");
    // execute_at_command("AT\r\n");
    
    OLED_ShowString(1, 5, "op12");
    // execute_at_command("AT+CWJAP=\"ChinaNet-XX\",\"qwert\"\r\n");  // ATE0 会自动连接wifi; 再已连接wifi的情况下，执行是 断开连接。
                    // 断开连接的情况下，执行下面的 AT+PING， 很快就 +timeout 了
    // .. disconnect后， ATE0 也不自动重连了。。 不是，账号密码错了, 所以 之前 断开了。
    
    
    OLED_Clear();  // ..最好不要删除 title。
    OLED_ShowString(1, 5, "op2");
//    execute_at_command("AT+PING=\"192.168.1.6\"");  // 为什么不行? 1.6是我的电脑  没有数据，一直卡在while上，也没有 超时。
//    execute_at_command("AT+PING=\"192.168.1.1\"\r\n");  // 不行

    // execute_at_command("AT+CIPSNTPTIME?\r\n");    // 没有连wifi 的情况下，1970-1-1  。。 有wifi 也是1970-1-1 ...
    
    // execute_at_command("AT+CMD\r\n");   // 返回 ERROR ...  带不带? 都是 ERROR  ... 好像没有这个命令。。 固件是1.7.4的， 官网最早选到 2.1.0.0， 里面没有 AT+CMD 
    // execute_at_command("AT+GMR\r\n");   // AT version 1.7.4.0     。。 github上就没有这个版本tag。 tag很乱， 2 3 4， 3个版本同时维护的。
                        // 这个怎么搞。。要刷固件啊。
                        
    // execute_at_command("AT+CIUPDATE\r\n");   // ERROR 估计没有这个命令。。 这个是 升级固件到最新版本
    
    
    // execute_at_command("AT+CWLAP\r\n");
    
    // execute_at_command("AT+CWLIF\r\n");  // error
    
    // execute_at_command("AT+CIFSR\r\n");   // ok 获得自己的ip
    
    // execute_at_command("AT+HTTPCLIENT=2,0,\"http://httpbin.org/get\",\"httpbin.org\",\"/get\",1\r\n"); // ERROR
    
    
    // g 等刷固件吧。得买。。
    
    // wtf? 本来keil都关了了，突然想到: 开回显 看下esp收到了什么命令。 结果 下面的ping 就可以了。。
    // 上面的 AT 还是要的，不然 一来就直接 ping， 连接都还没有建立。
    // wtf? 关回显， ping还是ok的   。。 是重启了keil了原因?  有点搞笑了。
    // execute_at_command("AT+PING=\"192.168.1.1\"\r\n");
    
    execute_at_command("AT+PING=\"www.baidu.com\"\r\n");  // ok
    
    
    // https://espressif-docs.readthedocs-hosted.com/projects/esp-at/zh-cn/release-v2.2.0.0_esp8266/AT_Command_Set/
    // , 都要转义，但是 没有这种转义啊。 我草， 这是 几个参数。 我一直当做一个string的，
    // 不过还是那个问题，  \,  没有这种转义啊，  难道 转义 可以对任意字符 转义? 我真不知道。
    // 不对，我通过串口就是 发 一个string 的啊。  要找个例子，到底怎么发的。
    // 难道 通过串口 分次发?  
    // 不，我上面 连wifi 就是 逗号分隔符 是 string的一个char啊。 wifi可以，说明 确实是这样用的。
    // 而且上面的网址中 给出的例子是 AT+CWJAP="comma\,backslash\\ssid","1234567890"   。 就是 内容中 是 , 那么要转义。
    // execute_at_command("AT+HTTPCLIENT=2\,0\,\"http://httpbin.org/get\"\,\"httpbin.org\"\,\"/get\"\,1\r\n");
    
    // Serial_SendString(cmd);
    
    
    // execute_at_command("AT+HTTPCLIENT=1,0,,\"httpbin.org\",\"/get\",1\r\n"); // error
    
    // execute_at_command("AT+HTTPGETSIZE=\"http://www.baidu.com/img/bdlogo.gif\"");  // 一直在等待，估计是因为 被重定向到 https了， esp估计没有 https 的能力。
    
    // ERROR
    execute_at_command("AT+HTTPCLIENT=2,0,\"http://httpbin.org/get\",\"httpbin.org\",\"/get\",1\r\n");
    
    // 还是刷固件吧， 1.7.4 文档都没有的。
    
    OLED_ShowString(1, 14, "end");
    
    
    while (1) {
    
    }
}

int main__ok() {

    for (int i = 0; i < 100; ++i) {
        arr[i] = '\0';
    }
    
    OLED_Init();
    Serial_Init_A();
    
    OLED_ShowString(1, 1, "esp01s");

    char* at = "ATE0\r\n";
    
    Serial_SendString(at);

    OLED_ShowString(1, 8, "01");
    
    // delay_ms(1000);
    
    if (Serial_GetRxFlag() == 1) {
        OLED_ShowString(1, 8, "02");
        uint8_t resp = Serial_GetRxData();
        OLED_ShowHexNum(3, 1, resp, 2);
        // delay_ms(1000);
        int idx = 3;
        while (Serial_GetRxFlag() == 1 && idx <= 14) {
            resp = Serial_GetRxData();
            OLED_ShowHexNum(3, idx, resp, 2);
            // delay_ms(1000);
            idx += 2;
        }
    } else {
        OLED_ShowString(1, 8, "E1");
    }
    
    
    delay_ms(1000);
    OLED_ShowNum(1, 8, idx, 3);
    
    for (int i = 0; i < idx; ) {
//        OLED_ShowHexNum(3, (i % 5) * 2, arr[i], 3);
//        if (i % 5 == 4) {
//            delay_ms(5000); 
//        }
//        OLED_ShowHexNum(3, 1, arr[i++], 16);
//        OLED_ShowHexNum(4, 1, arr[i], 16);
//        delay_ms(5000);
        
        for (int k = 2; k <= 4; ++k) {
            for (int j = 1; j <= 16 && i < idx; ++j, ++i) {
                if (arr[i] >= 32 && arr[i] <= 126)
                    OLED_ShowChar(k, j, (char) arr[i]);
                else
                    OLED_ShowChar(k, j, '`');
            }
        }
        delay_ms(5000);
    }
    
    
    OLED_ShowString(1, 14, "end");
//        
//    OLED_ShowString(2, 14, "end");
//        
//    OLED_ShowString(3, 14, "end");
//    
//    OLED_ShowString(3, 16, "O");
//    OLED_ShowString(2, 16, "O");
    
    while (1) {
        
    }
}