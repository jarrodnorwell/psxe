#include "frontend/screen.h"

#include "psx/input/sda.h"
#include "psx/input/guncon.h"

uint32_t screen_get_button(SDL_Keycode k)
{
    if (k == SDLK_X)
        return PSXI_SW_SDA_CROSS;
    if (k == SDLK_A)
        return PSXI_SW_SDA_SQUARE;
    if (k == SDLK_W)
        return PSXI_SW_SDA_TRIANGLE;
    if (k == SDLK_D)
        return PSXI_SW_SDA_CIRCLE;
    if (k == SDLK_RETURN)
        return PSXI_SW_SDA_START;
    if (k == SDLK_S)
        return PSXI_SW_SDA_SELECT;
    if (k == SDLK_UP)
        return PSXI_SW_SDA_PAD_UP;
    if (k == SDLK_DOWN)
        return PSXI_SW_SDA_PAD_DOWN;
    if (k == SDLK_LEFT)
        return PSXI_SW_SDA_PAD_LEFT;
    if (k == SDLK_RIGHT)
        return PSXI_SW_SDA_PAD_RIGHT;
    if (k == SDLK_Q)
        return PSXI_SW_SDA_L1;
    if (k == SDLK_E)
        return PSXI_SW_SDA_R1;
    if (k == SDLK_1)
        return PSXI_SW_SDA_L2;
    if (k == SDLK_3)
        return PSXI_SW_SDA_R2;
    if (k == SDLK_Z)
        return PSXI_SW_SDA_L3;
    if (k == SDLK_C)
        return PSXI_SW_SDA_R3;
    if (k == SDLK_2)
        return PSXI_SW_SDA_ANALOG;

    return 0;
}

uint32_t screen_get_button_joystick(uint8_t b)
{
    if (b == SDL_GAMEPAD_BUTTON_SOUTH)
        return PSXI_SW_SDA_CROSS;
    if (b == SDL_GAMEPAD_BUTTON_WEST)
        return PSXI_SW_SDA_SQUARE;
    if (b == SDL_GAMEPAD_BUTTON_NORTH)
        return PSXI_SW_SDA_TRIANGLE;
    if (b == SDL_GAMEPAD_BUTTON_EAST)
        return PSXI_SW_SDA_CIRCLE;
    if (b == SDL_GAMEPAD_BUTTON_START)
        return PSXI_SW_SDA_START;
    if (b == SDL_GAMEPAD_BUTTON_GUIDE)
        return PSXI_SW_SDA_SELECT;
    if (b == SDL_GAMEPAD_BUTTON_DPAD_UP)
        return PSXI_SW_SDA_PAD_UP;
    if (b == SDL_GAMEPAD_BUTTON_DPAD_DOWN)
        return PSXI_SW_SDA_PAD_DOWN;
    if (b == SDL_GAMEPAD_BUTTON_DPAD_LEFT)
        return PSXI_SW_SDA_PAD_LEFT;
    if (b == SDL_GAMEPAD_BUTTON_DPAD_RIGHT)
        return PSXI_SW_SDA_PAD_RIGHT;
    if (b == SDL_GAMEPAD_BUTTON_LEFT_SHOULDER)
        return PSXI_SW_SDA_L1;
    if (b == SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER)
        return PSXI_SW_SDA_R1;
    if (b == SDL_GAMEPAD_AXIS_LEFT_TRIGGER)
        return PSXI_SW_SDA_L2; // Can't map these yet
    if (b == SDL_GAMEPAD_AXIS_RIGHT_TRIGGER)
        return PSXI_SW_SDA_R2; // Can't map these yet
    if (b == SDL_GAMEPAD_BUTTON_LEFT_STICK)
        return PSXI_SW_SDA_L3;
    if (b == SDL_GAMEPAD_BUTTON_RIGHT_STICK)
        return PSXI_SW_SDA_R3;

    return 0;
}

SDL_Gamepad *screen_find_gamepad(void)
{
    int count = 0;
    SDL_GetGamepads(&count);
    for (int i = 0; i < count; i++)
        if (SDL_IsGamepad(i))
            return SDL_OpenGamepad(i);

    return NULL;
}

