#include <iostream>
#include <cstdlib>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <cmath>
#include <sstream>
#include <ctime>
#include <fstream>
#include <vector>
#include <algorithm>
#define MAX_L 20
#define MAX_B 100
using namespace std;
//-----------------------------------------------Classes-----------------------------------------------//
class image
{
private:
    SDL_Renderer *ren;
    SDL_Rect img_rect;

public:
    string path;
    SDL_Texture *img;
    int img_w, img_h;
    int x;
    int y;
    float sc_x;
    float sc_y;
    void run()
    {
        img = IMG_LoadTexture(ren, path.c_str());
        SDL_QueryTexture(img, NULL, NULL, &img_w, &img_h);
    }

    void load()
    {
        img_rect.x = x;
        img_rect.y = y;
        img_rect.w = img_w * sc_x;
        img_rect.h = img_h * sc_y;
        SDL_RenderCopy(ren, img, NULL, &img_rect);
    }
    image(string Path, int X, int Y, float SC, SDL_Renderer *Ren)
    {
        x = X;
        y = Y;
        path = Path;
        sc_x = SC;
        sc_y = SC;
        ren = Ren;
        run();
        load();
    }
    void setCenter(int X, int Y)
    {
        x = X - img_w * sc_x / 2;
        y = Y - img_h * sc_y / 2;
        load();
    }
    void render()
    {
        SDL_RenderCopy(ren, img, NULL, &img_rect);
    }
    ~image()
    {
        SDL_DestroyTexture(img);
    }
};
class btn
{
private:
    int x;
    int y;
    int he;
    int wi;

public:
    image *pic;
    btn(int X, int Y, string Path, float SC, SDL_Renderer *Ren)
    {
        pic = new image(Path, X, Y, SC, Ren);
        x = X;
        y = Y;
        he = pic->img_h;
        wi = pic->img_w;
    }
    ~btn()
    {
        delete pic;
    }
    void setXY(int X, int Y)
    {
        x = X;
        y = Y;
        pic->x = X;
        pic->y = Y;
        pic->load();
    }
    void setCenter(int X, int Y)
    {
        x = X - wi * pic->sc_x / 2;
        y = Y - he * pic->sc_y / 2;
        pic->x = x;
        pic->y = y;
        pic->load();
    }
    bool btn_clicked(int mouse_x, int mouse_y)
    {
        if (mouse_x >= x && mouse_x <= x + wi * pic->sc_x && mouse_y >= y && mouse_y <= y + he * pic->sc_y)
            return true;
        else
            return false;
    }
};
//----------------------------------------------Structs---------------------------------------------------------------------------------//
struct shootedBall
{
    int r, g, b;
    int x , y;
    bool ex;
    int type = 1;

};
shootedBall shootedBall[MAX_B];

struct Player {
    std::string username;
    int score;
};
struct shadow
{
    int r, g, b;
    int x , y;
    bool ex;
    int v;
};
shadow shadow[10][MAX_L];

//----------------------------------------------global variables-------------------------------------------------------------------//
const int xCannon = 720, yCannon = 600;
int xnxx, ynyy;
int xMouse, yMouse;
bool isShooted[MAX_B], isMoving[MAX_B];
int k = 0, shootedCount = 0;
int v = 8;
int dx[MAX_B], dy[MAX_B];
int lines = 12, m = 5;
double dY = 0.1, Y = 0.0;
int color[10][MAX_L];
int r[10][MAX_L], g[10][MAX_L], bi[10][MAX_L];
double xo[10][MAX_L], yo[10][MAX_L];
int rad = 25;
bool exist[10][MAX_L];
bool isIsland = false;
bool island[10][MAX_L];
bool isPaused = false;
double angle = 0.0;
int fireNum = 5, bombNum = 3, multiNum = 5, twoNum = 5;
bool gameOver = false;
int toops = 50;
int zaman = 120;
int yscore = 200;
int effect = 0;
int slowNum = 1, pauseNum = 1;
int P = 700, S = 700;
//vector<PlayerScore> scores;
TTF_Font* Fontt = TTF_OpenFont("calibrib", 24);
SDL_Event* e = new SDL_Event();
SDL_Event* s = new SDL_Event();
//-----------------------------------------------Functions-----------------------------------------------//
void betterText(SDL_Renderer *renderer, string S, int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a, int size, string Font);

string TextBox(SDL_Renderer *Renderer, int x_position, int y_position, int R, int G, int B);

void set_random_color1(int r[12][MAX_L], int g[12][MAX_L], int b[12][MAX_L]);

bool login(string UseEnter);

int registry(string newName);

string readNthValue(const string &filePath, int n);

bool searchStringInCSV(const string &filename, const string &target);

int findIndexInCSV(const string &filePath, const string &targetString);

void replaceNthStringInCSV(const string &filePath, int n, const string &replacement);

void betterTextM(SDL_Renderer *renderer, string S, int x, int y, int size, string Font);

bool isNameInFile(string name);

string intToString(int num);



void angleimage(SDL_Renderer* renderer,SDL_Texture *text, int x, int y, int w, int h, double alfa);

double ball_angle(double xnxx, double ynyy)
{
    double tetha = atan((ynyy - yCannon) / (xnxx - xCannon));
    if ((ynyy - yCannon > 0) && (xnxx - xCannon > 0))
    {
        tetha = (3.1415 / 2) - tetha;
    }
    if ((ynyy - yCannon < 0) && (xnxx - xCannon > 0))
    {
        tetha = (3.1415 / 2) - tetha;
    }
    if ((ynyy - yCannon < 0) && (xnxx - xCannon < 0))
    {
        tetha = (3 * 3.1415 / 2) - tetha;
    }
    if ((ynyy - yCannon > 0) && (xnxx - xCannon < 0))
    {
        tetha = (3 * 3.1415 / 2) - tetha;
    }
    return tetha;
}


void fire(double shootingAngle)
{
    if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_f && fireNum > 0)
    {
        e->type = 0;
        fireNum--;
        shootedBall[k].type = 2;
        shootingAngle = ball_angle(xMouse, yMouse);
        dx[k] = v * sin(shootingAngle);
        dy[k] = v * cos(shootingAngle);
        shootedBall[k+1].ex = true;
        isShooted[k] = true;
        isMoving[k] = true;
        k++;
    }
}

void bomb(double shootingAngle)
{
    if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_b && bombNum > 0)
    {
        e->type = 0;
        bombNum--;
        shootedBall[k].type = 3;
        shootingAngle = ball_angle(xMouse, yMouse);
        dx[k] = v * sin(shootingAngle);
        dy[k] = v * cos(shootingAngle);
        shootedBall[k+1].ex = true;
        isShooted[k] = true;
        isMoving[k] = true;
        k++;
    }
}

void multi_colored (double shootingAngle)
{
    if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_m && multiNum > 0)
    {
        e->type = 0;
        multiNum--;
        shootedBall[k].type = 5;
        shootingAngle = ball_angle(xMouse, yMouse);
        dx[k] = v * sin(shootingAngle);
        dy[k] = v * cos(shootingAngle);
        shootedBall[k+1].ex = true;
        isShooted[k] = true;
        isMoving[k] = true;
        k++;
    }
}

void two_colored_1 (double shootingAngle)
{
    if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_t && twoNum > 0)
    {
        e->type = 0;
        twoNum--;
        shootedBall[k].type = 4;
        shootingAngle = ball_angle(xMouse, yMouse);
        dx[k] = v * sin(shootingAngle);
        dy[k] = v * cos(shootingAngle);
        shootedBall[k+1].ex = true;
        isShooted[k] = true;
        isMoving[k] = true;
        k++;
    }
}

void two_colored_2 (double shootingAngle)
{
    if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_w && twoNum > 0)
    {
        e->type = 0;
        twoNum--;
        shootedBall[k].type = 6;
        shootingAngle = ball_angle(xMouse, yMouse);
        dx[k] = v * sin(shootingAngle);
        dy[k] = v * cos(shootingAngle);
        shootedBall[k+1].ex = true;
        isShooted[k] = true;
        isMoving[k] = true;
        k++;
    }
}

int find_m()
{
    int cnt[MAX_L];
    for (int j = 0; j < MAX_L; j++)
        cnt[j] = 0;
    for (int j = 0; j < MAX_L; j++)
        for (int i = 0; i < 10; i++)
            if (exist[i][j])
            {
                cnt[j]++;
                if (cnt[j] == 10)
                    return find_m();
            }
}

int connection(int i, int j)
{
    int c = 0;
    if (j % 2 == 1)
    {
        if (exist[i][j+1] && j != MAX_L-1)
            c++;
        if (exist[i+1][j+1] && j != MAX_L-1 && i != 9)
            c++;
        if (exist[i-1][j] && i != 0)
            c++;
        if (exist[i+1][j] && i != 9)
            c++;
        if (exist[i][j-1] && j != 0)
            c++;
        if (exist[i+1][j-1] && i != 9 && j != 0)
            c++;
        return c;
    }
    else
    {
        if (exist[i-1][j+1] && i != 0 && j != MAX_L-1)
            c++;
        if (exist[i][j+1] && j != MAX_L-1)
            c++;
        if (exist[i-1][j] && i != 0)
            c++;
        if (exist[i+1][j] && i != 9)
            c++;
        if (exist[i-1][j-1] && i != 0 && j != 0)
            c++;
        if (exist[i][j-1] && j != 0)
            c++;
        return c;
    }
}

double dist(int xo1, int yo1, int xo2, int yo2)
{
    return sqrt((yo2 - yo1)*(yo2 - yo1) + (xo2 - xo1)*(xo2 - xo1));
}

void pause_game()
{

}

void draw_moving_balls(SDL_Renderer* renderer)
{
    for(int j = 0; j < MAX_L; j++)
    {
        if (j%2 == 0)
        {
            for(int i = 0; i < 10; i++)
            {
                xo[i][j] = 0 + 1440/3 + i*2*rad;
//                shadow[i][j].x = xo[i][j];
                yo[i][j] = Y + rad + (lines - 1 - j) * rad * sqrt(3);
                if (exist[i][j])
                {
                    filledCircleRGBA(renderer, xo[i][j], yo[i][j],
                                     rad, r[i][j], g[i][j], bi[i][j], 250);
                    aacircleRGBA(renderer, xo[i][j], yo[i][j], rad,
                                 r[i][j], g[i][j], bi[i][j], 250);
                }
//                if (shadow[i][j].ex)
//                {
//                    shadow[i][j].y += shadow[i][j].v;
//                    filledCircleRGBA(renderer, shadow[i][j].x,
//shadow[i][j].y, rad, 245, 245, 245, 60);
//                }
            }
        }
        else
        {
            for(int i = 0; i < 10; i++)
            {
                xo[i][j] = 25 + 1440/3 + i*2*rad;
//                shadow[i][j].x = xo[i][j];
                yo[i][j] = Y + 25 + (lines - 1 - j) * rad * sqrt(3);
                if (exist[i][j])
                {
                    filledCircleRGBA(renderer, xo[i][j], yo[i][j],
                                     rad, r[i][j], g[i][j], bi[i][j], 250);
                    aacircleRGBA(renderer, xo[i][j], yo[i][j], rad,
                                 r[i][j], g[i][j], bi[i][j], 250);
                }

//                if (shadow[i][j].ex)
//                {
//                    shadow[i][j].y += shadow[i][j].v;
//                    filledCircleRGBA(renderer, shadow[i][j].x,
//shadow[i][j].y, rad, 245, 245, 245, 60);
//                }
            }
        }
    }
}

