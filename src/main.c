/*
*
*           BEES - PDE made for myself. - Alexander Presthus
*
*/


#include <stdio.h>
#include <SDL.h>
#include <stdbool.h>
#include <stdint.h>


void handle_input(SDL_Keysym key_symbol);

//editor modes either COMMAND or INSERT
typedef enum EditorMode_T{
    COMMAND,
    INSERT
}EditorMode;

typedef struct EditorBuffer_T{
    char *filename;
    char *buffer;
    int cursorX;
    int cursorY;
    int cursorWidth;
    int cursorHeight;
    int cursorBlinkSpeed;
    int bufferWidth;
    int bufferHeight;
    EditorMode mode;
}EditorBuffer;

typedef struct EditorConfig_T{
    int cursorX;
    int cursorY;
    int cursorWidth;
    int cursorHeight;
    int cursorBlinkSpeed;

    int windowWidth;
    int windowHeight;
    EditorMode mode;
    EditorBuffer *buffers;

}EditorConfig;


//editor config
static EditorConfig editorConfig = {
    .cursorX = 0,
    .cursorY = 0,
    .cursorWidth = 1,
    .cursorHeight = 1,
    .cursorBlinkSpeed = 500,
    .windowWidth = 1024,
    .windowHeight = 768,
    .mode = COMMAND
};

int main(void){

if (SDL_Init(SDL_INIT_VIDEO) != 0){
    printf("SDL_Init Error: %s\n", SDL_GetError());
    return 1;
}

SDL_Window *win = SDL_CreateWindow("Bees - >", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, editorConfig.windowWidth, editorConfig.windowHeight, SDL_WINDOW_RESIZABLE);

if (win == NULL){
    printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
}

SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
//set renderer color
SDL_SetRenderDrawColor(ren, 90,45,138, 255);
if (ren == NULL){
    SDL_DestroyWindow(win);
    printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
}

SDL_Event event;
bool quit = false;

while (!quit){
    SDL_PollEvent(&event);
    switch (event.type){
        case SDL_QUIT:
            quit = true;
            break;
        case SDL_KEYDOWN:
            handle_input(event.key.keysym);
            break;
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_RESIZED){
                editorConfig.windowWidth = event.window.data1;
                editorConfig.windowHeight = event.window.data2;
            }
            break;
    }
    SDL_RenderClear(ren);
    SDL_RenderPresent(ren);
}

SDL_DestroyRenderer(ren);
SDL_DestroyWindow(win);
SDL_Quit();

    return 0;
}


void open_buffer(char *filename){



}


void handle_input(SDL_Keysym key_symbol){

    switch (key_symbol.sym)
    {
    case SDLK_ESCAPE:
        editorConfig.mode = COMMAND;
        break;
    case SDLK_i:
    if(editorConfig.mode == COMMAND){ 
        editorConfig.mode = INSERT;
        };
        
    break;
    
    default:
        break;
    }

}