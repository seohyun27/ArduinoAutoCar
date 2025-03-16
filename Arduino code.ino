#include <SoftwareSerial.h>
#include <AFMotor.h>
AF_DCMotor motor_L(1); //모터 드라이버 쉴드 변수 선언
AF_DCMotor motor_R(4);

//양쪽 모터의 출력 다를 수 있으므로 최대한 직진하도록 조정
int Lspeed = 164;            //좌측 모터 속도 164
int Rspeed = 200;            //우측 모터 속도 200

//초음파센서 출력핀(trig)과 입력핀(echo), 변수 선언
int TrigPin = A0;            
int EchoPin = A1;
long duration, distance;
int count = 0; //Obstacle_Check() 함수를 위한 변수

//loop를 돌리기 위한 함수들을 선언
void Obstacle_Check();
void Distance_Measurement();
void Forward();
void Backward();
void Right();
void Left();
void Stop();

//setup//
void setup() {
  Serial.begin(9600);
  Serial.println("Eduino Smart Car Start!");

  pinMode(EchoPin, INPUT);               // EchoPin 입력
  pinMode(TrigPin, OUTPUT);              // TrigPin 출력

  motor_L.setSpeed(Lspeed);              // 왼쪽 모터의 속도
  motor_L.run(RELEASE);
  motor_R.setSpeed(Rspeed);              // 오른쪽 모터의 속도
  motor_R.run(RELEASE);
}

//loop//
void loop() {
  Forward(); //직진
  delay(50);
  Obstacle_Check(); //장애물 확인 및 회피 방향 결정
}

//장애물을 확인하고 회피 방향을 결정하는 함수//
void Obstacle_Check() {
  Distance_Measurement();
  delay(50);
  
  Serial.println(distance);

  while (distance < 500) {
    //장애물과의 거리가 200 이하라면 후진 후 다시 거리 측정
    if (distance < 200) {
      Backward();
      delay(800);
      Stop();
      delay(400); //완전히 정지한 뒤 거리를 측정하도록 정지 시간을 늘림
      Distance_Measurement();
      delay(100);
    }
    //장애물과의 거리가 200 이상 500 이하라면
    //왼쪽과 오른쪽을 번갈아가며 순차적으로 거리 측정
    else {
      if(count == 0){
        Left();
        delay(950);
        count++;
      }
      else if(count == 1){
        Right();
        delay(670);
        count++;
      }
      else if(count == 2){
        Right();
        delay(670);
        count++;
      }
      else {
        Left();
        delay(950);
        count=0;
      }
      Stop(); //회전을 완전히 정지하고 다음 동작을 하도록 딜레이를 줌
      delay(400);
      Distance_Measurement();
      delay(100);
    }
  }
}

//장애물과의 거리를 측정하는 함수//
void Distance_Measurement() {
  digitalWrite(TrigPin, LOW);
  delay(2);
  digitalWrite(TrigPin, HIGH); //trigPin에서 초음파 발생(echoPin도 HIGH)
  delayMicroseconds(10);
  digitalWrite(TrigPin, LOW);
  duration = pulseIn(EchoPin, HIGH); //echoPin 이 HIGH를 유지한 시간을 저장
  //duration값과 거리 공식으로 구해진 결과를 distance 변수에 저장
  distance = ((float)(340 * duration) / 1000) / 2; 
  delay(50);
}

//모터를 제어하는 함수들//
//직진과 후진의 경우 왼쪽 모터와 오른쪽 모터를 동일하게 동작
void Forward() { //직진 함수//
  motor_L.run(FORWARD);  motor_R.run(FORWARD);
  motor_L.setSpeed(Lspeed);  motor_R.setSpeed(Rspeed);
}

void Backward() { //후진 함수//
  motor_L.run(BACKWARD);  motor_R.run(BACKWARD);
  motor_L.setSpeed(Lspeed);  motor_R.setSpeed(Rspeed);
}

void Right() { //오른쪽으로 회전하는 함수//
  //왼쪽 바퀴는 앞으로, 오른쪽 바퀴는 뒤로 회전
  motor_L.run(FORWARD);  motor_R.run(BACKWARD);
  //오른쪽 바퀴의 속도를 50퍼센트 줄여 부드러운 회전이 가능하도록 설정
  motor_L.setSpeed(Lspeed);  motor_R.setSpeed(Rspeed*0.5);
}

void Left() { //왼쪽으로 회전하는 함수//
  motor_L.run(BACKWARD);  motor_R.run(FORWARD);
  motor_L.setSpeed(Lspeed*0.5);  motor_R.setSpeed(Rspeed);
}

void Stop() { //정지 함수//
  //양쪽 모터를 모두 RELEASE 상태로 설정
  motor_L.run(RELEASE);       motor_R.run(RELEASE);
  //양쪽 모터의 속도를 0으로 설정
  motor_L.setSpeed(0);  motor_R.setSpeed(0);
}