void moving_balls_color()
{
    for (int j = 0; j < MAX_L; j++)
        for (int i = 0; i < 10 ; i++)
        {
            if (exist[i][j]){
                int rc = rand() % 5;
                if (rc == 0)
                {
                    //soolati
                    r[i][j] = 255;
                    g[i][j] = 59;
                    bi[i][j] = 139;

                    color[i][j] = 1;
                }
                else if (rc == 1)
                {
                    //cyan
                    r[i][j] = 30;
                    g[i][j] = 240;
                    bi[i][j] = 206;

                    color[i][j] = 2;
                }
                else if (rc == 2)
                {
                    //abi_oili
                    r[i][j] = 0;
                    g[i][j] = 4;
                    bi[i][j] = 122;

                    color[i][j] = 3;
                }
                else if (rc == 3)
                {
                    //purple
                    r[i][j] = 172;
                    g[i][j] = 72;
                    bi[i][j] = 252;

                    color[i][j] = 4;
                }
                else if (rc == 4)
                {
                    //white
                    r[i][j] = 245;
                    g[i][j] = 245;
                    bi[i][j] = 245;

                    color[i][j] = 5;
                }

            }
        }
}

void moving_balls_down()
{
    Y += dY;
    if (Y > sqrt(3) * rad)
    {
        lines++;
        Y = 0;
    }
}

bool check_color(int l, int i, int j)
{
    for (int q = j-1; q <= j+1; q++)
        for (int p = i-1; p <= i+1; p++)
        {
            if (shootedBall[l].r == r[p][q] && shootedBall[l].g ==
                                               g[p][q] && shootedBall[l].b == bi[p][q])
                return true;
            else
                return false;
        }

}

void erase_ball(int l, int i, int j)
{
    for (int b = j-1; b <= j+1; b++)
        for (int a = i-1; a <= i+1; a++)
        {
            if ((sqrt((yo[a][b] - yo[i][j])*(yo[a][b] - yo[i][j]) +
                      (xo[a][b] - xo[i][j])*(xo[a][b] - xo[i][j])) <= 2*rad + 10) &&
                (g[i][j] == g[a][b]) && !(a == i && b == j) && exist[a][b]
                                                               == true /* && shootedBall[l].type == 1*/)
            {
                shadow[a][b].ex = true;
                shadow[i][j].ex = true;
                exist[a][b] = false;
                exist[i][j] = false;
                //cout << "aaaaaaaaaa" << endl << "//////////"<< endl;
                erase_ball(l, a, b);
            }
            else if ((sqrt((yo[a][b] - yo[i][j])*(yo[a][b] - yo[i][j])
                           + (xo[a][b] - xo[i][j])*(xo[a][b] - xo[i][j])) <= 2*rad + 10) &&
                     exist[a][b] == true && shootedBall[l].type == 2)
            {
                for (int q = j-2; q <= j+2; q++)
                    for (int p = i-2; p <= i+2; p++)
                    {
                        if (exist[p][q])
                            shadow[p][q].ex = true;
                        exist[p][q] = false;
                    }

            }
            else if ((sqrt((yo[a][b] - yo[i][j])*(yo[a][b] - yo[i][j])
                           + (xo[a][b] - xo[i][j])*(xo[a][b] - xo[i][j])) <= 2*rad + 10) &&
                     exist[a][b] == true && shootedBall[l].type == 3)
            {
                for (int q = j-1; q <= j+2; q++)
                    for (int p = 0; p < 10; p++)
                    {
                        if (exist[p][q])
                            shadow[p][q].ex = true;
                        exist[p][q] = false;
                    }

            }
            else if ((sqrt((yo[a][b] - yo[i][j])*(yo[a][b] - yo[i][j])
                           + (xo[a][b] - xo[i][j])*(xo[a][b] - xo[i][j])) <= 2*rad + 10) &&
                     (r[i][j] == 255 || r[i][j] == 30) && !(a == i && b
                                                                      == j) && exist[a][b] == true && shootedBall[l].type == 4)
            {
                shootedBall[l].type = 1;
                shadow[a][b].ex = true;
                shadow[i][j].ex = true;
                exist[a][b] = false;
                exist[i][j] = false;
                //cout << "aaaaaaaaaa" << endl << "//////////"<< endl;
                erase_ball(l, a, b);
            }
            else if ((sqrt((yo[a][b] - yo[i][j])*(yo[a][b] - yo[i][j])
                           + (xo[a][b] - xo[i][j])*(xo[a][b] - xo[i][j])) <= 2*rad + 10) &&
                     exist[a][b] == true && !(a == i && b == j) &&
                     shootedBall[l].type == 5)
            {
                shootedBall[l].type = 1;
                shadow[a][b].ex = true;
                shadow[i][j].ex = true;
                exist[a][b] = false;
                exist[i][j] = false;
                //cout << "aaaaaaaaaa" << endl << "//////////"<< endl;
                erase_ball(l, a, b);
            }
            else if ((sqrt((yo[a][b] - yo[i][j])*(yo[a][b] - yo[i][j])
                           + (xo[a][b] - xo[i][j])*(xo[a][b] - xo[i][j])) <= 2*rad + 10) &&
                     (r[i][j] == 0 || r[i][j] == 172) && !(a == i && b
                                                                     == j) && exist[a][b] == true && shootedBall[l].type == 6)
            {
                shootedBall[l].type = 1;
                shadow[a][b].ex = true;
                shadow[i][j].ex = true;
                exist[a][b] = false;
                exist[i][j] = false;
                //cout << "aaaaaaaaaa" << endl << "//////////"<< endl;
                erase_ball(l, a, b);
            }
            else if ((sqrt((yo[a][b] - yo[i][j])*(yo[a][b] - yo[i][j])
                           + (xo[a][b] - xo[i][j])*(xo[a][b] - xo[i][j])) <= 2*rad + 10) &&
                     !(r[i][j] == 0 || r[i][j] == 172) && !(a == i &&
                                                            b == j) && exist[a][b] == true && shootedBall[l].type == 6)
            {
                shootedBall[l].type = 1;
                shadow[i][j].ex = true;
                exist[i][j] = false;
            }
            else if ((sqrt((yo[a][b] - yo[i][j])*(yo[a][b] - yo[i][j])
                           + (xo[a][b] - xo[i][j])*(xo[a][b] - xo[i][j])) <= 2*rad + 10) &&
                     !(r[i][j] == 255 || r[i][j] == 30) && !(a == i &&
                                                             b == j) && exist[a][b] == true && shootedBall[l].type == 4)
            {
                shootedBall[l].type = 1;
                shadow[i][j].ex = true;
                exist[i][j] = false;
            }
        }
}

void draw_shooting_and_shooted_balls(SDL_Renderer* renderer)
{
    for (int i = 0; i < MAX_B; i++)
        if (shootedBall[i].ex)
        {
            if (shootedBall[i].type == 1)
            {
                filledCircleRGBA(renderer, shootedBall[i].x,
                                 shootedBall[i].y, rad, shootedBall[i].r, shootedBall[i].g,
                                 shootedBall[i].b, 250);
                aacircleRGBA(renderer, shootedBall[i].x,
                             shootedBall[i].y, rad, shootedBall[i].r, shootedBall[i].g,
                             shootedBall[i].b, 250);
            }
            if (shootedBall[i].type == 2)
            {
                //movaghatttt
                filledCircleRGBA(renderer, shootedBall[i].x,
                                 shootedBall[i].y, rad, shootedBall[i].r, shootedBall[i].g,
                                 shootedBall[i].b, 250);
                aacircleRGBA(renderer, shootedBall[i].x,
                             shootedBall[i].y, rad, shootedBall[i].r, shootedBall[i].g,
                             shootedBall[i].b, 250);
            }
            if (shootedBall[i].type == 3)
            {
                //movaghatttt
                filledCircleRGBA(renderer, shootedBall[i].x,
                                 shootedBall[i].y, rad, shootedBall[i].r, shootedBall[i].g,
                                 shootedBall[i].b, 250);
                aacircleRGBA(renderer, shootedBall[i].x,
                             shootedBall[i].y, rad, shootedBall[i].r, shootedBall[i].g,
                             shootedBall[i].b, 250);
            }
            if (shootedBall[i].type == 4)
            {
                //movaghatttt
                filledCircleRGBA(renderer, shootedBall[i].x,
                                 shootedBall[i].y, rad, shootedBall[i].r, shootedBall[i].g,
                                 shootedBall[i].b, 250);
                aacircleRGBA(renderer, shootedBall[i].x,
                             shootedBall[i].y, rad, shootedBall[i].r, shootedBall[i].g,
                             shootedBall[i].b, 250);
            }
            if (shootedBall[i].type == 5)
            {
                //movaghatttt
                filledCircleRGBA(renderer, shootedBall[i].x,
                                 shootedBall[i].y, rad, shootedBall[i].r, shootedBall[i].g,
                                 shootedBall[i].b, 250);
                aacircleRGBA(renderer, shootedBall[i].x,
                             shootedBall[i].y, rad, shootedBall[i].r, shootedBall[i].g,
                             shootedBall[i].b, 250);
            }
            if (shootedBall[i].type == 6)
            {
                //movaghatttt
                filledCircleRGBA(renderer, shootedBall[i].x,
                                 shootedBall[i].y, rad, shootedBall[i].r, shootedBall[i].g,
                                 shootedBall[i].b, 250);
                aacircleRGBA(renderer, shootedBall[i].x,
                             shootedBall[i].y, rad, shootedBall[i].r, shootedBall[i].g,
                             shootedBall[i].b, 250);
            }
        }

}

void set_shooting_ball_type(double shootingAngle)
{
    fire(shootingAngle);
    bomb(shootingAngle);
    multi_colored(shootingAngle);
    two_colored_1(shootingAngle);
    two_colored_2(shootingAngle);
}

void shoot_ball()
{
    double mouseAngle, shootingAngle;
    SDL_PollEvent(e);
    SDL_ShowCursor(SDL_ENABLE);
    xnxx = e->button.x;
    ynyy = e->button.y;

    //cout << "tik" << endl;

    if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT)
    {
        // checkpress(xnxx,ynyy,0,100,0,100);
        e->type = 0;
        shootedBall[k].type = 1;
        shootingAngle = ball_angle(xnxx, ynyy);
        dx[k] = v * sin(shootingAngle);
        dy[k] = v * cos(shootingAngle);
        shootedBall[k+1].ex = true;
        isShooted[k] = true;
        isMoving[k] = true;
        k++;
    }
    set_shooting_ball_type(shootingAngle);
}

void shooting_ball_move()
{
    for (int i = 0; i < MAX_B; i++)
    {
        shootedBall[i].x += dx[i];
        shootedBall[i].y += dy[i];
    }
}

void ball_reflection()
{
    if (shootedBall[k-1].x < 480 || shootedBall[k-1].x > 955)
        dx[k-1] = -dx[k-1];
}

void switch_ball(int k)
{
    if (e->button.button == SDL_BUTTON_RIGHT && e->type == SDL_MOUSEBUTTONDOWN)
    {
        e->type = 0 ;
        int rr,gg,bb;
        rr = shootedBall[k].r ;
        gg = shootedBall[k].g ;
        bb = shootedBall[k].b ;
        shootedBall[k].r = shootedBall[k+1].r;
        shootedBall[k].g = shootedBall[k+1].g;
        shootedBall[k].b = shootedBall[k+1].b;
        shootedBall[k+1].r = rr ;
        shootedBall[k+1].g = gg ;
        shootedBall[k+1].b = bb ;
    }
}

void shooting_ball_color()
{
    for (int i = 0; i < MAX_B; i++)
    {
        int rc = rand() % 4;
        if (rc == 0)
        {
            //soolati
            shootedBall[i].r = 255;
            shootedBall[i].g = 59;
            shootedBall[i].b = 139;
        }
        else if (rc == 1)
        {
            //cyan
            shootedBall[i].r = 30;
            shootedBall[i].g = 240;
            shootedBall[i].b = 206;
        }
        else if (rc == 2)
        {
            //abi_oili
            shootedBall[i].r = 0;
            shootedBall[i].g = 4;
            shootedBall[i].b = 122;
        }
        else if (rc == 3)
        {
            //purple
            shootedBall[i].r = 172;
            shootedBall[i].g = 72;
            shootedBall[i].b = 252;
        }
//        else if (rc == 4)
//        {
//            //white
//            shootedBall[i].r = 245;
//            shootedBall[i].g = 245;
//            shootedBall[i].b = 245;
//        }
    }
}

