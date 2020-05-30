#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <time.h>

typedef struct{
    unsigned char r;
    unsigned char g;
    unsigned char b;
}rgb;

rgb cga[16] = {
    {0, 0, 0}, 
    {0, 0, 0xAA},
    {0, 0xAA, 0}, 
    {0, 0xAA, 0xAA},
    {0xAA, 0, 0}, 
    {0xAA, 0, 0xAA},
    {0xAA, 0x55, 0}, 
    {0xAA, 0xAA, 0xAA},
    {0x55, 0x55, 0x55}, 
    {0x55, 0x55, 0xFF},
    {0x55, 0xFF, 0x55}, 
    {0x55, 0xFF, 0xFF},
    {0xFF, 0x55, 0x55}, 
    {0xFF, 0x55, 0xFF},
    {0xFF, 0xFF, 0x55}, 
    {0xFF, 0xFF, 0xFF},
};

typedef struct{
    SDL_Rect rect;
    rgb pal;
    char destroyed;
}brick;

void reload(brick *brks)
{
    brick bricks[8 * 8];
    for(unsigned int x = 0; x < 8; x++){
        for(unsigned int y = 0; y < 8; y++){
            brks[x + y * 8].pal = cga[rand() % 15];
            if(brks[x + y * 8].pal.r == 0 && brks[x + y * 8].pal.g == 0 && brks[x + y * 8].pal.b == 0xAA){
                brks[x + y * 8].pal = (rgb){0x55, 0x55, 0xFF};
            }
            int xoff = (y % 2) * 8;
            brks[x + y * 8].rect = (SDL_Rect){(24 + xoff) + (8 * x) + x * 16, 24 + (8 * y) + y * 8, 16, 8};
            brks[x + y * 8].destroyed = 0;
        }
    }
}

