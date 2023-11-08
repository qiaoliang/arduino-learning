// 车上左右红外测距接收器的引脚
#define INFRA_DISTANCE_DETECT_LEFT_PIN  11
#define INFRA_DISTANCE_DETECT_RIGHT_PIN 12

void ObsDetect_Init() {
  // 绑定红外测距接收器
  pinMode(INFRA_DISTANCE_DETECT_RIGHT_PIN, INPUT);
  pinMode(INFRA_DISTANCE_DETECT_LEFT_PIN, INPUT);
}

unsigned char Obs_detect() {
  unsigned char left= digitalRead(INFRA_DISTANCE_DETECT_LEFT_PIN);
  unsigned char right = digitalRead(INFRA_DISTANCE_DETECT_RIGHT_PIN) == HIGH);
  unsigned char result = 0;
  result |= left <<1;
  result |= right;
  return result;
}