void draw_aim(SDL_Renderer* renderer)
{
    bool ok = true;
    int ax, ay;
    int p = 0;
    int aimAngle = ball_angle(xMouse, yMouse);
    int distance = 15;
    ax = distance * (xMouse - xCannon) / sqrt((xMouse -
                                               xCannon)*(xMouse - xCannon) + (yMouse - yCannon)*(yMouse - yCannon));
    ay = distance * (yMouse - yCannon) / sqrt((xMouse -
                                               xCannon)*(xMouse - xCannon) + (yMouse - yCannon)*(yMouse - yCannon));
    //cout << ax << endl;
    if (xMouse > 480 && xMouse < 955 && yMouse < 1400 && yMouse > 0)
    {
        for (int i = 0; i < 100; i++)
        {
            int xAim = xCannon + i * ax - p * ax;
            int yAim = yCannon + i * ay;
            for (int p = 0; p < MAX_L; p++)
                for (int i = 0; i < 10; i++)
                    if (dist(xAim, yAim, xo[i][p], yo[i][p]) < rad &&
                        exist[i][p] == true)
                        ok = false;
            if (ok)
            {
                if (xAim > 960 || xAim < 475)
                {
                    p++;
                    aacircleRGBA(renderer, xAim - p * ax, yAim, 2,
                                 240, 240, 240, 100);
                    filledCircleRGBA(renderer, xAim - p * ax, yAim, 2,
                                     240, 240, 240, 100);
                }
                else
                {
                    aacircleRGBA(renderer, xAim, yAim, 2, 240, 240, 240, 100);
                    filledCircleRGBA(renderer, xAim, yAim, 2, 240,
                                     240, 240, 100);
                }
            }
        }
    }
}

void pop()
{
    int p, q;
    for (int j = 0; j < MAX_L; j++)
        for (int i = 0; i < 10; i++)
            if (dist(shootedBall[k-1].x, shootedBall[k-1].y, xo[i][j],
                     yo[i][j]) <= 2*rad &&
                exist[i][j] == true && shootedBall[k-1].ex == true)
            {
                dx[k-1] = 0;
                dy[k-1] = 0;
                if (shootedBall[k-1].x < xo[i][j])
                {
                    if (j % 2 == 1)
                    {
                        shootedBall[k-1].x = xo[i][j-1];
                        shootedBall[k-1].y = yo[i][j-1];
                        r[i][j-1] = shootedBall[k-1].r;
                        g[i][j-1] = shootedBall[k-1].g;
                        bi[i][j - 1] = shootedBall[k - 1].b;
                        p = i;
                        q = j-1;
                    }
                    else
                    {
                        shootedBall[k-1].x = xo[i-1][j-1];
                        shootedBall[k-1].y = yo[i-1][j-1];
                        r[i-1][j-1] = shootedBall[k-1].r;
                        g[i-1][j-1] = shootedBall[k-1].g;
                        bi[i - 1][j - 1] = shootedBall[k - 1].b;
                        p = i-1;
                        q = j-1;
                    }

                }
                else
                {
                    if (j % 2 == 1)
                    {
                        shootedBall[k-1].x = xo[i+1][j-1];
                        shootedBall[k-1].y = yo[i+1][j-1];
                        r[i+1][j-1] = shootedBall[k-1].r;
                        g[i+1][j-1] = shootedBall[k-1].g;
                        bi[i + 1][j - 1] = shootedBall[k - 1].b;
                        p = i+1;
                        q = j-1;
                    }
                    else
                    {
                        shootedBall[k-1].x = xo[i][j-1];
                        shootedBall[k-1].y = yo[i][j-1];
                        r[i][j-1] = shootedBall[k-1].r;
                        g[i][j-1] = shootedBall[k-1].g;
                        bi[i][j - 1] = shootedBall[k - 1].b;
                        p = i;
                        q = j-1;
                    }
                }
                //cout << xo[p][q] << " , " << yo[p][q] << endl;
                exist[p][q] = true;
                erase_ball(k-1,p, q);
                shootedBall[k-1].ex = false;
            }
}

void check_around()
{
    for (int j = 0; j < MAX_L; j++)
        for (int i = 0; i < 10; i++)
            if (connection(i, j) == 0)
                exist[i][j] = false;
}

void end_game()
{
    for (int j = 0; j < MAX_L; j++)
        for (int i = 0; i < 10; i++)
            if (exist[i][j])
            {
                if (yo[i][j] >= 650)
                    gameOver = true;
            }
}

void slow_mo_effect()
{
    if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_s && slowNum > 0)
    {
        cout << "slowinnnn" << endl;
        e->type = 0;
        effect = 1;
        slowNum = 0;
    }
    if (effect == 1 && S > 0)
    {
        dY = 0.08;
        S--;
    }
    else if (effect ==1 && S < 1)
    {
        effect = 0;
        dY = 0.2;
    }

}

void pause_effect()
{
    if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_p && pauseNum > 0)
    {
        cout << "pauseinnnnn" << endl;
        e->type = 0;
        effect = 2;
        pauseNum = 0;
    }
    if (effect == 2 && P > 0)
    {
        dY = 0;
        P--;
    }
    else if (effect == 2 && P < 1)
    {
        effect = 0;
        dY = 0.2;
    }

}

void shadow_move_and_draw(SDL_Renderer *renderer)
{
    for (int j = 0; j < MAX_L; j++)
        for (int i = 0; i < 10; i++)
        {
            shadow[i][j].v = 15;
            if(!shadow[i][j].ex)
            {
                shadow[i][j].y = yo[i][j];
                shadow[i][j].x = xo[i][j];
            }
            if (shadow[i][j].ex)
            {
                shadow[i][j].y += shadow[i][j].v;
                filledCircleRGBA(renderer, shadow[i][j].x,
                                 shadow[i][j].y, rad, 245, 245, 245, 60);
                aacircleRGBA(renderer, shadow[i][j].x, shadow[i][j].y,
                             rad, 245, 245, 245, 180);
            }
        }
}

void set_shooting_ball_type();

void end_game();

void recordPlayerScore(const string& username, int toops, int zaman);

//vector<PlayerScore> sortScores(const string& filename);

//void displayScores(SDL_Renderer* renderer, TTF_Font* font, const std::vector<PlayerScore>& scores);

bool ComparePlayers(const Player& a, const Player& b);

std::vector<Player> ReadPlayerData(const std::string& filename);

void SortPlayerData(std::vector<Player>& players);

