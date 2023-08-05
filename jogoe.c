#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>

#define SCREEN_W 800
#define SCREEN_H 600
#define MAX_BALLS 50

double sec = 1000;
double fps = 30;
double frame;
int initTime, endTime, tTime;

SDL_Color RED = {240, 15, 25}; 
SDL_Color BLUE = {55, 190, 255};
SDL_Color GREEN = {55, 255, 25}; 
SDL_Color YELLOW = {255, 255, 0};
SDL_Color WHITE = {255,255,255};

SDL_Surface* screen;
SDL_Surface* background;
SDL_Event event;
TTF_Font* gameFont;
TTF_Font* titleFont;
Mix_Music* music;

Mix_Chunk* pop;
Mix_Chunk* fill;
Mix_Chunk* get;

SDL_keysym keysym;

int fontSize = 30;
int titleSize = 150;

typedef struct{
	int posx, posy, color;
	float radius;
		
	SDL_Color cor;
	int opacity;
}OBJECT;

typedef struct{
	int pts, lives, bonus;
	OBJECT* obj;
}PLAYER;

typedef struct {
	int dx, dy;
	Mix_Chunk* sound;
	
	OBJECT* obj;
}BALL;

typedef struct {
	
	char* text;
	int posx, posy;
	char value[15];
	SDL_Color color;
}TEXT;

void init();

void game();
int menu(char image[20], char song[20], TEXT* t[4]);
void stage(PLAYER* p, BALL* b[MAX_BALLS], char image[20], char song[20], TEXT* t[3]);
void how_to_play(TEXT* t[3]);
void credits(TEXT* t[1]);

TEXT* init_text();
void load_text(TEXT* t, char txt[20], int x, int y);
void draw_font(char* text, int x, int y, SDL_Color cor);
void draw_title(char* text, int x, int y, SDL_Color cor);
void draw_text(TEXT* t, int info);


PLAYER* init_player();
void load_player(PLAYER* p);
void update_player(PLAYER* p);

BALL* init_ball();
void load_ball(BALL* b);

void draw(OBJECT* obj);
void update_ball(BALL* b);

int level(PLAYER* p);

int colision(PLAYER* p, BALL* b);
int test_colision(PLAYER* p, BALL* b);

void game_over();

int WinMain(int argc, char const *argv[]){
	init();
	atexit(SDL_Quit);
	
	srand(time(NULL));
	
	game();

	return 0;
}
//INICIALIZA O SDL
void init(){
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
		printf("Não foi possível inicializar\n %s", SDL_GetError());
		return;
	}
	if(TTF_Init() == -1){
		printf("Céus, erro ao abrir SDL_ttf: %s\n", TTF_GetError());
 		exit(0);
	} 
	
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
 	Mix_AllocateChannels(16);

	screen = SDL_SetVideoMode(SCREEN_W,SCREEN_H,32,SDL_DOUBLEBUF|SDL_HWSURFACE);
	gameFont = TTF_OpenFont("VT323.ttf", fontSize);
	titleFont = TTF_OpenFont("FascinateInline.ttf", titleSize);

	pop = Mix_LoadWAV("sound/pop.wav");
	get = Mix_LoadWAV("sound/get.wav");
	fill = Mix_LoadWAV("sound/fill.wav");
}
//CARREGA O PLAYER E SEUS ATRIBUTOS
PLAYER* init_player(){
	PLAYER* p = malloc(sizeof(PLAYER));
	p->obj = malloc(sizeof(OBJECT));

	return p;
}
void load_player(PLAYER* p){
	p->obj->posx = 0;
	p->obj->posy = 0;

	p->obj->cor = GREEN;
	p->obj->opacity = 230;
	
	p->obj->radius = 20;
	p->pts = 0;
	p->bonus = 1;
	p->lives = 5;
}

BALL* init_ball(){
	BALL* b = malloc(sizeof(BALL));
	b->obj = malloc(sizeof(OBJECT));
	
	b->obj->radius = 20;
	b->obj->opacity = 190;

	return b;
}

