#include "iGraphics.h"
#include "iSound.h"
#include <time.h>

#define SCRN_WIDTH 1000
#define SCRN_HEIGHT 600
#define BTN_TOTAL 6
#define BTN_EXIT (BTN_TOTAL - 1)
#define MAX_SLIDE_DURATION 1.f
#define JUMP_FRAME_OFFSET 10
#define SLIDE_FRAME_OFFSET 20
#define DEAD_FRAME_OFFSET 30
#define MAX_OBJECT 2
#define MAX_COIN 3

enum page_status
{
    HOME,
    PLAY,
    RESUME,
    SCENES,
    LEADERBOARD,
    HELP,
    EXIT,
    GAME_PAUSED
};

// Sound Variables
int is_sound_on = 1;
int sound_bg_chnl;
int running_sound;

enum page_status currentPage; // To track which page is being visited currently
Image level_bg_img[3];        // Background image for the level
Image home_coin_img;          // Image of coin on the home page
Image game_score_img;         // Image istead of score word in game
Sprite runner;                // Sprite for the runner character
int home_option_color[BTN_TOTAL] = {0};
// Button labels and positions for home page options
const char *home_option_labels[BTN_TOTAL] = {"New Game", "Saved Game", "Scenes", "Leaderboard", "Help", "Exit"};
const int home_option_x = 690;
const int home_option_w = 250;
const int home_option_h = 40;
const int runner_y_initial = 136;

// Sprite Images
Image idle_frames[10];
Image movement_frames[4 * 10];

// coordinate of the sprite image
int sprite_x = 192;
int sprite_y = 173;

// Game variables
int unlock_status[3] = {1, 1, 0};
int selected_status[3] = {1, 0, 0};
int game_running = 0;
int is_dying = 0;
int is_dying_counter = 0;
int in_game_score = 0;
int in_game_earned_coin = 0;
int is_paused = 0;
int is_game_over = 0;

// Menu variables
int paused_btn_highlight[2];
int gameover_btn_highlight[2];

// Username input string
char username[18] = "";
int start_btn_highlight = 0;

// Caret state for username input
int caret_visible = 1;
int caret_timer = 0;

float scene_scroll = 0.f;
float scene_scroll_velocity = 30.f;

// Jumping vairables
int is_jumping = 0;
int is_super_jumping = 0;
float jump_time = 0.0f;
float jump_duration = 0.8f;
float jump_velocity = 420.0f;
float super_jump_velocity = 650.0f;
float g = 1000.0f;

// Sliding variables
int is_sliding = 0;
float slide_time = 0.f;

// Obstacle variable
int object_active[MAX_OBJECT] = {0};
int object_idx[MAX_OBJECT] = {0};
int object_x[MAX_OBJECT];
int object_y[MAX_OBJECT];
int object_w[MAX_OBJECT], object_h[MAX_OBJECT];

// Coin variables
int coin_active[MAX_COIN];
int coin_collided[MAX_COIN];
int coin_x[MAX_COIN];
int coin_y[MAX_COIN];
int coin_w = 45 * 0.5, coin_h = 45 * 0.5;

// Objects img
Image box_img;
Image pillar_img;
Image stone_img;

// Objects Sprite
Sprite box;
Sprite stone;
Sprite pillar;

int get_random_object1_x()
{
    return 900 + rand() % 200; // random x between 900 and 1100
}

int check_collision_jumping(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    return (x1 + w1 / 2 >= x2 - w2 / 2 && y1 <= y2 + h2 && x1 - w1 / 2 <= x2 + w2 / 2);
}

int check_collision_sliding(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    return (x1 + w1 / 2 >= x2 - w2 / 2 && y1 + 6 * h1 / 5 >= y2 - 3 * h2 / 5 && x1 - w1 / 2 <= x2 + w2 / 2);
}

