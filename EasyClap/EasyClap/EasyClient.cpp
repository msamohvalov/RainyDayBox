
    //  ▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪  //

    #include <TChar.h>      //  Представление ANSI/UNICODE строк                            //
    //
                                                                                            //
    #include "Compress.hpp"     //  IncludeOnly реализация упаковщика                       //
    #include "NotifyIcon.hpp"   //  IncludeOnly функционал «клиентского» приложения         //
                                                                                            //
    VOID CALLBACK EventsHandler ( __in HWND hWnd, \
        __in UINT uMsg, __in const GUID & iconGuid, __inout_opt LPARAM lParam ) {           //
                                                                                            //
        /*  События, отправляемые TrayNotify: */                                            //
                                                                                            //
        /*  WM_CREATE, WM_DESTROY */                                                        //
        /*  WM_MOUSEMOVE, WM_CONTEXTMENU */                                                 //
        /*  WM_LBUTTONDOWN, WM_LBUTTONUP, WM_LBUTTONDBLCLK */                               //
        /*  WM_RBUTTONDOWN, WM_RBUTTONUP, WM_RBUTTONDBLCLK */                               //
        /*  WM_MBUTTONDOWN, WM_MBUTTONUP, WM_MBUTTONDBLCLK */                               //
                                                                                            //
        /*  NIN_SELECT, NIN_KEYSELECT */                                                    //
        /*  TTN_NEEDTEXT */                                                                 //
        /*  NIN_POPUPOPEN, NIN_POPUPCLOSE */                                                //
        /*  NIN_BALLOONSHOW, NIN_BALLOONHIDE, NIN_BALLOONUSERCLICK, NIN_BALLOONTIMEOUT */   //
                                                                                            //
        switch ( uMsg ) {                                                                   //

            case WM_CONTEXTMENU : {

                TrayNotifyIcon * pAssocObj = GetWndObjAssoc(hWnd);  /* Компаньон-объект */   //

                // 350 - Network
                // 550 - Lightning
                // 600 - LightBulb
                // 750 - Flag

                SetForegroundWindow ( hWnd );

                POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                //::GetCursorPos(&pt);

                HMENU hTrayIconMenu = CreatePopupMenu ();

                AppendMenu ( hTrayIconMenu, MF_STRING, 0x12345678, _T("Элемент меню №1") );
                AppendMenu ( hTrayIconMenu, MF_STRING, 0x12345678, _T("Элемент меню №2") );
                AppendMenu ( hTrayIconMenu, MF_STRING, 0x12345678, _T("Элемент меню №3") );
                AppendMenu ( hTrayIconMenu, MF_SEPARATOR, 0, NULL );
                AppendMenu ( hTrayIconMenu, MF_STRING, 0x12345678, _T("Элемент меню №4") );
                AppendMenu ( hTrayIconMenu, MF_STRING, 0x12345678, _T("Элемент меню №5") );

                BOOL bOK = ::TrackPopupMenu ( hTrayIconMenu, 0, pt.x, pt.y, 0, hWnd, NULL );

                
                OutputDebugString ( _T("WM_CONTEXTMENU\n") );
                break;
            }
        }                                                                                   //
    }                                                                                       //

    //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //

    INT APIENTRY _tWinMain ( __in HINSTANCE hInstance, __in __reserved HINSTANCE, \
        __in __nullterminated LPTSTR pszCmdLine, __in INT nCmdShow ) {                      //
                                                                                            //
        UNREFERENCED_PARAMETER( pszCmdLine );       /*  Неиспользуемый параметр */          //
        UNREFERENCED_PARAMETER( nCmdShow );         /*  Неиспользуемый параметр */          //

        HMENU hTrayIconMenu = NULL;
                                                                                            //
        return TrayNotifyIcon :: CreateInstance ( hInstance, \
        
            (FARPROC) EventsHandler, \
            
            Images ( NULL, _T("FLAG_A"), _T("FLAG_B"), _T("FLAG_C"), \
                _T("FLAG_D"), _T("FLAG_E"), _T("FLAG_F"), _T("FLAG_G") ), 550, \

            hTrayIconMenu
        )
          -> RunMessageOnly ( __no_args );               //

        //Compressor * pTest = /* FormatLZNT1, FormatXPress, FormatXPressHuff */ \
            Compressor :: CreateObject ( FormatXPressHuff | EngineMaximum );                //
                                                                                            //
        //BOOL f1 = pTest -> CompressFile \
            ( 1048576 /* 1Мб */, _T("D:\\TMP\\FILES\\05.BMP") );                            //
                                                                                            //
        //BOOL f2 = pTest -> UncompressFile ( _T("D:\\TMP\\FILES\\05.COMPACT") );             //
                                                                                            //
        //f1 && f2 && ( MessageBox(0, _T("SUCCESS"), _T("TEST"), 0), 1 ) || \
            MessageBox(0, _T("FAILED"), 0, 0);                                              //
                                                                                            //
        //Compressor :: DestroyObject ( pTest );                                              //
                                                                                            //
        return NO_ERROR;                                                                    //
    }                                                                                       //

    //  ▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪  //

