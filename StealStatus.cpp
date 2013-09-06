//---------------------------------------------------------------------------
// Copyright (C) 2009-2013 Krzysztof Grochocki
//
// This file is part of StealStatus
//
// StealStatus is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// StealStatus is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GNU Radio; see the file COPYING. If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street,
// Boston, MA 02110-1301, USA.
//---------------------------------------------------------------------------

#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#include <PluginAPI.h>

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	return 1;
}
//---------------------------------------------------------------------------

//Struktury-glowne-----------------------------------------------------------
TPluginLink PluginLink;
TPluginInfo PluginInfo;
//Skopiowany-opis------------------------------------------------------------
UnicodeString Status;
//---------------------------------------------------------------------------

//Ustawianie nowego opisu
void SetStatus(UnicodeString Status)
{
  TPluginStateChange PluginStateChange;
  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)&PluginStateChange,0);
  PluginStateChange.Status = Status.w_str();
  PluginStateChange.Force = true;
  PluginLink.CallService(AQQ_SYSTEM_SETSHOWANDSTATUS,0,(LPARAM)&PluginStateChange);
}
//---------------------------------------------------------------------------

//Serwis elementu w interfejsie
int __stdcall ServiceStealStatusItem(WPARAM wParam, LPARAM lParam)
{
  SetStatus(Status);

  return 0;
}
//---------------------------------------------------------------------------

//Usuwanie elementu z interfejsu
void DestroyStealStatusItem()
{
  TPluginAction StealStatusItem;
  ZeroMemory(&StealStatusItem, sizeof(TPluginAction));
  StealStatusItem.cbSize = sizeof(TPluginAction);
  StealStatusItem.pszName = L"StealStatusItem";
  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&StealStatusItem));
  PluginLink.DestroyServiceFunction(ServiceStealStatusItem);
}
//---------------------------------------------------------------------------

//Tworzenie elementu w interfejsie
void BuildStealStatusItem()
{
  //Ustalanie pozycji elementu "Wizytowka"
  TPluginItemDescriber PluginItemDescriber;
  ZeroMemory(&PluginItemDescriber, sizeof(TPluginItemDescriber));
  PluginItemDescriber.cbSize = sizeof(TPluginItemDescriber);
  PluginItemDescriber.FormHandle = 0;
  PluginItemDescriber.ParentName = L"muItem";
  PluginItemDescriber.Name = L"muProfile";
  TPluginAction Action = *(PPluginAction)(PluginLink.CallService(AQQ_CONTROLS_GETPOPUPMENUITEM,0,(LPARAM)(&PluginItemDescriber)));
  //Tworzenie elementu wtyczki
  PluginLink.CreateServiceFunction(L"sStealStatusItem",ServiceStealStatusItem);
  TPluginAction StealStatusItem;
  ZeroMemory(&StealStatusItem, sizeof(TPluginAction));
  StealStatusItem.cbSize = sizeof(TPluginAction);
  StealStatusItem.pszName = L"StealStatusItem";
  StealStatusItem.pszCaption = L"Ukradnij opis!";;
  StealStatusItem.Position = Action.Position + 2;
  StealStatusItem.IconIndex = -1;
  StealStatusItem.pszService = L"sStealStatusItem";
  StealStatusItem.pszPopupName = L"muItem";
  PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&StealStatusItem));
}
//---------------------------------------------------------------------------

//Hook na pokazywanie popumenu
int __stdcall OnSystemPopUp(WPARAM wParam, LPARAM lParam)
{
  TPluginPopUp PopUp = *(PPluginPopUp)lParam;
  //Pobieranie nazwy popupmenu
  UnicodeString PopUpName = (wchar_t*)PopUp.Name;
  //Popupmenu dostepne spod PPM na kontakcie w oknie kontaktow
  if(PopUpName=="muItem")
  {
	TPluginContact SystemPopUContact = *(PPluginContact)wParam;
	//Pobieranie opisu kontatku
	Status = (wchar_t*)SystemPopUContact.Status;
	//Usuwanie zbednych spacji
	Status.Trim();
	//Wlaczenie elementu
	if(!Status.IsEmpty())
	{
	  TPluginActionEdit PluginActionEdit;
	  PluginActionEdit.cbSize = sizeof(TPluginActionEdit);
	  PluginActionEdit.pszName = L"StealStatusItem";
	  PluginActionEdit.Caption = L"Ukradnij opis!";
	  PluginActionEdit.Hint = L"";
	  PluginActionEdit.Enabled = true;
	  PluginActionEdit.Visible = true;
	  PluginActionEdit.IconIndex = -1;
	  PluginActionEdit.Checked = false;
	  PluginLink.CallService(AQQ_CONTROLS_EDITPOPUPMENUITEM,0,(LPARAM)(&PluginActionEdit));
	}
	//Wylaczenie elementu
	else
	{
	  TPluginActionEdit PluginActionEdit;
	  PluginActionEdit.cbSize = sizeof(TPluginActionEdit);
	  PluginActionEdit.pszName = L"StealStatusItem";
	  PluginActionEdit.Caption = L"Ukradnij opis!";
	  PluginActionEdit.Hint = L"";
	  PluginActionEdit.Enabled = false;
	  PluginActionEdit.Visible = true;
	  PluginActionEdit.IconIndex = -1;
	  PluginActionEdit.Checked = false;
	  PluginLink.CallService(AQQ_CONTROLS_EDITPOPUPMENUITEM,0,(LPARAM)(&PluginActionEdit));
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Zaladowanie wtyczki
extern "C" int __declspec(dllexport) __stdcall Load(PPluginLink Link)
{
  //Linkowanie wtyczki z komunikatorem
  PluginLink = *Link;
  //Hook na pokazywanie popumenu
  PluginLink.HookEvent(AQQ_SYSTEM_POPUP,OnSystemPopUp);
  //Tworzenie elementu w interfejsie
  BuildStealStatusItem();

  return 0;
}
//---------------------------------------------------------------------------

//Wyladowanie wtyczki
extern "C" int __declspec(dllexport) __stdcall Unload()
{
  //Wyladowanie wszystkich hookow
  PluginLink.UnhookEvent(OnSystemPopUp);
  //Usuwanie elementu z interfejsu
  DestroyStealStatusItem();

  return 0;
}
//---------------------------------------------------------------------------

//Informacje o wtyczce
extern "C" __declspec(dllexport) PPluginInfo __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  PluginInfo.cbSize = sizeof(TPluginInfo);
  PluginInfo.ShortName = L"StealStatus";
  PluginInfo.Version = PLUGIN_MAKE_VERSION(2,2,1,0);
  PluginInfo.Description = L"Wtyczka s³u¿y do ustawiania opisu we wszystkich kontach, na podstawie opisu wybranego kontaktu.";
  PluginInfo.Author = L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = L"kontakt@beherit.pl";
  PluginInfo.Copyright = L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = L"http://beherit.pl";

  return &PluginInfo;
}
//---------------------------------------------------------------------------