//DESENHA OS OBJETOS NA TELA
void draw(OBJECT* obj){
	filledCircleRGBA(screen, obj->posx, obj->posy,obj->radius,obj->cor.r,obj->cor.g,obj->cor.b,obj->opacity); 
}

//ATUALIZA A POSIÇÃO DO PLAYER
void update_player(PLAYER* p){
		SDL_GetMouseState(&p->obj->posx, &p->obj->posy);
}
//NÍVEL DO JOGO 
int level(PLAYER* p){
	int lv = 0;

	if (p->pts > 1000){
		lv = 1;
	}
	if (p->pts > 3400){
		lv  = 2;
	}
	if (p->pts > 6100){
		lv  = 3;
	}
	if (p->pts > 9200){
		lv  = 4;
	}
	if (p->pts > 13200){
		lv  = 5;
	}
	return lv;
}
// INICIA AS BOLINHAS NA TELA
void load_ball(BALL* b){
	
	b->obj->color = rand()%120;

	if(b->obj->color >= 110){
		b->obj->cor = YELLOW;
		b->sound = get;
 		if (!b->sound) {
 			printf("Wav: SDL error=%s\n", SDL_GetError());
 		return;
 		}
	}
	else if(b->obj->color <= 45){
		b->obj->cor = RED;
		b->sound = pop;
 		if (!b->sound) {
 			printf("Wav: SDL error=%s\n", SDL_GetError());
 		return;
 		}
	}
	else{
		b->obj->cor = BLUE;
		b->sound = fill;
 		if (!b->sound) {
 			printf("Wav: SDL error=%s\n", SDL_GetError());
 		return;
 		}
	}

	b->obj->posx = rand() % SCREEN_W;
	b->obj->posy = rand()%250 -400;

	b->dx = rand()% 3+1;
	b->dy = rand()% 4+1;

	if(b->obj->posx > SCREEN_W /2){
		b->dx *= -1;
	}
}

//ATUALIZA A POSIÇÃO DAS BOLINHAS
void update_ball(BALL* b){
	if(b->obj->posx < -100 || b->obj->posx > (SCREEN_W +100) ||
		b->obj->posy > (SCREEN_H + 100)){
		load_ball(b);
	}
	else{
		b->obj->posx += b->dx;
		b->obj->posy += b->dy;
	}
}
void free_ball(BALL* b[]){
	int i = 0;
	for (i = 0; i < MAX_BALLS; ++i)
	{
		free(b[i]);
		b[i] = NULL;
	}
}
/*void free_text(TEXT* t){
	free(t);
	t = NULL;
}*/

// COLISÃO ENTRE OBJETOS
int colision(PLAYER* p, BALL* b){
	int x1, x2;
	int y1, y2;

	x1 = p->obj->posx;
	x2 = b->obj->posx;

	y1 = p->obj->posy;
	y2 = b->obj->posy;

	float relative = sqrt(((x2-x1)*(x2-x1)) + ((y2-y1)*(y2-y1)));

	if(relative <= (p->obj->radius + b->obj->radius)){
		return 1;
	}
	
	return 0;
}
//INICIALIZA O TEXTO
TEXT* init_text(){
	TEXT* t = malloc(sizeof(TEXT));
	return t;
}
void load_text(TEXT* t, char txt[20], int x, int y){
	t->posx = x;
	t->posy = y;
	t->color = WHITE;
	t->text = txt;

}
//DESENHA UM TEXTO NA TELA
void draw_font(char* text, int x, int y, SDL_Color cor){
  	SDL_Surface* src = TTF_RenderText_Blended(gameFont, text, cor);
  	SDL_Rect dst_rect = {x, y, 0, 0};
  
  	SDL_BlitSurface(src, NULL, screen, &dst_rect);
  
  	SDL_FreeSurface(src);
}

//DESENHA O TEXTO COM SUA INFORMAÇÃO
void draw_text(TEXT* t, int info){
		
	draw_font(t->text, t->posx,t->posy, t->color);
	sprintf(t->value, "%d", info);
	draw_font(t->value, t->posx + 15, t->posy + 25, t->color);
}

