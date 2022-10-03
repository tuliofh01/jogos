#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include <SOIL/SOIL.h>
#include <GL/freeglut.h> 
#include <bass.h>

/* Struct definitions */
struct Player{
    // Dynamic proprieties
    int position[2];
    int health;

    // Static proprieties
    int dimensions[2];
    char texture[100];
};

struct Alien{
    // Dynamic proprieties
    int position[2];
    int health;

    // Static proprieties
    int dimensions[2];
    char texture[100];
};

struct Bullet{
    int position[2];
    int dimensions[2];
    int damage;
    // Tracks the bullets source (1 for enemy, 0 for player, -1 for none).
    int origin;
};


/* Global variables */
// Instantiates player
struct Player player = {
                        .position = {0,35},
                        .health = 100,
                        .dimensions = {32,32},
                        .texture = "./media/images/player/playerSpaceship.png"
                        };

// Instantiates aliens
struct Alien aliens[16];

// Instantiates bullets
struct Bullet bullets[99999];

// Control current bullet position.
int currentBullet = 0;

// Control whether program is running for the first time or not.
int firstInstanceStatus = 1;

// Control bullet spawning.
int playerBulletMovementStatus = 0;

// Control alien movement delay.
int alienMovementStatus = 0;

// Control variable that sets the game's status (0 -> Running, 1 -> Paused).
int isPaused = 1;

// Path to the background texture
char backgroundTexture[100] = "./media/images/background/backgroundTexture.png";

// Path to the greetings screen texture
char greetingsScreenTexture[100] = "./media/images/background/greetingsScreenTexture.png";

// Path to game over screen texture
char gameOverScreenTexture[100] = "./media/images/background/gameOverTexture.png";

// Path to end game screen texture
char endGameTexture[100] = "./media/images/background/endGameTexture.png";

// Random number generator constant.
time_t t;


/* Functions and procedures */
void drawPlayer(){
    // Loads and sets-up player texture
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D); glEnable(GL_BLEND);
    GLuint textureID = SOIL_load_OGL_texture(
        player.texture,
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y
    );
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Sets drawing color.
    glColor4f(1, 1, 1, 1);

    // Draws square (represents player).
    glBegin(GL_POLYGON);
        glTexCoord2f(0, 0); glVertex3f(player.position[0], player.position[1], 0);
        glTexCoord2f(1, 0); glVertex3f(player.position[0] + player.dimensions[0], player.position[1], 0);
        glTexCoord2f(1, 1); glVertex3f(player.position[0] + player.dimensions[0], player.position[1] + player.dimensions[1] , 0);
        glTexCoord2f(0, 1); glVertex3f(player.position[0], player.position[1] + player.dimensions[1], 0);
    glEnd();

    // Disables texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void fireBullet(int origin, int x, int y){
    // Sets bullet specs
    bullets[currentBullet].position[0] = x;
    bullets[currentBullet].position[1] = y;
    bullets[currentBullet].origin = origin;

    if (bullets[currentBullet].origin == 0){
        bullets[currentBullet].damage = 20;
    }
    else{
        bullets[currentBullet].damage = 25;
    }

    // Updates current bullet
    currentBullet += 1;
}

void drawAlien(struct Alien alien){
    // Loads and sets-up alien texture
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D); glEnable(GL_BLEND);
    GLuint textureID = SOIL_load_OGL_texture(
        alien.texture,
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y
    );
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Sets drawing color.
    glColor4f(1, 1, 1, 1);

    // Draws square (represents player).
    glBegin(GL_POLYGON);
        glTexCoord2f(0, 0); glVertex3f(alien.position[0], alien.position[1], 0);
        glTexCoord2f(1, 0); glVertex3f(alien.position[0] + alien.dimensions[0], alien.position[1], 0);
        glTexCoord2f(1, 1); glVertex3f(alien.position[0] + alien.dimensions[0], alien.position[1] + alien.dimensions[1] , 0);
        glTexCoord2f(0, 1); glVertex3f(alien.position[0], alien.position[1] + alien.dimensions[1], 0);
    glEnd();

    // Disables texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void playAlienBlast(){
    char sound[100] = "./media/audio/alienGunSoundEffect.wav";
    HSTREAM stream = BASS_StreamCreateFile(0, sound, 0, 0, 0);
    BASS_ChannelPlay(stream, 0);
}

