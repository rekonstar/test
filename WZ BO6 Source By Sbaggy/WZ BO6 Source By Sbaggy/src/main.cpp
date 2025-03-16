#include "framework/overlay/overlay_setup.h"
#include <iostream>
#include "utils/kernel/kernel.h"
#include "world/engine/engine.h"
#include <thread>
#include "utils/protection/Lazy.h"


int main ( ) {
    while ( hwnd == NULL )
    {
        hwnd = FindWindowA ( nullptr, E ( "Call of Duty®" ) );
        LI_FN ( Sleep )( 100 );
    }

	imguisetup::Overlay ( );
	imguisetup::SetupImgui ( );
	imguisetup::GuiLoop ( );
}