int screen_get_base_width(psxe_screen_t *screen)
{
    int width = psx_get_dmode_width(screen->psx);

    switch (width)
    {
    case 256:
        return 256;
    case 320:
        return 320;
    case 368:
        return 384;
    }

    return 320;
}

psxe_screen_t *psxe_screen_create(void)
{
    return (psxe_screen_t *)malloc(sizeof(psxe_screen_t));
}

void psxe_screen_init(psxe_screen_t *screen, psx_t *psx)
{
    memset(screen, 0, sizeof(psxe_screen_t));

    if (screen->debug_mode)
    {
        screen->width = PSX_GPU_FB_WIDTH;
        screen->height = PSX_GPU_FB_HEIGHT;
    }
    else
    {
        screen->width = 320;
        screen->height = 240;
    }

    screen->scale = 1;
    screen->open = 1;
    screen->format = SDL_PIXELFORMAT_XBGR1555;
    screen->psx = psx;
    screen->pad = psx_get_pad(psx);

    screen->texture_width = PSX_GPU_FB_WIDTH;
    screen->texture_height = PSX_GPU_FB_HEIGHT;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD);
    SDL_SetRenderDrawColor(screen->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    screen->gamepad = screen_find_gamepad();
}

void psxe_screen_reload(psxe_screen_t *screen)
{
    if (screen->texture)
        SDL_DestroyTexture(screen->texture);
    if (screen->renderer)
        SDL_DestroyRenderer(screen->renderer);
    if (screen->window)
        SDL_DestroyWindow(screen->window);

    if (screen->debug_mode)
    {
        screen->width = PSX_GPU_FB_WIDTH;
        screen->height = PSX_GPU_FB_HEIGHT;
    }
    else
    {
        if (screen->vertical_mode)
        {
            screen->width = 240;
            screen->height = screen_get_base_width(screen);
        }
        else
        {
            screen->width = screen_get_base_width(screen);
            screen->height = 240;
        }
    }

    screen->window = SDL_CreateWindow(
        "psxe " STR(REP_VERSION) "-" STR(REP_COMMIT_HASH),
        screen->width * screen->scale,
        screen->height * screen->scale,
        0);

    screen->renderer = SDL_CreateRenderer(
        screen->window, NULL);

    screen->texture = SDL_CreateTexture(
        screen->renderer,
        screen->format,
        SDL_TEXTUREACCESS_STREAMING,
        screen->texture_width, screen->texture_height);

#if SDL_VERSION_ATLEAST(2, 0, 12)
    SDL_SetTextureScaleMode(screen->texture, screen->bilinear);
#endif

    // Check for retina displays
    int width = 0, height = 0;

    SDL_GetCurrentRenderOutputSize(screen->renderer, &width, &height);

    if (width != (screen->width * screen->scale))
    {
        float width_scale = (float)width / (float)(screen->width * screen->scale);
        float height_scale = (float)height / (float)(screen->height * screen->scale);

        SDL_SetRenderScale(screen->renderer, width_scale, height_scale);
    }

    SDL_SetRenderVSync(screen->renderer, true);

    screen->open = 1;
}

int psxe_screen_is_open(psxe_screen_t *screen)
{
    return screen->open;
}

void psxe_screen_toggle_debug_mode(psxe_screen_t *screen)
{
    screen->debug_mode = !screen->debug_mode;

    psxe_screen_set_scale(screen, screen->saved_scale);

    screen->texture_width = PSX_GPU_FB_WIDTH;
    screen->texture_height = PSX_GPU_FB_HEIGHT;

    psxe_gpu_dmode_event_cb(screen->psx->gpu);
}

// int frame = 0;

