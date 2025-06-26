#include "iGraphics.h"

#define SCRN_WIDTH 1000
#define SCRN_HEIGHT 600
#define BTN_TOTAL 6
#define BTN_EXIT (BTN_TOTAL - 1)

enum page_status
{
    HOME,
    PLAY,
    RESUME,
    SCENES,
    LEADERBOARD,
    HELP,
    EXIT
};

enum page_status currentPage; // To track which page is being visited currently
Image home_coin_img;          // Image of coin on the home page
Sprite runner;                // Sprite for the runner character
int home_option_color[BTN_TOTAL] = {0};
// Button labels and positions for home page options
const char *home_option_labels[BTN_TOTAL] = {"New Game", "Saved Game", "Scenes", "Leaderboard", "Help", "Exit"};
const int home_option_x = 690;
const int home_option_w = 250;
const int home_option_h = 40;
Image idle_frames[10];

// coordinate of the sprite image
int sprite_x = 192;
int sprite_y = 173;

// Game variables
int unlock_status[3] = {1, 1, 0};
int selected_status[3] = {1, 0, 0};

inline int get_home_option_y(int idx)
{
    return 30 + 70 * (BTN_TOTAL - idx);
}

float get_text_width(const char *str, float scale, void *font)
{
    float w = 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        w += glutStrokeWidth(font, str[i]);
    }
    return w * scale;
}

void draw_home_page_button(int idx, int highlight, const char *label)
{
    int x = home_option_x;
    int y = get_home_option_y(idx);
    if (highlight)
    {
        if (idx == BTN_EXIT)
            iSetTransparentColor(168, 2, 2, 0.4);
        else
            iSetTransparentColor(2, 168, 77, 0.4);

        iFilledRectangle(x, y, home_option_w, home_option_h);
    }
    else
    {
        iSetColor(255, 255, 255);
        iRectangle(x, y, home_option_w, home_option_h);
    }

    iSetColor(255, 255, 255);
    float scale = 0.15f;
    float text_width = get_text_width(label, scale, GLUT_STROKE_MONO_ROMAN);
    float text_x = x + (home_option_w - text_width) / 2.0f;
    int text_y = y + 13;
    iTextAdvanced(text_x, text_y, label, scale, 1, GLUT_STROKE_MONO_ROMAN);
}

void loadCoinData()
{

    iShowLoadedImage(820, 570, &home_coin_img);
    // Print Total Coin
    char total_coins[10] = "1000";
    iSetColor(255, 255, 255);
    iTextAdvanced(850, 575, total_coins, 0.1, 1, GLUT_STROKE_MONO_ROMAN);
}

void initialize_sprites()
{
    iLoadFramesFromFolder(idle_frames, "assets/img/sprite/idle");

    iInitSprite(&runner, -1);
    iSetSpritePosition(&runner, sprite_x, sprite_y);
    iScaleSprite(&runner, 0.23f);
    iChangeSpriteFrames(&runner, idle_frames, 10);
}

void load_images()
{
    // Load Coin Img
    iLoadImage(&home_coin_img, "assets/img/objects/coin/coin_small.png");
    iScaleImage(&home_coin_img, 0.5);
}

void iAnimSprites()
{
    // Animate the runner sprite
    iAnimateSprite(&runner);
}

