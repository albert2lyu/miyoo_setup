#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdbool.h>

#include <sys/ioctl.h>
#include <linux/vt.h>
#include <linux/kd.h>
#include <linux/fb.h>

#include <stdio.h>
#include <stdlib.h>

#include <linux/limits.h>

#include <sys/ioctl.h>

#define MIYOO_VIR_SET_MODE    _IOWR(0x100, 0, unsigned long)
#define MIYOO_VIR_SET_VER     _IOWR(0x101, 0, unsigned long)
#define MIYOO_SND_SET_VOLUME  _IOWR(0x100, 0, unsigned long)
#define MIYOO_KBD_GET_HOTKEY  _IOWR(0x100, 0, unsigned long)
#define MIYOO_KBD_SET_VER     _IOWR(0x101, 0, unsigned long)
#define MIYOO_FB0_PUT_OSD     _IOWR(0x100, 0, unsigned long)
#define MIYOO_FB0_SET_MODE    _IOWR(0x101, 0, unsigned long)
#define MIYOO_FB0_GET_VER     _IOWR(0x102, 0, unsigned long)
#define MIYOO_FB0_SET_FLIP    _IOWR(0x103, 0, unsigned long)
#define MIYOO_FB0_SET_FPBP    _IOWR(0x104, 0, unsigned long)
#define MIYOO_FB0_GET_FPBP    _IOWR(0x105, 0, unsigned long)

static const int SDL_WAKEUPEVENT = SDL_USEREVENT+1;

#define WIDTH  320
#define HEIGHT 240

#define BTN_X			SDLK_SPACE
#define BTN_A			SDLK_LCTRL
#define BTN_B			SDLK_LALT
#define BTN_Y			SDLK_LSHIFT
#define BTN_L			SDLK_TAB
#define BTN_R			SDLK_BACKSPACE
#define BTN_START		SDLK_RETURN
#define BTN_SELECT		SDLK_ESCAPE
#define BTN_BACKLIGHT	SDLK_3
#define BTN_POWER		SDLK_END
#define BTN_UP			SDLK_UP
#define BTN_DOWN		SDLK_DOWN
#define BTN_LEFT		SDLK_LEFT
#define BTN_RIGHT		SDLK_RIGHT
#define DBG(x)

SDL_RWops *rw;
SDL_Surface *screen = NULL;
SDL_Surface* img = NULL;
SDL_Rect bgrect;
SDL_Event event;

SDL_Color txtColor = {200, 200, 220};
SDL_Color titleColor = {200, 200, 0};
SDL_Color subTitleColor = {0, 200, 0};
SDL_Color powerColor = {200, 0, 0};

static char buf[1024];


void quit(int err) {
	DBG("");
	system("sync");
	SDL_Quit();
	exit(err);
}

void pushEvent() {
	SDL_Event user_event;
	user_event.type = SDL_KEYUP;
	SDL_PushEvent(&user_event);
}

void set_version(unsigned long version){
	int kbd,vir;
	kbd = open("/dev/miyoo_kbd", O_RDWR);
    	ioctl(kbd, MIYOO_KBD_SET_VER, version);
    	close(kbd);
    	vir = open("/dev/miyoo_vir", O_RDWR);
    	ioctl(vir, MIYOO_VIR_SET_VER, version);
    	close(vir);
}

int main(int argc, char* argv[]) {
	DBG("");
	signal(SIGINT, &quit);
	signal(SIGSEGV,&quit);
	signal(SIGTERM,&quit);

	char title[64] = "";

	printf("%s\n", title);


	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("Could not initialize SDL: %s\n", SDL_GetError());
		return -1;
	}
	SDL_PumpEvents();
	SDL_ShowCursor(0);

	screen = SDL_SetVideoMode(WIDTH, HEIGHT, 16, SDL_SWSURFACE);

	SDL_EnableKeyRepeat(1, 1);

	int setup_screen = 0;
	bool key_matrix = true;
	int miyoo_version = 2;
	set_version(miyoo_version);

	SDL_Surface* _img;
	int loop = 1;

	do {
		if (setup_screen == 0) {
			_img = IMG_Load("screen1.png");
			img = SDL_DisplayFormat(_img);
			SDL_FreeSurface(_img);
			SDL_BlitSurface(img, NULL, screen, &bgrect);
			SDL_Flip(screen);
			while (SDL_WaitEvent(&event)) {
				if (event.type == SDL_KEYUP && event.key.keysym.sym == BTN_SELECT) {
					setup_screen++;
					break;
				} else if (event.type == SDL_KEYUP && event.key.keysym.sym == BTN_A) {
					setup_screen++;
					key_matrix = false;
					miyoo_version = 3;
					set_version(miyoo_version);
					break;
				} else if (event.type == SDL_KEYUP) {
					printf("%d\n", event.key.keysym.sym);
				}
			}
		} else if (setup_screen == 1) {
			_img = IMG_Load("screen2.png");
			img = SDL_DisplayFormat(_img);
			SDL_FreeSurface(_img);
			SDL_BlitSurface(img, NULL, screen, &bgrect);
			SDL_Flip(screen);
			while (SDL_WaitEvent(&event)) {
				if (event.type == SDL_KEYUP && event.key.keysym.sym == BTN_DOWN) {
					setup_screen++;
					break;
				} else if (event.type == SDL_KEYUP && event.key.keysym.sym == BTN_UP) {
					setup_screen++;
					miyoo_version = 4;
					break;
				} else if (event.type == SDL_KEYUP) {
					printf("%d\n", event.key.keysym.sym);
				}
			}
		} else if (setup_screen == 2) {
			loop = 0;
		}

	} while (loop);
	char buf[100];
#if MIYOO
	sprintf(buf, "./miyoo_setup.sh %d %d  > /dev/null 2>&1", miyoo_version);
	system(buf);
#endif

	DBG("");
	system("sync");
	SDL_Quit();

	if (miyoo_version == 4) {
		int fb0;
		unsigned long ret = 1;
		fb0 = open("/dev/miyoo_fb0", O_RDWR);
		ioctl(fb0, MIYOO_FB0_SET_FLIP, &ret);
		set_version(miyoo_version);
		close(fb0);
	}

	exit(0);
	quit(0);
	return 0;
}