void recordPlayerScore2(const string& username, int toops, int zaman);
//-----------------------------------------------SOUNDS-----------------------------------------------//
Mix_Music *BackgroundMusic1 = NULL;
Mix_Music *BackgroundMusic2 = NULL;
Mix_Music *BackgroundMusic3 = NULL;
Mix_Music *BackgroundMusic4 = NULL;
Mix_Music *BackgroundMusic5 = NULL;
Mix_Music *BackgroundMusic6 = NULL;
Mix_Music *GameMusic1 = NULL;
Mix_Chunk *select;
Mix_Chunk *poping;
Mix_Chunk *win;
Mix_Chunk *lose;
Mix_Chunk *fireball;
Mix_Chunk *ignite;
Mix_Chunk *ice;
Mix_Chunk *explosion;
//--------------------------------------------------------------------------------------------------------//
int main(int argc, char *argv[])
{
    for (int j = m; j < MAX_L; j++)
        for (int i = 0; i < 10; i++)
        {
            exist[i][j] = true;
            island[i][j] = false;
        }
    for (int j = 0; j < m; j++)
        for (int i = 0; i < 10; i++)
        {
            exist[i][j] = false;
            island[i][j] = false;
        }
    for (int i = 0; i < MAX_B; i++)
    {
        shootedBall[i].x = xCannon;
        shootedBall[i].y = yCannon;
        shootedBall[i].ex = false;
        isShooted[i] = false;
        isMoving[i] = false;
        dx[i] = 0;
        dy[i] = 0;
    }
    shootedBall[0].ex = true;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1; // Or handle the error as appropriate
    }

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        return -1; // Or handle the error as appropriate
    }
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    SDL_Window *m_window = SDL_CreateWindow("The Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1440, 720, SDL_WINDOW_SHOWN);
    SDL_Renderer *m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    int ahang = 1000;
    // SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
    //------------------------------------------initialize sounds------------------------------------------------//
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    BackgroundMusic1 = Mix_LoadMUS("sounds/main music.mp3");
    BackgroundMusic2 = Mix_LoadMUS("sounds/main music2.mp3");
    BackgroundMusic3 = Mix_LoadMUS("Sounds/main music3.mp3");
    BackgroundMusic4 = Mix_LoadMUS("sounds/main music4.mp3");
    BackgroundMusic5 = Mix_LoadMUS("sounds/main music5.mp3");
    BackgroundMusic6 = Mix_LoadMUS("sounds/main music6.mp3");
    GameMusic1 = Mix_LoadMUS("sounds/GameMusic1.mp3");
    select = Mix_LoadWAV("sounds/select.mp3");
    ice = Mix_LoadWAV("sounds/ice.wav");
    poping = Mix_LoadWAV("sounds/pop.wav");
    win = Mix_LoadWAV("sounds/win.wav");
    lose = Mix_LoadWAV("sounds/lose.wav");
    fireball = Mix_LoadWAV("sounds/fireball.wav");
    ignite = Mix_LoadWAV("sounds/ignite.wav");
    explosion = Mix_LoadWAV("sounds/explosion.wav");
    //-----------------------------------------------IMAGES & BUTTONS-----------------------------------------------//
    SDL_Texture *cannonpic = IMG_LoadTexture(m_renderer, "pictures/cannonjungle.png");
    SDL_Texture *cannonpic2 = IMG_LoadTexture(m_renderer, "pictures/cannonspace.png");
    image *bg = new image("pictures/backgroundimage.png", 0, 0, 1.0, m_renderer);
    image *bg1 = new image("pictures/spacebackground.png", 0, 0, 1.0, m_renderer);
    image *bgr = new image("pictures/registering.png", 0, 0, 1.0, m_renderer);
    image *loginback = new image("pictures/loginback.png", 0, 0, 1.0, m_renderer);
    image *main1 = new image("pictures/maingame1.png", 0, 0, 1.0, m_renderer);
    image *hello1 = new image("pictures/hello1.png", 250, 300, 1.0, m_renderer);
    image *maingamejungle = new image("pictures/leafbackground2.png", 0, 0, 1.0, m_renderer);
    image *maingamespace = new image("pictures/spacemaingame.png", 0, 0, 1.0, m_renderer);
    image *loginjungle = new image ("pictures/jungleisgood.png", 0, 0, 1.0, m_renderer);
    image *reg1 = new image ("pictures/reg1.png", 700, 350, 1.0, m_renderer);
    image *spacesignup = new image ("pictures/spacesignup.png", 0, 0, 1.0, m_renderer);
    image *spacelogin = new image ("pictures/spacelogin.png", 0, 0, 1.0, m_renderer);
    image *bomb = new image ("pictures/bomb.png", 1250, 100, 0.7, m_renderer);
    image *bombspace = new image ("pictures/bombspace.png", 1250, 100, 0.7, m_renderer);
    image *multicolor = new image ("pictures/multicolor.png", 1250, 300, 0.7, m_renderer);
    image *multicolorspace = new image ("pictures/multicolorspace.png", 1250, 300, 0.7, m_renderer);
    image *gameover = new image ("pictures/gameover.png", 500, 200, 1.0, m_renderer);
    image *gameoverspace = new image ("pictures/gameoverspace.png", 480, 150, 0.5, m_renderer);
    btn *login_btn = new btn(650, 200, "pictures/buttons/loginbutton.png", 1.1, m_renderer);
    btn *loginspace_btn = new btn(590, 100, "pictures/buttons/loginspace.png", 1.1, m_renderer);
    image *login2 = new image("pictures/buttons/loginbutton2.png", 685, 200, 1.1, m_renderer);
    btn *signup_btn = new btn(650, 400, "pictures/buttons/signupbutton.png", 1.1, m_renderer);
    btn *signupspace_btn = new btn(600, 320, "pictures/buttons/signupspace.png", 1.0, m_renderer);
    image *signup2 = new image("pictures/buttons/signupbutton2.png", 685, 400, 1.1, m_renderer);
    btn *exit_btn = new btn(20, 600, "pictures/buttons/exitbutton.png", 1.0, m_renderer);
    btn *exitspace_btn = new btn(20, 570, "pictures/buttons/exitspace.png", 1.0, m_renderer);
    btn *signingspace_btn = new btn(350, 400, "pictures/buttons/signingupspace.png", 0.6, m_renderer);
    image *exit2 = new image("pictures/buttons/exitbutton2.png", 20, 800, 1.1, m_renderer);
    btn *setting_btn = new btn(1250, 600, "pictures/buttons/settingbutton.png", 1.1, m_renderer);
    btn *settingspace_btn = new btn(1250, 550, "pictures/buttons/settingspace.png", 1.1, m_renderer);
    btn *help_btn = new btn(1150, 550, "pictures/buttons/helpbutton.png", 1.0, m_renderer);
    image *helppage = new image("pictures/helpbackground.png", 460, 20, 0.8, m_renderer);
    image *helpspace = new image("pictures/helpspacebackground.png", 570, 100, 0.6, m_renderer);
    btn *back_btn = new btn(1320, 600, "pictures/buttons/backbutton.png", 0.7, m_renderer);
    btn *scoreboard_btn = new btn(50, 130, "pictures/buttons/scoreboard.png", 0.7, m_renderer);
    btn *sound_btn = new btn(1220, 450, "pictures/buttons/soundbutton.png", 1.0, m_renderer);
    btn *soundingame_btn = new btn(680, 300, "pictures/buttons/soundbutton.png", 1.0, m_renderer);
    btn *soundcancel_btn = new btn(700, 330, "pictures/buttons/soundcancelbutton.png", 1.0, m_renderer);
    btn *soundup_btn = new btn(820, 130, "pictures/buttons/soundupbutton.png", 1.0, m_renderer);
    btn *sounddown_btn = new btn(580, 130, "pictures/buttons/sounddownbutton.png", 1.0, m_renderer);
    btn *next_btn = new btn(1050, 430, "pictures/buttons/nextbutton.png", 1.0, m_renderer);
    btn *prev_btn = new btn(350, 430, "pictures/buttons/prevbutton.png", 1.0, m_renderer);
    btn *reguser_btn = new btn(619, 274, "pictures/Buttons/Cadr.png", 1.0, m_renderer);
    btn *regpass_btn = new btn(619, 412, "pictures/Buttons/Cadr.png", 1.0, m_renderer);
    btn *regcon_btn = new btn(619, 546, "pictures/Buttons/Cadr.png", 1.0, m_renderer);
    btn *themespace_btn = new btn(1300, 350, "pictures/buttons/themechangespace.png", 1.0, m_renderer);
    btn *themejungle_btn = new btn(1300, 350, "pictures/buttons/backbutton.png", 0.6, m_renderer);
    btn *spacehelp_btn = new btn(830, 200, "pictures/buttons/spacehelp.png", 1.0, m_renderer);
    btn *backspace_btn = new btn(1300, 500, "pictures/buttons/backspace.png", 0.7, m_renderer);
    btn *spacemusic_btn = new btn(430, 200, "pictures/buttons/spacemusic.png", 1.0, m_renderer);
    btn *spacemusicingame_btn = new btn(650, 250, "pictures/buttons/spacemusic.png", 1.0, m_renderer);
    btn *enteruser_btn = new btn(500, 400, "pictures/buttons/enteruserbtn.png", 1.0, m_renderer);
    btn *signing_btn = new btn(800, 700, "pictures/buttons/signingup.png", 1.0, m_renderer);
    btn *logingin_btn = new btn(800, 700, "pictures/buttons/enteruserbtn.png", 1.0, m_renderer);
    btn *timedmode_btn = new btn(400, 150, "pictures/buttons/timedmodebtn.png", 1.0, m_renderer);
    btn *randommode_btn = new btn(650, 150, "pictures/buttons/randommodebtn.png", 1.0, m_renderer);
    btn *infinitemode_btn = new btn(900, 150, "pictures/buttons/infinitemodebtn.png", 1.0, m_renderer);
    btn *timesspace_btn = new btn(400, 150, "pictures/buttons/timedspace.png", 1.0, m_renderer);
    btn *randomspace_btn = new btn(650, 150, "pictures/buttons/randomspace.png", 1.0, m_renderer);
    btn *infinitespace_btn = new btn(900, 150, "pictures/buttons/infinitespace.png", 1.0, m_renderer);
    btn *pausejungle_btn = new btn(30, 550, "pictures/buttons/pausebutton.png", 1.0, m_renderer);
    btn *resumejungle_btn = new btn(30, 550, "pictures/buttons/resumebutton.png", 1.0, m_renderer);
    btn *resumespace_btn = new btn(30, 550, "pictures/buttons/resumespace.png", 1.0, m_renderer);
    btn *pausespace_btn = new btn(30, 550, "pictures/buttons/pausespace.png", 1.0, m_renderer);
    //----------------------------------------------------------------------------------------------------//
    SDL_Event *e = new SDL_Event();
    SDL_Event *s = new SDL_Event();
    string mucicName = "Music";
    string gamename = "The Better Bouncing Balls";
    string reguse;
    string username;
    string hello = "welcome" + username;
    string hellospace = "welcome" + reguse;
    int x, y;
    int mode = 0;
    int faza = 0;
    int theme = 100;
    int musicspace = 10000;
    int usernameis = 0;
    bool quit = 0;
    bool play = 1;
    bool ahangfazaei = 1;
    bool reguserinp = 0;
    int regstat = 0;
    bool userinp = 0, passinp = 0, logcheck = 0, eshteb = 0;
    bool signingup = 0;
    bool logingin = 0;
    bool musicpaused = 0;
    int volume = 100;
    bool isPaused = 0;
    bool atgame = 1;
    bool atspace = 1;
    bool ahangebazi = 0;
    int gamemusic = 1000;
    bool armin = 0;
    double alfa;
    int X = 720;
    int Y = 600;
    bool showtime = 1;
    double timegone = 0;
    double timeleft = 120;
    //----------------------------------------------Main loop----------------------------------------------//
    //baste be themee entekhabi
    set_random_color1(r, g, bi);
    //set_random_color2();
    while (!quit) {
        while (theme % 2 == 0) {
            if (play){
            if (ahang  == 1003 || ahang == 1006 || ahang == 1009 || ahang == 997 || ahang == 994) {
                Mix_PlayMusic(BackgroundMusic2, -1);
                play = 0;
                mucicName = "Cheri Cheri Lady - Modern Talking";
            }
            if (ahang == 1002 || ahang == 1004 || ahang == 1006 || ahang == 1000 || ahang == 998 || ahang == 1008) {
                Mix_PlayMusic(BackgroundMusic1, -1);
                play = 0;
                mucicName = "Maniac - Michael Sembello";
            }
            if (ahang == 1001 || ahang == 1005 || ahang == 1007 || ahang == 999 || ahang == 995) {
                Mix_PlayMusic(BackgroundMusic3, -1);
                play = 0;
                mucicName = "Hala Hey - Armin Zareei";
            }
            if (ahang == 2000){
                Mix_PlayMusic(GameMusic1, -1);
                play = 0;
                mucicName = "Shomare jadidam - Armin Zareei";
            }
            }
            Mix_VolumeMusic(volume);
            while (mode == 0) { // Main Menu
                SDL_PollEvent(e);
                SDL_RenderClear(m_renderer);
                bg->render();
                login_btn->pic->render();
                signup_btn->pic->render();
                exit_btn->pic->render();
                setting_btn->pic->render();
                themespace_btn->pic->render();
                scoreboard_btn->pic->render();
                betterText(m_renderer, gamename, 370, 30, 255, 255, 255, 230, 80, "Rockinsoda");
                x = e->motion.x;
                y = e->motion.y;
                if (e->type == SDL_MOUSEBUTTONDOWN && login_btn->btn_clicked(x, y)) {
                     int channel = Mix_PlayChannel(-1, select, 0);
                    mode = 1;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && scoreboard_btn->btn_clicked(x, y)) {
                    int channel = Mix_PlayChannel(-1, select, 0);
                    mode = 109;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && signup_btn->btn_clicked(x, y)) {
                    int channel = Mix_PlayChannel(-1, select, 0);
                    mode = 2;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && setting_btn->btn_clicked(x, y)) {
                    int channel = Mix_PlayChannel(-1, select, 0);
                    SDL_Delay(500);
                    mode = 3;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && exit_btn->btn_clicked(x, y)) {
                    int channel = Mix_PlayChannel(-1, select, 0);
                    mode = 8;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && themespace_btn->btn_clicked(x, y)) {
                    int channel = Mix_PlayChannel(-1, select, 0);
                    theme++;
                    break;
                }
                SDL_RenderPresent(m_renderer);
            }
            while (mode == 1) {//login
                SDL_PollEvent(e);
                SDL_RenderClear(m_renderer);
                x = e->motion.x;
                y = e->motion.y;
                loginback->render();
                back_btn->pic->render();
                enteruser_btn->pic->render();

                if (e->type == SDL_MOUSEBUTTONDOWN && back_btn->btn_clicked(x, y)) {
                    SDL_Delay(500);
                    mode = 0;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && enteruser_btn->btn_clicked(x, y)){
                    username = TextBox(m_renderer, 300, 300, 255, 255, 0);
                    logingin = 1;
                }
                if (username != " "){
                    betterText(m_renderer, username, 300, 300, 255, 255, 0, 200, 75, "calibrib");
                }
                if (logingin == 1) {
                    if (isNameInFile(username)) {
                        mode = 39;
                    }
                    else{
                        mode = 2;
                        //username = " ";
                    }
                }
                SDL_RenderPresent(m_renderer);
                e->type = 0;
            }
            while (mode == 39){//signup
                SDL_PollEvent(e);
                SDL_RenderClear(m_renderer);
                x = e->motion.x;
                y = e->motion.y;
                bg->render();
                timedmode_btn->pic->render();
                infinitemode_btn->pic->render();
                randommode_btn->pic->render();
                back_btn->pic->render();
                betterText(m_renderer, hellospace, 260, 100, 255, 255, 255, 250, 50, "Rockinsoda");
                if (e->type == SDL_MOUSEBUTTONDOWN && back_btn->btn_clicked(x, y)) {
                    SDL_Delay(500);
                    mode = 0;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && timedmode_btn->btn_clicked(x, y)) {
                    SDL_Delay(500);
                    armin = 1;
                    mode = 41;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && infinitemode_btn->btn_clicked(x, y)) {
                    SDL_Delay(500);
                    armin = 1;
                    mode = 40;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && randommode_btn->btn_clicked(x, y)) {
                    SDL_Delay(500);
                    armin = 1;
                    mode = 40;
                }
                SDL_RenderPresent(m_renderer);
            }
            while (mode == 40) { //boro to bazi
//                 if (armin){
//                play = 1;
//                ahang = 2000;
//                armin = 0;
//            }
                //SDL_PollEvent(e);



                SDL_Event e;
                SDL_PollEvent(s);
                SDL_RenderClear(m_renderer);
                maingamejungle->render();
                pausejungle_btn->pic->render();
                bomb->render();
                multicolor->render();
                betterText(m_renderer, "Press F", 1250, 250, 255, 0, 0, 200, 45, "Rockinsoda");
                betterText(m_renderer, "Press M", 1250, 400, 255, 0, 0, 200, 45, "Rockinsoda");
                while(atgame){
                moving_balls_color();
                shooting_ball_color();
                atgame = 0;
                }
                betterText(m_renderer, intToString(timegone), 20, 25, 255, 255, 255, 100, 45, "Rockinsoda");

                timegone += 0.05;
                SDL_Delay(10);
                //SDL_SetRenderDrawColor(m_renderer,0,0,0,250);


                    if (k < MAX_B)
                    {
                        while (SDL_PollEvent(&e)) {
                            if (e.type == SDL_KEYDOWN) {
                                switch (e.key.keysym.sym) {
                                    case SDLK_SPACE:
                                        cout << "Space key pressed";
                                        mode = 38;
                                        break;
                                }
                            }
                            if(e.type == SDL_MOUSEMOTION) {
                                SDL_GetMouseState(&xMouse, &yMouse);
                                alfa = atan2((xMouse - X),(yMouse - Y));
                                angleimage(m_renderer, cannonpic, 680, 500, 78, 159, -1 * (alfa * 180 / M_PI + 180));
                            }
                        }
                        alfa = atan2((xMouse - X),(yMouse-Y));
                        angleimage(m_renderer,cannonpic, 680, 500, 78, 159, -1 * (alfa * 180 / M_PI + 180));
                        if (!gameOver){
                            pause_effect();
                            slow_mo_effect();
                            //------draw------
                            draw_aim(m_renderer);
                            draw_moving_balls(m_renderer);
                            draw_shooting_and_shooted_balls(m_renderer);
                            shadow_move_and_draw(m_renderer);
                            //----------------
                            pop();
                            check_around();
                            moving_balls_down();

                            end_game();
                            shoot_ball();
                            shooting_ball_move();
                            ball_reflection();
                    }
                    }
                    if (gameOver){
                        //recordPlayerScore(username, toops, zaman);
                        mode = 33;
                    }
                recordPlayerScore(username, toops, zaman);
                SDL_RenderPresent(m_renderer);
            }
            while (mode == 41){
                SDL_Event e;
                SDL_PollEvent(s);
                SDL_RenderClear(m_renderer);
                maingamejungle->render();
                pausejungle_btn->pic->render();
                bomb->render();
                multicolor->render();
                betterText(m_renderer, "Press F", 1250, 250, 255, 0, 0, 200, 45, "Rockinsoda");
                betterText(m_renderer, "Press M", 1250, 400, 255, 0, 0, 200, 45, "Rockinsoda");
                while(atgame){
                    moving_balls_color();
                    shooting_ball_color();
                    atgame = 0;
                }
                betterText(m_renderer, intToString(timeleft), 20, 25, 255, 255, 255, 100, 45, "Rockinsoda");

                timeleft -= 0.05;
                if (timeleft == 0){
                    mode = 33;
                    break;
                }
                SDL_Delay(10);
                //SDL_SetRenderDrawColor(m_renderer,0,0,0,250);


                if (k < MAX_B)
                {
                    while (SDL_PollEvent(&e)) {
                        if (e.type == SDL_KEYDOWN) {
                            switch (e.key.keysym.sym) {
                                case SDLK_SPACE:
                                    cout << "Space key pressed";
                                    mode = 38;
                                    break;
                            }
                        }
                        if(e.type == SDL_MOUSEMOTION) {
                            SDL_GetMouseState(&xMouse, &yMouse);
                            alfa = atan2((xMouse - X),(yMouse - Y));
                            angleimage(m_renderer, cannonpic, 680, 500, 78, 159, -1 * (alfa * 180 / M_PI + 180));
                        }
                    }
                    alfa = atan2((xMouse - X),(yMouse-Y));
                    angleimage(m_renderer,cannonpic, 680, 500, 78, 159, -1 * (alfa * 180 / M_PI + 180));
                    if (!gameOver){
                        pause_effect();
                        slow_mo_effect();
                        //------draw------
                        draw_aim(m_renderer);
                        draw_moving_balls(m_renderer);
                        draw_shooting_and_shooted_balls(m_renderer);
                        shadow_move_and_draw(m_renderer);
                        //----------------
                        pop();
                        check_around();
                        moving_balls_down();

                        end_game();
                        shoot_ball();
                        shooting_ball_move();
                        ball_reflection();
                    }
                }
                if(gameOver){
                    recordPlayerScore(username, toops, zaman);
                    mode = 33;
                    break;
                }
                recordPlayerScore2(username, toops, zaman);
                SDL_RenderPresent(m_renderer);

            }
            while (mode == 33){//lost game
                SDL_PollEvent(e);
                SDL_RenderClear(m_renderer);
                x = e->motion.x;
                y = e->motion.y;
                bg->render();
                back_btn->pic->render();
                gameover->render();
                if (e->type == SDL_MOUSEBUTTONDOWN && back_btn->btn_clicked(x, y)) {
                    int channel = Mix_PlayChannel(-1, select, 0);
                    mode = 0;
                }
                SDL_RenderPresent(m_renderer);


            }
            while (mode == 38){
                SDL_PollEvent(e);
                SDL_RenderClear(m_renderer);
                x = e->motion.x;
                y = e->motion.y;
                maingamejungle->render();
                back_btn->pic->render();
                soundingame_btn->pic->render();
                resumejungle_btn->pic->render();
                if (e->type == SDL_MOUSEBUTTONDOWN && resumejungle_btn->btn_clicked(x, y)) {
                    int channel = Mix_PlayChannel(-1, select, 0);
                    mode = 40;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && back_btn->btn_clicked(x, y)) {
                    int channel = Mix_PlayChannel(-1, select, 0);
                    mode = 0;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && soundingame_btn->btn_clicked(x, y)) {
                    int channel = Mix_PlayChannel(-1, select, 0);
                    mode = 37;
                }
                SDL_RenderPresent(m_renderer);
            }
            while (mode == 109){//select which scoreboard
                SDL_PollEvent(e);
                SDL_RenderClear(m_renderer);
                betterText(m_renderer, "SElect your scoreboard mode", 20, 20, 255, 255, 255,200, 75, "calibrib");
                x = e->motion.x;
                y = e->motion.y;
                bg->render();
                timedmode_btn->pic->render();
                randommode_btn->pic->render();
                back_btn->pic->render();
                if (e->type == SDL_MOUSEBUTTONDOWN && timedmode_btn->btn_clicked(x, y)){
                    mode = 110;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && randommode_btn->btn_clicked(x, y)){
                    mode = 111;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && back_btn->btn_clicked(x, y)) {
                    SDL_Delay(500);
                    mode = 0;
                    break;
                }
                SDL_RenderPresent(m_renderer);


            }
            while (mode == 110){//timed mode scoreboard
                SDL_PollEvent(e);
                SDL_RenderClear(m_renderer);
                betterText(m_renderer, "Timed Mode ScoreBoard", 20, 20, 255, 255, 255,200, 75, "calibrib");
                x = e->motion.x;
                y = e->motion.y;
                bg->render();
                back_btn->pic->render();
                //sortScores("player_scores.txt");
                //displayScores(m_renderer,Fontt, sortScores("player_scores.txt"));
                std::vector<Player> players = ReadPlayerData("player_scores.txt");
                SortPlayerData(players);

                for (const Player& player : players) {
                    // Create a string with player information
                    std::string playerInfo = player.username + ": " + std::to_string(player.score);
                    betterText(m_renderer, playerInfo, 100, yscore, 255, 255, 255, 200, 45, "calibrib");
                    cout << endl;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && back_btn->btn_clicked(x, y)) {
                    SDL_Delay(500);
                    mode = 0;
                    //break;
                }

                SDL_RenderPresent(m_renderer);

            }
            while (mode == 111){//scoreboard randommode
                SDL_PollEvent(e);
                SDL_RenderClear(m_renderer);
                x = e->motion.x;
                y = e->motion.y;
                bg->render();
                back_btn->pic->render();
                //sortScores("player_scores.txt");
                //displayScores(m_renderer,Fontt, sortScores("player_scores.txt"));
                std::vector<Player> players = ReadPlayerData("player_scores2.txt");
                SortPlayerData(players);

                for (const Player& player : players) {
                    // Create a string with player information
                    std::string playerInfo = player.username + ": " + std::to_string(player.score);
                    betterText(m_renderer, playerInfo, 100, yscore, 255, 255, 255, 200, 45, "calibrib");
                    cout << endl;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && back_btn->btn_clicked(x, y)) {
                    SDL_Delay(500);
                    mode = 0;
                    //break;
                }

                SDL_RenderPresent(m_renderer);


            }
            while (mode == 85){
                mode = 37;
            }
            while (mode == 37){
                SDL_PollEvent(e);
                SDL_RenderClear(m_renderer);
                x = e->motion.x;
                y = e->motion.y;
                maingamejungle->render();
                soundcancel_btn->pic->render();
                soundup_btn->pic->render();
                sounddown_btn->pic->render();
                back_btn->pic->render();
                next_btn->pic->render();
                prev_btn->pic->render();
                betterText(m_renderer, mucicName, 500, 650, 255, 255, 255, 250, 50, "Rockinsoda");
                betterText(m_renderer, intToString(volume), 650, 500, 255, 255, 255, 250, 50, "Rockinsoda");
                if (e->type == SDL_MOUSEBUTTONDOWN && soundcancel_btn->btn_clicked(x, y)){
                    if (musicpaused) {
                        Mix_ResumeMusic();
                    } else {
                        Mix_PauseMusic();
                    }
                    musicpaused = !musicpaused;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && sounddown_btn->btn_clicked(x, y)){
                    mode = 85;
                    volume -= 5;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && soundup_btn->btn_clicked(x, y)){
                    mode = 85;
                    volume += 5;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && prev_btn->btn_clicked(x, y)) {
                    SDL_Delay(300);
                    int channel = Mix_PlayChannel(-1, select, 0);
                    mode = 85;
                    play = 1;
                    ahang--;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && next_btn->btn_clicked(x, y)) {
                    SDL_Delay(300);
                    int channel = Mix_PlayChannel(-1, select, 0);
                    mode = 85;
                    play = 1;
                    ahang++;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && back_btn->btn_clicked(x, y)) {
                    SDL_Delay(500);
                    int channel = Mix_PlayChannel(-1, select, 0);
                    mode = 38;
                }
                SDL_RenderPresent(m_renderer);
            }
            while (mode == 2) { // sign up
                SDL_PollEvent(e);
                SDL_RenderClear(m_renderer);
                x = e->motion.x;
                y = e->motion.y;
                loginjungle->render();
                back_btn->pic->render();
                enteruser_btn->pic->render();
                signing_btn->pic->render();
                if (e->type == SDL_MOUSEBUTTONDOWN && enteruser_btn->btn_clicked(x, y)) {
                    reguse = TextBox(m_renderer, 300, 300, 255, 255, 0);
                    signingup = 1;
                }
                if (reguse != " "){
                     betterText(m_renderer, reguse, 300, 300, 255, 255, 0, 200, 75, "calibrib");
                }
                if(signingup == 1) {
                    ofstream user("User.txt", ios::app);
                    if (searchStringInCSV("User.txt", username)) {
                        reguse = "";
                        betterTextM(m_renderer, "username already exists", 355, 500, 60, "");
                    }
                    if (reguse != "") {
                        user << reguse;
//                        scorenormal << "0,";
//                        scoretimed << "0,";
//                        scoreinf << "0,";
//                        scorenormal.close();
//                        scoreinf.close();
//                        scoretimed.close();
                        user << endl;
                    }
                    signingup = false;
                    user.close();
                    mode = 39;
                }
                else {
                    ifstream user("User.txt", ios::in);
                    if (findIndexInCSV("User.txt", username) != -1) {
                        user.close();
                        mode = 39;
                    } else {
                        username = "";
                    }
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && back_btn->btn_clicked(x, y)) {
                    SDL_Delay(500);
                    int channel = Mix_PlayChannel(-1, select, 0);
                    mode = 0;
                }
                SDL_RenderPresent(m_renderer);
            }
            while (mode == 3) { // setting
                SDL_PollEvent(e);
                SDL_RenderClear(m_renderer);
                x = e->motion.x;
                y = e->motion.y;
                bg->render();
                login_btn->pic->render();
                signup_btn->pic->render();
                exit_btn->pic->render();
                setting_btn->pic->render();
                help_btn->pic->render();
                sound_btn->pic->render();
                if (e->type == SDL_MOUSEBUTTONDOWN && setting_btn->btn_clicked(x, y)) {
                    SDL_Delay(500);
                    int channel = Mix_PlayChannel(-1, select, 0);
                    mode = 0;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && help_btn->btn_clicked(x, y)) {
                    SDL_Delay(500);
                    int channel = Mix_PlayChannel(-1, select, 0);
                    mode = 4;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && sound_btn->btn_clicked(x, y)) {
                    SDL_Delay(400);
                    int channel = Mix_PlayChannel(-1, select, 0);
                    mode = 5;
                }
                SDL_RenderPresent(m_renderer);
            }
            while (mode == 4) {
                SDL_PollEvent(e);
                SDL_RenderClear(m_renderer);
                x = e->motion.x;
                y = e->motion.y;
                bg->render();
                helppage->render();
                back_btn->pic->render();
                if (e->type == SDL_MOUSEBUTTONDOWN && back_btn->btn_clicked(x, y)) {
                    SDL_Delay(500);
                    int channel = Mix_PlayChannel(-1, select, 0);
                    mode = 0;
                }
                SDL_RenderPresent(m_renderer);
            }
            while (mode == 99)
                mode = 5;
            while (mode == 5) {
                SDL_PollEvent(e);
                SDL_RenderClear(m_renderer);
                x = e->motion.x;
                y = e->motion.y;
                bg->render();
                soundcancel_btn->pic->render();
                soundup_btn->pic->render();
                sounddown_btn->pic->render();
                back_btn->pic->render();
                next_btn->pic->render();
                prev_btn->pic->render();
                betterText(m_renderer, mucicName, 500, 650, 255, 255, 255, 250, 50, "Rockinsoda");
                betterText(m_renderer, intToString(volume), 650, 500, 255, 255, 255, 250, 50, "Rockinsoda");
                if (e->type == SDL_MOUSEBUTTONDOWN && soundcancel_btn->btn_clicked(x, y)){
                    if (musicpaused) {
                        Mix_ResumeMusic();
                    } else {
                        Mix_PauseMusic();
                    }
                    musicpaused = !musicpaused;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && sounddown_btn->btn_clicked(x, y)){
                    mode = 99;
                    volume -= 5;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && soundup_btn->btn_clicked(x, y)){
                    mode = 99;
                    volume += 5;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && prev_btn->btn_clicked(x, y)) {
                    SDL_Delay(300);
                    int channel = Mix_PlayChannel(-1, select, 0);
                    mode = 99;
                    play = 1;
                    ahang--;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && next_btn->btn_clicked(x, y)) {
                    SDL_Delay(300);
                    int channel = Mix_PlayChannel(-1, select, 0);
                    mode = 99;
                    play = 1;
                    ahang++;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && back_btn->btn_clicked(x, y)) {
                    SDL_Delay(500);
                    int channel = Mix_PlayChannel(-1, select, 0);
                    mode = 0;
                }
                SDL_RenderPresent(m_renderer);
            }
            if (mode == 8) // Quit
                quit = true;
            break;
        }
        while (theme % 2 == 1)
        {
            if (ahangfazaei && musicspace % 3 == 0)
            {
                Mix_PlayMusic(BackgroundMusic5, -1);
                ahangfazaei = 0;
                mucicName = "Cheghad Khoobe - Armin 2AFM";
            }
            if (ahangfazaei && musicspace % 3 == 1)
            {
                Mix_PlayMusic(BackgroundMusic4, -1);
                ahangfazaei = 0;
                mucicName = "Koala - koorosh";
            }
            if (ahangfazaei && musicspace % 3 == 2)
            {
                Mix_PlayMusic(BackgroundMusic6, -1);
                ahangfazaei = 0;
                mucicName = "Gang Beraghse - Alireza JJ";
            }
            Mix_VolumeMusic(volume);
            while (faza == 0)
            { // Main Menu
                SDL_PollEvent(s);
                SDL_RenderClear(m_renderer);
                bg1->render();
                exitspace_btn->pic->render();
                settingspace_btn->pic->render();
                loginspace_btn->pic->render();
                signupspace_btn->pic->render();
                themejungle_btn->pic->render();
                scoreboard_btn->pic->render();
                betterText(m_renderer, gamename, 370, 30, 255, 255, 255, 230, 80, "Rockinsoda");
                x = s->motion.x;
                y = s->motion.y;
                if (s->type == SDL_MOUSEBUTTONDOWN && loginspace_btn->btn_clicked(x, y))
                {
                     int channel = Mix_PlayChannel(-1, select, 0);
                    faza = 1;
                }
                if (s->type == SDL_MOUSEBUTTONDOWN && signupspace_btn->btn_clicked(x, y))
                {
                     int channel = Mix_PlayChannel(-1, select, 0);
                    faza = 2;
                }
                if (s->type == SDL_MOUSEBUTTONDOWN && settingspace_btn->btn_clicked(x, y))
                {
                    int channel = Mix_PlayChannel(-1, select, 0);
                    faza = 3;
                }
                if (s->type == SDL_MOUSEBUTTONDOWN && exitspace_btn->btn_clicked(x, y))
                {
                    int channel = Mix_PlayChannel(-1, select, 0);
                    faza = 8;
                }
                if (s->type == SDL_MOUSEBUTTONDOWN && scoreboard_btn->btn_clicked(x, y))
                {
                    int channel = Mix_PlayChannel(-1, select, 0);
                    faza = 109;
                }

                if (s->type == SDL_MOUSEBUTTONDOWN && themejungle_btn->btn_clicked(x, y))
                {
                    int channel = Mix_PlayChannel(-1, select, 0);
                    theme++;
                    break;
                }
                SDL_RenderPresent(m_renderer);
            }
            while (faza == 109){
                SDL_PollEvent(e);
                SDL_RenderClear(m_renderer);
                betterText(m_renderer, "SElect your scoreboard mode", 20, 20, 255, 255, 255,200, 75, "calibrib");
                x = e->motion.x;
                y = e->motion.y;
                bg1->render();
                timesspace_btn->pic->render();
                randommode_btn->pic->render();
                backspace_btn->pic->render();
                if (e->type == SDL_MOUSEBUTTONDOWN && timesspace_btn->btn_clicked(x, y)){
                    faza = 110;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && randomspace_btn->btn_clicked(x, y)){
                    faza = 111;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && backspace_btn->btn_clicked(x, y)) {
                    SDL_Delay(500);
                    faza = 0;
                    break;
                }
                SDL_RenderPresent(m_renderer);
            }
            while (faza == 110){
                SDL_PollEvent(e);
                SDL_RenderClear(m_renderer);
                betterText(m_renderer, "Timed Mode ScoreBoard", 20, 20, 255, 255, 255,200, 75, "calibrib");
                x = e->motion.x;
                y = e->motion.y;
                bg1->render();
                backspace_btn->pic->render();
                //sortScores("player_scores.txt");
                //displayScores(m_renderer,Fontt, sortScores("player_scores.txt"));
                std::vector<Player> players = ReadPlayerData("player_scores.txt");
                SortPlayerData(players);

                for (const Player& player : players) {
                    // Create a string with player information
                    std::string playerInfo = player.username + ": " + std::to_string(player.score);
                    betterText(m_renderer, playerInfo, 100, yscore, 255, 255, 255, 200, 45, "calibrib");
                    cout << endl;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && backspace_btn->btn_clicked(x, y)) {
                    SDL_Delay(500);
                    faza = 0;
                    //break;
                }

                SDL_RenderPresent(m_renderer);
            }
            while (faza == 111){
                SDL_PollEvent(e);
                SDL_RenderClear(m_renderer);
                x = e->motion.x;
                y = e->motion.y;
                bg1->render();
                backspace_btn->pic->render();
                //sortScores("player_scores.txt");
                //displayScores(m_renderer,Fontt, sortScores("player_scores.txt"));
                std::vector<Player> players = ReadPlayerData("player_scores2.txt");
                SortPlayerData(players);

                for (const Player& player : players) {
                    // Create a string with player information
                    std::string playerInfo = player.username + ": " + std::to_string(player.score);
                    betterText(m_renderer, playerInfo, 100, yscore, 255, 255, 255, 200, 45, "calibrib");
                    cout << endl;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && backspace_btn->btn_clicked(x, y)) {
                    SDL_Delay(500);
                    faza = 0;
                    //break;
                }

                SDL_RenderPresent(m_renderer);


            }
            while (faza == 1){
                SDL_PollEvent(e);
                SDL_RenderClear(m_renderer);
                x = e->motion.x;
                y = e->motion.y;
                spacelogin->render();
                backspace_btn->pic->render();
                signingspace_btn->pic->render();
                if (e->type == SDL_MOUSEBUTTONDOWN && backspace_btn->btn_clicked(x, y)) {
                    SDL_Delay(500);
                    int channel = Mix_PlayChannel(-1, select, 0);
                    faza = 0;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && signingspace_btn->btn_clicked(x, y)){
                    username = TextBox(m_renderer, 300, 300, 255, 255, 0);
                    logingin = 1;
                }
                if (username != " "){
                    betterText(m_renderer, username, 300, 300, 255, 255, 0, 200, 75, "calibrib");
                }
                if (logingin == 1) {
                    if (isNameInFile(username)) {
                        cout<<8;
                        faza = 39;
                    }
                    else{
                        faza = 2;
                        username = " ";
                    }
                }
                SDL_RenderPresent(m_renderer);
                e->type = 0;
            }
            while (faza == 2){
                SDL_PollEvent(e);
                SDL_RenderClear(m_renderer);
                x = e->motion.x;
                y = e->motion.y;
                spacesignup->render();
                backspace_btn->pic->render();
                signingspace_btn->pic->render();
                if (e->type == SDL_MOUSEBUTTONDOWN && signingspace_btn->btn_clicked(x, y)) {
                    reguse = TextBox(m_renderer, 300, 300, 255, 255, 0);
                    signingup = 1;
                }
                if (reguse != " "){
                    betterText(m_renderer, reguse, 300, 300, 255, 255, 0, 200, 75, "calibrib");
                }
                if(signingup == 1) {
                    ofstream user("User.txt", ios::app);
                    if (searchStringInCSV("User.txt", username)) {
                        reguse = "";
                        betterTextM(m_renderer, "username already exists", 355, 500, 60, "");
                    }
                    if (reguse != "") {
                        user << reguse;
//                        scorenormal << "0,";
//                        scoretimed << "0,";
//                        scoreinf << "0,";
//                        scorenormal.close();
//                        scoreinf.close();
//                        scoretimed.close();
                        user << endl;
                    }
                    signingup = false;
                    user.close();
                    faza = 39;
                }
                else {
                    ifstream user("User.txt", ios::in);
                    if (findIndexInCSV("User.txt", username) != -1) {
                        user.close();
                        faza = 39;
                    } else {
                        username = "";
                    }
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && backspace_btn->btn_clicked(x, y)) {
                    SDL_Delay(500);
                    int channel = Mix_PlayChannel(-1, select, 0);
                    faza = 0;
                }
                SDL_RenderPresent(m_renderer);
            }
            while (faza == 85){
                faza = 37;
            }
            while (faza == 37){
                SDL_PollEvent(e);
                SDL_RenderClear(m_renderer);
                x = e->motion.x;
                y = e->motion.y;
                maingamespace->render();
                soundcancel_btn->pic->render();
                soundup_btn->pic->render();
                sounddown_btn->pic->render();
                backspace_btn->pic->render();
                next_btn->pic->render();
                prev_btn->pic->render();
                betterText(m_renderer, mucicName, 500, 650, 255, 255, 255, 250, 50, "Rockinsoda");
                betterText(m_renderer, intToString(volume), 650, 500, 255, 255, 255, 250, 50, "Rockinsoda");
                if (e->type == SDL_MOUSEBUTTONDOWN && soundcancel_btn->btn_clicked(x, y)){
                    if (musicpaused) {
                        Mix_ResumeMusic();
                    } else {
                        Mix_PauseMusic();
                    }
                    musicpaused = !musicpaused;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && sounddown_btn->btn_clicked(x, y)){
                    faza = 85;
                    volume -= 5;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && soundup_btn->btn_clicked(x, y)){
                    faza = 85;
                    volume += 5;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && prev_btn->btn_clicked(x, y)) {
                    SDL_Delay(300);
                    int channel = Mix_PlayChannel(-1, select, 0);
                    faza = 85;
                    play = 1;
                    ahang--;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && next_btn->btn_clicked(x, y)) {
                    SDL_Delay(300);
                    int channel = Mix_PlayChannel(-1, select, 0);
                    faza = 85;
                    play = 1;
                    ahang++;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && backspace_btn->btn_clicked(x, y)) {
                    SDL_Delay(500);
                    int channel = Mix_PlayChannel(-1, select, 0);
                    faza = 38;
                }
                SDL_RenderPresent(m_renderer);
            }
            while (faza == 38){
                SDL_PollEvent(e);
                SDL_RenderClear(m_renderer);
                x = e->motion.x;
                y = e->motion.y;
                maingamespace->render();
                backspace_btn->pic->render();
                spacemusicingame_btn->pic->render();
                resumespace_btn->pic->render();
                if (e->type == SDL_MOUSEBUTTONDOWN && resumespace_btn->btn_clicked(x, y)) {
                    int channel = Mix_PlayChannel(-1, select, 0);
                    faza = 40;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && backspace_btn->btn_clicked(x, y)) {
                    int channel = Mix_PlayChannel(-1, select, 0);
                    faza = 0;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && spacemusicingame_btn->btn_clicked(x, y)) {
                    int channel = Mix_PlayChannel(-1, select, 0);
                    faza = 37;
                }
                SDL_RenderPresent(m_renderer);
            }
            while (faza == 40){
                SDL_Event e;
                SDL_PollEvent(s);
                SDL_RenderClear(m_renderer);
                maingamespace->render();
                pausespace_btn->pic->render();
                bombspace->render();
                multicolorspace->render();
                betterText(m_renderer, "Press F", 1250, 250, 255, 0, 0, 200, 45, "Rockinsoda");
                betterText(m_renderer, "Press M", 1250, 400, 255, 0, 0, 200, 45, "Rockinsoda");
                while(atgame){
                    moving_balls_color();
                    shooting_ball_color();
                    atgame = 0;
                }
                betterText(m_renderer, intToString(timegone), 20, 25, 255, 255, 255, 100, 45, "Rockinsoda");

                timegone += 0.05;
                SDL_Delay(10);
                //SDL_SetRenderDrawColor(m_renderer,0,0,0,250);


                if (k < MAX_B)
                {
                    while (SDL_PollEvent(&e)) {
                        if (e.type == SDL_KEYDOWN) {
                            switch (e.key.keysym.sym) {
                                case SDLK_SPACE:
                                    cout << "Space key pressed";
                                    faza = 38;
                                    break;
                            }
                        }
                        if(e.type == SDL_MOUSEMOTION) {
                            SDL_GetMouseState(&xMouse, &yMouse);
                            alfa = atan2((xMouse - X),(yMouse - Y));
                            angleimage(m_renderer, cannonpic2, 680, 500, 78, 159, -1 * (alfa * 180 / M_PI + 180));
                        }
                    }
                    alfa = atan2((xMouse - X),(yMouse-Y));
                    angleimage(m_renderer,cannonpic2, 680, 500, 78, 159, -1 * (alfa * 180 / M_PI + 180));
                    if (!gameOver){
                        pause_effect();
                        slow_mo_effect();
                        //------draw------
                        draw_aim(m_renderer);
                        draw_moving_balls(m_renderer);
                        draw_shooting_and_shooted_balls(m_renderer);
                        shadow_move_and_draw(m_renderer);
                        //----------------
                        pop();
                        check_around();
                        moving_balls_down();

                        end_game();
                        shoot_ball();
                        shooting_ball_move();
                        ball_reflection();
                    }
                }
                if(gameOver){
                    faza = 33;
                    break;
                }
                recordPlayerScore(username, toops, zaman);
                SDL_RenderPresent(m_renderer);
            }
            while (faza == 41){
                SDL_Event e;
                SDL_PollEvent(s);
                SDL_RenderClear(m_renderer);
                maingamespace->render();
                pausespace_btn->pic->render();
                bombspace->render();
                multicolorspace->render();
                betterText(m_renderer, "Press F", 1250, 250, 255, 0, 0, 200, 45, "Rockinsoda");
                betterText(m_renderer, "Press M", 1250, 400, 255, 0, 0, 200, 45, "Rockinsoda");
                while(atgame){
                    moving_balls_color();
                    shooting_ball_color();
                    atgame = 0;
                }
                betterText(m_renderer, intToString(timeleft), 20, 25, 255, 255, 255, 0, 45, "Rockinsoda");

                timeleft -= 0.05;
                if(timeleft == 0){
                    faza = 33;
                    break;
                }
                SDL_Delay(10);
                //SDL_SetRenderDrawColor(m_renderer,0,0,0,250);


                if (k < MAX_B)
                {
                    while (SDL_PollEvent(&e)) {
                        if (e.type == SDL_KEYDOWN) {
                            switch (e.key.keysym.sym) {
                                case SDLK_SPACE:
                                    cout << "Space key pressed";
                                    faza = 38;
                                    break;
                            }
                        }
                        if(e.type == SDL_MOUSEMOTION) {
                            SDL_GetMouseState(&xMouse, &yMouse);
                            alfa = atan2((xMouse - X),(yMouse - Y));
                            angleimage(m_renderer, cannonpic2, 680, 500, 78, 159, -1 * (alfa * 180 / M_PI + 180));
                        }
                    }
                    alfa = atan2((xMouse - X),(yMouse-Y));
                    angleimage(m_renderer,cannonpic2, 680, 500, 78, 159, -1 * (alfa * 180 / M_PI + 180));
                    if (!gameOver){
                        pause_effect();
                        slow_mo_effect();
                        //------draw------
                        draw_aim(m_renderer);
                        draw_moving_balls(m_renderer);
                        draw_shooting_and_shooted_balls(m_renderer);
                        shadow_move_and_draw(m_renderer);
                        //----------------
                        pop();
                        check_around();
                        moving_balls_down();

                        end_game();
                        shoot_ball();
                        shooting_ball_move();
                        ball_reflection();
                    }
                }
                if(gameOver){
                    faza = 33;
                    break;
                }
                recordPlayerScore2(username, toops, zaman);
                SDL_RenderPresent(m_renderer);
            }
            while (faza == 33){
                SDL_PollEvent(s);
                SDL_RenderClear(m_renderer);
                x = s->motion.x;
                y = s->motion.y;
                bg1->render();
                bg1->render();
                gameoverspace->render();
                backspace_btn->pic->render();
                if (s->type == SDL_MOUSEBUTTONDOWN && backspace_btn->btn_clicked(x, y))
                {
                    int channel = Mix_PlayChannel(-1, select, 0);
                    faza = 0;
                }
                SDL_RenderPresent(m_renderer);

            }
            while (faza == 39){
                SDL_PollEvent(e);
                SDL_RenderClear(m_renderer);
                x = e->motion.x;
                y = e->motion.y;
                bg1->render();
                randomspace_btn->pic->render();
                timesspace_btn->pic->render();
                infinitespace_btn->pic->render();
                backspace_btn->pic->render();
                betterText(m_renderer, hellospace, 260, 100, 255, 255, 255, 250, 50, "Rockinsoda");
                if (e->type == SDL_MOUSEBUTTONDOWN && backspace_btn->btn_clicked(x, y))
                {
                    SDL_Delay(500);
                    int channel = Mix_PlayChannel(-1, select, 0);
                    faza = 0;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && randomspace_btn->btn_clicked(x, y)){
                    faza = 40;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && timesspace_btn->btn_clicked(x, y)){
                    faza = 41;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && infinitespace_btn->btn_clicked(x, y)){
                    faza = 40;
                }
                SDL_RenderPresent(m_renderer);
            }
            while (faza == 3)
            {
                SDL_PollEvent(s);
                SDL_RenderClear(m_renderer);
                x = s->motion.x;
                y = s->motion.y;
                bg1->render();
                spacehelp_btn->pic->render();
                backspace_btn->pic->render();
                spacemusic_btn->pic->render();
                if (s->type == SDL_MOUSEBUTTONDOWN && backspace_btn->btn_clicked(x, y))
                {
                    int channel = Mix_PlayChannel(-1, select, 0);
                    faza = 0;
                }
                if (s->type == SDL_MOUSEBUTTONDOWN && spacehelp_btn->btn_clicked(x, y))
                {
                    int channel = Mix_PlayChannel(-1, select, 0);
                    faza = 4;
                }
                if (s->type == SDL_MOUSEBUTTONDOWN && spacemusic_btn->btn_clicked(x, y))
                {
                    int channel = Mix_PlayChannel(-1, select, 0);
                    faza = 5;
                }
                SDL_RenderPresent(m_renderer);
            }
            while (faza == 4)
            {
                SDL_PollEvent(s);
                SDL_RenderClear(m_renderer);
                x = s->motion.x;
                y = s->motion.y;
                bg1->render();
                helpspace->render();
                backspace_btn->pic->render();
                if (s->type == SDL_MOUSEBUTTONDOWN && backspace_btn->btn_clicked(x, y))
                {
                    int channel = Mix_PlayChannel(-1, select, 0);
                    faza = 3;
                }
                SDL_RenderPresent(m_renderer);
            }
            while (faza == 99)
                faza = 5;
            while (faza == 5)
            {
                SDL_PollEvent(s);
                SDL_RenderClear(m_renderer);
                x = s->motion.x;
                y = s->motion.y;
                bg1->render();
                backspace_btn->pic->render();
                soundcancel_btn->pic->render();
                soundup_btn->pic->render();
                sounddown_btn->pic->render();
                next_btn->pic->render();
                prev_btn->pic->render();
                betterText(m_renderer, mucicName, 500, 600, 255, 255, 255, 250, 50, "Rockinsoda");
                if (s->type == SDL_MOUSEBUTTONDOWN && backspace_btn->btn_clicked(x, y))
                {
                    int channel = Mix_PlayChannel(-1, select, 0);
                    faza = 3;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && soundcancel_btn->btn_clicked(x, y)){
                    if (musicpaused) {
                        Mix_ResumeMusic();
                    } else {
                        Mix_PauseMusic();
                    }
                    musicpaused = !musicpaused;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && sounddown_btn->btn_clicked(x, y)){
                    volume -= 5;
                }
                if (e->type == SDL_MOUSEBUTTONDOWN && soundup_btn->btn_clicked(x, y)){
                    volume += 5;
                }
                if (s->type == SDL_MOUSEBUTTONDOWN && prev_btn->btn_clicked(x, y))
                {
                    int channel = Mix_PlayChannel(-1, select, 0);
                    faza = 99;
                    ahangfazaei = 1;
                    musicspace--;
                }
                if (s->type == SDL_MOUSEBUTTONDOWN && next_btn->btn_clicked(x, y))
                {
                    int channel = Mix_PlayChannel(-1, select, 0);
                    faza = 99;
                    ahangfazaei = 1;
                    musicspace++;
                }
                SDL_RenderPresent(m_renderer);
            }
            if (faza == 8)
                quit = true;
            break;
        }
    }
    //-----------------------------------------End of the Main loop-----------------------------------------//
    Mix_FreeChunk(select);
    TTF_CloseFont(Fontt);
    SDL_DestroyWindow(m_window);
    SDL_DestroyRenderer(m_renderer);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
//-----------------------------------------------Functions-----------------------------------------------//
void betterText(SDL_Renderer *renderer, string S, int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a, int size, string Font)
{
    if (!TTF_WasInit())
        TTF_Init();
    string fontDir = "./fonts/" + Font + ".ttf"; // should have fonts as .ttf files in ./fonts folder from project path
    TTF_Font *font = TTF_OpenFont(fontDir.c_str(), size);
    SDL_Color color = {r, g, b, a};
    SDL_Surface *surface = TTF_RenderText_Solid(font, S.c_str(), color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect;
    int h, w;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    rect.h = h;
    rect.w = w;
    rect.x = x;
    rect.y = y;
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    // free resources
    TTF_CloseFont(font);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

string TextBox(SDL_Renderer *Renderer, int x_position, int y_position, int R, int G, int B)
{
    bool caps_lock = false;
    int A = 0.5;
    int W = 1280;
    int H = 800;
    bool quit = false;
    char ch = 0;
    SDL_Event *event = new SDL_Event();
    string name = "";
    char lastkey;
    name = name.substr(0, name.length() - 1);
    while (!quit)
    {
        ch = 0;
        // SDL_RenderClear(Renderer);
        SDL_RenderPresent(Renderer);
        if (SDL_PollEvent(event) && (event->type == SDL_KEYUP || event->type == SDL_MOUSEBUTTONDOWN))
        {
            if (event->type == SDL_MOUSEBUTTONDOWN)
                quit = true;
            switch (event->key.keysym.sym)
            {
                case SDLK_SPACE:
                    // quit = true;
                    ch = ' ';
                    break;
                case SDLK_RETURN:
                    quit = true;
                    break;
                case SDLK_KP_ENTER:
                    quit = true;
                    break;
                case SDLK_TAB:
                    quit = true;
                    break;
                case SDLK_CAPSLOCK:
                    ch = 0;
                    caps_lock = !caps_lock;
                    break;
                case SDLK_BACKSPACE:
                    ch = 0;
                    if (name.length() != 0)
                    {
                        betterText(Renderer, name.c_str(), x_position, y_position, 221, 239, 163, 256, 75, "calibrib");
                        name.pop_back();
                    }
                    break;
                default:
                    if (event->key.keysym.sym != SDLK_UP && event->key.keysym.sym != SDLK_DOWN && event->key.keysym.sym != SDLK_LEFT && event->key.keysym.sym != SDLK_RIGHT)
                        ch = event->key.keysym.sym;
                    break;
            }
            if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch == ' ') || (ch >= '0' && ch <= '9') && name.length() < 25 && !quit && ch != 0)
            {
                if (caps_lock == 1 && ch != ' ' && (ch < '0' || ch > '9'))
                    name += ch + 'A' - 'a';
                else
                    name += ch;
            }
            if (name != "" && !quit)
                betterText(Renderer, name.c_str(), x_position, y_position, R, G, B, A, 75, "calibrib");
        }
        SDL_RenderPresent(Renderer);
        lastkey = event->key.keysym.sym;
    }
    string output;
    if (lastkey == name[name.length() - 1])
        output = name.substr(0, name.length() - 1);
    else
        output = name;
    return output;
}

void set_random_color1(int r[12][MAX_L], int g[12][MAX_L], int b[12][MAX_L])
{
    for (int j = 0; j < MAX_L; j++)
        for (int i = 0; i < 12; i++)
        {
            int rc = rand() % 5;
            if (rc == 0)
            {
                //soolati
                r[i][j] = 255;
                g[i][j] = 59;
                b[i][j] = 139;
            }
            else if (rc == 1)
            {
                //cyan
                r[i][j] = 30;
                g[i][j] = 240;
                b[i][j] = 206;
            }
            else if (rc == 2)
            {
                //abi_oili
                r[i][j] = 0;
                g[i][j] = 4;
                b[i][j] = 122;
            }
            else if (rc == 3)
            {
                //purple
                r[i][j] = 172;
                g[i][j] = 72;
                b[i][j] = 252;
            }
            else if (rc == 4)
            {
                //white
                r[i][j] = 245;
                g[i][j] = 245;
                b[i][j] = 245;
            }
        }
}

bool login(string UseEnter){
    string UserName;
    bool On_Off = true;
    bool Off = true;
    while(On_Off){

        fstream user;
        user.open("Users.txt", ios::in);
        if(Off && user.is_open()){
            string read;
            while(getline(user,read)){
                stringstream convertor(read);
                convertor >> UserName ;
                if(UseEnter == UserName)
                    Off = false;
            }
        }
        if(!Off){
            user.close();
            On_Off = false;
            return 1;
        }
        else
            return 0;
    }
}

int registry(string newName)
{

    string checkName;
    bool if_ON = true;
    bool oo = true;

//UserName Validation
    while(oo){
        fstream yusers;
        yusers.open("Users.txt",ios::in);
        if(if_ON && yusers.is_open()){
            string readd;
            while(getline(yusers,readd)){
                stringstream OnlyUsers(readd);
                OnlyUsers >> checkName;
                if(newName == checkName){
                    if_ON = false;
                    return 1;
                }
            }
            yusers.close();
            if(!if_ON){
                if_ON = true;
            }
            else{
                oo = false;
            }
        }
    }
}

string readNthValue(const string &filePath, int n) {
    ifstream file(filePath);

    if (!file.is_open()) {
        cerr << "Error opening file: " << filePath << endl;
        return "";  // Return an empty string indicating failure
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string value;

        for (int i = 1; i <= n; ++i) {
            if (!getline(ss, value, ',')) {
                cerr << "Error reading value at position " << n << " in line: " << line << endl;
                return "";  // Return an empty string indicating failure
            }
        }

        if (n == 1) {
            return value;  // Return the nth value
        }
    }

    cerr << "Error: Could not find value at position " << n << " in the file." << endl;
    return "";  // Return an empty string indicating failure
}

bool searchStringInCSV(const string &filename, const string &target) {
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return false;
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string cell;

        while (getline(iss, cell, ',')) {
            if (cell == target) {
                file.close();
                return true;
            }
        }
    }

    file.close();
    return false;
}

