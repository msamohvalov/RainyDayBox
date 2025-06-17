
    //  ▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪  //

    #include <TChar.h>      //  Представление ANSI/UNICODE строк                            //
    //
                                                                                            //
    #include "Compress.hpp"     //  IncludeOnly реализация упаковщика                       //
    #include "NotifyIcon.hpp"   //  IncludeOnly функционал «клиентского» приложения         //
                                                                                            //
    VOID CALLBACK CmdHandler ( __in HWND hWnd, __in UINT uMsg, __in UINT idIcon ) {         //
                                                                                            //
        ( uMsg == WM_LBUTTONDBLCLK ) && DestroyWindow ( hWnd );                             //
    }                                                                                       //

    //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //

    INT APIENTRY _tWinMain ( __in HINSTANCE hInstance, __in __reserved HINSTANCE, \
        __in __nullterminated LPTSTR pszCmdLine, __in INT nCmdShow ) {                      //
                                                                                            //
        UNREFERENCED_PARAMETER( hInstance );        /*  Неиспользуемый параметр */          //
        UNREFERENCED_PARAMETER( pszCmdLine );       /*  Неиспользуемый параметр */          //
        UNREFERENCED_PARAMETER( nCmdShow );         /*  Неиспользуемый параметр */          //
                                                                                            //
        //TrayNotifyIcon :: CreateInstance \
            ( hInstance, (FARPROC) CmdHandler ) -> RunMessageOnly ( __no_args );

        Compressor * pTest = /* FormatLZNT1, FormatXPress, FormatXPressHuff */ \
            Compressor :: CreateObject ( FormatXPressHuff | EngineMaximum );                //
                                                                                            //
        BOOL f1 = pTest -> CompressFile \
            ( 1048576 /* 1Мб */, _T("D:\\TMP\\FILES\\05.BMP") );                            //
                                                                                            //
        BOOL f2 = pTest -> UncompressFile ( _T("D:\\TMP\\FILES\\05.COMPACT") );             //
                                                                                            //
        f1 && f2 && ( MessageBox(0, _T("SUCCESS"), _T("TEST"), 0), 1 ) || \
            MessageBox(0, _T("FAILED"), 0, 0);                                              //
                                                                                            //
        Compressor :: DestroyObject ( pTest );                                              //
                                                                                            //
        return NO_ERROR;                                                                    //
    }                                                                                       //

    //  ▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪  //

