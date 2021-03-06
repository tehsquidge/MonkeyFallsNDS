// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <PA9.h>       // Include for PA_Lib
#include <fat.h>
// Converted using PAGfx
#include "all_gfx.h"
//music
//#include "reggae_dreams.h"

using namespace std;


class Tukituki{
	public:
	int x;
	int y;
	int spriteNo;
	int mode; //0 = falling, 1 = landed
	Tukituki(){
		x = -32;
		y = -32;
	}
	Tukituki(int new_no){
		x = -32;
		y = -32;
		spriteNo = new_no;
		PA_DualCreateSprite(spriteNo, (void*)tukituki_Sprite,OBJ_SIZE_32X32, 1, 0, x, y);
	}

	void reset(){
		x = (spriteNo - 1) * 32;
		y = PA_RandMinMax(0,3);
		PA_DualSetSpriteXY(spriteNo, x, y);
	}

	void update(){
		y += 3;
		PA_DualSetSpriteY(spriteNo, y);
	}





	void hide(){
		x = -64; //hide offscreen
		PA_DualSetSpriteX(spriteNo, x);
	}
};


class Monkey {

	public:
	int x;
	int y;
	int spriteNo;
	int speed;
	int hit;

	Monkey(){
		x = 32;
		y = 402;
		speed = 2;
		hit = 0;
	}

	Monkey(int new_spriteNo){
		x = 32;
		y = 402;
		speed = 2;
		hit = 0;
		spriteNo = new_spriteNo;
		PA_DualCreateSprite(spriteNo, (void*)monkey_Sprite,OBJ_SIZE_16X16, 1, 0, x, y);
	}

	void reset(){
		x = 32;
		y = 400;
		speed = 2;
		hit = 0;
	}

	void stand(){
		 PA_DualSetSpriteAnim(spriteNo, 0);
	}

	void startWalkingAnimation(){
		PA_DualStartSpriteAnim(spriteNo, 0, 2, 8);
	}

	void moveRight(){
		PA_DualSetSpriteHflip(spriteNo,1);
		if(x + speed < 256 - 16)
			x += speed;
	}
	void moveLeft(){
		PA_DualSetSpriteHflip(spriteNo,0);
		if(x - speed > 0)
			x -= speed;
	}

	void update(){
		PA_DualSetSpriteX(spriteNo, x);
	}

};

class Timer{
	public:
	int seconds;
	int minutes;
	int hours;


	Timer(){
		seconds = 0;
		minutes = 0;
		hours = 0;
	}

	int getSeconds(){
		return seconds;
	}

	int getMinutes(){
		return minutes;
	}

	int getHours(){
		return hours;
	}

	void incSeconds(){
		seconds++;
		if(seconds == 60){
			seconds = 0;
			minutes++;
			if(minutes == 60){
				minutes = 0;
				hours++;
			}
		}
	}

	void reset(){
		seconds = 0;
		minutes = 0;
		hours = 0;
	}




};


// Function: main()
int main(int argc, char ** argv)
{
	PA_Init();    // Initializes PA_Lib
	PA_InitVBL(); // Initializes a standard VBL
	fatInitDefault(); //initializes teh FAT shit
	PA_InitText(0,0);
	PA_SetTextCol(0, 0, 0, 0);
	PA_InitRand();
	//sound
	//AS_Init(AS_MODE_SURROUND | AS_MODE_16CH );
	//AS_SetDefaultSettings(AS_PCM_8BIT, 11025, AS_SURROUND);

	PA_LoadSpritePal(1,0, (void*)sprites_Pal);
	PA_LoadSpritePal(0,0, (void*)sprites_Pal);

	PA_LoadBackground(0, 3, &bottom);
	PA_LoadBackground(1, 3, &title);

	for (int i = 32; i >= 0; i--) {
			PA_WindowFade(0, 0, i);
			PA_WindowFade(1, 1, i);
			PA_WaitForVBL(); // To slow down the fades, we wait a frame...
	}



	bool ready = false;

	//title screen

	//gameover sprite spriteNo 9
	PA_DualCreateSprite(9, (void*)gameover_Sprite,OBJ_SIZE_64X64, 1, 0, -128, -128);
	PA_DualCreateSprite(10, (void*)play_Sprite,OBJ_SIZE_64X64, 1, 0, 100, 300);
	PA_DualCreateSprite(11, (void*)paused_Sprite,OBJ_SIZE_64X64, 1, 0, -128, -128);
	PA_DualStartSpriteAnim(11, 0, 1, 1);
	PA_DualSetSpritePrio(11, 0);

	//load best time in seconds
	FILE* saveFile = fopen ("/monkey_falls.sav", "rb");
	int highscore_seconds = 0;
	fread (&highscore_seconds, 4, 1, saveFile);

	//covert fro seconds to time
	int hours = 0;
	int minutes = 0;
	int seconds = highscore_seconds;
	while(seconds > 60){
	minutes++;
	seconds -= 60;
	}
	while(minutes > 60){
	hours++;
	minutes -= 60;
	}

	PA_OutputText(0,0,0,"best: h%d m%d s%d", hours, minutes, seconds);

	while(ready == false){

		if(Stylus.Newpress){
			ready = true;
		}



		PA_WaitForVBL();
	}

	//move play button away
	PA_DualSetSpriteXY(10, -128, - 128);
	//load top background over the title one
	PA_LoadBackground(1, 3, &top);



	//create game objects

	Tukituki *tukis[8];

	for(int i = 0; i < 8; i++){
		tukis[i] = new Tukituki(i + 1); //plus 1 because sprite 0 is monkey!
		tukis[i]->reset();
	}

	Monkey monkey(0);

	Timer timer;

	int hidden_tuki = PA_RandMinMax(0,7); //the tuki that will be put off screen

	tukis[hidden_tuki]->hide();

	int safex_start = (hidden_tuki * 32) - 2; //there is an extra 2 pixel allowed each side
	int safex_stop = safex_start + 18;

	int mode = 0; //falling... 1 = landed
	int reset_countdown = 160; //no of frames before reset

	int paused = 0;

	int frames = 0; //frame counter for updating the timer every 60 frames


	//fade into game
	for (int i = 32; i >= 0; i--) {
			PA_WindowFade(0, 1, i);
			PA_WindowFade(1, 1, i);
			PA_WaitForVBL(); // To slow down the fades, we wait a frame...
	}

	// Infinite loop to keep the program running
	while (1)
	{
		//PLAYING
		if(monkey.hit == 0 and paused == 0){

			if(mode == 0){
				for(int i = 0; i < 8; i++){
						tukis[i]->update();
					if(tukis[i]->y > 384){ //when it lands
						tukis[i]->y = 384;
					}
				}
				if(tukis[0]->y == 384){
					mode = 1;
				}
			}else{
				reset_countdown -= 1;
			}

			if(reset_countdown == 0){
					for(int i = 0; i < 8; i++)
 						tukis[i]->reset();
					hidden_tuki = PA_RandMinMax(0,7); //the tuki that will be put off screen
					reset_countdown = 160;
					mode = 0;
			}


			//move monkey move
			if (Stylus.Held){
				if(Stylus.X > monkey.x)
					monkey.moveRight();
				if(Stylus.X < monkey.x)
					monkey.moveLeft();
			}else{
					monkey.stand();
			}
			if(Stylus.Newpress){
				monkey.startWalkingAnimation();
			}
			monkey.update();

			//collision/safety check
			if(tukis[0]->mode == 0){
				safex_start = hidden_tuki * 32 - 2;
				safex_stop = safex_start + 18;
				if( !(monkey.x < safex_stop and monkey.x > safex_start) and (tukis[hidden_tuki]->y + 32 > monkey.y) )
					monkey.hit = 1;
			}



			if(tukis[0]->mode == 0) //when the mode is reset
				tukis[hidden_tuki]->hide();

		//time
		if(frames == 60){
			timer.incSeconds();
			frames = 0;
		}else{
			//increase the noOfFrames
			frames++;
		}

		//display time
		PA_OutputText(0,0,23,"time: h%d m%d s%d      ", timer.getHours(), timer.getMinutes(), timer.getSeconds());

	//PAUSE
		}else if(paused == 1 and monkey.hit == 0){
			//do nowt

	//GAME OVER
		}else{

				//SHOW GAME OVER SPRITE, ID 9
			PA_DualSetSpriteXY(9, 100, 100);
			//show play sprite
			PA_DualSetSpriteXY(10, 100, 300);
			int totalSeconds = timer.getSeconds() + (timer.getHours() * 60 * 60) + (timer.getMinutes() * 60);

			if(totalSeconds > highscore_seconds)
				PA_OutputText(0,16,5,"new highscore!!!");

			if(Stylus.Newpress){
				//clear "highscore"
				PA_OutputText(0,16,5,"                ");


				//saving
				int totalSeconds = timer.getSeconds() + (timer.getHours() * 60 * 60) + (timer.getMinutes() * 60);

				if(totalSeconds > highscore_seconds){
					FILE* savefile = fopen ("/monkey_falls.sav", "wb");
 					PA_OutputText(0,16,5,"                       ");
					PA_OutputText(0,0,0,"best: h%d m%d s%d      ", timer.getHours(), timer.getMinutes(), timer.getSeconds());
					highscore_seconds = totalSeconds;
					fwrite(&totalSeconds, 4, 1, savefile);
					fclose(savefile);
				}
				for(int i = 0; i < 8; i++)
 					tukis[i]->reset();
				hidden_tuki = PA_RandMinMax(0,7); //the tuki that will be put off screen
				reset_countdown = 160;
				mode = 0;
				monkey.reset();
				timer.reset();
				//PA_PlayMod(reggae_dreams);
				frames = 0;
				//hide the gameover and play sprite
				PA_DualSetSpriteXY(9, -128, -128);
				PA_DualSetSpriteXY(10, -128, -128);
				//fade out
				for (int i = 32; i >= 0; i--) {
					PA_WindowFade(0, 1, i);
					PA_WindowFade(1, 1, i);
					PA_WaitForVBL(); // To slow down the fades, we wait a frame...
				}

			}

		}

	if(Pad.Newpress.Start and paused == 0){
		paused = 1;
		//show paused sprite
		PA_DualSetSpriteXY(11, 100, 100);
		//PA_PauseMod(1);
	}else if(Pad.Newpress.Start and paused == 1){
		PA_DualSetSpriteXY(11, -128, -128);
		paused = 0;
	}

	//keep it 60fps
	PA_WaitForVBL();
	}

	return 0;
} // End of main()
