
#define pwm0_pin PB1
#define pwm1_pin PB15
#define pwm2_pin PB14
#define pwm3_pin PB13

#define pwm1_input_pin PB5
#define pwm2_input_pin PB3
#define pwm3_input_pin PA10

#include <Servo.h>

Servo motor_out[4];
int motor_pwm[4];

double Kp = 1;
double Ki = 1;
double Kd = 1;

int vx = 0;
int vy = 0;
int va = 0;

int motor_offset[4];
int pwm_in[3];

void initParameter();
void serialPrintf(const char *fmt, ...);

volatile int pwm_value = 0;
volatile int prev_time = 0;

void setup()
{
  Serial.begin(9600);
  initParameter();
  motor_out[0].attach(pwm0_pin);
  motor_out[1].attach(pwm1_pin);
  motor_out[2].attach(pwm2_pin);
  motor_out[3].attach(pwm3_pin);

  for (int i = 0; i < 4; i++)
    motor_out[0].writeMicroseconds(1500);

  
  pinMode(pwm1_input_pin, INPUT);
  pinMode(pwm2_input_pin, INPUT);
  pinMode(pwm3_input_pin, INPUT);
}

void loop()
{
  pwm_in[0] = pulseIn(pwm1_input_pin, HIGH, 25000);
  pwm_in[1] = pulseIn(pwm2_input_pin, HIGH, 25000);
  pwm_in[2] = pulseIn(pwm3_input_pin, HIGH, 25000);

  if (Serial.available())
  {
    char temp = Serial.read();
    if (temp == 'w')
    {
      vy = 0;
      vx = 100;
      va = 0;
    }else if (temp == 's')
    {
      vy = 0;
      vx = -100;
      va = 0;
    }
    else if (temp == 'd')
    {
      vy = -100;
      vx = 0;
      va = 0;
    }else if (temp == 'a')
    {
      vy = 100;
      vx = 0;
      va = 0;
    }
    else if (temp == 'q')
    {
      vy = 0;
      vx = 0;
      va = -100;
    }else if (temp == 'e')
    {
      vy = 0;
      vx = 0;
      va = 100;
    }
    else if (temp == 'z')
    {
      vy = 0;
      vx = 0;
      va = 0;
    }
  }else{
    vy = pwm_in[0] - 1500;
    vx = pwm_in[1] - 1500;
    va = pwm_in[2] - 1500;
  }

  motor_pwm[0] = vx + vy + va + 1500;
  motor_pwm[1] = vx - vy + va + 1500;
  motor_pwm[2] = vx + vy - va + 1500;
  motor_pwm[3] = vx - vy - va + 1500;

  for(int i =0; i < 4; i++){
    motor_out[i].writeMicroseconds(motor_pwm[i]);
  }
  Serial.print(motor_pwm[0]);
  Serial.print("\t");
  Serial.print(motor_pwm[1]);
  Serial.print("\t");
  Serial.print(motor_pwm[2]);
  Serial.print("\t");
  Serial.print(motor_pwm[3]);
  Serial.print("\t");
  Serial.print(pwm_in[0]);
  Serial.print(" ");
  Serial.print(pwm_in[1]);
  Serial.print(" ");
  Serial.print(pwm_in[2]);
  Serial.print("\t\n");

  // delay(20);
  // serialPrintf("%d\t%d\t%d\t%d\n",motor_pwm[0],motor_pwm[1],motor_pwm[2],motor_pwm[3]);
}

void initParameter()
{
  motor_offset[0] = 0;
  motor_offset[1] = 0;
  motor_offset[2] = 0;
  motor_offset[3] = 0;
}

void serialPrintf(const char *fmt, ...) {
  /* Buffer for storing the formatted data */
  char buff[512];
  /* pointer to the variable arguments list */
  va_list pargs;
  /* Initialise pargs to point to the first optional argument */
  va_start(pargs, fmt);
  /* create the formatted data and store in buff */
  vsnprintf(buff, 512, fmt, pargs);
  va_end(pargs);
  Serial.print(buff);
}
