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
//---------------------------------------------------------------------------

//zamiana AnsiString->wchar_t*
wchar_t* AnsiTowchar_t(AnsiString Str)
{                                 
  const char* Text = Str.c_str();
  int size = MultiByteToWideChar(GetACP(), 0, Text, -1, 0,0);
  wchar_t* wbuffer = new wchar_t[size+1];

  MultiByteToWideChar(GetACP(), 0, Text, -1, wbuffer, size+1);

  return wbuffer;
}
//---------------------------------------------------------------------------

//utworzenie obiektow do struktur
TPluginAction PluginActionButton;
TPluginAction PluginActionSeparator;
TPluginActionEdit PluginActionEdit;
TPluginLink PluginLink;
TPluginInfo PluginInfo;
TPluginStateChange PluginStateChange;

bool Polish=1; //Do lokalizacji

//Kradziony opis
AnsiString opis;

//Serwis
int __stdcall StealStatusService (WPARAM, LPARAM)
{
  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),0);

  PluginStateChange.Status = AnsiTowchar_t(opis);
  PluginStateChange.Force = true;

  PluginLink.CallService(AQQ_SYSTEM_SETSHOWANDSTATUS,0,(LPARAM)(&PluginStateChange));

  return 0;
}
//---------------------------------------------------------------------------

//Przypisywanie skrÛtu w popPlugins
void PrzypiszSkrot()
{
  PluginActionButton.cbSize = sizeof(TPluginAction);
  PluginActionButton.pszName = (wchar_t*)L"StealStatusButton";
  if(Polish==1)
   PluginActionButton.pszCaption = (wchar_t*) L"Ukradnij opis!";
  else
   PluginActionButton.pszCaption = (wchar_t*) L"Steal status!";
  PluginActionButton.Position = 10;
  PluginActionButton.IconIndex = -1;
  PluginActionButton.pszService = (wchar_t*) L"serwis_StealStatusService";
  PluginActionButton.pszPopupName = (wchar_t*) L"muItem";
  PluginActionButton.Checked = false;

  PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&PluginActionButton));
  PluginLink.CreateServiceFunction(L"serwis_StealStatusService",StealStatusService);

  PluginActionSeparator.cbSize = sizeof(TPluginAction);
  PluginActionSeparator.pszCaption = (wchar_t*) L"-";
  PluginActionSeparator.pszName = (wchar_t*)L"StealStatusSeparator";
  PluginActionSeparator.Position = 11;
  PluginActionSeparator.pszPopupName = (wchar_t*) L"muItem";
  PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&PluginActionSeparator));
}
//---------------------------------------------------------------------------

//Wylaczanie skrotu
void SkrotOff()
{
  PluginActionEdit.cbSize = sizeof(TPluginActionEdit);
  PluginActionEdit.pszName = (wchar_t*)L"StealStatusButton";
  if(Polish==1)
   PluginActionEdit.Caption = (wchar_t*) L"Ukradnij opis!";
  else
   PluginActionEdit.Caption = (wchar_t*) L"Steal status!";
  PluginActionEdit.Hint = (wchar_t*)L"";
  PluginActionEdit.Enabled = false;
  PluginActionEdit.Visible = true;
  PluginActionEdit.IconIndex = -1;
  PluginActionEdit.Checked = false;

  PluginLink.CallService(AQQ_CONTROLS_EDITPOPUPMENUITEM,0,(LPARAM)(&PluginActionEdit));
}
//---------------------------------------------------------------------------

//Wlaczanie skrotu
void SkrotOn()
{
  PluginActionEdit.cbSize = sizeof(TPluginActionEdit);
  PluginActionEdit.pszName = (wchar_t*)L"StealStatusButton";
  if(Polish==1)
   PluginActionEdit.Caption = (wchar_t*) L"Ukradnij opis!";
  else
   PluginActionEdit.Caption = (wchar_t*) L"Steal status!";
  PluginActionEdit.Hint = (wchar_t*)L"";
  PluginActionEdit.Enabled = true;
  PluginActionEdit.Visible = true;
  PluginActionEdit.IconIndex = -1;
  PluginActionEdit.Checked = false;

  PluginLink.CallService(AQQ_CONTROLS_EDITPOPUPMENUITEM,0,(LPARAM)(&PluginActionEdit));
}
//---------------------------------------------------------------------------

//Hook
int __stdcall OnSystemPopUp (WPARAM wParam, LPARAM lParam)
{
  PPluginPopUp PopUp = (PPluginPopUp)lParam;
  AnsiString PopUpName = (wchar_t*)(PopUp->Name);
  if(PopUpName=="muItem")
  {
    PPluginContact Contact = (PPluginContact)wParam;
    opis = (wchar_t*)(Contact->Status);
    if(opis!="")
     SkrotOn();
    else
     SkrotOff();
  }

  return 0;
}
//---------------------------------------------------------------------------

//Program
extern "C"  __declspec(dllexport) PPluginInfo __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  PluginInfo.cbSize = sizeof(TPluginInfo);
  PluginInfo.ShortName = (wchar_t*)L"StealStatus";
  PluginInfo.Version = PLUGIN_MAKE_VERSION(1,0,1,0);
  PluginInfo.Description = (wchar_t *)L"Bπdü z≥odziejem - ukradnij opis :)";
  PluginInfo.Author = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = (wchar_t *)L"beherit666@vp.pl";
  PluginInfo.Copyright = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = (wchar_t *)L"";

  return &PluginInfo;
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport) __stdcall Load(PPluginLink Link)
{
  PluginLink = *Link;

  //Rozpoznanie lokalizacji
  AnsiString Lang = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETLANGSTR,0,(LPARAM)(L"Password")));
  if(Lang=="Has≥o")
   Polish=1;
  else
   Polish=0;
  //Koniec

  PrzypiszSkrot();

  PluginLink.HookEvent(AQQ_SYSTEM_POPUP,OnSystemPopUp);

  return 0;
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport) __stdcall Unload()
{
  PluginLink.DestroyServiceFunction(StealStatusService);
  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&PluginActionButton));
  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&PluginActionSeparator));
  PluginLink.UnhookEvent(OnSystemPopUp);

  return 0;
}
//---------------------------------------------------------------------------