int check_collision_with_coin(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    return (x1 + w1 >= x2 - w2 / 2 && y1 - h1 / 2 <= y2 + h2 / 2 && x1 - w1 / 2 <= x2 + w2 / 2);
}

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
    // First 10 running, second 10 jumping, third 10 sliding, last 10 dead
    iLoadFramesFromFolder(movement_frames, "assets/img/sprite/running");
    iLoadFramesFromFolder(movement_frames + JUMP_FRAME_OFFSET, "assets/img/sprite/jump");
    iLoadFramesFromFolder(movement_frames + SLIDE_FRAME_OFFSET, "assets/img/sprite/slide");
    iLoadFramesFromFolder(movement_frames + DEAD_FRAME_OFFSET, "assets/img/sprite/dead");

    iInitSprite(&runner, -1);
    iSetSpritePosition(&runner, sprite_x, sprite_y);
    iScaleSprite(&runner, 0.23f);
    iChangeSpriteFrames(&runner, idle_frames, 10);
    
    iInitSprite(&box, -1);
    iSetSpritePosition(&box, sprite_x, sprite_y);
    iScaleSprite(&box, 1.5f);
    iChangeSpriteFrames(&box, &box_img, 1);
    
    iInitSprite(&stone, -1);
    iSetSpritePosition(&stone, sprite_x, sprite_y);
    iScaleSprite(&stone, 1.5f);
    iChangeSpriteFrames(&stone, &stone_img, 1);

    iInitSprite(&pillar, -1);
    iSetSpritePosition(&pillar, sprite_x, sprite_y);
    iScaleSprite(&pillar, 1.6f);
    iChangeSpriteFrames(&pillar, &pillar_img, 1);

    box.x = 0;
    box.y = 0;
    stone.x = 0;
    stone.y = 0;
    pillar.x = 0;
    pillar.y = 0;
}

void load_images()
{
    // Load Coin Img
    iLoadImage(&home_coin_img, "assets/img/objects/coin/coin_small.png");
    iScaleImage(&home_coin_img, 0.5);
    iLoadImage(&game_score_img, "assets/img/objects/medal/score.png");
    iScaleImage(&game_score_img, 0.07);
    iLoadImage(&box_img, "assets/img/objects/killer/box.png");
    iLoadImage(&stone_img, "assets/img/objects/killer/stone.png");
    iLoadImage(&pillar_img, "assets/img/objects/killer/pillar.png");

    for (int i = 0; i < 3; i++)
    {
        char bg_path[50];
        sprintf(bg_path, "assets/img/bg/game_bg_%03d.png", i + 1);
        printf("%s\n", bg_path);
        iLoadImage(&level_bg_img[i], bg_path);
    }
}

void iAnimateSpriteWithOffset(Sprite *sprite)
{
    if (!sprite || sprite->totalFrames <= 1 || !sprite->frames)
        return;

    if (is_jumping || is_super_jumping)
        sprite->currentFrame = (sprite->currentFrame + 1) % 10 + JUMP_FRAME_OFFSET;
    else if (is_sliding)
        sprite->currentFrame = (sprite->currentFrame + 1) % 10 + SLIDE_FRAME_OFFSET;
    else if (is_dying)
        sprite->currentFrame = (sprite->currentFrame + 1) % 10 + DEAD_FRAME_OFFSET;
    else
        sprite->currentFrame = (sprite->currentFrame + 1) % 10;

    iUpdateCollisionMask(sprite);
}

