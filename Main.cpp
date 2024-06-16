//Written by Travis D
//The purpose of this program is to enable anyone to quickly create a local backup of the Cybergrind Steam leaderboards for the game Ultrakill.


#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <Urlmon.h>
#include <tinyxml2.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <nanogui/nanogui.h>

#pragma comment(lib, "urlmon.lib")

#define _UNICODE


bool Harmless = false;
bool Lenient = false;
bool Standard = false;
bool Violent = false;
bool Brutal = false;
//bool UKMD = false;
//placeholder for unreleased leaderboard

bool downloadAll = false;
//this bool determines if all entries for the selected leaderboard are downloaded, if false only first 5001.

void downloader(LPCWSTR url, LPCWSTR board,const char* Filename) {
	//download file
	HRESULT hr;
	hr = URLDownloadToFile(NULL, url, board, BINDF_GETNEWESTVERSION, NULL);
	//open file
	tinyxml2::XMLDocument First;
	First.LoadFile(Filename);
	
	//edit file to remove each row that has "details" and "UGCID"
	tinyxml2::XMLElement* detailsMarker;
	tinyxml2::XMLElement* ugcidMarker;
	tinyxml2::XMLElement* entryMarker;
	//set the entry marker to the first entry
	entryMarker = First.RootElement();
	entryMarker = First.FirstChildElement("response");
	entryMarker = entryMarker->FirstChildElement("entries");
	entryMarker = entryMarker->FirstChildElement("entry");
	//loop through all 5001 entries.
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

void downloaderAll(LPCWSTR url, LPCWSTR board, const char* Filename) {
	//download file
	HRESULT hr;
	hr = URLDownloadToFile(NULL, url, board, BINDF_GETNEWESTVERSION, NULL);
	//open file
	tinyxml2::XMLDocument* First = new tinyxml2::XMLDocument;
	First->LoadFile(Filename);

	//edit file to remove each row that has "details" and "UGCID"
	tinyxml2::XMLElement* detailsMarker;
	tinyxml2::XMLElement* ugcidMarker;
	tinyxml2::XMLElement* entryMarker;
	tinyxml2::XMLElement* entriesMarkerMain;
	tinyxml2::XMLElement* entriesMarkerCopy;
	//set the entry marker to the first entry
	entryMarker = First->RootElement();
	entryMarker = First->FirstChildElement("response");
	entryMarker = entryMarker->FirstChildElement("entries");
	entryMarker = entryMarker->FirstChildElement("entry");
	//loop through all 5001 entries.
	for (int i = 0; i < 5001; i++) {
		detailsMarker = entryMarker->FirstChildElement("details");
		ugcidMarker = entryMarker->FirstChildElement("ugcid");
		entryMarker->DeleteChild(detailsMarker);
		entryMarker->DeleteChild(ugcidMarker);
		entryMarker = entryMarker->NextSiblingElement("entry");

	}

	//creating variables for getting the next leaderboard link from the downloaded file.
	tinyxml2::XMLElement* nextLinkEle;
	LPCWSTR nextLink;
	std::string Step1;
	std::wstring Step2;
	
	//grabs the nextrequestURL from first downloaded file and converts to lcpwstr so it can be used later.
	nextLinkEle = First->FirstChildElement("response");
	nextLinkEle = nextLinkEle->FirstChildElement("nextRequestURL");
	Step1 = nextLinkEle->GetText();
	Step2 = std::wstring(Step1.begin(),Step1.end());
	nextLink = Step2.c_str();
	//makes new XMLDoc that will be repeatedly overwritten and copied into first file.
	tinyxml2::XMLDocument* Repeat = new tinyxml2::XMLDocument;
	//bool for knowing when to break out of while loop.
	bool finished = false;
	while (true) {
		hr = URLDownloadToFile(NULL, nextLink, board, BINDF_GETNEWESTVERSION, NULL);
		Repeat->LoadFile(Filename);

		//checks if the nextRequestURl for the repeat XML Doc is valid to know when to set finished to true.
		nextLinkEle = Repeat->FirstChildElement("response");
		if (nextLinkEle->FirstChildElement("nextRequestURL") == NULL) {
			finished = true;
		}
		else {
			nextLinkEle = nextLinkEle->FirstChildElement("nextRequestURL");
			Step1 = nextLinkEle->GetText();
			Step2 = std::wstring(Step1.begin(), Step1.end());
			nextLink = Step2.c_str();
		}
		//sets entryMarker to the first entry in the list.
		entryMarker = Repeat->RootElement();
		entryMarker = Repeat->FirstChildElement("response");
		entryMarker = entryMarker->FirstChildElement("entries");
		entryMarker = entryMarker->FirstChildElement("entry");
		//removes details and ugcid from each entry
		while(true) {
			detailsMarker = entryMarker->FirstChildElement("details");
			ugcidMarker = entryMarker->FirstChildElement("ugcid");
			entryMarker->DeleteChild(detailsMarker);
			entryMarker->DeleteChild(ugcidMarker);
			entryMarker = entryMarker->NextSiblingElement("entry");
			
			//when entryMarker cannot point to a new entry, stop.
			if (entryMarker->NextSiblingElement("entry")==NULL) {
				break;
			}
		}



		//Make new markers pointing to the first entry of the Repeat document and to the last entry of the First document.
		entriesMarkerCopy = Repeat->FirstChildElement("response");
		entriesMarkerCopy = entriesMarkerCopy->FirstChildElement("entries");
		entriesMarkerCopy = entriesMarkerCopy->FirstChildElement("entry");


		entriesMarkerMain = First->FirstChildElement("response");
		entriesMarkerMain = entriesMarkerMain->FirstChildElement("entries");
		tinyxml2::XMLNode* Copied;



		while(true) {
			//go through every entry on repeat and copy them under the last entry of First.
			Copied = entriesMarkerCopy->DeepClone(First);


			entriesMarkerMain->InsertEndChild(Copied);

			entriesMarkerCopy = entriesMarkerCopy->NextSiblingElement();

			//Once the next entry is NULL, stop.
			if (entriesMarkerCopy->NextSiblingElement() == NULL) {
				break;
			}
		}
		//if finished then break from while loop.
		if (finished) {
			break;
		}
	}
	
	//Save the document over the original one downloaded.
	First->SaveFile(Filename);
	
	
}

void submitted() {
	//first checks for no options selected and notifies the user with a popup window.
	if (!Harmless && !Lenient && !Standard && !Violent && !Brutal) {
		nanogui::Screen* errorScreen = new nanogui::Screen(nanogui::Vector2i(290, 100), "Submission Error");
		nanogui::FormHelper* gui = new nanogui::FormHelper(errorScreen);
		nanogui::ref<nanogui::Window> errorWindow = gui->addWindow(Eigen::Vector2i(15, 15), "Error!");
		gui->addGroup("Please select atleast one difficulty!");
		errorScreen->setVisible(true);
		errorScreen->performLayout();
		nanogui::mainloop();
	}
	else {
		if (downloadAll) {
			//if any board is selected it is downloaded
			if (Harmless) {
				downloaderAll(L"https://steamcommunity.com/stats/1229490/leaderboards/6418847/?xml=1&start=1", L"Harmless.xml", "Harmless.xml");
			}

			if (Lenient) {
				downloaderAll(L"https://steamcommunity.com/stats/1229490/leaderboards/6418838/?xml=1&start=1", L"Lenient.xml", "Lenient.xml");
			}

			if (Standard) {
				downloaderAll(L"https://steamcommunity.com/stats/1229490/leaderboards/5816994/?xml=1&start=1", L"Standard.xml", "Standard.xml");
			}

			if (Violent) {
				downloaderAll(L"https://steamcommunity.com/stats/1229490/leaderboards/5816999/?xml=1&start=1", L"Violent.xml", "Violent.xml");
			}

			if (Brutal) {
				downloaderAll(L"https://steamcommunity.com/stats/1229490/leaderboards/13452166/?xml=1&start=1", L"Brutal.xml", "Brutal.xml");
			}
			//creates window for notifying the user which boards were downloaded.
			nanogui::Screen* successScreen = new nanogui::Screen(nanogui::Vector2i(430, 275), "Success!");
			nanogui::FormHelper* gui = new nanogui::FormHelper(successScreen);
			nanogui::ref<nanogui::Window> errorWindow = gui->addWindow(Eigen::Vector2i(15, 15), "The following leaderboards were downloaded in their entirety.");
			if (Harmless)
				gui->addGroup("Harmless");
			if (Lenient)
				gui->addGroup("Lenient");
			if (Standard)
				gui->addGroup("Standard");
			if (Violent)
				gui->addGroup("Violent");
			if (Brutal)
				gui->addGroup("Brutal");

			//gui->addGroup("UKMD");
			//Placeholder for unreleased leaderboard

			successScreen->setVisible(true);
			successScreen->performLayout();
			nanogui::mainloop();

		}
		else {
			//if any board is selected it is downloaded
			if (Harmless) {
				downloader(L"https://steamcommunity.com/stats/1229490/leaderboards/6418847/?xml=1&start=1", L"Harmless.xml", "Harmless.xml");
			}

			if (Lenient) {
				downloader(L"https://steamcommunity.com/stats/1229490/leaderboards/6418838/?xml=1&start=1", L"Lenient.xml", "Lenient.xml");
			}

			if (Standard) {
				downloader(L"https://steamcommunity.com/stats/1229490/leaderboards/5816994/?xml=1&start=1", L"Standard.xml", "Standard.xml");
			}

			if (Violent) {
				downloader(L"https://steamcommunity.com/stats/1229490/leaderboards/5816999/?xml=1&start=1", L"Violent.xml", "Violent.xml");
			}

			if (Brutal) {
				downloader(L"https://steamcommunity.com/stats/1229490/leaderboards/13452166/?xml=1&start=1", L"Brutal.xml", "Brutal.xml");
			}
			//creates window for notifying the user which boards were downloaded.
			nanogui::Screen* successScreen = new nanogui::Screen(nanogui::Vector2i(330, 275), "Success!");
			nanogui::FormHelper* gui = new nanogui::FormHelper(successScreen);
			nanogui::ref<nanogui::Window> errorWindow = gui->addWindow(Eigen::Vector2i(15, 15), "The following leaderboards were downloaded.");
			if (Harmless)
				gui->addGroup("Harmless");
			if (Lenient)
				gui->addGroup("Lenient");
			if (Standard)
				gui->addGroup("Standard");
			if (Violent)
				gui->addGroup("Violent");
			if (Brutal)
				gui->addGroup("Brutal");

			//gui->addGroup("UKMD");
			//Placeholder for unreleased leaderboard

			successScreen->setVisible(true);
			successScreen->performLayout();
			nanogui::mainloop();
		}
	}
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow){
	nanogui::init();

	//mainloop
	{
		nanogui::Screen* screen = new nanogui::Screen(nanogui::Vector2i(250,290), "Ultrakill Leaderboard Downloader");
		nanogui::FormHelper* gui = new nanogui::FormHelper(screen);
		nanogui::ref<nanogui::Window> window = gui->addWindow(Eigen::Vector2i(40, 15),"Difficulty Selection");
		gui->addVariable("Harmless", Harmless);
		gui->addVariable("Lenient", Lenient);
		gui->addVariable("Standard", Standard);
		gui->addVariable("Violent", Violent);
		gui->addVariable("Brutal", Brutal);
		//gui->addVariable("UKMD", UKMD);
		//placeholder for unreleased leaderboard
		gui->addGroup("Additonal Options");
		gui->addVariable("Download all entries?", downloadAll);
		gui->addButton("Submit", []() { submitted(); }); //calls submited function to handle the function calls and checks

		screen->setVisible(true);
		screen->performLayout();

		nanogui::mainloop();
	}

	
	//Placeholder for unreleased leaderboard
	//downloader(L"https://steamcommunity.com/stats/1229490/leaderboards/", L"UKMD.xml", "UKMD.xml");

	return 0;
}