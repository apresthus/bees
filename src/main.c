/*
*
*           BEES - PDE made for myself. - Alexander Presthus
*
*/

// This is a "in the evenings when I feel like it" kind of project.

//TODO: Open files & save files
//TODO: Ability to edit text files (Duh!)
//TODO: Add line numbers
//TODO: OpenGL???
//TODO: Cursor
//TODO: Support TrueType fonts
//TODO: Add syntax highlighting / lexer parser
//TODO: Prefix tree for autocompletion / discovery of files
//TODO: Add ability to open terminal in place of buffer
//TODO: Dynamic layout of buffers
//TODO: Implement concept of workspaces / projects and save its configuration





#include <stdio.h>
#include <SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include "bees_math.h"
typedef struct color_T{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
}color;
typedef enum EditorMode_T{
    COMMAND,
    INSERT
}EditorMode;

typedef struct EditorBuffer_T{
    int id;
    char *filename;
    char *buffer;
    float posX;
    float posY;
    int cursorX;
    int cursorY;
    int cursorWidth;
    int cursorHeight;
    int cursorBlinkSpeed;
    int bufferWidth;
    int bufferHeight;
    color bufferColor;
    EditorMode mode;
}EditorBuffer;

typedef struct EditorConfig_T{
    int cursorX;
    int cursorY;
    int cursorWidth;
    int cursorHeight;
    int cursorBlinkSpeed;
    int bufferCount;
    int windowWidth;
    int windowHeight;
    EditorMode mode;
    EditorBuffer *buffers;
    color statusLineColor;
    color commandLineColor;
    color tabColor;
    color frameColor;
    color lineNumbersPanelColor;

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
    .mode = COMMAND,
    .statusLineColor = {
        .r = 0,
        .g = 0,
        .b = 0,
        .a = 255
    },
    .commandLineColor = {
        .r = 150,
        .g = 0,
        .b = 180,
        .a = 255
    },
    .tabColor = {
        .r = 0,
        .g = 0,
        .b = 0,
        .a = 255
    },
    .frameColor = {
        .r = 187,
        .g = 175,
        .b = 161,
        .a = 255
    },
    .lineNumbersPanelColor = {
        .r = 0,
        .g = 0,
        .b = 0,
        .a = 255
    }
};
void handle_input(SDL_Keysym key_symbol);
void draw_editor(SDL_Renderer *ren, EditorConfig *editorConfig);
void draw_cursor(SDL_Renderer *ren, EditorConfig *editorConfig);
void draw_editor(SDL_Renderer *ren, EditorConfig *editorConfig);
void draw_buffer(SDL_Renderer *ren, EditorBuffer *buffer);
void drawCommandLine(SDL_Renderer *ren, EditorConfig *editorConfig);
void drawLineNumbersPanel(SDL_Renderer *ren, EditorConfig *editorConfig, Vec2 pos);
void open_buffer (EditorConfig *editorConfig, char *filename);
void save_buffer (EditorConfig *editorConfig, char *filename);
void recalculate_layout(EditorConfig *editorConfig);


int main(void){

EditorBuffer buffer = {
    .id = 0,
    .filename = "test.txt",
    .buffer = "test",
    .cursorX = 0,
    .cursorY = 0,
    .posX = 0,
    .posY = 0,
    .cursorWidth = 1,
    .cursorHeight = 1,
    .cursorBlinkSpeed = 500,
    .bufferWidth = 400,
    .bufferHeight = 400,
    .mode = COMMAND,
    .bufferColor = {
        .r = 23,
        .g = 23,
        .b = 23,
        .a = 255
    }
};

EditorBuffer buffer2 = {
    .id = 1,
    .filename = "test2.txt",
    .buffer = "test2",
    .cursorX = 0,
    .cursorY = 0,
    .posX = 0,
    .posY = 0,
    .cursorWidth = 1,
    .cursorHeight = 1,
    .cursorBlinkSpeed = 500,
    .bufferWidth = 0,
    .bufferHeight = 0,
    .mode = COMMAND,
    .bufferColor = {
        .r = 23,
        .g = 23,
        .b = 23,
        .a = 255
    }
};

editorConfig.buffers = malloc(sizeof(EditorBuffer) * 1);
editorConfig.bufferCount = 1;

editorConfig.buffers[0] = buffer;
//editorConfig.buffers[1] = buffer2;


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

         SDL_SetRenderDrawColor(ren, 0, 0, 0, 255); 
         SDL_RenderClear(ren);
         draw_editor(ren, &editorConfig);
         drawCommandLine(ren, &editorConfig);
         SDL_RenderPresent(ren);

}