void iAnimSprites()
{
    if (is_paused || is_game_over)
        return;

    iAnimateSpriteWithOffset(&runner);
    if (currentPage == PLAY && game_running)
    {
        scene_scroll += scene_scroll_velocity;
        if (scene_scroll > 2000)
            scene_scroll = 0;
    }

    if (currentPage == PLAY && is_jumping && game_running)
    {
        jump_time += 0.1f;
        float y = runner_y_initial + jump_velocity * jump_time - 0.5f * g * jump_time * jump_time;
        if (y <= runner_y_initial)
        {
            y = runner_y_initial;
            is_jumping = 0;
            jump_time = 0.0f;
            runner.currentFrame = 0;
        }
        iSetSpritePosition(&runner, runner.x, y);
    }

    if (currentPage == PLAY && is_super_jumping && game_running)
    {
        jump_time += 0.1f;
        float y = runner_y_initial + super_jump_velocity * jump_time - 0.5f * g * jump_time * jump_time;
        if (y <= runner_y_initial)
        {
            y = runner_y_initial;
            is_super_jumping = 0;
            jump_time = 0.0f;
            runner.currentFrame = 0;
        }
        iSetSpritePosition(&runner, runner.x, y);
    }

    if (currentPage == PLAY && is_sliding && game_running)
    {
        slide_time += 0.1f;
        if (slide_time >= MAX_SLIDE_DURATION)
        {
            is_sliding = 0;
            slide_time = 0.f;
            runner.currentFrame = 0;
        }
        iSetSpritePosition(&runner, runner.x, runner.y);
    }

    if (currentPage == PLAY && game_running)
    {

        for (int i = 0; i < MAX_OBJECT; i++)
        {
            if (!object_active[i] || object_x[i] + object_w[i] < 0)
            {
                object_idx[i] = rand() % 3;
                if (object_idx[i] == 0)
                {
                    object_w[i] = 45 * 1.5, object_h[i] = 35 * 1.5;
                }
                else if (object_idx[i] == 1)
                {
                    object_w[i] = 45 * 1.5, object_h[i] = 130 * 1.5;
                }
                else if (object_idx[i] == 2)
                {
                    object_w[i] = 76 * 1.6, object_h[i] = 77 * 1.6;
                }
                if (i == 0)
                {
                    object_x[i] = get_random_object1_x() + 400;
                }
                else if (i == 1)
                {
                    object_x[i] = object_x[0] + 750;
                }
                if (object_idx[i] == 2)
                {
                    object_y[i] = runner_y_initial + 100;
                }
                else
                {
                    object_y[i] = runner_y_initial;
                }
                object_active[i] = 1;
            }
            else
            {
                object_x[i] -= scene_scroll_velocity;
            }

            if (object_idx[i] == 0)
            {
                iSetSpritePosition(&box, object_x[i], object_y[i]);
            }
            else if (object_idx[i] == 1)
            {
                iSetSpritePosition(&pillar, object_x[i], object_y[i]);
            }
            else if (object_idx[i] == 2)
            {
                iSetSpritePosition(&stone, object_x[i], object_y[i]);
            }
        }

        // Check collision with runner
        int runner_w = runner.frames[runner.currentFrame].width * runner.scale;
        int runner_h = runner.frames[runner.currentFrame].height * runner.scale;

        for (int i = 0; i < MAX_COIN; i++)
        {
            if (!coin_active[i] || coin_x[i] + coin_w < 0)
            {
                if (coin_collided[i] && coin_x[i] + coin_w > 0)
                {
                    coin_x[i] -= scene_scroll_velocity;
                }
                else
                {
                    coin_active[i] = 1;
                    int on_top = rand() % 2;
                    if (i == 0 && on_top)
                    {
                        int obj_idx = 0;
                        coin_x[i] = object_x[obj_idx] + (object_w[obj_idx] / 2) - (coin_w / 2);
                        coin_y[i] = object_y[obj_idx] + object_h[obj_idx] + 20; // 20 pixels above the object
                    }
                    else
                    {
                        coin_y[i] = runner_y_initial;
                        coin_x[i] = object_x[0] + rand() % (600 - 100 + 1) + 100;
                    }
                    coin_collided[i] = 0;
                }
            }
            else
            {
                coin_x[i] -= scene_scroll_velocity;
            }

            if (coin_active[i] && check_collision_with_coin(runner.x, runner.y, runner_w, runner_h, coin_x[i], coin_y[i], coin_w, coin_h))
            {
                coin_active[i] = 0;
                coin_collided[i] = 1;
                if (!is_dying)
                    in_game_earned_coin++;
                if (is_sound_on)
                {
                    iPlaySound("assets/sound/coin_collect.wav", false, 50);
                }
            }
        }

        if (currentPage == PLAY && game_running)
        {
            bool cond = false;
            for (int i = 0; i < MAX_OBJECT; i++)
            {
                if (object_active[i])
                {
                    if (object_idx[i] == 0)
                        cond = cond || iCheckCollision(&runner, &box);
                    if (object_idx[i] == 1)
                        cond = cond || iCheckCollision(&runner, &pillar);
                    if (object_idx[i] == 2)
                        cond = cond || iCheckCollision(&runner, &stone);
                }
            }
            if (cond)
            {
                is_jumping = 0;
                is_super_jumping = 0;
                is_sliding = 0;
                is_dying = 1;
                scene_scroll_velocity = 0;
            }
        }

        if (is_dying == 1)
        {
            is_dying_counter++;
            if (is_dying_counter == 8)
            {
                scene_scroll_velocity = 30.0f;
                is_dying = 0;
                is_dying_counter = 0;
                for (int i = 0; i < MAX_OBJECT; i++)
                {
                    object_active[i] = 0;
                }
                runner.y = runner_y_initial;

                memset(coin_x, 0, sizeof(coin_x));
                memset(coin_y, 0, sizeof(coin_y));
                is_game_over = 1;
            }
            if (is_dying_counter == 1)
            {
                if (is_sound_on)
                {
                    iPlaySound("assets/sound/game_over.wav", false, 50);
                }
            }
        }
        else
        {
            in_game_score += 2;
        }
    }
}

