/*test1 bass*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include "bass.h"

using namespace std;


bool checkVersionBass(){
	if (HIWORD(BASS_GetVersion())!=BASSVERSION) {		
		return FALSE;
	}
	return TRUE;
}

char* stringToChar(string str){
	char *result = new char[str.length()+1];
	strcpy (result,str.c_str());
	return result;
}

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

string* getStations(string fileName){
	int count = countFile(fileName);
	ifstream fileList;
	
	string *result;
	if (count>0){
		result = new string [count];
			
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

int increaseStation(int max, int cur){
	int result;
	if((cur+1)==max){
		result = 0;
	}else{
		result = ++cur;
	}
	return result;
}

double increaseVolume(double vol){
	double result = vol;
	if (vol < 0.94){
		result = vol+0.05;
	}
	return result;	
}

double dicreaseVolume(double vol){
	double result = vol;
	if (vol > 0.06){
		result = vol-0.05;
	}
	return result;	
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

	string *list = getStations("stations.txt");
	//cout<<(sizeof(list)/sizeof(int))<<"\n";
	
	//char radioStation[10000] = "http://87.98.242.213:8046/stream";
	int stationsCount = sizeof(list)/sizeof(int);
	int currentStation = 1;
	double vol = 0.25;
	
	if(stationsCount>0){	
	
		HSTREAM stream;
		BASS_SetVolume(vol);
		
		string s = "";
		while(TRUE){
			cin >> s;
			if (s == "s"){
				BASS_ChannelStop(stream);
				//BASS_StreamFree(stream);
				//BASS_Free ();
				//return 0;	
			}
			
			if (s == "n"){
				BASS_ChannelStop(stream);				
				currentStation = increaseStation(stationsCount, currentStation);
				stream = BASS_StreamCreateURL(stringToChar(list[currentStation]), 0, 0, NULL, 0);
				//cout<< '' << "[2J";
				cout<<"Ok! Next\n";
				cout<<"Playing "<<stringToChar(list[currentStation])<<"\n";
				BASS_ChannelPlay(stream,TRUE);
			}
			if(s == "p"){
				stream=BASS_StreamCreateURL(stringToChar(list[currentStation]), 0, 0, NULL, 0);
				BASS_ChannelPlay(stream,TRUE);
				cout<<"Playing "<<stringToChar(list[currentStation])<<"\n";
				BASS_CHANNELINFO info;
				BASS_ChannelGetInfo(stream,&info);
				
				TAG_ID3 *id3 = (TAG_ID3*)BASS_ChannelGetTags(stream,BASS_TAG_ID3);
				cout<<id3->artist<<"\n";
			}
			if(s == "e"){
				BASS_ChannelStop(stream);
				BASS_StreamFree(stream);
				BASS_Free ();
				return 0;
			}
			if(s == "+"){
				vol = increaseVolume(vol);
				BASS_SetVolume(vol);
				cout<<"Vol: "<<vol<<"\n";
			}
			if(s == "-"){
				vol = dicreaseVolume(vol);
				BASS_SetVolume(vol);
				cout<<"Vol: "<<vol<<"\n";
			}
		}
	}
}