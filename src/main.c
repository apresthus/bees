/*
*
*           BEES - PDE made for myself. - Alexander Presthus
*
*/

// This is a "in the evenings when I feel like it" kind of project.

//Motivation: Bees (Because Everything Else Sucks), not really, but I just like the idea of my own editor built from almost scratch (will probably ditch  SDL at some point
//or it will just be a glorified OPENGL initializer, which is fine.).

//TODO: Open files & save files - kinda able to open files now.
//TODO: Ability to edit text files (Duh!)
//TODO: Externalize Editor config. Do I want my own format or use something like Lua or whatever?, probably the former.
//TODO: Add line numbers
//TODO: OpenGL??? - At this point probably, if I want to do some fancy experimental GUI stuff.
//TODO: Cursor
//TODO: Support TrueType fonts - kinda done.
//TODO: Add syntax highlighting / lexer parser - No LSP. I want to do this myself. Also just limited intellisense as I feel like too much of it actually makes me a worse programmer.
//TODO: Prefix tree for autocompletion / discovery of files. I've done this many times before, so it should be easy.
//TODO: Add ability to open terminal in place of buffer. - Probably just a terminal emulator in a buffer.
//TODO: Dynamic layout of buffers
//TODO: Implement concept of workspaces / projects and save its configuration
//TODO: Integration of compiling and debugging of especially C projects. - lldb with some kind of GUI?


#include <stdio.h>
#include <SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include "bees_math.h"
#include <SDL2/SDL_ttf.h>
#define SV_IMPLEMENTATION
#include "../libs/StringView/sv.h"

typedef struct color_T{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
}color;  //TODO: Probably want to have a RGB, HSL and HSV color structs
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
    char *titleBar;
    EditorMode mode;
    EditorBuffer *buffers;
    EditorBuffer *activeBuffer;
    color statusLineColor;
    color commandLineColor;
    color tabColor;
    color frameColor;
    color lineNumbersPanelColor;
    TTF_Font *font;

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
    },
    .titleBar = "Bees -> scratch buffer (not saved)"
};
void handle_input(SDL_Keysym key_symbo, SDL_Event event);
void draw_editor(SDL_Renderer *ren, EditorConfig *editorConfig);
void draw_cursor(SDL_Renderer *ren, EditorConfig *editorConfig);
void draw_editor(SDL_Renderer *ren, EditorConfig *editorConfig);
void draw_buffer(SDL_Renderer *ren, EditorBuffer *buffer);
void draw_text(SDL_Renderer *ren, EditorBuffer *buffer ,Vec2 pos, char *text);
void draw_char(SDL_Renderer *ren, EditorBuffer *buffer, char c, Vec2 pos);
void draw_status_line(SDL_Renderer *ren, EditorConfig *editorConfig);
void drawLineNumbersPanel(SDL_Renderer *ren, EditorConfig *editorConfig, Vec2 pos);
bool open_buffer (EditorConfig *editorConfig, char *filename, EditorBuffer *_buffer);
void save_buffer (EditorConfig *editorConfig, char *filename);
void recalculate_layout(EditorConfig *editorConfig);
void loadFonts();





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

void draw_status_line(SDL_Renderer *ren, EditorConfig *_editorConfig){
    SDL_Rect commandLine = {
        .x = 0,
        .y = editorConfig.windowHeight - 20,
        .w = editorConfig.windowWidth,
        .h = 20
    };
    SDL_SetRenderDrawColor(ren, editorConfig.commandLineColor.r, editorConfig.commandLineColor.g, editorConfig.commandLineColor.b, editorConfig.commandLineColor.a);
    SDL_RenderFillRect(ren, &commandLine);
    char* string = "";
    if (editorConfig.mode == COMMAND){
        string = "-COMMAND-";
    }
    else{
        string = "-INSERT-";
    }
    SDL_Color color = {255,255,255,255};
     SDL_Surface *surface = TTF_RenderUTF8_Solid(editorConfig.font, string, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(ren, surface);
    SDL_Rect rect = {
        .x = 25,
        .y = editorConfig.windowHeight - 19,
        .w = surface->w,
        .h = surface->h
    };
    SDL_RenderCopy(ren, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    
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

bool open_buffer (EditorConfig *editorConfig, char *filename, EditorBuffer *_buffer){

    FILE *file  = fopen("/Users/apresthus/Dev/Bees/data/test.txt", "r");
    if (file == NULL){
        printf("Error opening file: %s\n", filename);
        return false;
        exit(1);

    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(fileSize + 1);
    fread(buffer, fileSize, 1, file);
    fclose(file);

    buffer[fileSize] = '\0';

    _buffer->buffer = buffer;
    _buffer->cursorX = 0;
    _buffer->cursorY = 0;

    return true;
}


void loadFonts(){
    editorConfig.font = TTF_OpenFont("../../fonts/FiraMono-Regular.ttf", 16);
    if (editorConfig.font == NULL){
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
        exit(1);
    }
}

void draw_text(SDL_Renderer *ren, EditorBuffer *buffer ,Vec2 pos, char *text){

    SDL_Color color = {255, 255, 0, 255};
    SDL_Surface *surface = TTF_RenderUTF8_Solid_Wrapped(editorConfig.font, text, color, 0);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(ren, surface);
    SDL_Rect rect = {
        .x = pos.x,
        .y = pos.y+18,
        .w = surface->w,
        .h = surface->h
    };
    SDL_RenderCopy(ren, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}   

void handle_input(SDL_Keysym key_symbol, SDL_Event event){

    switch (key_symbol.sym)
    {
    case SDLK_ESCAPE:
        editorConfig.mode = COMMAND;
      
        break;
    case SDLK_i:
    if(editorConfig.mode == COMMAND){ 
        editorConfig.mode = INSERT;
;
        };
        
    break;

    case SDL_TEXTINPUT:{
        if (editorConfig.mode == INSERT){
            char *text = event.text.text;
            printf("%s", text);        }

    }
    
    default:
        //    draw_char();
        break;
    }

}

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

if (TTF_Init() != 0){
    printf("TTF_Init Error: %s\n", TTF_GetError());
    return 1;
}

loadFonts();


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
            handle_input(event.key.keysym, event);
            break;
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_RESIZED){
                editorConfig.windowWidth = event.window.data1;
                editorConfig.windowHeight = event.window.data2;

            //    recalculate_layout(&editorConfig);
            }
            break;
    }

         SDL_SetRenderDrawColor(ren, 0, 0, 0, 255); 
         SDL_RenderClear(ren);
         draw_editor(ren, &editorConfig);
         draw_status_line(ren, &editorConfig);
         if (open_buffer(&editorConfig, "../../data/test.txt", &buffer) != true){
                printf("error opening buffer\n");
         }
         editorConfig.activeBuffer = &buffer;
         draw_text(ren, &buffer, (Vec2){.x = buffer.posX+70, .y = buffer.posY +10}, buffer.buffer);
         SDL_RenderPresent(ren);
         SDL_SetWindowTitle(win,editorConfig.titleBar);

}

SDL_DestroyRenderer(ren);
SDL_DestroyWindow(win);
SDL_Quit();

    return 0;
}