void playExplosion(){
    char sound[100] = "./media/audio/explosionSoundEffect.wav";
    HSTREAM stream = BASS_StreamCreateFile(0, sound, 0, 0, 0);
    BASS_ChannelPlay(stream, 0);
}


void setUpAliens(int arg){
    if(firstInstanceStatus == 1){
        for(int i = 0; i < 16; i++){
            char tempTexturePath[100] = "./media/images/enemy/alienSpaceship.png";
            aliens[i].health = 100;
            aliens[i].dimensions[0] = 32; aliens[i].dimensions[1] = 32;
            if(i < 8){
                aliens[i].position[0] = 64*i; aliens[i].position[1] = 400;
            }
            else{
                aliens[i].position[0] = 64*(i-8); aliens[i].position[1] = 300;
            }
            memcpy(aliens[i].texture, tempTexturePath, sizeof(tempTexturePath));
        }
    }
    
    else{
        for(int i = 0; i < 16; i++){
            if(alienMovementStatus > 65){
                if(aliens[i].health > 0){
                    // Updates Y position
                    if(aliens[i].position[1] % 2 == 0){
                        aliens[i].position[1] -= 21;
                    }
                    else{
                        aliens[i].position[1] += 21;
                    }
                    // Updates X position
                    if(aliens[i].position[0] % 2 == 0){
                        aliens[i].position[0] += 11;
                    }
                    else{
                        aliens[i].position[0] -= 11;
                    }
                }
                alienMovementStatus = 0;
            }
            else{
                alienMovementStatus += 1;
            }
            // Draws alien onto the screen
            if(aliens[i].health > 0){
                drawAlien(aliens[i]);
                int alienFireFlag = rand() % 128;
                if (alienFireFlag == 0){
                    playAlienBlast();
                    fireBullet(1, aliens[i].position[0] + (aliens[i].dimensions[0]/2), (aliens[i].position[1] + 1));
                }
            }
            else{
                if(aliens[i].position[1] != 600){
                    playExplosion();
                    aliens[i].position[1] = 600;
                }
            }
        }
    }
}

void initializeBullets(){
    // Initializes bullet array
    for(int i = 0; i < 9999; i++){
        bullets[i].position[0] = 0;
        bullets[i].position[1] = 0;
        bullets[i].dimensions[0] = 4;
        bullets[i].dimensions[1] = 8;
        bullets[i].damage = 0;
        bullets[i].origin = -1;
    }
}

int countPlayerBullets(){
    int startValue;
    if (currentBullet < 50)
        startValue = 0;
    else
        startValue = currentBullet - 50;

    playerBulletMovementStatus = 0;
    for(int i = startValue; i < currentBullet; i++){
        if(bullets[i].origin == 0){
            playerBulletMovementStatus += 1;
        }
    }
    return playerBulletMovementStatus;
}

void drawBullets(){
    int startValue;
    if (currentBullet < 50)
        startValue = 0;
    else
        startValue = currentBullet - 50;

    for(int i = startValue; i < currentBullet; i++){
        if(bullets[i].origin != -1){
            if(bullets[i].origin == 0){
                // Sets drawing color (player).
                glColor4f(0, 1, 0, 1);
            }
            else{
                // Sets drawing color (enemy).
                glColor4f(1, 0, 0, 1);
            }
            if(bullets[i].position[1] > 0 && bullets[i].position[1] < 500){
                glBegin(GL_POLYGON);
                    glVertex3f(bullets[i].position[0],  bullets[i].position[1], 0);
                    glVertex3f(bullets[i].position[0] + bullets[i].dimensions[0], bullets[i].position[1], 0);
                    glVertex3f(bullets[i].position[0] + bullets[i].dimensions[0], bullets[i].position[1] + bullets[i].dimensions[1] , 0);
                    glVertex3f(bullets[i].position[0],  bullets[i].position[1] +  bullets[i].dimensions[1], 0);
                glEnd();
            }
            else {
                bullets[i].origin = -1;
            }
            
            if(bullets[i].origin == 0){
                // Updates bullet trajectory
                bullets[i].position[1] += 2;
            }
            else{
                // Updates bullet trajectory
                bullets[i].position[1] -= 4;
            }
                
        }
    }
}