void psxe_screen_update(psxe_screen_t *screen)
{
    void *display_buf = screen->debug_mode ? psx_get_vram(screen->psx) : psx_get_display_buffer(screen->psx);

    // printf("res=(%u,%u) off=(%u,%u) disp=(%u,%u-%u,%u) draw=(%u,%u-%u,%u) vres=%u\n",
    //     screen->texture_width,
    //     screen->texture_height,
    //     screen->psx->gpu->disp_x,
    //     screen->psx->gpu->disp_y,
    //     screen->psx->gpu->disp_x1,
    //     screen->psx->gpu->disp_y1,
    //     screen->psx->gpu->disp_x2,
    //     screen->psx->gpu->disp_y2,
    //     screen->psx->gpu->draw_x1,
    //     screen->psx->gpu->draw_y1,
    //     screen->psx->gpu->draw_x2,
    //     screen->psx->gpu->draw_y2,
    //     screen->psx->gpu->disp_y2 - screen->psx->gpu->disp_y1
    // );

    if ((screen->psx->gpu->disp_y + screen->texture_height) > 512)
        display_buf = psx_get_vram(screen->psx);

    SDL_UpdateTexture(screen->texture, NULL, display_buf, PSX_GPU_FB_STRIDE);
    SDL_RenderClear(screen->renderer);

    if (!screen->debug_mode)
    {
        SDL_FRect dstrect;

        dstrect.x = screen->image_xoff;
        dstrect.y = screen->image_yoff;
        dstrect.w = screen->image_width;
        dstrect.h = screen->image_height;

        SDL_RenderTextureRotated(
            screen->renderer,
            screen->texture,
            NULL, &dstrect,
            screen->vertical_mode ? 270 : 0,
            NULL, SDL_FLIP_NONE);
    }
    else
    {
        SDL_RenderTexture(screen->renderer, screen->texture, NULL, NULL);
    }

    SDL_RenderPresent(screen->renderer);

    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
        {
            screen->open = 0;
        }
        break;

        case SDL_EVENT_KEY_DOWN:
        {
            switch (event.key.key)
            {
            case SDLK_F1:
            {
                psxe_screen_toggle_debug_mode(screen);

                return;
            }
            break;

            case SDLK_F2:
            {
                SDL_Surface *surface = SDL_CreateSurface(
                    screen->width,
                    screen->height,
                    SDL_PIXELFORMAT_XBGR1555);

                SDL_RenderReadPixels(
                    screen->renderer,
                    NULL);

                SDL_SaveBMP(surface, "snap/screenshot.bmp");

                SDL_DestroySurface(surface);
            }
            break;

            case SDLK_F3:
            {
                screen->vertical_mode = !screen->vertical_mode;

                psxe_gpu_dmode_event_cb(screen->psx->gpu);
            }
            break;

            case SDLK_F4:
            {
                screen->bilinear = !screen->bilinear;

                psxe_gpu_dmode_event_cb(screen->psx->gpu);
            }
            break;

            case SDLK_F11:
            {
                screen->fullscreen = !screen->fullscreen;

                SDL_SetWindowFullscreen(
                    screen->window,
                    screen->fullscreen);

                psxe_gpu_dmode_event_cb(screen->psx->gpu);

                SDL_SetRenderDrawColor(screen->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderClear(screen->renderer);
            }
            break;

            case SDLK_F5:
            {
                psx_soft_reset(screen->psx);
            }
            break;

            case SDLK_F6:
            {
                psx_swap_disc(screen->psx, ".\\roms\\Street Fighter II Movie (Japan) (Disc 2)\\Street Fighter II Movie (Japan) (Disc 2).cue");
            }
            break;
            }

            uint32_t mask = screen_get_button(event.key.key);

            psx_pad_button_press(screen->pad, 0, mask);

            if (event.key.scancode == SDLK_RETURN)
                psx_exp2_atcons_put(screen->psx->exp2, 13);
        }
        break;

        case SDL_EVENT_GAMEPAD_ADDED:
        {
            if (!screen->gamepad)
                screen->gamepad = SDL_OpenGamepad(event.cdevice.which);
        }
        break;

        case SDL_EVENT_GAMEPAD_REMOVED:
        {
            SDL_Joystick *joy = SDL_GetGamepadJoystick(screen->gamepad);
            SDL_JoystickID id = SDL_GetJoystickID(joy);

            if (screen->gamepad && (event.cdevice.which == id))
            {
                SDL_CloseGamepad(screen->gamepad);

                screen->gamepad = screen_find_gamepad();
            }
        }
        break;

        case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
        {
            SDL_Joystick *joy = SDL_GetGamepadJoystick(screen->gamepad);
            SDL_JoystickID id = SDL_GetJoystickID(joy);

            if (screen->gamepad && (event.gdevice.which == id))
            {
                uint32_t mask = screen_get_button_joystick(event.gbutton.button);

                psx_pad_button_press(screen->pad, 0, mask);
            }
        }
        break;

        case SDL_EVENT_GAMEPAD_BUTTON_UP:
        {
            SDL_Joystick *joy = SDL_GetGamepadJoystick(screen->gamepad);
            SDL_JoystickID id = SDL_GetJoystickID(joy);

            if (screen->gamepad && (event.gdevice.which == id))
            {
                uint32_t mask = screen_get_button_joystick(event.gbutton.button);

                psx_pad_button_release(screen->pad, 0, mask);
            }
        }
        break;

        case SDL_EVENT_GAMEPAD_AXIS_MOTION:
        {
            SDL_Joystick *joy = SDL_GetGamepadJoystick(screen->gamepad);
            SDL_JoystickID id = SDL_GetJoystickID(joy);

            if (screen->gamepad && (event.gdevice.which == id))
            {
                int mapped_axis = ((int)event.gaxis.value + INT16_MAX + 1) / 0x100;

                switch (event.gaxis.axis)
                {
                case SDL_GAMEPAD_AXIS_RIGHTX:
                    psx_pad_analog_change(screen->pad, 0, PSXI_AX_SDA_RIGHT_HORZ, mapped_axis);
                    break;

                case SDL_GAMEPAD_AXIS_RIGHTY:
                    psx_pad_analog_change(screen->pad, 0, PSXI_AX_SDA_RIGHT_VERT, mapped_axis);
                    break;

                case SDL_GAMEPAD_AXIS_LEFTX:
                    psx_pad_analog_change(screen->pad, 0, PSXI_AX_SDA_LEFT_HORZ, mapped_axis);
                    break;

                case SDL_GAMEPAD_AXIS_LEFTY:
                    psx_pad_analog_change(screen->pad, 0, PSXI_AX_SDA_LEFT_VERT, mapped_axis);
                    break;
                }
            }
        }
        break;

            // To-do: GunCon
            // case SDL_MOUSEMOTION: {
            //     psx_pad_analog_change(screen->pad, 0, PSXI_AX_GUNCON_SX, psx_get_dmode_width(screen->psx));
            //     psx_pad_analog_change(screen->pad, 0, PSXI_AX_GUNCON_SY, psx_get_dmode_height(screen->psx));
            //     psx_pad_analog_change(screen->pad, 0, PSXI_AX_GUNCON_X, event.motion.x * (1.0f / (float)screen->scale));
            //     psx_pad_analog_change(screen->pad, 0, PSXI_AX_GUNCON_Y, event.motion.y * (1.0f / (float)screen->scale));
            // } break;

            // case SDL_MOUSEBUTTONDOWN: {
            //     switch (event.button.button) {
            //         case SDL_BUTTON_LEFT: {
            //             psx_pad_button_press(screen->pad, 0, PSXI_SW_GUNCON_A);
            //         } break;

            //         case SDL_BUTTON_RIGHT: {
            //             psx_pad_button_press(screen->pad, 0, PSXI_SW_GUNCON_B);
            //         } break;

            //         case SDL_BUTTON_MIDDLE: {
            //             psx_pad_button_press(screen->pad, 0, PSXI_SW_GUNCON_TRIGGER);
            //         } break;
            //     }
            // } break;

            // case SDL_MOUSEBUTTONUP: {
            //     switch (event.button.button) {
            //         case SDL_BUTTON_LEFT: {
            //             psx_pad_button_release(screen->pad, 0, PSXI_SW_GUNCON_A);
            //         } break;

            //         case SDL_BUTTON_RIGHT: {
            //             psx_pad_button_release(screen->pad, 0, PSXI_SW_GUNCON_B);
            //         } break;

            //         case SDL_BUTTON_MIDDLE: {
            //             psx_pad_button_release(screen->pad, 0, PSXI_SW_GUNCON_TRIGGER);
            //         } break;
            //     }
            // } break;

        case SDL_EVENT_TEXT_INPUT:
        {
            psx_exp2_atcons_put(screen->psx->exp2, event.text.text[0]);
        }
        break;

        case SDL_EVENT_KEY_UP:
        {
            uint32_t mask = screen_get_button(event.key.key);

            psx_pad_button_release(screen->pad, 0, mask);
        }
        break;
        }
    }
}

