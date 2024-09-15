#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int birdY = SCREEN_HEIGHT / 2;   // Bird's starting Y position
int birdX = SCREEN_WIDTH / 4;    // Bird's X position
int gravity = 1;                 // Gravity force
int lift = -5;                   // Force when flapping
int velocity = 0;                // Bird's velocity
int pipeX = SCREEN_WIDTH;        // Initial pipe position
int pipeGap = 25;                // Gap size in pipes
int pipeWidth = 10;              // Pipe width
int pipeGapHeight = SCREEN_HEIGHT / 3; // Size of the gap between the pipes
int score = 0;                   // Player's score
int highScore = 0;               // High score
bool gameOver = false;           // Game over flag
bool gameStarted = false;        // Flag to indicate if the game is started

// Button pins
int playButton = 18;  // GPIO 18 for play/flap button
int resetButton = 19; // GPIO 19 for quit/reset button

void setup() {
  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.display();

  // Initialize serial communication for debugging
  Serial.begin(115200);

  // Initialize buttons
  pinMode(playButton, INPUT_PULLUP);  // Play button connected to GPIO 18
  pinMode(resetButton, INPUT_PULLUP); // Reset button connected to GPIO 19
}

void drawBird() {
  display.fillCircle(birdX, birdY, 3, WHITE); // Draw bird as a small circle
}

void drawPipes() {
  // Draw upper pipe
  display.fillRect(pipeX, 0, pipeWidth, pipeGapHeight, WHITE);
  // Draw lower pipe
  display.fillRect(pipeX, pipeGapHeight + pipeGap, pipeWidth, SCREEN_HEIGHT - (pipeGapHeight + pipeGap), WHITE);
}

void checkCollision() {
  // Check if the bird hits the top or bottom of the screen
  if (birdY <= 0 || birdY >= SCREEN_HEIGHT) {
    gameOver = true;
  }

  // Check if the bird hits the pipes
  if (birdX >= pipeX && birdX <= pipeX + pipeWidth) {
    if (birdY <= pipeGapHeight || birdY >= pipeGapHeight + pipeGap) {
      gameOver = true;
    }
  }
}

void resetGame() {
  birdY = SCREEN_HEIGHT / 2;
  velocity = 0;
  pipeX = SCREEN_WIDTH;
  score = 0;
  gameOver = false;
  gameStarted = false;
  // Do not reset the high score here to keep it across resets
}

void updateHighScore() {
  if (score > highScore) {
    highScore = score;
  }
}

void loop() {
  // Check if the reset button is pressed
  if (digitalRead(resetButton) == LOW) {
    resetGame(); // Reset the game but keep the high score intact
  }

  if (!gameStarted) {
    // Display start screen
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(20, 20);
    display.print(F("FLAPPY BIRD"));
    display.setTextSize(1);
    display.setCursor(20, 50);
    display.print(F("PRESS PLAY"));
    display.display();

    // Wait for the play button to start the game
    if (digitalRead(playButton) == LOW) {
      gameStarted = true;
    }
  } else if (!gameOver) {
    // Clear display
    display.clearDisplay();

    // Bird physics
    velocity += gravity;
    birdY += velocity;

    // Pipe movement
    pipeX -= 4; // Move pipes slower
    if (pipeX < -pipeWidth) {
      pipeX = SCREEN_WIDTH; // Reset pipe position
      pipeGapHeight = random(10, SCREEN_HEIGHT - pipeGap - 10); // Randomize the gap height
      score++; // Increment score when a pipe is passed
    }

    // Draw bird and pipes
    drawBird();
    drawPipes();

    // Check for collisions
    checkCollision();

    // Display the score and high score
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print(F("Score: "));
    display.print(score);
    display.setCursor(0, 10);
    display.print(F("High Score: "));
    display.print(highScore);

    // Update display
    display.display();

    // Flap bird when the play button is pressed (GPIO 18)
    if (digitalRead(playButton) == LOW) {
      velocity = lift; // Apply lift to the bird
    }

    // Adjust delay for slowing down the game
    delay(80); // Increase the delay to make the game slower
  } else {
    // Update high score before showing Game Over
    updateHighScore();

    // Display "Game Over" message
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(10, 20);
    display.print(F("GAME OVER"));
    display.setTextSize(1);
    display.setCursor(10, 50);
    display.print(F("PRESS RESET"));
    display.display();

    // Wait for the reset button to be pressed to restart
    if (digitalRead(resetButton) == LOW) {
      resetGame();
    }
  }
}
