// 车上左右红外测距接收器的引脚
#define INFRA_DISTANCE_DETECT_LEFT_PIN  8
#define INFRA_DISTANCE_DETECT_RIGHT_PIN 9

void OBS_Init() {
  // 绑定红外测距接收器
  pinMode(INFRA_DISTANCE_DETECT_RIGHT_PIN, INPUT);
  pinMode(INFRA_DISTANCE_DETECT_LEFT_PIN, INPUT);
}
unsigned char last_obsdata,current_obsdata;
unsigned char OBS_Detect() {
  last_obsdata = current_obsdata;
  unsigned char left= digitalRead(INFRA_DISTANCE_DETECT_LEFT_PIN);
  unsigned char right = digitalRead(INFRA_DISTANCE_DETECT_RIGHT_PIN);
  current_obsdata = left+right;
  return current_obsdata;
}

void OBS_DebugInfo(){
  OBS_Detect();
  if(current_obsdata!= last_obsdata){
    Serial.print("OBS data: ");
    Serial.println(current_obsdata);
  }
}