void detectPlayerHits(){
    int startValue;
    if (currentBullet < 50)
        startValue = 0;
    else
        startValue = currentBullet - 50;
    
    for(int i = startValue; i < currentBullet; i++){
        for(int j = 0; j < 16; j++){
            if(bullets[i].position[0] >= aliens[j].position[0] && bullets[i].position[0] <= aliens[j].position[0] + aliens[j].dimensions[0]){
                if(bullets[i].position[1] >= aliens[j].position[1] && bullets[i].position[1] <= aliens[j].position[1] + aliens[j].dimensions[1]){
                    if(bullets[i].origin == 0){
                        aliens[j].health -= bullets[i].damage;
                        bullets[i].position[1] = 600;
                    }  
                }
            }
        }
    }
}

void detectAlienHits(){
    int startValue;
    if (currentBullet < 50)
        startValue = 0;
    else
        startValue = currentBullet - 50;

    for(int i = startValue; i < currentBullet; i++){
        for(int j = 0; j < 16; j++){
            if(bullets[i].position[0] >= player.position[0] && bullets[i].position[0] <= player.position[0] + player.dimensions[0]){
                if(bullets[i].position[1] >= player.position[1] && bullets[i].position[1] <= player.position[1] + player.dimensions[1]){
                    if(bullets[i].origin == 1){
                        player.health -= bullets[i].damage;
                        bullets[i].position[1] = 600;
                    }  
                }
            }
        }
    }
}

void playSoundTrack(int arg){
    char sound[100] = "./media/audio/gameplayBackgroundMusic.mp3";
    HSTREAM stream = BASS_StreamCreateFile(0, sound, 0, 0, 0);
    BASS_ChannelPlay(stream, 0);
    glutTimerFunc((1000 * 60 * 2) + (1000* 20), playSoundTrack, 0);
}

void playPlayerBlast(){
    char sound[100] = "./media/audio/laserBlasterSoundEffect.wav";
    HSTREAM stream = BASS_StreamCreateFile(0, sound, 0, 0, 0);
    BASS_ChannelPlay(stream, 0);
}

void drawGameOverScreen(){
    // Loads and sets-up player texture
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D); glEnable(GL_BLEND);
    GLuint textureID = SOIL_load_OGL_texture(
        gameOverScreenTexture,
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y
    );
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Sets drawing color.
    glColor4f(1, 1, 1, 1);

    // Draws square covering the entire window.
    glBegin(GL_POLYGON);
        glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
        glTexCoord2f(1, 0); glVertex3f(530, 0, 0);
        glTexCoord2f(1, 1); glVertex3f(530, 530, 0);
        glTexCoord2f(0, 1); glVertex3f(0, 530, 0);
    glEnd();

    // Disables texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void drawGreetingsScreen(){
    // Loads and sets-up player texture
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D); glEnable(GL_BLEND);
    GLuint textureID = SOIL_load_OGL_texture(
        greetingsScreenTexture,
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y
    );
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Sets drawing color.
    glColor4f(1, 1, 1, 1);

    // Draws square covering the entire window.
    glBegin(GL_POLYGON);
        glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
        glTexCoord2f(1, 0); glVertex3f(530, 0, 0);
        glTexCoord2f(1, 1); glVertex3f(530, 530, 0);
        glTexCoord2f(0, 1); glVertex3f(0, 530, 0);
    glEnd();

    // Disables texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void drawBackground(){
    // Loads and sets-up player texture
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D); glEnable(GL_BLEND);
    GLuint textureID = SOIL_load_OGL_texture(
        backgroundTexture,
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y
    );
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Sets drawing color.
    glColor4f(1, 1, 1, 1);

    // Draws square covering the entire window.
    glBegin(GL_POLYGON);
        glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
        glTexCoord2f(1, 0); glVertex3f(530, 0, 0);
        glTexCoord2f(1, 1); glVertex3f(530, 530, 0);
        glTexCoord2f(0, 1); glVertex3f(0, 530, 0);
    glEnd();

    // Disables texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void drawEndGame(){
    // Loads and sets-up player texture
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D); glEnable(GL_BLEND);
    GLuint textureID = SOIL_load_OGL_texture(
        endGameTexture,
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y
    );
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Sets drawing color.
    glColor4f(1, 1, 1, 1);

    // Draws square covering the entire window.
    glBegin(GL_POLYGON);
        glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
        glTexCoord2f(1, 0); glVertex3f(530, 0, 0);
        glTexCoord2f(1, 1); glVertex3f(530, 530, 0);
        glTexCoord2f(0, 1); glVertex3f(0, 530, 0);
    glEnd();

    // Disables texture
    glBindTexture(GL_TEXTURE_2D, 0);

}

