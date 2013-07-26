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

//utworzenie obiektow do struktur
TPluginAction PluginActionButton;
TPluginAction PluginActionSeparator;
TPluginActionEdit PluginActionEdit;
TPluginLink PluginLink;
TPluginInfo PluginInfo;
TPluginStateChange PluginStateChange;

PPluginPopUp PopUp;
UnicodeString PopUpName;
PPluginContact Contact;

bool Polish=1; //Do lokalizacji

//Kradziony opis
UnicodeString opis;

//Serwis
int __stdcall StealStatusService (WPARAM, LPARAM)
{
  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),0);

  PluginStateChange.cbSize = sizeof(TPluginStateChange);
  PluginStateChange.Status = opis.w_str();
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
  PluginActionButton.Position = 12;
  PluginActionButton.IconIndex = -1;
  PluginActionButton.pszService = (wchar_t*) L"serwis_StealStatusService";
  PluginActionButton.pszPopupName = (wchar_t*) L"muItem";
  PluginActionButton.Checked = false;

  PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&PluginActionButton));
  PluginLink.CreateServiceFunction(L"serwis_StealStatusService",StealStatusService);

  PluginActionSeparator.cbSize = sizeof(TPluginAction);
  PluginActionSeparator.pszCaption = (wchar_t*) L"-";
  PluginActionSeparator.pszName = (wchar_t*)L"StealStatusSeparator";
  PluginActionSeparator.Position = 13;
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
  PopUp = (PPluginPopUp)lParam;
  PopUpName = (wchar_t*)(PopUp->Name);
  if(PopUpName=="muItem")
  {
    Contact = (PPluginContact)wParam;
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
  PluginInfo.Version = PLUGIN_MAKE_VERSION(2,0,0,0);
  PluginInfo.Description = (wchar_t *)L"Bπdü z≥odziejem - ukradnij opis :)";
  PluginInfo.Author = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = (wchar_t *)L"sirbeherit@gmail.com";
  PluginInfo.Copyright = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = (wchar_t *)L"http://beherit.pl/";
  PluginInfo.Flag = 0;
  PluginInfo.ReplaceDefaultModule = 0;

  return &PluginInfo;
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport) __stdcall Load(PPluginLink Link)
{
  PluginLink = *Link;

  //Rozpoznanie lokalizacji
  UnicodeString Lang = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETLANGSTR,0,(LPARAM)(L"Password")));
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
//--------------------------------------------------------------------------