int main(void)
{
    srand(time(NULL));

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0){
        printf("[SDL2]: %s\n", SDL_GetError());
        return -1;
    }

    if(TTF_Init() < 0){
        printf("[SDL2_ttf]: %s\n", TTF_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Breaky", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 640, 0);
    if(!window){
        printf("[SDL2]: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!renderer){
        printf("[SDL2]: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_RenderSetLogicalSize(renderer, 240, 320);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    TTF_Font *font = TTF_OpenFont("oswald.ttf", 16);
    char scoretext[10];
    char lifetext[2];

    SDL_Color col = {255, 255, 255, 255};

    SDL_Surface *tmp = TTF_RenderText_Solid(font, "Score:", col);
    SDL_Texture *scorehalf = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_Rect scorehalf_rect = {0, -4, tmp->w, tmp->h};
    SDL_FreeSurface(tmp);

    SDL_Surface *tmp1 = TTF_RenderText_Solid(font, scoretext, (SDL_Color){255, 255, 255, 255});
    SDL_Texture *scorer = SDL_CreateTextureFromSurface(renderer, tmp1);
    SDL_Rect scorer_rect = {32, -4, 32, 16};
    SDL_FreeSurface(tmp1);

    SDL_Surface *tmp2 = TTF_RenderText_Solid(font, "Lives:", col);
    SDL_Texture *lifer = SDL_CreateTextureFromSurface(renderer, tmp2);
    SDL_Rect lifer_rect = {96, -4, tmp2->w, tmp2->h};
    SDL_FreeSurface(tmp2);

    SDL_Surface *tmps = TTF_RenderText_Solid(font, "Paused!", col);
    SDL_Texture *pauser = SDL_CreateTextureFromSurface(renderer, tmps);
    SDL_Rect pauser_rect = {96, 144, tmps->w, tmps->h};
    SDL_FreeSurface(tmps);

    SDL_Rect paddle = {
        .x = (240 / 2) - 14,
        .y = 280,
        .w = 28,
        .h = 4
    };

    SDL_Rect ball = {
        .x = (240 / 2) - 2,
        .y = 180,
        .w = 4,
        .h = 4
    };
    int dx = 1;
    int dy = 1;

    brick bricks[8 * 8];
    reload(bricks);

    int score = 0;
    int lives = 3;
    int quit_attempt = 0;

    snprintf(lifetext, 2, "%d", lives);

    SDL_Surface *tmp3 = TTF_RenderText_Solid(font, lifetext, col);
    SDL_Texture *live = SDL_CreateTextureFromSurface(renderer, tmp3);
    SDL_Rect live_rect = {134, -4, tmp3->w, tmp3->h};
    SDL_FreeSurface(tmp3);

    int quitter = 0;
    SDL_Event e;
    while(!quitter){
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) {
                quitter = 1;
            }else if( e.type == SDL_KEYDOWN ){
                switch( e.key.keysym.sym ){
                    case SDLK_ESCAPE:{
                        ++quit_attempt;
                        if(quit_attempt > 1){
                            quitter = 1;
                            break;
                        }
                    }break;
                    case SDLK_TAB:
                        quit_attempt = 0;
                        break;
                    default:
                        break;
                }
            }
        }

        if(quit_attempt == 0){
            switch(dx){
                case 0:{
                    --ball.x;
                    if(ball.x <= 0)
                        ++dx;
                }break;
                case 1:{
                    ++ball.x;
                    if(ball.x >= 240 - 2)
                        --dx;
                }break;
            }

            switch(dy){
                case 0:{
                    --ball.y;
                    if(ball.y <= 18)
                        ++dy;
                }break;
                case 1:{
                    ++ball.y;
                    if(ball.y >= 320 - 2){
                        --dy;
                        ball.x = (240 / 2) - 2;
                        ball.y = 180;
                        --lives;

                        snprintf(lifetext, 2, "%d", lives);
                        SDL_Surface *tmper = TTF_RenderText_Solid(font, lifetext, col);
                        SDL_DestroyTexture(live);
                        live = SDL_CreateTextureFromSurface(renderer, tmper);
                        live_rect = (SDL_Rect){134, -4, tmper->w, tmper->h};
                        SDL_FreeSurface(tmper);
                    }
                }break;
            }

            int wipe = 0;
            for(unsigned int x = 0; x < 8; x++){
                for(unsigned int y = 0; y < 8; y++){
                    if(SDL_HasIntersection(&ball, &bricks[x + y * 8].rect) && !bricks[x + y * 8].destroyed){
                        bricks[x + y * 8].destroyed = 1;

                        if(ball.x + ball.w < bricks[x + y * 8].rect.x + (bricks[x + y * 8].rect.w / 2)){
                            dx = 0;
                        }else{
                            dx = 1;
                        }

                        if(ball.y + ball.h < bricks[x + y * 8].rect.y + (bricks[x + y * 8].rect.h / 2)){
                            dy = 0;
                        }else{
                            dy = 1;
                        }
                        score += 100;
                        snprintf(scoretext, 10, "%d", score);

                        SDL_Surface *tmper = TTF_RenderText_Solid(font, scoretext, col);
                        SDL_DestroyTexture(scorer);
                        scorer = SDL_CreateTextureFromSurface(renderer, tmper);
                        scorer_rect = (SDL_Rect){scorehalf_rect.w + 16, -4, tmper->w, tmper->h};
                        SDL_FreeSurface(tmper);
                    }
                    if(bricks[x + y * 8].destroyed)
                        ++wipe;
                }
            }

            if(wipe == 8 * 8){
                reload(bricks);

                ball.x = (240 / 2) - 2;
                ball.y = 180;
            }

            if(lives == 0){
                reload(bricks);
                ball.x = (240 / 2) - 2;
                ball.y = 180;

                lives = 3;

                quit_attempt = 1;
            }

            const Uint8* cur_keyboard = SDL_GetKeyboardState(NULL);

            if(cur_keyboard[SDL_SCANCODE_A]){
                if(paddle.x >= 0)
                    paddle.x -= 2;
            }

            if(cur_keyboard[SDL_SCANCODE_D]){
                if(paddle.x + paddle.w <= 240)
                    paddle.x += 2;
            }

            SDL_Rect info = {0, 0, 0, 0};
            if(SDL_IntersectRect(&ball, &paddle, &info)){
                if(ball.x + ball.w < paddle.x + (paddle.w / 2)){
                    dx = 0;
                }else{
                    dx = 1;
                }
                dy ^= 1;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0xA8, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_Rect board = {0, 0, 240, 18};
        SDL_RenderFillRect(renderer, &board);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &paddle);
        SDL_RenderFillRect(renderer, &ball);

        for(unsigned int x = 0; x < 8; x++){
            for(unsigned int y = 0; y < 8; y++){
                if(!bricks[x + y * 8].destroyed){
                    SDL_SetRenderDrawColor(renderer, bricks[x + y * 8].pal.r, bricks[x + y * 8].pal.g, bricks[x + y * 8].pal.b, 255);
                    SDL_RenderFillRect(renderer, &bricks[x + y * 8].rect);
                }
            }
        }

        SDL_RenderCopy(renderer, scorehalf, NULL, &scorehalf_rect);
        SDL_RenderCopy(renderer, scorer, NULL, &scorer_rect);
        SDL_RenderCopy(renderer, lifer, NULL, &lifer_rect);
        SDL_RenderCopy(renderer, live, NULL, &live_rect);

        if(quit_attempt >= 1){
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 192);
            SDL_Rect scr = {0, 0, 240, 320};
            SDL_RenderFillRect(renderer, &scr);
            SDL_RenderCopy(renderer, pauser, NULL, &pauser_rect);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(pauser);
    SDL_DestroyTexture(live);
    SDL_DestroyTexture(lifer);
    SDL_DestroyTexture(scorer);
    SDL_DestroyTexture(scorehalf);

    TTF_CloseFont(font);
    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}