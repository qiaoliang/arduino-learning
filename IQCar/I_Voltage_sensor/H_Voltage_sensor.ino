// 板子上的LED可亮，但VS不工作。
// 还不能工作，不知道VS接的对不对。
#define VOLT_SENSOR A5
#define LED_PIN 13
void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(VOLT_SENSOR, INPUT);
}

void loop() {
    digitalWrite(LED_PIN, LOW);
    delay(1000);
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    int val;

    float temp;

    val=analogRead(VOLT_SENSOR);

    temp=val/40.92;  

    val=(int)temp;

    Serial.println(val);

    delay(1000);
}