SDL_DestroyRenderer(ren);
SDL_DestroyWindow(win);
SDL_Quit();

    return 0;
}

void draw_cursor(SDL_Renderer *ren, EditorConfig *editorConfig){
    SDL_Rect cursor = {
        .x = editorConfig->cursorX,
        .y = editorConfig->cursorY,
        .w = editorConfig->cursorWidth,
        .h = editorConfig->cursorHeight
    };
    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
    SDL_RenderFillRect(ren, &cursor);
}


void draw_buffer(SDL_Renderer *ren, EditorBuffer *buffer){
    SDL_Rect bufferRect = {
        .x = buffer->posX,
        .y = buffer->posY,
        .w = buffer->bufferWidth,
        .h = buffer->bufferHeight,
    
    };

    SDL_SetRenderDrawColor(ren, buffer->bufferColor.r, buffer->bufferColor.g, buffer->bufferColor.b, buffer->bufferColor.a);
    SDL_RenderFillRect(ren, &bufferRect);
    draw_cursor(ren, &editorConfig);
}

void draw_editor(SDL_Renderer *ren, EditorConfig *editorConfig) {
   
    int bufferWidth = editorConfig->windowWidth / editorConfig->bufferCount;
    int bufferHeight = editorConfig->windowHeight;

    for (int i = 0; i < editorConfig->bufferCount; i++) {
        EditorBuffer *buffer = &editorConfig->buffers[i];
        int bufferX = i * bufferWidth+50;
        int bufferY = 0;

        // draw diving line bwtween buffers
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderDrawLine(ren, bufferX, bufferY, bufferX, bufferHeight);

        // Set the buffer position and size
        buffer->bufferWidth = bufferWidth-50;
        buffer->bufferHeight = bufferHeight - 20;
        buffer->cursorX = bufferX + buffer->cursorX;
        buffer->cursorY = bufferY + buffer->cursorY;
        buffer->posX = bufferX;
        buffer->posY = bufferY;
        Vec2 bufferpos = vec2(bufferX, bufferY);
        // Draw the buffer
        draw_buffer(ren, buffer);
         drawLineNumbersPanel( ren,  editorConfig, bufferpos);
    }
   


}

void drawCommandLine(SDL_Renderer *ren, EditorConfig *editorConfig){
    SDL_Rect commandLine = {
        .x = 0,
        .y = editorConfig->windowHeight - 20,
        .w = editorConfig->windowWidth,
        .h = 20
    };
    SDL_SetRenderDrawColor(ren, editorConfig->commandLineColor.r, editorConfig->commandLineColor.g, editorConfig->commandLineColor.b, editorConfig->commandLineColor.a);
    SDL_RenderFillRect(ren, &commandLine);
}

void drawLineNumbersPanel(SDL_Renderer *ren, EditorConfig *editorConfig, Vec2 bufferpos){
    SDL_Rect lineNumbersPanel = {
        .x = 0,
        .y = 0,
        .w = 50,
        .h = editorConfig->windowHeight
    };
    SDL_SetRenderDrawColor(ren, editorConfig->lineNumbersPanelColor.r, editorConfig->lineNumbersPanelColor.g, editorConfig->lineNumbersPanelColor.b, editorConfig->lineNumbersPanelColor.a);
    SDL_RenderFillRect(ren, &lineNumbersPanel);
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