void draw_title(char* text, int x, int y, SDL_Color cor){
  	SDL_Surface* src = TTF_RenderText_Blended(titleFont, text, cor);
  	SDL_Rect dst_rect = {x, y, 0, 0};
  
  	SDL_BlitSurface(src, NULL, screen, &dst_rect);
  
  	SDL_FreeSurface(src);
}

//TESTA E TRATA COLISÃO ENTRE OBJETOS
int test_colision(PLAYER* p, BALL* b){
	static int points = 0;
	static int streak = 0;


	if(colision(p,b)){
		Mix_PlayChannel(-1, b->sound, 0);

		if (b->obj->color >= 110){
			p->pts += points * p->bonus;
			p->obj->radius = 20;
			points = 0;
			streak = 0;
		}
		else if (b->obj->color > 45){
			points += 21;
			p->obj->radius += 3;
			streak ++;

		}

		else{
			

				p->obj->radius = 20;
				p->lives --;

				points = 0;
				streak = 0;
		}
			p->bonus = 1+ (streak/5);

		return 1;	
	}
	if(p->bonus > 5){
		p->bonus = 5;
	}
	return 0;
}

//SE PERDER, JÁ SABE...
void game_over(){
	char gameover[10] = "Try Again";
	background = IMG_Load("image/gameOver.jpg");
	SDL_Rect dst_rect = {(SCREEN_W/2)-(background->w/2),(SCREEN_H/2)-(background->h/2) };
	
	Mix_HaltMusic();
	music = Mix_LoadMUS("sound/youlose.ogg");

 	if (!music) {
 			printf("Mus: SDL error=%s\n", SDL_GetError());
 		return ;
 	}

	Mix_PlayMusic(music, 0);
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format,0,0,0));
	SDL_BlitSurface(background,NULL,screen,&dst_rect);
	
	draw_font(gameover, 350, 400, WHITE);
	SDL_Flip(screen);

	free(background);

	SDL_Delay(6400);
}
void game(){
	init();
	
	PLAYER* p = init_player();
	BALL* b[MAX_BALLS];

	TEXT* t[4];
	int i, state = 0;

	for (i = 0; i < MAX_BALLS; i++){
		b[i] = NULL;
	}
	for (i = 0; i < 4; i++){
		t[i] = init_text();
	}

	for(i = 0; i<MAX_BALLS; i++){
		b[i] = init_ball();
	}


	SDL_ShowCursor(0);

	while(state <= 3){

		state = menu("image/menu.jpg","sound/menu.ogg",t);
		if (state == 1)
			stage(p,b,"image/game.png","sound/music1.ogg",t);
		if (state == 2)
			how_to_play(t);
		if (state == 3)
			credits(t);
	}
	free(p);
	for (i = 0; i < MAX_BALLS; i++){
		free(b[i]);
	}
	for (i = 0; i < 4; i++){
		free(t[i]);
	}
	free(p);

	return;
}