void psxe_screen_set_scale(psxe_screen_t *screen, unsigned int scale)
{
    if (screen->debug_mode)
    {
        screen->scale = 1;
    }
    else
    {
        screen->scale = scale;
        screen->saved_scale = scale;
    }
}

void psxe_screen_destroy(psxe_screen_t *screen)
{
    SDL_DestroyTexture(screen->texture);
    SDL_DestroyRenderer(screen->renderer);
    SDL_DestroyWindow(screen->window);

    SDL_Quit();

    free(screen);
}

void psxe_gpu_dmode_event_cb(psx_gpu_t *gpu)
{
    psxe_screen_t *screen = gpu->udata[0];

    // printf("res=(%u,%u) off=(%u,%u) disp=(%u,%u-%u,%u) draw=(%u,%u-%u,%u) vres=%u\n",
    //     screen->texture_width,
    //     screen->texture_height,
    //     screen->psx->gpu->disp_x,
    //     screen->psx->gpu->disp_y,
    //     screen->psx->gpu->disp_x1,
    //     screen->psx->gpu->disp_y1,
    //     screen->psx->gpu->disp_x2,
    //     screen->psx->gpu->disp_y2,
    //     screen->psx->gpu->draw_x1,
    //     screen->psx->gpu->draw_y1,
    //     screen->psx->gpu->draw_x2,
    //     screen->psx->gpu->draw_y2,
    //     screen->psx->gpu->disp_y2 - screen->psx->gpu->disp_y1
    // );

    screen->format = psx_get_display_format(screen->psx) ? SDL_PIXELFORMAT_RGB24 : SDL_PIXELFORMAT_XBGR1555;

    if (screen->debug_mode)
    {
        screen->width = PSX_GPU_FB_WIDTH;
        screen->height = PSX_GPU_FB_HEIGHT;
        screen->texture_width = PSX_GPU_FB_WIDTH;
        screen->texture_height = PSX_GPU_FB_HEIGHT;
    }
    else
    {
        if (screen->fullscreen)
        {
            const SDL_DisplayMode *dm = SDL_GetCurrentDisplayMode(0);

            screen->width = dm->w;
            screen->height = dm->h;

            if (screen->vertical_mode)
            {
                screen->image_width = screen->height;
                screen->image_height = (double)screen->height / psx_get_display_aspect(screen->psx);

                int off = (screen->image_width - screen->image_height) / 2;

                screen->image_xoff = (screen->width / 2) - (screen->image_width / 2);
                screen->image_yoff = off;
            }
            else
            {
                screen->image_width = (double)screen->height * psx_get_display_aspect(screen->psx);
                screen->image_height = screen->height;
                screen->image_xoff = (screen->width / 2) - (screen->image_width / 2);
                screen->image_yoff = 0;
            }
        }
        else
        {
            if (screen->vertical_mode)
            {
                screen->width = 240 * screen->scale;
                screen->height = screen_get_base_width(screen) * screen->scale;
                screen->image_width = screen->height;
                screen->image_height = screen->width;

                int off = (screen->image_width - screen->image_height) / 2;

                screen->image_xoff = -off;
                screen->image_yoff = off;
            }
            else
            {
                screen->width = screen_get_base_width(screen) * screen->scale;
                screen->height = 240 * screen->scale;
                screen->image_width = screen->width;
                screen->image_height = screen->height;
                screen->image_xoff = 0;
                screen->image_yoff = 0;
            }
        }

        screen->texture_width = psx_get_display_width(screen->psx);
        screen->texture_height = psx_get_display_height(screen->psx);
    }

    SDL_DestroyTexture(screen->texture);

    screen->texture = SDL_CreateTexture(
        screen->renderer,
        screen->format,
        SDL_TEXTUREACCESS_STREAMING,
        screen->texture_width, screen->texture_height);

#if SDL_VERSION_ATLEAST(2, 0, 12)
    SDL_SetTextureScaleMode(screen->texture, screen->bilinear);
#endif

    SDL_SetWindowSize(screen->window, screen->width, screen->height);
}

void psxe_gpu_vblank_event_cb(psx_gpu_t *gpu)
{
    psxe_screen_t *screen = gpu->udata[0];

    psxe_screen_update(screen);

    psxe_gpu_vblank_timer_event_cb(gpu);
}