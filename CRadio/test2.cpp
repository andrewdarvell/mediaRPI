/*test1 bass*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "bass.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

HSTREAM stream;

bool lastfm = FALSE;


string IntToString (int a)
{
    ostringstream temp;
    temp<<a;
    return temp.str();
}

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

int sendToJava(char *message, int size){
	int sock;

	//char message[] = "Hello there!\n";
	char buf[1024];
	
	struct sockaddr_in addr;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0)
	{
		perror("socket");
		return 1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(3425); // или любой другой порт...
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	
	if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("connect");
		return 2;
	}

	//send(sock, message, sizeof(message), 0);
	send(sock, message, size , 0);
	recv(sock, buf, 1024, 0);
	cout<<buf<<"\n";
	
	close(sock);

	return 0;	
}


//Выводит инф. о том, что сейчас играет
void getInfoStream(){
	
	const char *text;
	if (text = BASS_ChannelGetTags(stream, BASS_TAG_META)){
		
		string s = (const char*)text;
		//cout << s <<"\n";
		int p = s.find(";");
		string resStr = s.substr(13,p-14);
		cout << resStr <<"\n";
		time_t t = time(0);  // t is an integer type
		string s1 = IntToString(t);
		//cout<<s1.length();
		sendToJava(stringToChar("P_"+s1+"P_"+resStr),resStr.length()+5+s1.length());
	}else{
		cout<<BASS_ErrorGetCode()<<"\n";
	}
}

void sendStop(){
	sendToJava(stringToChar("S_"),3);	
}

//Для автоматического вызова инф при смене трека
void CALLBACK MetaSync(HSYNC handle, DWORD channel, DWORD data, void *user)   
{   
	sendStop();
    getInfoStream();   
}  

void playBss(string track){
	
	stream=BASS_StreamCreateURL(stringToChar(track), 0, 0, NULL, 0);
	BASS_ChannelSetSync(stream,BASS_SYNC_META,0,&MetaSync,0);
	BASS_ChannelPlay(stream,TRUE);
	getInfoStream(); 
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
	double vol = 0.2;
	bool isPlay = FALSE;
	
	//sendToJava();
	
	//if(socketConnect()==0){
	//	lastfm = TRUE;
	//}
	
	if(stationsCount>0){	
	
		
		//BASS_SetVolume(vol);
		BASS_SetConfig(BASS_CONFIG_GVOL_STREAM,vol*10000);
		
		
		string s = "";
		while(TRUE){
			cin >> s;
			if (s == "s"){
				BASS_ChannelStop(stream);
				if (isPlay){
					sendStop();
				}
				isPlay = FALSE;								
			}
			
			if (s == "n"){
				BASS_ChannelStop(stream);
				if (isPlay){
					sendStop();
				}
				currentStation = increaseStation(stationsCount, currentStation);
				playBss(list[currentStation]);
				if (BASS_ChannelIsActive(stream) == BASS_ACTIVE_PLAYING){
					isPlay = TRUE;						
				}else{
					isPlay = FALSE;
					cout<<"error"<<"\n";
				}
			}
			if(s == "p"){
				if (isPlay == FALSE){
					playBss(list[currentStation]);
					if (BASS_ChannelIsActive(stream) == BASS_ACTIVE_PLAYING){
						isPlay = TRUE;						
					}else{
						isPlay = FALSE;
						cout<<"error"<<"\n";
					}
					
				}
			}
			if(s == "e"){
				sendStop();
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
