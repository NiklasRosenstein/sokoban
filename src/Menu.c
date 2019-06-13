
#include "Menu.h"

bool MENU_SHOWN = true;

static void draw_text(int x, int y, int w, int h, char *text) {
    // TODO: Implement draw text function
}

static void draw_rect_border(int x, int y, int w, int h, int size) {
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    // TODO: Use SDL_RenderDrawRects() instead of loop
    for (int i = 0; i < size; i++) {
        SDL_SetRenderDrawColor(WINRENDERER, 0x21, 0x21, 0x21, 0xFF);
        SDL_RenderDrawRect(WINRENDERER, &rect);

        rect.x += 1;
        rect.y += 1;
        rect.w -= 2;
        rect.h -= 2;
    }
}

static void draw_background(int w, int h) {
    SDL_Rect back_rect;
    back_rect.x = 0;
    back_rect.y = 0;
    back_rect.w = w;
    back_rect.h = h;

    // Draw background
    SDL_SetRenderDrawColor(WINRENDERER, 0x32, 0x32, 0x32, 0xFF);
    SDL_RenderFillRect(WINRENDERER, &back_rect);

    // Draw border
    draw_rect_border(back_rect.x, back_rect.y, back_rect.w, back_rect.h, 5);
}

static void draw_button(int x, int y, int w, int h, char *text) {
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    // Draw background
    SDL_SetRenderDrawColor(WINRENDERER, 0x46, 0x46, 0x46, 0xFF);
    SDL_RenderFillRect(WINRENDERER, &rect);

    // Draw border
    draw_rect_border(x, y, w, h, 5);

    // Draw text
    draw_text(x + 20, y + 10, w - 40, h - 20, text);
}

void show_menu() {
    int w = BOARD_WIDTH * 32;
    int h = BOARD_HEIGHT * 32;

    draw_background(w, h);

    draw_button(48, 48, w - 96, 48, "Button 1");
    draw_button(48, 144, w - 96, 48, "Button 2");
}