int detectVictory(){
    int deadAlienCounter = 0;
    for (int i = 0; i < 16; i++){
        if(aliens[i].health <= 0){
            deadAlienCounter +=1;
        }
    }
    if (deadAlienCounter == 16){
        return 0;
    }
    else {
        return 1;
    }
}

void drawScreen(){
    // Clears previous content.
    glClear(GL_COLOR_BUFFER_BIT);

    if(firstInstanceStatus == 0 && player.health > 0 && detectVictory() == 1){
        // Draws game content
        drawBackground();
        detectPlayerHits();
        detectAlienHits();
        drawBullets();
        setUpAliens(0);
        drawPlayer();
    }
    else if(player.health <= 0){
        if (firstInstanceStatus == 0){
            playExplosion();
            firstInstanceStatus = 1;
        }
        drawGameOverScreen();
    }
    else if (firstInstanceStatus == 0 && player.health > 0 && detectVictory() == 0) {
        drawEndGame();
    }
    else{
        BASS_Init(-1, 44100, BASS_DEVICE_STEREO, 0, 0);
        setUpAliens(0);
        drawGreetingsScreen();
        // Starts soundtrack
        playSoundTrack(0);
    }
    
    // Cycles between buffers.
    glutSwapBuffers();
}

void reshapeWindow(int width, int height) {
    // Keeps proportions when resizing.
    glViewport(0, 0, width, height);

    // Sets up display projection.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 530, 0, 530, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void updateScreen(int arg){
    if (isPaused == 0){
        glutPostRedisplay();
    } 
    glutTimerFunc(33, updateScreen, 0);
}

void keyboardEventHandler(unsigned char key, int x, int y) {    
    if (key == 27)
        exit(0);

    if (firstInstanceStatus == 1 && key == 13 && player.health > 0 && detectVictory() == 1){
        updateScreen(0);
        initializeBullets();
        firstInstanceStatus = 0;
        isPaused = 0;
    }

    if (key == 112){
        if(isPaused == 0)
            isPaused = 1;
        else
            isPaused = 0;
    }
    
    if (isPaused == 0){
        if(key == 32){
            if (countPlayerBullets() < 4 && player.health > 0){
                playPlayerBlast();
                fireBullet(0, player.position[0] + (player.dimensions[0]/2), player.position[1] + player.dimensions[1]);
            }
        }
        else if(key == 100 && player.health > 0){
            if (player.position[0] + 10 > 500) 
				player.position[0] = 500;
            else
				player.position[0] += 10;
        }
        else if (key == 97 && player.health > 0){
            if (player.position[0] - 10 < 0) 
				player.position[0] = 0;
            else
				player.position[0] -= 10;
        }
    }
}

int main(int argc, char** argv){    
    // Starts random number generator
    srand((unsigned) time(&t));

    // Initializes and sets up glut instance.
    glutInit(&argc, argv); glutInitContextVersion(1, 1); 
    glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
    
    // Sets up a few glut settings.
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 0);
    glClearColor(1, 1, 1, 1);
    
    // Creates a window.	
    glutCreateWindow("!!! SPACE INVADERS !!!");
    
    // Sets up the display function for our window.
    glutDisplayFunc(drawScreen);
    
    // Sets up a function to manage keyboard events
    glutKeyboardFunc(keyboardEventHandler);
    
    // Sets resize window function.
    glutReshapeFunc(reshapeWindow);
    
    // Sets clear color to white.
    glClearColor(1, 1, 1, 1);
    
    // Keeps glut instance alive.	
    glutMainLoop();

    return 0;
}