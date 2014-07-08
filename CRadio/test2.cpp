/*test1 bass*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include "bass.h"

using namespace std;

HSTREAM stream;

//Проверка версии BASS
bool checkVersionBass(){
	if (HIWORD(BASS_GetVersion())!=BASSVERSION) {		
		return FALSE;
	}
	return TRUE;
}

//Преобразование строки в массив Char
char* stringToChar(string str){
	char *result = new char[str.length()+1];
	strcpy (result,str.c_str());
	return result;
}

//Считаем сколько радиостанций в файле
int countFile(string fileName){
	ifstream fileList;
	fileList.open(stringToChar(fileName));
	//fileList.open("stations.txt",ios::in);
	int result = 0;
	
	while(!fileList.eof()){
		string tmp;
		fileList>>tmp;
		result++;
	}
	fileList.close();
	return result;
}

//Читаем радиотанции из файла. 
string* getStations(string fileName, int* count){
	*count = countFile(fileName);
	ifstream fileList;
	
	string *result;
	if (*count>0){
		result = new string [*count];
			
		fileList.open(stringToChar(fileName));
		int i = 0;
		string buff;
		while(!fileList.eof()){
			fileList >> buff;
			result[i] = buff;
			i++;
		}
	}
	fileList.close();
	return result;
}

//Увеличить позицию в плейлисте на 1, или гнать по кругу
int increaseStation(int max, int cur){
	int result;
	if((cur+1)==max){
		result = 0;
	}else{
		result = ++cur;
	}
	return result;
}

//Увеличиваем громкость, чтобы не выйти за пределы
double increaseVolume(double vol){
	double result = vol;
	if (vol < 0.94){
		result = vol+0.05;
	}
	return result;	
}

//Уменьшаем громкость, чтобы не выйти за пределы
double dicreaseVolume(double vol){
	double result = vol;
	if (vol > 0.06){
		result = vol-0.05;
	}
	return result;	
}

void getInfoStream(){
	
	const char *text;
	if (text = BASS_ChannelGetTags(stream, BASS_TAG_META)){
		
		string s = (const char*)text;
		//cout << s <<"\n";
		int p = s.find(";");
		cout << s.substr(13,p-14) <<"\n";
			
		
		
	}else{
		cout<<BASS_ErrorGetCode()<<"\n";
	}
}

int main(void){
	cout<<"Hello!\n"<<"This is Radio!!!\n";
	// check the correct BASS was loaded
	
	if (!checkVersionBass){
		cout << "Ошибка версии BASS\n";
	}
		
	if (!BASS_Init(-1,44100,0,NULL,NULL)) {
		cout << "Can't initialize device";
		return 1;
	}
	//Получаем список воспроизведения и количество элементов в очереди
	int stationsCount;
	string *list = getStations("stations.txt", &stationsCount);

	//Подготовка к запуску
	int currentStation = 1;
	double vol = 0.4;
	bool isPlay = FALSE;
	
	if(stationsCount>0){	
	
		
		BASS_SetVolume(vol);
		BASS_SetConfig(BASS_CONFIG_GVOL_STREAM,vol*10000);
		
		string s = "";
		while(TRUE){
			cin >> s;
			if (s == "s"){
				BASS_ChannelStop(stream);
				isPlay = FALSE;
			}
			
			if (s == "n"){
				BASS_ChannelStop(stream);				
				currentStation = increaseStation(stationsCount, currentStation);
				stream = BASS_StreamCreateURL(stringToChar(list[currentStation]), 0, 0, NULL, 0);
				//cout<<"Playing "<<stringToChar(list[currentStation])<<"\n";
				BASS_ChannelPlay(stream,TRUE);
				getInfoStream();
				isPlay = TRUE;
			}
			if(s == "p"){
				if (isPlay == FALSE){
					stream=BASS_StreamCreateURL(stringToChar(list[currentStation]), 0, 0, NULL, 0);
					
					BASS_ChannelPlay(stream,TRUE);
					
					getInfoStream();
					//cout<<"Playing "<<stringToChar(list[currentStation])<<"\n";
					//BASS_CHANNELINFO info;
					//BASS_ChannelGetInfo(stream,&info);
					
					//TAG_INFO tags = BASS_ChannelGetTags(stream,BASS_TAG_HTTP);
					isPlay = TRUE;
				}
					

				//char *id3 = (TAG_ID3*)BASS_ChannelGetTags(stream,BASS_TAG_HTTP);
				//cout<<id3->artist<<"\n";
			}
			if(s == "e"){
				BASS_ChannelStop(stream);
				BASS_StreamFree(stream);
				BASS_Free ();
				return 0;
			}
			if(s == "+"){
				vol = increaseVolume(vol);
				//BASS_SetVolume(vol);
				BASS_SetConfig(BASS_CONFIG_GVOL_STREAM,vol*10000);
				cout<<"Vol: "<<vol<<"\n";
			}
			if(s == "-"){
				vol = dicreaseVolume(vol);
				//BASS_SetVolume(vol);
				BASS_SetConfig(BASS_CONFIG_GVOL_STREAM,vol*10000);
				cout<<"Vol: "<<vol<<"\n";
			}
		}
	}
}