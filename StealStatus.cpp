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
TPluginAction StealStatusItem;
PPluginContact SystemPopUContact;
PPluginPopUp PopUp;
//---------------------------------------------------------------------------
UnicodeString StolenStatus;
//---------------------------------------------------------------------------

//Ustawianie nowego opisu
void SetStatus(UnicodeString Status)
{
  TPluginStateChange PluginStateChange;
  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)&PluginStateChange,0);
  PluginStateChange.cbSize = sizeof(TPluginStateChange);
  PluginStateChange.Status = Status.w_str();
  PluginStateChange.Force = true;
  PluginLink.CallService(AQQ_SYSTEM_SETSHOWANDSTATUS,0,(LPARAM)&PluginStateChange);
}
//---------------------------------------------------------------------------

//Serwis elementu w interfejsie
int __stdcall ServiceStealStatusItem(WPARAM wParam, LPARAM lParam)
{
  SetStatus(StolenStatus);

  return 0;
}
//---------------------------------------------------------------------------

//Usuwanie elementu z interfejsu
void DestroyStealStatusItem()
{
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
  PluginItemDescriber.cbSize = sizeof(TPluginItemDescriber);
  PluginItemDescriber.FormHandle = 0;
  PluginItemDescriber.ParentName = L"muItem";
  PluginItemDescriber.Name = L"muProfile";
  PPluginAction Action = (PPluginAction)PluginLink.CallService(AQQ_CONTROLS_GETPOPUPMENUITEM,0,(LPARAM)(&PluginItemDescriber));
  int Position = Action->Position;
  //Tworzenie elementu wtyczki
  PluginLink.CreateServiceFunction(L"sStealStatusItem",ServiceStealStatusItem);
  StealStatusItem.cbSize = sizeof(TPluginAction);
  StealStatusItem.pszName = L"StealStatusItem";
  StealStatusItem.pszCaption = L"Ukradnij opis!";;
  StealStatusItem.Position = Position + 2;
  StealStatusItem.IconIndex = -1;
  StealStatusItem.pszService = L"sStealStatusItem";
  StealStatusItem.pszPopupName = L"muItem";
  PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&StealStatusItem));
}
//---------------------------------------------------------------------------

//Hook na pokazywanie popumenu
int __stdcall OnSystemPopUp(WPARAM wParam, LPARAM lParam)
{
  PopUp = (PPluginPopUp)lParam;
  //Pobieranie nazwy popupmenu
  UnicodeString PopUpName = (wchar_t*)PopUp->Name;
  //Popupmenu dostepne spod PPM na kontakcie w oknie kontaktow
  if(PopUpName=="muItem")
  {
	SystemPopUContact = (PPluginContact)wParam;
	//Pobieranie opisu kontatku
	UnicodeString Status = (wchar_t*)SystemPopUContact->Status;
	//Usuwanie zbednych spacji
	Status = Status.Trim();
	//Przekazanie opisu do globalnej zmiennej
	StolenStatus = Status;
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
  PluginInfo.Version = PLUGIN_MAKE_VERSION(2,2,0,0);
  PluginInfo.Description = L"Wtyczka napisana z nudów, realizuje propozycjê jednego z u¿ytkowników forum. Jest przeznaczona wy³¹cznie dla tych, którzy lubi¹ kraœæ czyjeœ opisy i ustawiaæ je u siebie.";
  PluginInfo.Author = L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = L"kontakt@beherit.pl";
  PluginInfo.Copyright = L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = L"http://beherit.pl";

  return &PluginInfo;
}
//---------------------------------------------------------------------------
