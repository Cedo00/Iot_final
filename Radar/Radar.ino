#include <Servo.h>
#include <IRremote.h>

int RECV_PIN = 11;
IRrecv irrecv(RECV_PIN);

decode_results results;

// 创建一个舵机对象
Servo servo;

// 超声波模块引脚
const int pin_trig = 2;
const int pin_echo = 3;

// 初始化
int angle = 0;     // 角度
int distance = 0;  // 距离
int d_flag = 0;    // 旋转方向

void setup() {
    // put your setup code here, to run once:
    
    // 串口频率
    Serial.begin(9600);

    // 设置舵机引脚
    servo.attach(9);

    // 设置超声波模块引脚状态
    pinMode(pin_trig, OUTPUT);
    pinMode(pin_echo, INPUT);

    //初始化红外遥控
    irrecv.enableIRIn();
}

void loop() {
    // put your main code here, to run repeatedly:
    
    if (irrecv.decode(&results)) {
        irrecv.resume(); // 接收下一个值
        Serial.println(results.value);
    }
    
    if(results.value == 16724175) {
        // 设置舵机角度
        servo.write(181 - angle);
    
        // 超声波模块 测量距离 并 发送数据到串口
        UltrosonicWave();
        delay(60);
    
        // 判断方向 并 设置角度
        if (d_flag == 0) {
            angle ++;
            if (angle >= 180) {
              d_flag = 1;
            }
        }
        else {
            angle --;
            if (angle < 1) {
                d_flag = 0;
            }
        }
    }
    else {
    }
}

void UltrosonicWave() {
    // 产生一个10微妙的高脉冲去触发trig
    digitalWrite(pin_trig, LOW);
    delayMicroseconds(2);
    digitalWrite(pin_trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(pin_trig, LOW);

    // 检测脉冲宽度 并 计算距离
    distance = pulseIn(pin_echo, HIGH) / 58;

    // 输出到串口 
    Serial.print( "<" );
    Serial.print(angle);
    Serial.print("|");
    Serial.print(distance);
    Serial.print( ">" );
    Serial.println();
    delayMicroseconds(2); 
}

