
    //  ▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪  //

    #include <TChar.h>      //  Представление ANSI/UNICODE строк                            //
    //
                                                                                            //
    #include "Compress.hpp"     //  IncludeOnly реализация упаковщика                       //
    #include "NotifyIcon.hpp"   //  IncludeOnly функционал «клиентского» приложения         //
                                                                                            //
    VOID CALLBACK EventsDispatch ( __in HWND hWnd, \
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
        //  350 ms - Network                                                                //
        //  550 ms - Lightning                                                              //
        //  600 ms - LightBulb                                                              //
        //  750 ms - Flag                                                                   //
                                                                                            //
        switch ( uMsg ) {                                                                   //
                                                                                            //
        }                                                                                   //
    }                                                                                       //

    //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //

    INT APIENTRY _tWinMain ( __in HINSTANCE hInstance, __in __reserved HINSTANCE, \
        __in __nullterminated LPTSTR pszCmdLine, __in INT nCmdShow ) {                      //
                                                                                            //
        UNREFERENCED_PARAMETER( pszCmdLine );       /*  Неиспользуемый параметр */          //
        UNREFERENCED_PARAMETER( nCmdShow );         /*  Неиспользуемый параметр */          //

         MENUITEMINFO mii = {0};

        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_ID | MIIM_STRING | MIIM_BITMAP | MIIM_STATE | MIIM_FTYPE | MIIM_DATA;   ///MIIM_SUBMENU/
        mii.fType = MFT_STRING;
        mii.hbmpItem = HBMMENU_CALLBACK;
        mii.fState = MFS_CHECKED;

        mii.wID = 1000;
        mii.dwTypeData = (PTSTR) (ULONG_PTR) _T("Hello, World!");
        mii.cch = lstrlen ( mii.dwTypeData );

        /*HMENU hContextMenu = CreatePopupMenu();

        InsertMenuItem ( hContextMenu, 0, TRUE, &mii );

        mii.wID = 1000;
        mii.dwTypeData = (PTSTR)(ULONG_PTR)_T("Привет, Мир!");
        mii.cch = lstrlen(mii.dwTypeData);
        mii.fState = MFS_UNCHECKED;

        InsertMenuItem(hContextMenu, 1, TRUE, &mii);

        //AppendMenu ( hContextMenu, MF_STRING, 0x12345678, _T("Элемент меню №1") );
        //AppendMenu ( hContextMenu, MF_STRING, 0x12345678, _T("Элемент меню №2") );
        //AppendMenu ( hContextMenu, MF_STRING, 0x12345678, _T("Элемент меню №3") );
        //AppendMenu ( hContextMenu, MF_SEPARATOR, 0, NULL );
        //AppendMenu ( hContextMenu, MF_STRING, 0x12345678, _T("Элемент меню №4") );
        //AppendMenu ( hContextMenu, MF_STRING, 0x12345678, _T("Элемент меню №5") );*/

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
        
        /*  Создание Singleton-экземпляра приложения c TrayNotify-значком */                //
        return TrayNotifyIcon :: CreateInstance ( /*  Хэндл экземпляра */ hInstance, \
                                                                                            //
            _T( "{7169521D-5243-42FE-92A3-F0520D285490}" )  /* Уникальный GUID */ , \
                                                                                            //
            (FARPROC) EventsDispatch  /*  Обработчик событий трей-значка */, \
                                                                                            //
            //  Набор иконок ( список «HIMAGELIST» ), ассоциированных с создаваемым         //
            //  значком в системной области уведомлений, если набор содержит несколько      //
            //  иконок - параметр «uIconsDelayTimeout» определяет скорость их ротации       //
                                                                                            //
            UsingImages ( NULL, _T("FLAG_A"), _T("FLAG_B"), _T("FLAG_C"), \
                _T("FLAG_D"), _T("FLAG_E"), _T("FLAG_F"), _T("FLAG_G") ), 550, \
                                                                                            //
            CtxMenu (   /*  Представление контекстного меню для значка уведомлений */       //
                                                                                            //
                UsingImages ( NULL, /*  Набор иконок, используемых для пунктов меню */ \
                    _T("CloseApp"), _T("Settings"), _T("JobQuery"), _T("Suspend"), \
                    _T("Repeat"), _T("Instance_1"), _T("Instance_2"), _T("Instance_3") ), \
                                                                                            //
                /*  Пункты ( или подменю), содержащиеся в контекстном меню значка */        //
                Item( 1001, 0, _T("Выход")), Item( 1002, 1, _T("Настройки") ), \
                    
                SubMenuWithIcon( 2, \

                    _T("Задания"), \

                    Item( 2001, 7, _T("x3") ), \
                    Item( 2002, 6, _T("x2") ), \
                    Item( 2003, 5, _T("x1") )
                ), \

                Item( 1004, 3, _T("Остановка") ), \
                Item( 1005, 4, _T("Запуск") )
            )                                                                               //
        )                                                                                   //
          -> RunMessageOnly ( __no_args );  /*  Запуск экземпляра приложения */             //
    }                                                                                       //

    //  ▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪  //

