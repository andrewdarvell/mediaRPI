#include <stdio.h>
#include <stdlib.h>
#include "bass.h"
#include <unistd.h>
#include <iostream>
using namespace std;


void geterror(char* msg)
{
	printf("%s:%d\n", msg, BASS_ErrorGetCode());
}

int main(int argc, char *argv[])
{
	int pos = 0;
	int len = 0;
	HSTREAM stream;
	char *filepath = argv[1];

	printf("\n-----------------------------------------------------------\n");

	DWORD ver = BASS_GetVersion();
	printf ("\nBASS Version: %d.%d.%d.%d\n\n",(ver>>24)&0xff, (ver>>16)&0xff, (ver>>8) & 0xff, (ver & 0xff));

	if(!BASS_Init(-1, 44100, BASS_DEVICE_DMIX, 0, NULL))
	{
		//geterror("Init Error");
		cout<<"Error   "<<BASS_ErrorGetCode()<<"\n";
	}
	

	// ------------------ Get Devices --------------------
	int a;
	BASS_DEVICEINFO info;
	printf("*****Devices Info:*****\n");
	for(a=0; BASS_GetDeviceInfo(a, &info);a++)
	{
		printf("Device %d:%s - %s - %d\n", a, info.name, info.driver, info.flags);
	}
	printf("\n");
	// ---------------------------------------------------



	
	// ----------------- Current Device ------------------
	if(BASS_GetDevice() == -1)
		//geterror("Device Error");
	cout<<"Error   "<<BASS_ErrorGetCode()<<"\n";
	else
		printf("Selected Device:%d\n", BASS_GetDevice());
	// ---------------------------------------------------
	


	
	// ---------------- Get Global Volume ----------------
	if(BASS_GetVolume() == -1.0)
		//geterror("Cann't Get Volume Error");
	cout<<"Error   "<<BASS_ErrorGetCode()<<"\n";
	else
		printf("%f\n", BASS_GetVolume());
	// ---------------------------------------------------
		

	// ---------------- Create Stream --------------------
	stream = BASS_StreamCreateFile(FALSE, filepath, 0, 0, 0);
	if(stream == 0)
		//geterror("StreamCreate Error");
	cout<<"Error   "<<BASS_ErrorGetCode()<<"\n";
	else
		printf("StreamHandle:%d\n",stream);
	// ---------------------------------------------------
	
	
	len = BASS_ChannelBytes2Seconds(stream,BASS_ChannelGetLength(stream, BASS_POS_BYTE));
	pos = 0;

	// ---------------- Get Channel Info -------------------
	BASS_CHANNELINFO chan_info;
	BASS_ChannelGetInfo(stream, &chan_info);
	printf("\n*****Channel Info:*****\n");
	printf("Freq:%d\n",chan_info.freq);
	printf("Channels:%d\n",chan_info.chans);
	printf("Bps%d\n",chan_info.origres);
	printf("Filename:%s\n\n",chan_info.filename);
	printf("\n");
	// -----------------------------------------------------
	
        if(BASS_ChannelPlay(TRUE, stream))
	{	
		//geterror("Play");
		cout<<"Error   "<<BASS_ErrorGetCode()<<"\n";
	}
	while(1)
	{
		if(BASS_ChannelIsActive(stream) != 0)
		{
			pos = BASS_ChannelBytes2Seconds(stream, BASS_ChannelGetPosition(stream, BASS_POS_BYTE));

			if(len == pos)
				break;

			printf("%02d:%02d - %02d:%02d\n",(int)(len/60), (int)(len%60), (int)(pos/60), (int)(pos%60));
		}
		usleep(100000);	
	}
	BASS_ChannelStop(stream);
	BASS_StreamFree(stream);

	BASS_Free();
	

	return 0;
}
