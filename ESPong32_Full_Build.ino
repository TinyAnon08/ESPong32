#include <TFT_eSPI.h> 
#include <SPI.h>

// Pins
#define JOY_PIN    35 
#define BUZZER_PIN 9  

TFT_eSPI tft = TFT_eSPI(); 
TFT_eSprite img = TFT_eSprite(&tft); 

// Game Dimensions
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 128;

// Paddle Dimensions & Variables
const int PADDLE_WIDTH = 4;
const int PADDLE_HEIGHT = 20;
int paddleX = 5;
int paddleY = 54;

// Ball Dimensions & Variables
const int BALL_SIZE = 4;
float ballX = 64.0; 
float ballY = 64.0; 

// Ball Velocity & Speed Tracking
float ballDX = 2.0; 
float ballDY = 2.0;
const float SPEED_INCREMENT = 1.05; 

// Game State
int score = 0;
bool gameOver = false;

// SFX

void playWallSound() {
  noTone(BUZZER_PIN);        
  tone(BUZZER_PIN, 587, 50); // D5 snap
}

void playScoreSound() {
  noTone(BUZZER_PIN);         
  tone(BUZZER_PIN, 1318, 90); // E6 chime
}

void playStartSound() {
  noTone(BUZZER_PIN);
  tone(BUZZER_PIN, 523, 150);  // C5 note
  delay(150);                  
  noTone(BUZZER_PIN);
  tone(BUZZER_PIN, 784, 250);  // G5 note
  delay(250);                 
}

void playGameOverSound() {
  noTone(BUZZER_PIN);          
  tone(BUZZER_PIN, 294, 300);  // D4 note
  delay(300);                  
  noTone(BUZZER_PIN);
  tone(BUZZER_PIN, 147, 500);  // D3 note
  delay(500);  
}

void setup() {
  Serial.begin(115200);
  
  // Initialize Pins
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); 
  
  // Initialize Screen
  tft.init();
  tft.setRotation(2); 
  tft.fillScreen(TFT_BLACK);
  
  // Create a full-screen sprite buffer
  img.createSprite(SCREEN_WIDTH, SCREEN_HEIGHT);
  
  resetGame();
}

void loop() {
  if (gameOver) {
    int joyVal = analogRead(JOY_PIN);
    if (joyVal < 1000 || joyVal > 3000) {
      resetGame();
      delay(300); 
    }
    return;
  }

  // Read Joystick & Move Paddle
  int joyValue = analogRead(JOY_PIN); 
  paddleY = map(joyValue, 0, 4095, 0, SCREEN_HEIGHT - PADDLE_HEIGHT);
  
  // Move Ball
  ballX += ballDX;
  ballY += ballDY;

  // Wall Collisions
  if ((int)ballY <= 0 || (int)ballY >= (SCREEN_HEIGHT - BALL_SIZE)) {
    ballDY = -ballDY;
    playWallSound(); 
  }
  if ((int)ballX >= (SCREEN_WIDTH - BALL_SIZE)) {
    ballDX = -ballDX;
    playWallSound(); 
  }

  // Paddle Collisions
  if ((int)ballX <= (paddleX + PADDLE_WIDTH) && (int)ballX >= paddleX) {
    if ((int)ballY + BALL_SIZE >= paddleY && (int)ballY <= paddleY + PADDLE_HEIGHT) {
      ballDX = -ballDX;
      ballX = paddleX + PADDLE_WIDTH; 
      score++;
      playScoreSound(); 
      
      // Multiply current direction vectors by 1.05
      ballDX *= SPEED_INCREMENT;
      ballDY *= SPEED_INCREMENT;
    }
  }

  // Game Loss Condition
  if (ballX < 0) {
    displayGameOver();
    return;
  }

  // Render Everything to the Sprite
  img.fillSprite(TFT_BLACK); 

  // Draw Score
  img.setCursor(80, 5);
  img.setTextColor(TFT_MAROON);
  img.setTextSize(1);
  img.print("Pts: ");
  img.print(score);

  // Draw Paddle and Ball
  img.fillRect(paddleX, paddleY, PADDLE_WIDTH, PADDLE_HEIGHT, TFT_WHITE);
  img.fillRect((int)ballX, (int)ballY, BALL_SIZE, BALL_SIZE, TFT_WHITE);

  // Push frame
  img.pushSprite(0, 0);

  delay(10); 
}

void displayGameOver() {
  gameOver = true;
  tft.fillScreen(TFT_BLACK);
  
  tft.setCursor(15, 45);
  tft.setTextColor(TFT_RED);
  tft.setTextSize(2);
  tft.print("GAME OVER");
  
  tft.setCursor(35, 75);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.print("Score: ");
  tft.print(score);

  tft.setCursor(5, 100);
  tft.print("Move Stick to Begin Again");

  playGameOverSound(); 
}

void resetGame() {
  score = 0;
  ballX = 64.0;
  ballY = 64.0;
  ballDX = 2.0;
  ballDY = 2.0;
  gameOver = false;
  tft.fillScreen(TFT_BLACK);
  
  playStartSound(); 
}