int menu(char image[20], char song[20], TEXT* t[4]){
	
	background = IMG_Load(image);
	

	int red, inc, enable = 0,
		select = 0, notRed = 1;
 	
 	SDL_Color glowingRed;

 	inc = 16;

	music = Mix_LoadMUS(song);

 	if (!music) {
 			printf("Mus: SDL error=%s\n", SDL_GetError());
 		exit(0);
 	}
 	load_text(t[0], "START", 370, 410);
 	load_text(t[1], "HOW TO PLAY", 335, 440);
 	load_text(t[2], "CREDITS", 360, 470);
 	load_text(t[3], "EXIT", 375, 500);


 	if (!Mix_PlayingMusic()){
 		Mix_PlayMusic(music, -1);
 	}
 	while(1){

 		initTime = SDL_GetTicks();
 		red = 255;
 		notRed += inc;

 		if(notRed >= 255 || notRed == 0)
 			inc *= -1;

 		glowingRed.r = red;
 		glowingRed.g = glowingRed.b = notRed;

 		while(SDL_PollEvent(&event) !=0){
			switch(event.type){
				case SDL_KEYDOWN:
					keysym = event.key.keysym;

				if(keysym.sym == SDLK_RETURN){
					enable = 1;
				}
				if(keysym.sym == SDLK_DOWN && select < 3){
					select ++;
				}
				if(keysym.sym == SDLK_UP && select > 0){
					select --;
				}
				if(keysym.sym == SDLK_ESCAPE){
					exit(0);
					printf("Saindo...\n");
				}
				break;

				case SDL_QUIT:
				exit(0);
			}
			
		}
		if(enable == 0){
			if (select == 0){
				t[0]->color = glowingRed;
			}else{
				t[0]->color = WHITE;
			}

			if (select == 1){
				t[1]->color = glowingRed;
			}else{
				t[1]->color = WHITE;
			}

			if (select == 2){
				t[2]->color = glowingRed;
			}else{
				t[2]->color = WHITE;
			}

			if (select == 3){
				t[3]->color = glowingRed;
			}else{
				t[3]->color = WHITE;
			}

			SDL_BlitSurface(background,NULL,screen,NULL);

			draw_title("Blitz", 190, 150, WHITE);
	 		draw_font(t[0]->text, t[0]->posx,t[0]->posy,t[0]->color);
	 		draw_font(t[1]->text, t[1]->posx,t[1]->posy,t[1]->color);
	 		draw_font(t[2]->text, t[2]->posx,t[2]->posy,t[2]->color);
	 		draw_font(t[3]->text, t[3]->posx,t[3]->posy,t[3]->color);

	 		SDL_Flip(screen);
 		}
 		else{
			return select +1;
 		}
 		endTime = SDL_GetTicks();

		tTime = endTime - initTime;

		frame = ((sec - tTime) / frame);
		if(tTime < frame)
			SDL_Delay(frame - tTime);
 	}
}

void how_to_play(TEXT* t[3]){
	
	int clicks = 0;

	while(clicks < 4){
		SDL_BlitSurface(background,NULL,screen,NULL);

		while(SDL_PollEvent(&event) != 0){
			switch(event.type){
				case SDL_MOUSEBUTTONDOWN:
				clicks++;		
				break;
				case SDL_QUIT:
				exit(0);
			}
		}
		load_text(t[1], "Click to continue...", 0, 500);

		if (clicks == 0){
			load_text(t[0],"Touch blue balls to amount some points", 160, 220);

			filledCircleRGBA(screen, 370, 390,40,GREEN.r,GREEN.g,GREEN.b,255); 
			filledCircleRGBA(screen, 430, 340,40,BLUE.r,BLUE.g,BLUE.b,255); 

		}if (clicks == 1){
	 		load_text(t[0],"Touch yellow balls to get amounted points", 150, 220);
			
			filledCircleRGBA(screen, 340, 410,40,GREEN.r,GREEN.g,GREEN.b,255); 
			filledCircleRGBA(screen, 450, 360,40,YELLOW.r,YELLOW.g,YELLOW.b,255); 

		}if (clicks == 2){
	 		load_text(t[0],"If you touch red balls, you lose a life", 180, 220);
	 		load_text(t[2],"LIVES DOWN -", 210, 320);
			
			filledCircleRGBA(screen, 360, 430,40,GREEN.r,GREEN.g,GREEN.b,255); 
			filledCircleRGBA(screen, 430, 340,40,RED.r,RED.g,RED.b,255); 
	 		draw_font(t[2]->text, t[2]->posx,t[2]->posy,t[2]->color);

		}if (clicks == 3){
	 		load_text(t[0],"You only have 5 lives, good luck", 200, 230);

		}
	 	draw_font(t[0]->text, t[0]->posx,t[0]->posy,t[0]->color);
	 	draw_font(t[1]->text, t[1]->posx,t[1]->posy,t[1]->color);

		SDL_Flip(screen);
	}

}