/*
function iDraw() is called again and again by the system.
*/
void iDraw()
{
    // place your drawing codes here
    iClear();
    if (currentPage == HOME)
    {
        iClear();
        // Load Bg Img
        iShowImage(0, 0, "assets/img/bg/home_bg_002.jpg");
        // Space for coins
        loadCoinData();
        // Draw Runner Sprite
        iSetSpritePosition(&runner, 192, 173);
        iShowSprite(&runner);

        // Draw home page buttons
        for (int i = 0; i < BTN_TOTAL; i++)
        {
            draw_home_page_button(i, home_option_color[i], home_option_labels[i]);
        }
    }
    else if (currentPage == PLAY)
    {
        iClear();
        iText(50, 50, "Play Page");
    }
    else if (currentPage == RESUME)
    {
        iClear();
        iText(50, 50, "Resume Page");
    }
    else if (currentPage == SCENES)
    {
        iClear();
        // Load Bg Img
        iShowImage(0, 0, "assets/img/bg/scene_bg_001.jpeg");
        // Space for coins
        loadCoinData();

        iSetTransparentColor(20, 20, 20, 0.85);
        iFilledRectangle(0, 100, 1000, 400);

        //  Scenes
        iSetColor(255, 255, 255);
        
        char scene_name[3][50] = {"Ground Runner", "Mountain Runner", "Horror Runner"};
        for (int i = 0; i < 3; i++)
        {
            char scene_status[20];
            if(unlock_status[i] == 1 && selected_status[i] == 1){
                strcpy(scene_status, "Selected");
                iSetColor(2, 168, 77);
                iFilledRectangle(55 * (i + 1) + 260 * i - 3, 207 - 3, 266, 192);
                iSetColor(255, 255, 255);
            }else if(unlock_status[i] == 0){
                strcpy(scene_status, "Locked");
            }else{
                strcpy(scene_status, "Unlocked");
            }

            iShowImage(55 * (i + 1) + 260 * i, 207, "assets/img/scenes/1.png");
            //char scene_name[50] = "Gound Runner";
            float text_scale_1 = 0.15;
            float text_width_1 = get_text_width(scene_status, text_scale_1, GLUT_STROKE_MONO_ROMAN);
            iTextAdvanced(55 * (i + 1) + 260 * i + (260 - text_width_1) / 2, 410, scene_status, text_scale_1, 2, GLUT_STROKE_MONO_ROMAN);

            float text_width_2 = get_text_width(scene_name[i], text_scale_1, GLUT_STROKE_MONO_ROMAN);
            iTextAdvanced(55 * (i + 1) + 260 * i + (260 - text_width_2) / 2, 180, scene_name[i], text_scale_1, 1, GLUT_STROKE_MONO_ROMAN);

            if (unlock_status[i] == 0)
            {
                iShowImage(55 * (i + 1) + 260 * i + 97.5, 207 + 52, "assets/img/scenes/lock.png");
            }
        }
    }
    else if (currentPage == LEADERBOARD)
    {
        iClear();
        // Load Bg Img
        iShowImage(0, 0, "assets/img/bg/leaderboard_bg_001.jpg");
        // Space for coins
        loadCoinData();
        // Text
        char str[20] = "Leaderboard";
        float text_scale = 0.25f;
        float text_width = get_text_width(str, text_scale, GLUT_STROKE_MONO_ROMAN);

        iSetTransparentColor(20, 20, 20, 0.5);
        iFilledRectangle((SCRN_WIDTH / 2 - text_width), 505, (text_width * 2), 50);
        iFilledRectangle((SCRN_WIDTH / 2 - text_width), 45, (text_width * 2), 395);

        iSetColor(255, 255, 255);
        iTextAdvanced((SCRN_WIDTH - text_width) / 2, 520, str, text_scale, 2, GLUT_STROKE_MONO_ROMAN);

        iLine((SCRN_WIDTH / 2 - text_width), 400, (SCRN_WIDTH / 2 - text_width) + (text_width * 2), 400);
        // First row of Table Head
        iTextAdvanced((SCRN_WIDTH / 2 - text_width) + 50, 415, "Position", 0.12, 1, GLUT_STROKE_ROMAN);
        iTextAdvanced((SCRN_WIDTH / 2 - text_width) + 160, 415, "Name", 0.12, 1, GLUT_STROKE_ROMAN);
        iTextAdvanced((SCRN_WIDTH / 2 - text_width) + 440, 415, "Score", 0.12, 1, GLUT_STROKE_ROMAN);

        // All Rows of Table Body

        for (int i = 0; i < 8; i++)
        {
            if (i == 0)
            {
                iShowImage((SCRN_WIDTH / 2 - text_width) + 50, 345 - 40 * i, "assets/img/objects/medal/gold.png");
            }
            else if (i == 1)
            {
                iShowImage((SCRN_WIDTH / 2 - text_width) + 50, 345 - 40 * i, "assets/img/objects/medal/silver.png");
            }
            else if (i == 2)
            {
                iShowImage((SCRN_WIDTH / 2 - text_width) + 50, 345 - 40 * i, "assets/img/objects/medal/bronze.png");
            }
            else
            {
                char x[5];
                x[0] = i + 48 + 1;
                x[1] = '\0';
                iTextAdvanced((SCRN_WIDTH / 2 - text_width) + 50, 350 - 40 * i, x, 0.12, 1, GLUT_STROKE_ROMAN);
            }
            iTextAdvanced((SCRN_WIDTH / 2 - text_width) + 160, 350 - 40 * i, "Sifat", 0.12, 1, GLUT_STROKE_ROMAN);
            iTextAdvanced((SCRN_WIDTH / 2 - text_width) + 440, 350 - 40 * i, "500", 0.12, 1, GLUT_STROKE_ROMAN);
        }
    }
    else if (currentPage == HELP)
    {
        iClear();
        // Load Bg Img
        iShowImage(0, 0, "assets/img/bg/help_bg_001.png");
        iShowSprite(&runner);
        // Space for coins
        loadCoinData();
    }
}

