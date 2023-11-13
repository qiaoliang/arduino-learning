#include "ultraSound.h"
#include "myMoto.h"

float last_dist =0.0;

extern Rover* rover;
float UltSound_detect() {

  digitalWrite(ULTRASOUND_TRIG_PIN, LOW);
  delayMicroseconds(2);  // 在将Trigger 端置为高电平之前,需要确保Echo端已经处于低电平稳定状态，所以等要待2us.
  digitalWrite(ULTRASOUND_TRIG_PIN, HIGH); // 将Triger 端置高电平,发送超声波,
  delayMicroseconds(10); // 等待10us,让超声波发出
  digitalWrite(ULTRASOUND_TRIG_PIN, LOW);                        // 将Trig端置低,结束发送。
  float distance = pulseIn(ULTRASOUND_ECHO_PIN, HIGH) / 29.033/2;  // pulseIn 测量Echo端接受到高电平信号的时间。超声波每厘米需要29.033us。因为是往返时间，所以计算时要除 2。结果的单位是厘米
  delay(10);             // 给传感器一些恢复时间。传感部分的传感器和电路需要一些时间来恢复到准备状态。
  return distance;
}

void UltraSound_Init() {
  pinMode(ULTRASOUND_TRIG_PIN, OUTPUT);
  pinMode(ULTRASOUND_ECHO_PIN, INPUT);
}


void US_DebugInfo(){
  float curr_dist = UltSound_detect();
  if (curr_dist > 1000) // 这个超声波器件最远测距是 10米以内。超过 10 米就不打印输出了
  {
    return;
  }
  if (abs(curr_dist - last_dist) >= 12)
  { // 为了减少输出，只有当两次距离差大小1时，才通过 USB 串口发回测出的距离
    Serial.print("超声波:");
    Serial.println(curr_dist);
    last_dist = curr_dist;
  }
}

/**
 * @brief 超声波魔法手. 把手放在 超声波前 6 ~ 12 cm之间, 小车会跟着手走.
 *        如果小于 6 cm , 小车后退.
 *        如可大于 12 cm, 小车停止.
 *
 */
void UltraSound_Following_enable()
{
  float distance = UltSound_detect();

  if (distance < 10)
  {
    rover->back();
  }
  else if (distance >= 10 && distance <= 30)
  {
    rover->forward();
  }
  else if (distance > 30)
  {
    rover->stop();
  }
}