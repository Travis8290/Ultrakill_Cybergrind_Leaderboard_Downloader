//Written by Travis D
//The purpose of this program is to enable anyone to quickly create a local backup of the Cybergrind Steam leaderboards for the game Ultrakill.
//This program will save the first 5001 entries of each leaderboard to the same folder the executable is run from.

#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <Urlmon.h>
#include <tinyxml2.h>
#pragma comment(lib, "urlmon.lib")


void downloader(LPCWSTR url, LPCWSTR board,const char* Filename) {
	//read the downloaded file

	HRESULT hr;
	hr = URLDownloadToFile(NULL, url, board, BINDF_GETNEWESTVERSION, NULL);
	tinyxml2::XMLDocument First;
	First.LoadFile(Filename);
	
	//edit file to remove each row that has "details", "UGCID" and the footer

	tinyxml2::XMLElement* detailsMarker;
	tinyxml2::XMLElement* ugcidMarker;
	tinyxml2::XMLElement* entryMarker;
	//set the entry marker to the first element
	entryMarker = First.RootElement();
	entryMarker = First.FirstChildElement("response");
	entryMarker = entryMarker->FirstChildElement("entries");
	entryMarker = entryMarker->FirstChildElement("entry");
	//loop through all 5001 entries. Yes steam defaults to giving 5001 entries.
	for (int i = 0; i < 5001; i++) {
		detailsMarker = entryMarker->FirstChildElement("details");
		ugcidMarker = entryMarker->FirstChildElement("ugcid");
		entryMarker->DeleteChild(detailsMarker);
		entryMarker->DeleteChild(ugcidMarker);
		entryMarker = entryMarker->NextSiblingElement("entry");

	}
	//Save the document over the original one downloaded.
	First.SaveFile(Filename);
}






int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow){
	downloader(L"https://steamcommunity.com/stats/1229490/leaderboards/13452166/?xml=1&start=1", L"Brutal.xml", "Brutal.xml");
	downloader(L"https://steamcommunity.com/stats/1229490/leaderboards/5816999/?xml=1&start=1", L"Violent.xml", "Violent.xml");
	downloader(L"https://steamcommunity.com/stats/1229490/leaderboards/5816994/?xml=1&start=1", L"Standard.xml", "Standard.xml");
	downloader(L"https://steamcommunity.com/stats/1229490/leaderboards/6418838/?xml=1&start=1", L"Lenient.xml", "Lenient.xml");
	downloader(L"https://steamcommunity.com/stats/1229490/leaderboards/6418847/?xml=1&start=1", L"Harmless.xml", "Harmless.xml");
	//Placeholder for unreleased leaderboard
	//downloader(L"https://steamcommunity.com/stats/1229490/leaderboards/", L"UKMD.xml", "UKMD.xml");

	return 0;
}