int findIndexInCSV(const string &filePath, const string &targetString) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filePath << endl;
        return -1; // Return -1 to indicate an error
    }

    string line;
    getline(file, line); // Read the header line

    istringstream headerStream(line);
    string column;
    int index = 0;

    while (getline(headerStream, column, ',')) {
        if (column == targetString) {
            file.close(); // Close the file after finding the index
            return index;
        }
        ++index;
    }

    file.close(); // Close the file if the target string is not found
    return -1; // Return -1 to indicate that the target string is not in the CSV
}

void replaceNthStringInCSV(const string &filePath, int n, const string &replacement) {
    fstream file(filePath, ios::in | ios::out);
    n++;
    if (file.is_open()) {
        vector<string> tokens;
        string line;

        while (getline(file, line)) {
            stringstream ss(line);
            string token;

            while (getline(ss, token, ',')) {
                tokens.push_back(token);
            }

            if (tokens.size() >= n) {
                tokens[n - 1] = replacement;  // Assuming 1-based index
                file.seekp(0, ios::cur);  // Move the write pointer to the current position
                file.seekg(0, ios::beg);  // Move the read pointer to the beginning

                for (const auto &t: tokens) {
                    file << t;
                    if (&t != &tokens.back()) file << ',';
                }
                file << '\n';
                break;  // Stop after processing the first line
            }
            tokens.clear();
        }

        file.close();

    } else {
        cerr << "Error opening the file.\n";
    }
}