void iAnimCaret()
{
    if (game_running == 1 || currentPage != PLAY)
        return;
    caret_timer++;
    if (caret_timer >= 4)
    {
        caret_visible = !caret_visible;
        caret_timer = 0;
    }
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
        // Draw scrolling background
        int bg_width = 2000;
        int bg_x1 = -scene_scroll;
        int bg_x2 = bg_x1 + bg_width;
        iShowLoadedImage(bg_x1, 0, &level_bg_img[0]);
        iShowLoadedImage(bg_x2, 0, &level_bg_img[0]);

        if (game_running == 0 && is_dying == 0)
        {
            int object1_w = 350, object1_h = 50;
            iSetTransparentColor(20, 20, 20, 0.45);
            iFilledRectangle(0, 0, SCRN_WIDTH, SCRN_HEIGHT);

            int object1_x = (SCRN_WIDTH - object1_w) / 2;
            int object1_y = (SCRN_HEIGHT - object1_h) / 2 + 60;
            // Draw label
            char *label = "Input Username";
            float label_scale = 0.13f;
            float label_width = get_text_width(label, label_scale, GLUT_STROKE_MONO_ROMAN);
            iSetColor(255, 255, 255);
            iTextAdvanced(object1_x + (object1_w - label_width) / 2, object1_y + object1_h + 18, label, label_scale, 1, GLUT_STROKE_MONO_ROMAN);
            // Draw textbox
            iRectangle(object1_x, object1_y, object1_w, object1_h);
            float scale = 0.18f;
            float text_width = get_text_width(username, scale, GLUT_STROKE_MONO_ROMAN);
            iTextAdvanced(object1_x + (object1_w - text_width) / 2, object1_y + 13, username, scale, 1, GLUT_STROKE_MONO_ROMAN);
            // Draw caret
            if (caret_visible && strlen(username) < 18)
            {
                float caret_x = object1_x + (object1_w - text_width) / 2 + text_width + 2;
                int caret_y = object1_y + 10;
                iSetColor(255, 255, 255);
                iLine(caret_x, caret_y, caret_x, caret_y + 28);
            }

            // Draw the Start button
            int start_btn_w = 180, start_btn_h = 40;
            int start_btn_x = (SCRN_WIDTH - start_btn_w) / 2;
            int start_btn_y = object1_y - 70;
            if (start_btn_highlight)
                iSetTransparentColor(2, 168, 77, 0.4);
            else
                iSetColor(255, 255, 255);
            if (start_btn_highlight)
                iFilledRectangle(start_btn_x, start_btn_y, start_btn_w, start_btn_h);
            else
                iRectangle(start_btn_x, start_btn_y, start_btn_w, start_btn_h);
            const char *start_label = "Start";
            float start_text_width = get_text_width(start_label, scale, GLUT_STROKE_MONO_ROMAN);
            iSetColor(255, 255, 255);
            iTextAdvanced(start_btn_x + (start_btn_w - start_text_width) / 2, start_btn_y + 13, start_label, scale, 1, GLUT_STROKE_MONO_ROMAN);
        }

        else
        {
            if (!is_game_over)
                iShowSprite(&runner);

            iPauseSound(sound_bg_chnl);
            // Draw objects
            for (int i = 0; i < MAX_OBJECT; i++)
            {
                if (object_active[i])
                {
                    if (object_idx[i] == 0)
                    {
                        iSetSpritePosition(&box, object_x[i], object_y[i]);
                        iShowSprite(&box);
                    }
                    else if (object_idx[i] == 1)
                    {
                        iSetSpritePosition(&pillar, object_x[i], object_y[i]);
                        iShowSprite(&pillar);
                    }
                    else if (object_idx[i] == 2)
                    {
                        iSetSpritePosition(&stone, object_x[i], object_y[i]);
                        iShowSprite(&stone);

                    }
                }
            }
            if (!is_paused && !is_game_over)
            {
                // Draw coins
                for (int i = 0; i < MAX_COIN; i++)
                {
                    if (coin_active[i])
                    {
                        iShowLoadedImage(coin_x[i], coin_y[i], &home_coin_img);
                    }
                }

                iSetTransparentColor(20, 20, 20, 0.7);
                iFilledRectangle(860, 530, 140, 80);
                iSetColor(255, 255, 255);
                iShowLoadedImage(875, 572, &home_coin_img);
                iShowLoadedImage(875, 540, &game_score_img);

                char in_game_coin_str[10];
                sprintf(in_game_coin_str, "%d", in_game_earned_coin);
                iTextAdvanced(875 + 40, 577, in_game_coin_str, 0.1, 1, GLUT_STROKE_MONO_ROMAN); // for coin

                char in_game_score_str[10];
                sprintf(in_game_score_str, "%d", in_game_score);
                iTextAdvanced(875 + 40, 545, in_game_score_str, 0.1, 1, GLUT_STROKE_MONO_ROMAN); // for score
            }
        }

        if (is_paused == 1)
        {
            iSetTransparentColor(20, 20, 20, 0.45);
            iFilledRectangle(0, 0, SCRN_WIDTH, SCRN_HEIGHT);
            // Game Paused text
            const char *paused_label = "Game Paused";
            float paused_scale = 0.22f;
            float paused_width = get_text_width(paused_label, paused_scale, GLUT_STROKE_MONO_ROMAN);
            iSetColor(255, 255, 255);
            iTextAdvanced((SCRN_WIDTH - paused_width) / 2, SCRN_HEIGHT - 120, paused_label, paused_scale, 2, GLUT_STROKE_MONO_ROMAN);
            // Buttons
            int btn_w = 250, btn_h = 50;
            int btn_x = (SCRN_WIDTH - btn_w) / 2;
            int btn_y_resume = SCRN_HEIGHT / 2 + 10;
            int btn_y_exit = SCRN_HEIGHT / 2 - 70;
            // Resume button
            if (paused_btn_highlight[0])
                iSetTransparentColor(2, 168, 77, 0.4);
            else
                iSetColor(255, 255, 255);
            if (paused_btn_highlight[0])
                iFilledRectangle(btn_x, btn_y_resume, btn_w, btn_h);
            else
                iRectangle(btn_x, btn_y_resume, btn_w, btn_h);
            const char *resume_label = "Resume";
            float resume_width = get_text_width(resume_label, 0.15f, GLUT_STROKE_MONO_ROMAN);
            iSetColor(255, 255, 255);
            iTextAdvanced(btn_x + (btn_w - resume_width) / 2, btn_y_resume + 15, resume_label, 0.15f, 1, GLUT_STROKE_MONO_ROMAN);
            // Exit to Menu button
            if (paused_btn_highlight[1])
                iSetTransparentColor(168, 2, 2, 0.4);
            else
                iSetColor(255, 255, 255);
            if (paused_btn_highlight[1])
                iFilledRectangle(btn_x, btn_y_exit, btn_w, btn_h);
            else
                iRectangle(btn_x, btn_y_exit, btn_w, btn_h);
            const char *exit_label = "Exit to Menu";
            float exit_width = get_text_width(exit_label, 0.15f, GLUT_STROKE_MONO_ROMAN);
            iSetColor(255, 255, 255);
            iTextAdvanced(btn_x + (btn_w - exit_width) / 2, btn_y_exit + 15, exit_label, 0.15f, 1, GLUT_STROKE_MONO_ROMAN);
        }

        if (is_game_over)
        {
            iSetTransparentColor(20, 20, 20, 0.55);
            iFilledRectangle(0, 0, SCRN_WIDTH, SCRN_HEIGHT);
            const char *over_label = "Game Over!";
            float over_scale = 0.22f;
            float over_width = get_text_width(over_label, over_scale, GLUT_STROKE_MONO_ROMAN);
            iSetColor(255, 0, 0);
            iTextAdvanced((SCRN_WIDTH - over_width) / 2, SCRN_HEIGHT - 120, over_label, over_scale, 2, GLUT_STROKE_MONO_ROMAN); // Show score and coin count

            // Stats info
            char score_str[32], coin_str[32];
            sprintf(score_str, "Score: %d", in_game_score);
            sprintf(coin_str, "Coins: %d", in_game_earned_coin);
            float info_scale = 0.15f;
            float score_width = get_text_width(score_str, info_scale, GLUT_STROKE_MONO_ROMAN);
            float coin_width = get_text_width(coin_str, info_scale, GLUT_STROKE_MONO_ROMAN);
            iSetColor(255, 255, 0);
            iTextAdvanced((SCRN_WIDTH - score_width) / 2, SCRN_HEIGHT / 2 + 120, score_str, info_scale, 1, GLUT_STROKE_MONO_ROMAN);
            iSetColor(255, 255, 0);
            iTextAdvanced((SCRN_WIDTH - coin_width) / 2, SCRN_HEIGHT / 2 + 90, coin_str, info_scale, 1, GLUT_STROKE_MONO_ROMAN);

            // Buttons
            int btn_w = 250, btn_h = 50;
            int btn_x = (SCRN_WIDTH - btn_w) / 2;
            int btn_y_play = SCRN_HEIGHT / 2 + 10;
            int btn_y_exit = SCRN_HEIGHT / 2 - 70;
            // Play Again button
            if (gameover_btn_highlight[0])
                iSetTransparentColor(2, 168, 77, 0.4);
            else
                iSetColor(255, 255, 255);
            if (gameover_btn_highlight[0])
                iFilledRectangle(btn_x, btn_y_play, btn_w, btn_h);
            else
                iRectangle(btn_x, btn_y_play, btn_w, btn_h);
            const char *play_label = "Play Again";
            float play_width = get_text_width(play_label, 0.15f, GLUT_STROKE_MONO_ROMAN);
            iSetColor(255, 255, 255);
            iTextAdvanced(btn_x + (btn_w - play_width) / 2, btn_y_play + 15, play_label, 0.15f, 1, GLUT_STROKE_MONO_ROMAN);
            // Exit to Menu button
            if (gameover_btn_highlight[1])
                iSetTransparentColor(168, 2, 2, 0.4);
            else
                iSetColor(255, 255, 255);
            if (gameover_btn_highlight[1])
                iFilledRectangle(btn_x, btn_y_exit, btn_w, btn_h);
            else
                iRectangle(btn_x, btn_y_exit, btn_w, btn_h);
            const char *exit_label = "Exit to Menu";
            float exit_width = get_text_width(exit_label, 0.15f, GLUT_STROKE_MONO_ROMAN);
            iSetColor(255, 255, 255);
            iTextAdvanced(btn_x + (btn_w - exit_width) / 2, btn_y_exit + 15, exit_label, 0.15f, 1, GLUT_STROKE_MONO_ROMAN);
        }
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
            if (unlock_status[i] == 1 && selected_status[i] == 1)
            {
                strcpy(scene_status, "Selected");
                iSetColor(2, 168, 77);
                iFilledRectangle(55 * (i + 1) + 260 * i - 3, 207 - 3, 266, 192);
                iSetColor(255, 255, 255);
            }
            else if (unlock_status[i] == 0)
            {
                strcpy(scene_status, "Locked");
            }
            else
            {
                strcpy(scene_status, "Unlocked");
            }

            iShowImage(55 * (i + 1) + 260 * i, 207, "assets/img/scenes/1.png");
            // char scene_name[50] = "Gound Runner";
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

        char help_page_title[3][50] = {"Navigation", "Game Control", "Developers"};
        float txt_scale = 0.2f;
        float text_width[3];
        for (int i = 0; i < 3; i++)
        {
            int multiplier;
            if (i == 1)
            {
                multiplier = 170;
            }
            else
            {
                multiplier = 200;
            }
            iSetTransparentColor(20, 20, 20, 0.7);
            iFilledRectangle(500, 500 - multiplier * i, 400, 50);
            iSetColor(255, 255, 255);

            text_width[i] = get_text_width(help_page_title[i], txt_scale, GLUT_STROKE_ROMAN);
            iTextAdvanced(500 + (400 - text_width[i]) / 2, 518 - multiplier * i, help_page_title[i], txt_scale, 1.0, GLUT_STROKE_ROMAN);

            if (i == 0)
            {
                iSetColor(0, 0, 0);
                iTextAdvanced(515, 518 - multiplier * i - 50, "* Press ESC button from anywhere to go back", 0.12, 1.0, GLUT_STROKE_ROMAN);
                iTextAdvanced(515, 518 - multiplier * i - 80, "* Press B/b button from anywhere to turn off", 0.12, 1.0, GLUT_STROKE_ROMAN);
                iTextAdvanced(515, 518 - multiplier * i - 100, "  Sound", 0.12, 1.0, GLUT_STROKE_ROMAN);
            }
            else if (i == 1)
            {
                iSetColor(0, 0, 0);
                iTextAdvanced(515, 518 - multiplier * i - 50, " W: For Jump", 0.12, 1.0, GLUT_STROKE_ROMAN);
                iTextAdvanced(515, 518 - multiplier * i - 80, " S: For Slide", 0.12, 1.0, GLUT_STROKE_ROMAN);
                iTextAdvanced(515, 518 - multiplier * i - 110, " D: For High Jump", 0.12, 1.0, GLUT_STROKE_ROMAN);
                iSetColor(255, 0, 0);
                iTextAdvanced(515, 518 - multiplier * i - 140, " Note: You can press the buttons now to ", 0.12, 1.0, GLUT_STROKE_ROMAN);
                iTextAdvanced(515, 518 - multiplier * i - 160, " watch demo!", 0.12, 1.0, GLUT_STROKE_ROMAN);
            }
            else if (i == 2)
            {
                iSetColor(0, 0, 0);
                iTextAdvanced(515, 518 - multiplier * i - 50, " Under the supervision of Abdur Rafi Sir,", 0.12, 1.0, GLUT_STROKE_ROMAN);
                iSetColor(255, 255, 255);
                iTextAdvanced(515 + get_text_width(" Under the supervision of", 0.12, GLUT_STROKE_ROMAN), 518 - multiplier * i - 50, " Abdur Rafi Sir,", 0.12, 1.0, GLUT_STROKE_ROMAN);
                iSetColor(0, 0, 0);
                iTextAdvanced(515, 518 - multiplier * i - 80, " This game was developed by", 0.12, 1.0, GLUT_STROKE_ROMAN);
                iSetColor(255, 255, 255);
                iTextAdvanced(515, 518 - multiplier * i - 110, " Shabit Zaman (2405013) & Sifat Al Islam (2405014)", 0.11, 1.0, GLUT_STROKE_ROMAN);
            }
        }
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
    else if (currentPage == PLAY)
    {
        if (game_running == 0)
        {
            int object1_h = 50;
            int object1_y = (SCRN_HEIGHT - object1_h) / 2 + 60;
            int start_btn_w = 180, start_btn_h = 40;
            int start_btn_x = (SCRN_WIDTH - start_btn_w) / 2;
            int start_btn_y = object1_y - 70;
            if (strlen(username) > 0 && (mx >= start_btn_x && mx <= start_btn_x + start_btn_w) && (my >= start_btn_y && my <= start_btn_y + start_btn_h))
                start_btn_highlight = 1;
            else
                start_btn_highlight = 0;
        }
    }
    if (is_paused == 1)
    {
        int btn_w = 250, btn_h = 50;
        int btn_x = (SCRN_WIDTH - btn_w) / 2;
        int btn_y_resume = SCRN_HEIGHT / 2 + 10;
        int btn_y_exit = SCRN_HEIGHT / 2 - 70;
        paused_btn_highlight[0] = (mx >= btn_x && mx <= btn_x + btn_w && my >= btn_y_resume && my <= btn_y_resume + btn_h);
        paused_btn_highlight[1] = (mx >= btn_x && mx <= btn_x + btn_w && my >= btn_y_exit && my <= btn_y_exit + btn_h);
    }

    if (is_game_over)
    {
        int btn_w = 250, btn_h = 50;
        int btn_x = (SCRN_WIDTH - btn_w) / 2;
        int btn_y_play = SCRN_HEIGHT / 2 + 10;
        int btn_y_exit = SCRN_HEIGHT / 2 - 70;
        gameover_btn_highlight[0] = (mx >= btn_x && mx <= btn_x + btn_w && my >= btn_y_play && my <= btn_y_play + btn_h);
        gameover_btn_highlight[1] = (mx >= btn_x && mx <= btn_x + btn_w && my >= btn_y_exit && my <= btn_y_exit + btn_h);
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
        else if (game_running == 0 && strlen(username) > 0 && currentPage == PLAY && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            // Check if start button is clicked
            int object1_h = 50;
            int object1_y = (SCRN_HEIGHT - object1_h) / 2 + 60;
            int start_btn_w = 180, start_btn_h = 40;
            int start_btn_x = (SCRN_WIDTH - start_btn_w) / 2;
            int start_btn_y = object1_y - 70;
            if ((mx >= start_btn_x && mx <= start_btn_x + start_btn_w) && (my >= start_btn_y && my <= start_btn_y + start_btn_h))
            {
                game_running = 1;
                iSetSpritePosition(&runner, sprite_x, sprite_y);
                iChangeSpriteFrames(&runner, movement_frames, 4 * 10);
                iSetSpritePosition(&runner, SCRN_WIDTH / 2 - 300, runner_y_initial);
                iShowSprite(&runner);
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
        else if (currentPage == PLAY && is_paused == 1 && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            int btn_w = 250, btn_h = 50;
            int btn_x = (SCRN_WIDTH - btn_w) / 2;
            int btn_y_resume = SCRN_HEIGHT / 2 + 10;
            int btn_y_exit = SCRN_HEIGHT / 2 - 70;
            if (mx >= btn_x && mx <= btn_x + btn_w && my >= btn_y_resume && my <= btn_y_resume + btn_h)
            {
                is_paused = 0;
            }
            else if (mx >= btn_x && mx <= btn_x + btn_w && my >= btn_y_exit && my <= btn_y_exit + btn_h)
            {
                currentPage = HOME;
                game_running = 0;
                iSetSpritePosition(&runner, sprite_x, sprite_y);
                iChangeSpriteFrames(&runner, idle_frames, 10);
                is_paused = 0;
                in_game_score = 0;
                in_game_earned_coin = 0;
                memset(coin_x, 0, sizeof(coin_x));
                memset(coin_y, 0, sizeof(coin_y));
            }
        }
        else if (currentPage == PLAY && is_game_over && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            int btn_w = 250, btn_h = 50;
            int btn_x = (SCRN_WIDTH - btn_w) / 2;
            int btn_y_play = SCRN_HEIGHT / 2 + 10;
            int btn_y_exit = SCRN_HEIGHT / 2 - 70;
            if (mx >= btn_x && mx <= btn_x + btn_w && my >= btn_y_play && my <= btn_y_play + btn_h)
            {
                game_running = 1;
                is_game_over = 0;
                in_game_score = 0;
                in_game_earned_coin = 0;
                memset(coin_x, 0, sizeof(coin_x));
                memset(coin_y, 0, sizeof(coin_y));
            }
            else if (mx >= btn_x && mx <= btn_x + btn_w && my >= btn_y_exit && my <= btn_y_exit + btn_h)
            {
                currentPage = HOME;
                game_running = 0;
                iSetSpritePosition(&runner, sprite_x, sprite_y);
                iChangeSpriteFrames(&runner, idle_frames, 10);
                is_game_over = 0;
                in_game_score = 0;
                in_game_earned_coin = 0;
                memset(coin_x, 0, sizeof(coin_x));
                memset(coin_y, 0, sizeof(coin_y));
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

    if (currentPage == PLAY)
    {
        if (game_running == 0)
        {
            if (key == 8) // Backspace
            {
                int len = strlen(username);
                if (len > 0)
                    username[len - 1] = '\0';
            }
            else if (key >= 32 && key <= 126 && strlen(username) < 18)
            {
                int len = strlen(username);
                username[len] = key;
                username[len + 1] = '\0';
            }
        }
        else if (game_running == 1 && !is_paused && !is_game_over)
        {
            if (key == 27)
            {
                is_paused = !is_paused;
            }
            if (!is_jumping && !is_super_jumping && !is_sliding && !is_dying)
            {

                if (key == 'w')
                {
                    is_jumping = 1;
                    runner.currentFrame = 0;
                    jump_time = 0.0f;
                }
                if (key == 'd')
                {
                    is_super_jumping = 1;
                    runner.currentFrame = 0;
                    jump_time = 0.0f;
                }

                if (key == 's')
                {
                    is_sliding = 1;
                    runner.currentFrame = 0;
                }
            }
        }
    }

    if (key == 'B' || key == 'b')
    {
        if (is_sound_on == 1)
        {
            iPauseSound(sound_bg_chnl);
            is_sound_on = 0;
        }
        else
        {
            iResumeSound(sound_bg_chnl);
            is_sound_on = 1;
        }
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
    srand(time(0));
    glutInit(&argc, argv);
    // place your own initialization codes here.
    load_images();
    initialize_sprites();
    iSetTimer(50, iAnimSprites);
    iSetTimer(100, iAnimCaret); // Add caret animation timer

    // initialize sounds
    iInitializeSound();
    sound_bg_chnl = iPlaySound("assets/sound/bg.wav", true, 50);
    iInitialize(SCRN_WIDTH, SCRN_HEIGHT, "Obstacle Runner");
    return 0;
}