void credits(TEXT* t[1]){
	
	int clicks = 0;

	SDL_BlitSurface(background,NULL,screen,NULL);
	
	load_text(t[0],"Musics by: ", 20, 150);
	draw_font(t[0]->text, t[0]->posx,t[0]->posy,t[0]->color);

	load_text(t[0],"Game: Super Meat Boy - Meat Spin Spoiled", 20, 190);
	draw_font(t[0]->text, t[0]->posx,t[0]->posy,t[0]->color);
	
	load_text(t[0],"Menu: Electro-Light - Symbolism [NCS Release]", 20, 230);
	draw_font(t[0]->text, t[0]->posx,t[0]->posy,t[0]->color);

	load_text(t[0],"Programmer: ", 60, 360);
	draw_font(t[0]->text, t[0]->posx,t[0]->posy,t[0]->color);
	
	load_text(t[0],"Gustavo Siqueira ", 60, 400);
	draw_font(t[0]->text, t[0]->posx,t[0]->posy,t[0]->color);
	

	load_text(t[0],"TANK YOU FOR PLAYING !!", 300, 500);
	draw_font(t[0]->text, t[0]->posx,t[0]->posy,t[0]->color);
	
	load_text(t[0],"a 2016 game", 360, 530);
	draw_font(t[0]->text, t[0]->posx,t[0]->posy,t[0]->color);

	load_text(t[0],"Click to continue...", 500, 570);
	draw_font(t[0]->text, t[0]->posx,t[0]->posy,t[0]->color);
	
	SDL_Flip(screen);

	while(1){

		while(SDL_WaitEvent(&event) != 0){
			switch(event.type){
				case SDL_MOUSEBUTTONDOWN:
				clicks++;		
				break;
				case SDL_QUIT:
				exit(0);
			}
			if (clicks != 0)
				return;
		}
	}
}

//JOGO
void stage(PLAYER* p, BALL* b[MAX_BALLS], char image[20], char song[20], TEXT* t[3]){

	int i = 0;
	int inc = 8;
	
	load_player(p);

	for(i  = 0; i<MAX_BALLS; i++){
		load_ball(b[i]);
	}
	
	load_text(t[0],"SCORE",0,0);
	load_text(t[1],"BONUS",0,(SCREEN_H - 60));
	load_text(t[2],"LIVES",(SCREEN_W - 70),(SCREEN_H - 60));


	background = IMG_Load(image);

	music = Mix_LoadMUS(song);
 	
 	if (!music) {
 		printf("Mus: SDL error=%s\n", SDL_GetError());
 		return;
 	}

	Mix_PlayMusic(music, -1);
	
	t[1]->color.r = t[1]->color.g = t[1]->color.b = 127;
	
	while(p->lives != 0){
	initTime = SDL_GetTicks(); 

		t[1]->color.r = t[1]->color.g = t[1]->color.b += inc;
		if(t[1]->color.r >= 255 || t[1]->color.r<= 127){
			inc *= -1;
		}

		while(SDL_PollEvent(&event) !=0){

			switch(event.type){
				case SDL_KEYDOWN:
					keysym = event.key.keysym;

				if(keysym.sym == SDLK_ESCAPE)
					Mix_HaltMusic();
					return;
				break;

				case SDL_QUIT:
					exit(0);
			}			
		}

		SDL_BlitSurface(background,NULL,screen,NULL);

		update_player(p);

		for (i = 0; i < ((level(p)*7) + 15); i++){
			update_ball(b[i]);
			if(test_colision(p,b[i])){ 
				load_ball(b[i]); 
			}
			draw(b[i]->obj);
		}

		draw_text(t[0], p->pts);
		
		draw_text(t[1], p->bonus);
		draw_font("x ",0,(SCREEN_H -35),t[1]->color);

		draw_text(t[2], p->lives);

		draw(p->obj);

		SDL_Flip(screen);

		endTime = SDL_GetTicks();

		tTime = endTime - initTime;

		frame = ((sec - tTime) / frame);
		if(tTime < frame)
			SDL_Delay(frame - tTime);	
	}

	game_over();
}