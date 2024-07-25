#ifndef WIFISCREEN_H
#define WIFISCREEN_H

#include "mainmenu2.h"
#include "../tcommon/wifi.h"
#include "../tcommon/gui/screen.h"
#include "../tcommon/gui/button.h"
#include "../tcommon/gui/spinner.h"

class WifiScreen : public Screen, public ButtonListener, public SpinnerListener {
	private:
		MainMenu2* menu;
		TextField* ssidField;
		Spinner* wepTypeSpinner;
		Spinner* wepKeyTypeSpinner;
		TextField* wepKeyField;
		Spinner* ipConfigSpinner;
		TextField* ipField;
		TextField* subnetMaskField;
		TextField* gatewayField;
		TextField* dnsField;
		Button* backButton;
		Button* testButton;

		void UpdateWifiConfig();

	public:
		WifiScreen(GUI* gui, MainMenu2* menu);
		virtual ~WifiScreen();

		virtual void Activate();
		virtual void Deactivate();
		virtual void Cancel();
		virtual void Update(u32& down, u32& held, touchPosition touch);
		virtual void DrawBackground();
		virtual void OnSpinnerChanged(Spinner* spinner);
		virtual void OnButtonPressed(Button* button);
};

#endif
