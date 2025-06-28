
    //  ▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪  //

    #include <TChar.h>      //  Представление ANSI/UNICODE строк                            //
    //
                                                                                            //
    #include "Compress.hpp"     //  IncludeOnly реализация упаковщика                       //
    #include "NotifyIcon.hpp"   //  IncludeOnly функционал «клиентского» приложения         //
                                                                                            //
    VOID CALLBACK EventsHandler \
        ( __in HWND hWnd, __in UINT uMsg, __in const GUID & iconGuid ) {                    //

        static HWND hwndTT = NULL;
                                                                                            //
        switch ( uMsg ) {                                                                     //
            
            case WM_MOUSEMOVE :
            
                //OutputDebugString ( _T("WM_MOUSEMOVE\n") );
                break;

            case WM_LBUTTONDOWN: {

                OutputDebugString ( _T("WM_LBUTTONDOWN\n") );
                break;
            }

            case WM_LBUTTONUP:

                OutputDebugString ( _T("WM_LBUTTONUP\n") );
                break;

            case WM_LBUTTONDBLCLK:
                OutputDebugString ( _T("WM_LBUTTONDBLCLK\n") ); \
                
                //( GetWndObjAssoc ( hWnd )) -> \
                    TrayNotifyChangeIconImages ( iconGuid, Images(NULL, _T("LIGHTBULB_A"), _T("LIGHTBULB_B")), 600 );

                DestroyWindow ( hWnd );
                break;

            case WM_RBUTTONDOWN:

                OutputDebugString ( _T("WM_RBUTTONDOWN\n") ); \
                break;

            case WM_RBUTTONUP:

                OutputDebugString ( _T("WM_RBUTTONUP\n") ); \
                break;

            case WM_RBUTTONDBLCLK:

                OutputDebugString ( _T("WM_RBUTTONDBLCLK\n") ); \
                break;

            case WM_MBUTTONDOWN:

                /* Уведомление */

                ( GetWndObjAssoc ( hWnd )) -> \
                    TrayNotifyIconMessage ( iconGuid, _T("Hello, World!"), _T("DEBUG") );

                OutputDebugString ( _T("WM_MBUTTONDOWN\n") ); \
                break;

            case WM_MBUTTONUP:

                OutputDebugString ( _T("WM_MBUTTONUP\n") ); \
                break;

            case WM_MBUTTONDBLCLK:

                OutputDebugString ( _T("WM_MBUTTONDBLCLK\n") ); \
                break;

            case WM_CONTEXTMENU : {

                //TrayNotifyIcon * pAssocObj = GetWndObjAssoc(hWnd);  /* Компаньон-объект */   //

                //GUID id = {0};

                // 350 - Network
                // 550 - Lightning
                // 600 - LightBulb
                // 750 - Flag

                //CoCreateGuid(&id);
                
                //pAssocObj -> CreateTrayNotifyIcon (id, Images(NULL, _T("LIGHTBULB_A"), _T("LIGHTBULB_B") ), 600 );

                OutputDebugString ( _T("WM_CONTEXTMENU\n") );
                break;
            }

            case NIN_SELECT:

                OutputDebugString ( _T("NIN_SELECT\n") );
                break;

            case NIN_KEYSELECT:

                OutputDebugString ( _T("NIN_SELECT\n") );
                break;

            case NIN_POPUPOPEN: {

                ( GetWndObjAssoc ( hWnd )) -> TrayNotifyIconBalloonShow ( _T("Hello, World!"), _T("DEBUG") );

                OutputDebugString ( _T("NIN_POPUPOPEN\n") );
                break;
            }

            case NIN_POPUPCLOSE:

                ( GetWndObjAssoc ( hWnd ) ) -> TrayNotifyIconBalloonHide ( __no_args );

                OutputDebugString ( _T("NIN_POPUPCLOSE\n") );
                break;

            case NIN_BALLOONSHOW:

                OutputDebugString ( _T("NIN_BALLOONSHOW\n") );
                break;

            case NIN_BALLOONHIDE:

                OutputDebugString ( _T("NIN_BALLOONHIDE\n") );
                break;

            case NIN_BALLOONUSERCLICK:

                OutputDebugString ( _T("NIN_BALLOONUSERCLICK\n") );
                break;

            case NIN_BALLOONTIMEOUT:

                OutputDebugString ( _T("NIN_BALLOONTIMEOUT\n") );
                break;

            default: {
            
                DebugBreak();
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

        //INITCOMMONCONTROLSEX iccex = {0};

        //iccex.dwSize = sizeof ( iccex );
        //iccex.dwICC = ICC_TREEVIEW_CLASSES | ICC_BAR_CLASSES | ICC_TAB_CLASSES;

        //InitCommonControlsEx ( &iccex );
                                                                                            //
        TrayNotifyIcon :: CreateInstance ( hInstance, (FARPROC) EventsHandler, \
            
            Images ( NULL, _T("LIGHTNING_A"), _T("LIGHTNING_B"), _T("LIGHTNING_C"), _T("LIGHTNING_D")), 550 \
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

