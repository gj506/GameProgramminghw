//Granville Jones
//Intro to Game Programming
// Hw#2
// libraries are for the angle brackets
//the quotes are for the header files
#ifdef _WINDOWS
#include "GL/glew.h"
#endif
#include "SDL.h"
#include "SDL_opengl.h"
#include "SDL_image.h"
#include "ShaderProgram.h"
#include "Matrix.h"


#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

GLuint LoadTexture(const char *image_path){
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(surface);

	return textureID;
}

void blendSprite(){
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void drawSprite(GLuint& textureID, float vertices[], float texCoords[], ShaderProgram& program){
	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);

	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program.texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, textureID);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);


}

struct paddle {
	float paddlePositionX;
	float paddlePositionY;
	float paddleWidth;
	float paddleHeight;
	float angle;
};

struct ball{
	float ballPositionX;
	float ballPositionY;
	float ballDirectionX;
	float ballDirectionY;
	float ballSpeed;
	float ballAngle;
	float ballWidth;
	float ballHeight;
};

float randomizeBallMovement(){
	int direction = rand() % 2 + 1;
	//this gets a random number which will always yield a 1 or a 2. We can also just leave it at 0 or 1 or even 2 or 3. But this is my code so my preference =).
	if (direction == 1){ return 1.0f; }
	else { return -1.0f; }

}

int main(int argc, char *argv[])
{
	// Basic Setup
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Ping Pong Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif

// for the width and the height of the paddles and the ball, i actually toyed around with the number until it visually looked like the ball was colliding with the paddle and not just the invisible box that the paddle was embedded in
#pragma region leftPaddleVariables
	paddle paddle1;
	paddle1.angle = 0;
	paddle1.paddlePositionX = -3.35f;
	paddle1.paddlePositionY = 0.0f;
	paddle1.paddleWidth = 0.38f;
	paddle1.paddleHeight = 0.6f;
#pragma endregion

#pragma region RightPaddleVariables
	paddle paddle2;
	paddle2.angle = 0;
	paddle2.paddlePositionX = 3.35f;
	paddle2.paddlePositionY = 0.0f;
	paddle2.paddleWidth = 0.38f;
	paddle2.paddleHeight = 0.6;
#pragma endregion

#pragma region ball variable
	ball ball;
	ball.ballPositionX = 0.0f;
	ball.ballPositionY = 0.0f;
	ball.ballDirectionX = 0.0f;
	ball.ballDirectionY = 0.0f;
	ball.ballSpeed = 1.0f;
	ball.ballAngle = 0.0f;
	ball.ballWidth = 0.125f;
	ball.ballHeight = 0.125f;
#pragma endregion

#pragma region glShaderAndMatrix/KeyboardStuff
	float lastFrameTicks = 0.0f;
	glViewport(0, 0, 640, 360);
	glEnable(GL_BLEND);
	ShaderProgram program = ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	Matrix modelMatrix;
	Matrix viewMatrix;
	Matrix projectionMatrix;
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0, 2.0, -1.0, 1.0);
#pragma endregion

#pragma region textureID variables
	GLuint textureID1 = LoadTexture("Racket1.png");
	GLuint textureID2 = LoadTexture("Racket2.png");
	GLuint textureID3 = LoadTexture("pongBall.png");
