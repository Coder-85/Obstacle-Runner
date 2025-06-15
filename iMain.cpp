#include "iGraphics.h"

#define SCRN_WIDTH 1000
#define SCRN_HEIGHT 600

enum page_status{
    HOME,
    PLAY,
    RESUME,
    LEADERBOARD,
    SCENES,
    HELP
};
enum page_status currentPage; // To track which page is being visited currently


Image home_coin_img; // image of coin on the home page


int home_option_color[6] = {0};
/*
function iDraw() is called again and again by the system.
*/
void iDraw()
{
    // place your drawing codes here
    iClear();
    if(currentPage == HOME){
        char total_coins[10] = "1000";
        iClear();
        // Load Bg Img
        iShowImage(0, 0, "assets/img/bg/home_bg_002.jpg");
        // Load Coin Img
        iLoadImage(&home_coin_img, "assets/img/objects/coin/coin_small.png");
        iScaleImage(&home_coin_img, 0.5);
        iShowLoadedImage(820, 570, &home_coin_img);
        // Print Total Coin
        iTextAdvanced(850, 575, total_coins, 0.1, 1, GLUT_STROKE_MONO_ROMAN);

        // Add options

        if(home_option_color[0] == 0){
            iSetColor(255, 255, 255);
            iRectangle(690, 450, 250, 40);
        }else{
            iSetTransparentColor(2, 168, 77, 0.4);
            iFilledRectangle(690, 450, 250, 40);
        }
        iSetColor(255, 255, 255);
        iTextAdvanced(755, 463, "New Game", 0.15, 1, GLUT_STROKE_MONO_ROMAN);

        if(home_option_color[1] == 0){
            iSetColor(255, 255, 255);
            iRectangle(690, 380, 250, 40);
        }else{
            iSetTransparentColor(2, 168, 77, 0.4);
            iFilledRectangle(690, 380, 250, 40);
        }
        iSetColor(255, 255, 255);
        iTextAdvanced(740, 393, "Saved Game", 0.15, 1, GLUT_STROKE_MONO_ROMAN);

        if(home_option_color[2] == 0){
            iSetColor(255, 255, 255);
            iRectangle(690, 310, 250, 40);
        }else{
            iSetTransparentColor(2, 168, 77, 0.4);
            iFilledRectangle(690, 310, 250, 40);
        }
        iSetColor(255, 255, 255);
        iTextAdvanced(770, 323, "Scenes", 0.15, 1, GLUT_STROKE_MONO_ROMAN);

        if(home_option_color[3] == 0){
            iSetColor(255, 255, 255);
            iRectangle(690, 240, 250, 40);
        }else{
            iSetTransparentColor(2, 168, 77, 0.4);
            iFilledRectangle(690, 240, 250, 40);
        }
        iSetColor(255, 255, 255);
        iTextAdvanced(730, 253, "Leaderboard", 0.15, 1, GLUT_STROKE_MONO_ROMAN);

        if(home_option_color[4] == 0){
            iSetColor(255, 255, 255);
            iRectangle(690, 170, 250, 40);
        }else{
            iSetTransparentColor(2, 168, 77, 0.4);
            iFilledRectangle(690, 170, 250, 40);
        }
        iSetColor(255, 255, 255);
        iTextAdvanced(785, 183, "Help", 0.15, 1, GLUT_STROKE_MONO_ROMAN);

        if(home_option_color[5] == 0){
            iSetColor(255, 255, 255);
            iRectangle(690, 100, 250, 40);
        }else{
            iSetTransparentColor(168, 2, 2, 0.4);
            iFilledRectangle(690, 100, 250, 40);
        }
        iSetColor(255, 255, 255);
        iTextAdvanced(785, 113, "Exit", 0.15, 1, GLUT_STROKE_MONO_ROMAN);


    }
}

/*
function iMouseMove() is called when the user moves the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouseMove(int mx, int my)
{
    // when mouse hovering the home page options
    if(currentPage == HOME){

        if((mx>=690 && mx<=940) && (my>=450 && my<=490)){
            home_option_color[0] = 1;
        }else{
            home_option_color[0] = 0;
        }

        if((mx>=690 && mx<=940) && (my>=380 && my<=420)){
            home_option_color[1] = 1;
        }else{
            home_option_color[1] = 0;
        }

        if((mx>=690 && mx<=940) && (my>=310 && my<=350)){
            home_option_color[2] = 1;
        }else{
            home_option_color[2] = 0;
        }

        if((mx>=690 && mx<=940) && (my>=240 && my<=280)){
            home_option_color[3] = 1;
        }else{
            home_option_color[3] = 0;
        }

        if((mx>=690 && mx<=940) && (my>=170 && my<=210)){
            home_option_color[4] = 1;
        }else{
            home_option_color[4] = 0;
        }

        if((mx>=690 && mx<=940) && (my>=100 && my<=140)){
            home_option_color[5] = 1;
        }else{
            home_option_color[5] = 0;
        }
    }
}

/*
function iMouseDrag() is called when the user presses and drags the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouseDrag(int mx, int my)
{
    // place your codes here
}

/*
function iMouse() is called when the user presses/releases the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouse(int button, int state, int mx, int my)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        printf("%d %d\n", mx, my);
    }
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        // place your codes here
    }
}

/*
function iMouseWheel() is called when the user scrolls the mouse wheel.
dir = 1 for up, -1 for down.
*/
void iMouseWheel(int dir, int mx, int my)
{
    // place your code here
}

/*
function iKeyboard() is called whenever the user hits a key in keyboard.
key- holds the ASCII value of the key pressed.
*/
void iKeyboard(unsigned char key)
{
    switch (key)
    {
    case 'q':
        // do something with 'q'
        break;
    // place your codes for other keys here
    default:
        break;
    }
}

/*
function iSpecialKeyboard() is called whenver user hits special keys likefunction
keys, home, end, pg up, pg down, arraows etc. you have to use
appropriate constants to detect them. A list is:
GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6,
GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11,
GLUT_KEY_F12, GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN,
GLUT_KEY_PAGE_UP, GLUT_KEY_PAGE_DOWN, GLUT_KEY_HOME, GLUT_KEY_END,
GLUT_KEY_INSERT */
void iSpecialKeyboard(unsigned char key)
{
    switch (key)
    {
    case GLUT_KEY_END:
        // do something
        break;
    // place your codes for other keys here
    default:
        break;
    }
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    // place your own initialization codes here.
    iInitialize(SCRN_WIDTH, SCRN_HEIGHT, "Obstacle Runner");
    return 0;
}