/*
function iMouseMove() is called when the user moves the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouseMove(int mx, int my)
{
    if (currentPage == HOME)
    {
        for (int i = 0; i < BTN_TOTAL; i++)
        {
            int x1 = home_option_x;
            int x2 = x1 + home_option_w;
            int y1 = get_home_option_y(i);
            int y2 = y1 + home_option_h;
            if ((mx >= x1 && mx <= x2) && (my >= y1 && my <= y2))
                home_option_color[i] = 1;
            else
                home_option_color[i] = 0;
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
        if (currentPage == HOME)
        {
            for (int i = 0; i < BTN_TOTAL; i++)
            {
                int x1 = home_option_x;
                int x2 = x1 + home_option_w;
                int y1 = get_home_option_y(i);
                int y2 = y1 + home_option_h;
                if ((mx >= x1 && mx <= x2) && (my >= y1 && my <= y2))
                {
                    switch (i + 1)
                    {
                    case 1:
                        currentPage = PLAY;
                        break;

                    case 2:
                        currentPage = RESUME;
                        break;

                    case 3:
                        currentPage = SCENES;
                        break;

                    case 4:
                        currentPage = LEADERBOARD;
                        break;

                    case 5:
                        currentPage = HELP;
                        iSetSpritePosition(&runner, 142, 173);
                        break;

                    default:
                        exit(0);
                        break;
                    }
                }
            }
        }
        else if (currentPage == SCENES)
        {
            // Check if any scene is clicked
            for (int i = 0; i < 3; i++)
            {
                int x1 = 55 * (i + 1) + 260 * i;
                int x2 = x1 + 260;
                int y1 = 207;
                int y2 = y1 + 192;
                if ((mx >= x1 && mx <= x2) && (my >= y1 && my <= y2) && unlock_status[i] == 1)
                {
                    memset(selected_status, 0, sizeof(selected_status));
                    selected_status[i] = 1;
                }
            }
        }
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
        // Escape key
    case 27:
        if (currentPage == HELP || currentPage == LEADERBOARD || currentPage == SCENES)
        {
            iSetSpritePosition(&runner, sprite_x, sprite_y);
            currentPage = HOME;
        }
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
        iSetSpritePosition(&runner, sprite_x, sprite_y);
        currentPage = HOME;
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
    initialize_sprites();
    load_images();
    iSetTimer(100, iAnimSprites);
    iInitialize(SCRN_WIDTH, SCRN_HEIGHT, "Obstacle Runner");
    return 0;
}