void betterTextM(SDL_Renderer *renderer, string S, int x, int y, int size, string Font) {
    if (!TTF_WasInit())
        TTF_Init();
    string fontDir = "Modak-Regular.ttf"; // should have fonts as .ttf files in ./fonts folder from project path
    TTF_Font *font = TTF_OpenFont(fontDir.c_str(), size);
    SDL_Color color = {0, 0, 0, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(font, S.c_str(), color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect;
    int h, w;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    rect.h = h;
    rect.w = w;
    rect.x = x;
    rect.y = y;
    //cout << h << " " << w << endl;
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    // free resources
    TTF_CloseFont(font);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

}

bool isNameInFile(string name) {
    string UserName;
    bool On_Off = true;
    bool Off = true;
    while(On_Off){

        fstream user;
        user.open("User.txt", ios::in);
        if(Off && user.is_open()){
            string line;
            while(getline(user,line)){
                stringstream convertor(line);
                convertor >> UserName;
                if(name == UserName) {
                    Off = false;
                }
            }
        }
        if(!Off){
            user.close();
            On_Off = false;
            return true;
        }
        else
            return false;
    }
}

string intToString(int num) {
    return to_string(num);
}


void recordPlayerScore(const string& username, int toops, int zaman) {
    int score = (toops * 10) - (zaman / 60);
    string filePath = "player_scores.txt";
    ofstream outFile;
    outFile.open(filePath, ios_base::app);

    if (!outFile) {
        cerr << "Failed to open file for writing." << endl;
    } else {
        outFile << username << " " << score << endl;
        outFile.close();
        cout << "Score recorded successfully." <<endl;
    }
}

void recordPlayerScore2(const string& username, int toops, int zaman) {
    int score = (toops * 100) - (zaman / 20);
    string filePath = "player_scores2.txt";
    ofstream outFile;
    outFile.open(filePath, ios_base::app);

    if (!outFile) {
        cerr << "Failed to open file for writing." << endl;
    } else {
        outFile << username << " " << score << endl;
        outFile.close();
        cout << "Score recorded successfully." <<endl;
    }
}

//vector<PlayerScore> sortScores(const string& filename) {
//    vector<PlayerScore> scores;
//    ifstream file(filename);
//    string line;
//    while (getline(file, line)) {
//        istringstream iss(line);
//        string username;
//        int score;
//        if (!(iss >> username >> score)) { break; }
//        scores.emplace_back(username, score);
//    }
//    sort(scores.rbegin(), scores.rend());
//}

//void displayScores(SDL_Renderer* renderer, TTF_Font* font, const std::vector<PlayerScore>& scores) {
//    int x = 100;
//    int y = 100;
//
//    for (const auto& score : scores) {
//        std::string text = score.username + ": " + std::to_string(score.score);
//        SDL_Color textColor = {255, 255, 255};
//        SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), textColor);
//        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
//        int textWidth = surface->w;
//        int textHeight = surface->h;
//        SDL_Rect renderQuad = {x, y, textWidth, textHeight};
//        SDL_RenderCopy(renderer, texture, nullptr, &renderQuad);
//        SDL_FreeSurface(surface);
//        SDL_DestroyTexture(texture);
//
//        y += textHeight + 5;
//    }
//    SDL_RenderPresent(renderer);
//}

bool ComparePlayers(const Player& a, const Player& b) {
    return a.score > b.score;
}

std::vector<Player> ReadPlayerData(const std::string& filename) {
    std::vector<Player> players;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return players;
    }

    std::string username;
    int score;

    while (file >> username >> score) {
        Player player;
        player.username = username;
        player.score = score;
        players.push_back(player);
    }

    file.close();

    // Sort players by score in descending order
    std::sort(players.begin(), players.end(), ComparePlayers);

    return players;
}


void SortPlayerData(std::vector<Player>& players) {
    // Sort players by score in descending order
    std::sort(players.begin(), players.end(), ComparePlayers);
}

void angleimage(SDL_Renderer* renderer, SDL_Texture *text, int x, int y, int w, int h, double alfa)
{
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderCopyEx(renderer, text, NULL, &rect, alfa, NULL, SDL_FLIP_NONE);
}