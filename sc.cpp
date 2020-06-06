#include <bangtal.h>
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#define frequency 0.02f
#define state_num 4				// 4가지 상태를 가짐
#define state_num_per_beat 2 // 2박자마다 반복되는 상태
float state_seperate[state_num] = { 0, 0.3, 1, 1.3 };

SceneID scene1;
ObjectID piece[3];
TimerID timer1;
TimerID timer2;
SoundID sound_theme;

struct _timeb tstruct;
short ct;

short locateX[3] = {200, 320, 440};
short locateY[3] = { 200, 200, 200 };
short dx[4] = { 25, 50, 25, 0 };
short dy[4] = { 25, 0, 25, 0 };

long long start_time;
char current_statement = 0;

void plus_ct () {
	ct++;
	if (ct > 39) ct -= 40;
}

void changeImgState(char state){
	current_statement = state;
	for (int i = 0; i < 3; i++) {
		locateObject(piece[i], scene1, locateX[i] + dx[state], locateY[i] + dy[state]);
	}
	
}
long long gettime() {
	_ftime64_s(&tstruct);
	return tstruct.time * 1000 + tstruct.millitm;
}

char getstatement(long long time) {
	double beat = ((double)time / 227000) * 568; // 음악 총길이 227초, 그동안 568박자
	double beat_in_period = beat - (int)(beat / 2) * 2;
	
	char state = 0;
	for (int i = state_num - 1; i >= 0; i--) {
		if (beat_in_period >= state_seperate[i]) {
			state = i;
			break;
		}
	}
	return state;
}


void timerCallback(TimerID timer) {
	if (timer == timer1) {
		
		setTimer(timer1, frequency);
		startTimer(timer1);

		long long current_time = gettime();
		long long delta_time = current_time - start_time;
		
		char state = getstatement(delta_time);
		
		if (state != current_statement) {
			changeImgState(state);
		}
	}
	// music start (First start)
	else if (timer == timer2) {
		stopTimer(timer2);
		playSound(sound_theme);
		start_time = gettime();
		printf("state time : %lld\n", start_time);
		
	}
}

void soundCallback(SoundID sound) {
	// music repeat (stat again)
	if (sound == sound_theme) {
		playSound(sound_theme);
		start_time = gettime();
		printf("state time : %lld", start_time);
	}
}

int main() {
	setTimerCallback(timerCallback);
	setSoundCallback(soundCallback);
	scene1 = createScene("테스트", "Images/배경.jpg");

	for (int i=0; i<3; i++){
		piece[i] = createObject("Images/start.png");		
		locateObject(piece[i], scene1, locateX[i], locateY[i]);
		showObject(piece[i]);
		
	}
	sound_theme = createSound("Sounds/Vitality.mp3");

	timer1 = createTimer(1.5f);
	startTimer(timer1);
	timer2 = createTimer(1.0f);
	startTimer(timer2);
	startGame(scene1);
}