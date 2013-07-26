//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#include "Aqq.h"
//---------------------------------------------------------------------------

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
  return 1;
}

wchar_t *AnsiTowchar_t(AnsiString Str) //zamiana AnsiString->wchar_t*
{
  wchar_t *str = new wchar_t[Str.WideCharBufSize()];
  return Str.WideChar(str, Str.WideCharBufSize());
}

//utworzenie obiektow do struktur
PluginAction TPluginAction;
PluginLink TPluginLink;
PluginInfo TPluginInfo;
PluginStateChange TPluginStateChange;

//Kradziony opis
AnsiString opis;

//Serwis
int __stdcall StoleStatusService (WPARAM, LPARAM)
{
  TPluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&TPluginStateChange),0);

  TPluginStateChange.Status = AnsiTowchar_t(opis);
  TPluginStateChange.Force = true;

  TPluginLink.CallService(AQQ_SYSTEM_SETSHOWANDSTATUS,0,(LPARAM)(&TPluginStateChange));

  return 0;
}                       

//Hook
int __stdcall OnContactsBuddySelected (WPARAM wParam, LPARAM lParam)
{
  PluginContact *Contact = (PluginContact*)lParam;
  opis = (wchar_t*)(Contact->Status);

  return 0;
}

//Program
extern "C"  __declspec(dllexport) PluginInfo* __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  TPluginInfo.cbSize = sizeof(PluginInfo);
  TPluginInfo.ShortName = (wchar_t*)L"StoleStatus";
  TPluginInfo.Version = PLUGIN_MAKE_VERSION(1,0,0,0);
  TPluginInfo.Description = (wchar_t *)L"B¹dŸ z³odziejem - ukradnij opis :)";
  TPluginInfo.Author = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  TPluginInfo.AuthorMail = (wchar_t *)L"beherit666@vp.pl";
  TPluginInfo.Copyright = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  TPluginInfo.Homepage = (wchar_t *)L"";
 
  return &TPluginInfo;
}

//Przypisywanie skrótu w popPlugins
void PrzypisywanieSkrotu()
{
  TPluginAction.cbSize = sizeof(PluginAction);
  TPluginAction.Action = (wchar_t*)L"";
  TPluginAction.pszName = (wchar_t*)L"StoleStatusButton";
  TPluginAction.pszCaption = (wchar_t*) L"Ukradnij opis!";
  TPluginAction.Position = 10;
  TPluginAction.IconIndex = -1;
  TPluginAction.pszService = (wchar_t*) L"serwis_StoleStatusService";
  TPluginAction.pszPopupName = (wchar_t*) L"muItem";

  TPluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&TPluginAction));
  TPluginLink.CreateServiceFunction(L"serwis_StoleStatusService",StoleStatusService);
}

extern "C" int __declspec(dllexport) __stdcall Load(PluginLink *Link)
{
  TPluginLink = *Link;

  PrzypisywanieSkrotu();

  TPluginLink.HookEvent(AQQ_CONTACTS_BUDDY_SELECTED, OnContactsBuddySelected);

  return 0;
}

extern "C" int __declspec(dllexport) __stdcall Unload()
{
  TPluginLink.DestroyServiceFunction(StoleStatusService);
  TPluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&TPluginAction));
  TPluginLink.UnhookEvent(OnContactsBuddySelected);

  return 0;
}