#pragma endregion

	SDL_Event event;
	bool done = false;

	while (!done) {

#pragma region eventHandle
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
#pragma endregion

#pragma region colorChange
		glClearColor(0.0f, 0.2f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(program.programID);
#pragma endregion

#pragma region elapsedSetUp
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
#pragma endregion

#pragma region MatrixSetUp
		program.setModelMatrix(modelMatrix);
		program.setViewMatrix(viewMatrix);
		program.setProjectionMatrix(projectionMatrix);
# pragma endregion

#pragma region playerOneKeys
		if (keys[SDL_SCANCODE_W] && paddle1.paddlePositionY < 1.7){ paddle1.paddlePositionY += elapsed * 4; }
		else if (keys[SDL_SCANCODE_S] && paddle1.paddlePositionY > -1.7){ paddle1.paddlePositionY -= elapsed * 4; }
#pragma endregion

#pragma region playerOnePaddle
		modelMatrix.identity();
		blendSprite();
		modelMatrix.Translate(paddle1.paddlePositionX, paddle1.paddlePositionY, 0.0);
		float vertices1[] = { -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };
		float texCoords1[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
		program.setModelMatrix(modelMatrix);
		drawSprite(textureID1, vertices1, texCoords1, program);
#pragma endregion

#pragma region playerTwoKeys
		if (keys[SDL_SCANCODE_UP] && paddle2.paddlePositionY < 1.7){ paddle2.paddlePositionY += elapsed * 4; }
		else if (keys[SDL_SCANCODE_DOWN] && paddle2.paddlePositionY > -1.7){ paddle2.paddlePositionY -= elapsed * 4; }
#pragma endregion

#pragma region playerTwoPaddle
		modelMatrix.identity();
		blendSprite();
		modelMatrix.Translate(paddle2.paddlePositionX, paddle2.paddlePositionY, 0.0);
		program.setModelMatrix(modelMatrix);
		float vertices2[] = { -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };
		float texCoords2[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
		drawSprite(textureID2, vertices2, texCoords2, program);
#pragma endregion

#pragma region ballStuffAndCollisionStuffAndActuallyStartingTheGame
		if (keys[SDL_SCANCODE_L] && ball.ballPositionX == 0 && ball.ballPositionY == 0){
			/*this randomizes the direction of the ball. the function call always returns either 1 or -1*/
			ball.ballDirectionX = randomizeBallMovement();
			ball.ballDirectionY = randomizeBallMovement();
		}

		ball.ballSpeed += 0.0001;

		ball.ballPositionX += ball.ballDirectionX * ball.ballSpeed * elapsed;
		ball.ballPositionY += ball.ballDirectionY * ball.ballSpeed * elapsed;

		if (ball.ballPositionY >= 1.9) { ball.ballDirectionY = -1.0f; /*Directions are normally 1 or -1 and therefore we switch the direction if the ball hits either the top or the bottom of the screen. 1 means that the ball is moving in the up direction. -1 means that the ball is moving in the down direction*/ }
		else if (ball.ballPositionY < -1.9){ ball.ballDirectionY = 1.0f; /*same thing here*/ }

		else if (ball.ballPositionX >= 3.65 || ball.ballPositionX <= -3.65){
			ball.ballPositionX = 0.0f;
			ball.ballPositionY = 0.0f;
			ball.ballDirectionX = 0.0f;
			ball.ballDirectionY = 0.0f;
			ball.ballSpeed = 1.25;
		}

		// The ball hitting the left paddle.
		if (!(paddle1.paddlePositionX + paddle1.paddleWidth / 2 < ball.ballPositionX - ball.ballWidth / 2 || paddle1.paddlePositionX - paddle1.paddleWidth / 2 > ball.ballPositionX + ball.ballWidth / 2 || paddle1.paddlePositionY + paddle1.paddleHeight / 2 < ball.ballPositionY - ball.ballHeight / 2 || paddle1.paddlePositionY - paddle1.paddleHeight / 2 > ball.ballPositionY + ball.ballHeight / 2)){
			ball.ballDirectionX = 1.0f;
			// this changes the direction to start going "rightwards"
		}

		//The ball hitting the right paddle
		if (!(paddle2.paddlePositionX - paddle2.paddleWidth / 2 > ball.ballPositionX + ball.ballWidth / 2 || paddle2.paddlePositionX + paddle2.paddleWidth / 2 < ball.ballPositionX - ball.ballWidth / 2 || paddle2.paddlePositionY + paddle2.paddleHeight / 2 < ball.ballPositionY - ball.ballHeight / 2 || paddle2.paddlePositionY - paddle2.paddleHeight / 2 > ball.ballPositionY + ball.ballHeight / 2)){
			ball.ballDirectionX = -1.0f;
			// this changes the direction to start going "leftwards"
		}

		modelMatrix.identity();
		blendSprite();
		modelMatrix.Translate(ball.ballPositionX, ball.ballPositionY, 0.0);
		program.setModelMatrix(modelMatrix);
		float vertices3[] = { -0.125f, -0.125f, 0.125f, 0.125f, -0.125f, 0.125f, 0.125f, 0.125f, -0.125f, -0.125f, 0.125f, -0.125f };
		float texCoords3[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
		drawSprite(textureID3, vertices3, texCoords3, program);
#pragma endregion

		SDL_GL_SwapWindow(displayWindow);
	}

	
	SDL_Quit();
	return 0;
}
