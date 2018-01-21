import processing.serial.*;

// 实例化一个串口对象
Serial my_port;
// 用于接受串口数据的变量
int my_value;

// 画布基本布局
int m_width = 600;   // 画布宽度
int m_height = 300;  // 画布高度
int m_width_half = m_width / 2;
int m_height_half = m_height / 2;
int m_block = 20;    // 分区大小
int m_block_num = m_width / m_block;  // 分区数量

// 最大,最小测量距离
int m_max_distance = 100;
int m_min_distance = 5;

// 基本数据初始化
int m_angle = 0;     // 角度
int m_distance = 0;  // 距离
int temp_angle = 0;  // 用于角度读取出错时

// 所有角度上障碍物的距离
int[] m_distance_arr = new int[181];


// 初始化设置
void setup() {
    // 设置画布大小
    size(600, 300);
    
    // 设置画面刷新速度 fps
    frameRate(60);
    
    // 得到串口的名字
    String port_name = Serial.list()[1];  // com*
    
    // 设置串口属性
    my_port = new Serial(this, port_name, 9600);
}


// 循环扫描作图
void draw() {
    // 得到数据 并 储存到数组中
    getData();
    
    // 画出背景
    drawBackGround();
    
    // 画出扫描线
    drawScanLine();
    
    // 画出障碍物
    drawBarriers();
}


// 得到数据 并 储存到数组中
void getData() {
    if (my_port.available() > 0) {
        my_value = my_port.read();
        
        if (my_value == '<') {
            m_angle = 0;
            m_distance = 0;
            
            // 读取角度数据
            while ((my_value = my_port.read()) != '|') {
                m_angle = 10 * m_angle + (my_value - '0');
                
                // 角度有误就设置为90度
                if (m_angle < 0 || m_angle > 180) {
                    m_angle = 90;
                }
            }
            
            // 读取距离数据
            while ((my_value = my_port.read()) != '>') {
                m_distance = 10 * m_distance + (my_value - '0');
            }
            
            // 储存数据到数组中
            if (m_distance > m_max_distance || m_distance < m_min_distance) {
                // 距离超过最大距离或不到最小距离则放弃该数据
                m_distance_arr[m_angle] = -1;
            }
            else {
                m_distance_arr[m_angle] = m_distance;
            }
            m_distance_arr[90] = -1;
        }
    }
}

// 画出背景
void drawBackGround() {
    // 初始化画布背景为黑色
    background(0);
    
    // 区块分割
    for (int i=0; i < m_block_num; i++) {
        // 设置分割线条颜色
        stroke(20, 100, 20, 100);
    
        // 画出横向分割线
        line(0, i * m_block, m_width, i * m_block);
        // 画出纵向分割线
        line(i * m_block, 0, i * m_block, m_height);
        
        // 大区块线条颜色加深
        if ((i % 5) == 0) {
            stroke(20, 150, 20, 150);
            line(0, i * m_block, m_width, i * m_block);
            line(i * m_block, 0, i * m_block, m_height);
        }
    }
    
    // 弧形扫描区域
    // 轴心
    stroke(20, 150, 20, 150);
    fill(20, 150, 20, 150);
    arc(m_width_half, m_height, m_block, m_block, PI, PI * 2);
    // 内层弧
    stroke(20, 150, 20, 250);
    noFill();
    arc(m_width_half, m_height, m_height, m_height, PI, PI * 2);
    // 外层弧
    stroke(20, 150, 20, 200);
    noFill();
    arc(m_width_half, m_height, m_width, m_width, PI, PI * 2);
}


// 画出扫描线
void drawScanLine() {
    noStroke();
    
    if (m_angle == 90) {  // 角度数据有误
        fill(20, 150, 20, 80);
        arc(m_width_half, m_height, m_width, m_width, PI + (temp_angle / 181.0f - 0.1f) * PI, PI + (temp_angle / 181.0f + 0.1f ) * PI, PIE);  
    }
    else {  // 角度数据正确
        fill(50, 200, 50, 250);
        text("angle: ", 10, 30);
        text(m_angle, 70, 30);
        text("distance: ", 10, 50);
        text(m_distance, 70, 50);
        
        fill(20, 150, 20, 80);
        arc(m_width_half, m_height, m_width, m_width, PI + (m_angle / 181.0f - 0.1f) * PI, PI + (m_angle / 181.0f + 0.1f ) * PI, PIE);
        
        temp_angle = m_angle;
    }
}


// 画出障碍物
void drawBarriers() {
    noStroke();
    fill(150, 20, 20, 150);
    
    // 循环画出所有角度上的障碍物
    for (int i_angle=0; i_angle < 180; i_angle ++) {
        int distance = m_distance_arr[i_angle];
        
        // 放弃不再最大、最小距离区间的数据
        if (distance > m_max_distance || distance < m_min_distance) {
            continue;
        }
        
        if (distance > 70) {
            fill(120, 20, 20, 150);
        }
        else if (distance > 50) {
            fill(160, 20, 20, 200);
        }
        else {
            fill(200, 20, 20, 250);
        }
        
        // 设置障碍物坐标
        distance *= (m_width_half / m_max_distance);
        int point_x = (int)(m_width_half - distance * cos(i_angle * PI/180));
        int point_y = m_height - (int)(distance * sin(i_angle * PI/180));
        
        println("angle: " + i_angle + " distance: " + distance/2 + " x: " + point_x + " y: " + point_y);
        
        // 画出障碍物
        ellipse(point_x, point_y, 20, 20);
    }
}