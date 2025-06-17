

    //  ▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪  //

    #pragma once    //  Include-Guard подключаемого модуля                                  //
                                                                                            //
    #include <Unknwn.h>     //  Описание интерфейса IUnknown                                //
    #include <AtlBase.h>    //  Функционал взаимодействия с подсистемой COM                 //
    #include <Windows.h>    //  Функционал Windows API                                      //
    #include <WindowsX.h>   //  Расширенный функционал Windows API                          //
                                                                                            //
    #include "Auxiliary.h"  //  Дополнительные средства препроцессора                       //
                                                                                            //
    #ifdef _WIN64   /*  Установка и зарос компаньон-объекта для окна … */                   //
                                                                                            //
        //  Реализация для 64-х разрядной компоновки приложения                             //
                                                                                            //
        #define SetWndObjAssoc( hwnd, ptr ) /*  … установка */ \
            ( !SetWindowLongPtr ( hwnd, GWLP_USERDATA, (LONG_PTR) ptr ) )                   //
                                                                                            //
        #define GetWndObjAssoc( hwnd ) /*  … запрос */ \
            ( TrayNotifyIcon * )( LONG_PTR ) GetWindowLongPtr ( hwnd, GWLP_USERDATA )       //
                                                                                            //
    #else                                                                                   //
                                                                                            //
        //  Реализация для 32-х разрядной компоновки приложения                             //
                                                                                            //
        #define SetWndObjAssoc( hwnd, ptr ) /*  … установка */ \
            ( !SetWindowLong ( hwnd, GWL_USERDATA, (LONG) ptr ) )                           //
                                                                                            //
        #define GetWndObjAssoc( hwnd ) /*  … запрос */ \
            ( TrayNotifyIcon * )( LONG ) GetWindowLong ( hwnd, GWL_USERDATA )               //
                                                                                            //
    #endif                                                                                  //
                                                                                            //
    #define WM_NOTIFYICON   ( WM_USER + 0x100 )  /* Идентификатор сообщения TRAY_ICON */    //
                                                                                            //
    /*  Распаковщик сообщения «WM_NOTIFYICON» */                                            //
    #define HANDLE_WM_NOTIFYICON( hwnd, wParam, lParam, fn ) \
        ((fn)((hwnd), (UINT)(lParam),(UINT)(wParam)), 1L)                                   //
                                                                                            //
    #define FORWARD_WM_NOTIFYICON( hwnd, msg, id, pfn ) /* Проброс сообщения */ \
        ( ((VOID ( CALLBACK *) (HWND, UINT, UINT))(pfn))(hwnd,msg,id), 1L )                 //

    //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //

    class TrayNotifyIcon : public IUnknown {    /*  Имплементация IUnknown */               //
                                                                                            //
        private :   /*  Закрытые свойства и методы класса */                                //
                                                                                            //
        LONG m_cRef;    //  Счётчик ссылок на объект класса ( IUnknown::m_cRef )            //
                                                                                            //
        HINSTANCE m_hInstance;  //  Дескриптор экземпляра приложения                        //
                                                                                            //
        HWND m_hWnd;            //  Дескриптор связанного окна                              //
        FARPROC m_pfnHandler;   //  Пользовательский обработчик сообщений                   //
                                                                                            //
        /*  Закрытый конструктор объектов класса ( напрямую не используется ) */            //
                                                                                            //
        TrayNotifyIcon ( __in HINSTANCE hInstance, __in FARPROC pfnMsgHandler ) : \
            m_hInstance ( hInstance ), m_pfnHandler ( pfnMsgHandler ) {                     //
                                                                                            //
            WNDCLASSEX wc = {0};    /*  Структура WinAPI с описанием класса окна */         //
                                                                                            //
            /*  Заполнение «тривиальных» полей WNDCLASSEX-структуры … */                    //
            ( wc.cbSize = sizeof ( WNDCLASSEX ), wc.lpfnWndProc = MessageOnlyWndProc, \
                wc.hInstance = m_hInstance, wc.lpszClassName = _T("EasyClient"), \
                    RegisterClassEx ( &wc ) /*  … регистрация класса окна */) && \
                                                                                            //
            //  Работа клиентского приложения будет осуществляться без полноценной          //
            //  оконной визуализации, но в системном трее - будет создана служебная         //
            //  иконка, отображающая текущее состояние и принимающая команды                //
                                                                                            //
            ( m_hWnd = CreateWindowEx /*  Создание MessageOnly-окна … */ \
                ( 0, wc.lpszClassName, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, \
                    (PVOID) this /*  Экземпляр объекта связывается с окном */) );           //
        }                                                                                   //
        
        //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //
        
        /*  Инициализация окна при его создании */                                          //
                                                                                            //
        static BOOL OnCreateMessageOnlyWnd \
            ( __in HWND hWnd, __in __refparam LPCREATESTRUCT lpCreateInfo ) {               //

            #define WM_USER_SHELLICON (WM_USER + 1)

            NOTIFYICONDATA data = { 0 };

            data.cbSize = NOTIFYICONDATA_V3_SIZE;
            data.uVersion = NOTIFYICON_VERSION;
            data.hWnd = hWnd;
            data.uID = 1;          // Можно поставить любой идентификатор, всё равно иконка только одна
            data.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
            data.uCallbackMessage = WM_NOTIFYICON;
            data.hIcon = LoadIcon(NULL, MAKEINTRESOURCE((WORD) IDI_APPLICATION));
            lstrcpy(data.szTip, _T("EasyClapClient"));
            data.uVersion = NOTIFYICON_VERSION;

            Shell_NotifyIcon(NIM_ADD, &data);
                                                                                            //
            TrayNotifyIcon * pAssocObj = NULL;  /*  Указатель ассоциированного объекта */   //
                                                                                            //
            //  Создание очереди сообщений MessageOnly-окна и реализация их обработки -     //
            //  осуществляются, пока существует связанный с ним компаньон-объект            //
                                                                                            //
            //  Обработчик «WM_CREATE» производит захват и удержание ссылки на этот         //
            //  объект, что препятствует его уничтожению; обработчик «WM_DESTROY» -         //
            //  должен завершить цикл обработки сообщений вызовом «PostQuitMessage» и       //
            //  освободить занятую ссылку, что приведёт к уничтожению компаньон-объекта     //
                                                                                            //
            return lpCreateInfo -> lpCreateParams /*  Ссылка задана … */ && \
                                                                                            //
                /*  Захват ссылки и увеличение её счётчика - успешны … */
                SUCCEEDED( ( ( TrayNotifyIcon* ) lpCreateInfo -> lpCreateParams ) -> \
                    QueryInterface ( IID_IUnknown, (PVOID*) &pAssocObj ) ) && \
                                                                                            //
                SetWndObjAssoc ( hWnd, pAssocObj );   /*  Установка ассоциации */           //
        }                                                                                   //

        //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //

        //  Вызов пользовательского обработчика для сообщений трей-иконки                   //

        static VOID OnNotifyShellIcon \
            ( __in HWND hWnd, __in UINT uMsg, __in UINT idIcon ) {                          //
                                                                                            //
            TrayNotifyIcon * pAssocObj = GetWndObjAssoc ( hWnd );  /* Компаньон-объект */   //
                                                                                            //
            pAssocObj /*  Если ассоциированный с окном объект - существует … */ && \
                pAssocObj -> m_pfnHandler /*  Пользовательский обработчик назначен … */&& \
                                                                                            //
            /*  Осуществление вызова обработчика с передачей параметров … */                //
            FORWARD_WM_NOTIFYICON( hWnd, uMsg, idIcon, pAssocObj -> m_pfnHandler);          //
        }                                                                                   //

        //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //

        /*  Очистка ресурсов окна при его уничтожени */                                     //
                                                                                            //
        static VOID OnDestroyMessageOnlyWnd ( __in HWND hWnd ) {                            //
                                                                                            //
            TrayNotifyIcon * pAssocObj = NULL;   /*  Ассоциированный объект */              //
            ULONG uThisRefCount = -1;   /*  Счётчик ссылок при уничтожении объекта */       //
                                                                                            //
            ( pAssocObj = GetWndObjAssoc ( hWnd ) ) &&   /*  Запрос указателя */  \
                                                                                            //
            //  Вызов «PostQuitMessage» - обеспечит завершение очереди сообщений окна;      //
            //  последующий вызов «IUnknown::Release» - освободит удерживаемую ссылку,      //
            //  что приведёт к вызову деструктора компаньон-объекта                         //
                                                                                            //
            ( PostQuitMessage ( NO_ERROR ), uThisRefCount = pAssocObj -> Release(), \
                                                                                            //
            SetWndObjAssoc ( hWnd, NULL ),  /*  Сброс ассоциативного указателя */           //
                                                                                            //
            //  В нормальном режиме работы - объявленное ниже исключение - невозможно,      //
            //  но при нарушении целостности счётчика ссылок - оно генерируется             //
                                                                                            //
            uThisRefCount != 0 &&   /*  Нарушена целостность счётчика-ссылок … */ \
                ( RaiseException ( ERROR_ASSERTION_FAILURE, 0, 0, NULL ), 1L ));            //
        }                                                                                   //

        //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //
        
        /*  Реализация оконной процедуры для MessageOnly-окна */                            //
                                                                                            //
        static LRESULT CALLBACK MessageOnlyWndProc ( __in HWND hWnd, \
            __in UINT uMsg, __in WPARAM wParam, __in LPARAM lParam ) {                      //
                                                                                            //
            switch ( uMsg ) {   /*  Тривиальная обработка сообщений */                      //
                                                                                            //
                HANDLE_MSG ( hWnd, WM_CREATE, OnCreateMessageOnlyWnd );     /* CREATE */    //
                HANDLE_MSG ( hWnd, WM_NOTIFYICON, OnNotifyShellIcon );      /* NOTIFY */    //
                HANDLE_MSG ( hWnd, WM_DESTROY, OnDestroyMessageOnlyWnd );  /* DESTROY */    //
            }                                                                               //
                                                                                            //
            return DefWindowProc ( hWnd, uMsg, wParam, lParam );  /* … по-умолчанию … */    //
        }                                                                                   //
        
        //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //

        public :    /*  Открытые свойства и методы класса */                                //
                                                                                            //
        virtual ~TrayNotifyIcon ( __reserved VOID ) {   /*  Деструктор объекта */           //
                                                                                            //
        }                                                                                   //
                                                                                            //
        /*  Исполнение рабочего состояния клиентской части */                               //
                                                                                            //
        __inline static TrayNotifyIcon * CreateInstance \
            ( __in HINSTANCE hInstance, __in FARPROC pfnMsgHandler ) \
            RETURNS ( DYNCREATE_OBJ ( TrayNotifyIcon, hInstance, pfnMsgHandler ));          //
                                                                                            //
        /*  Запуск очереди сообщений для MessageOnly-окна  */                               //
                                                                                            //
        __inline INT_PTR RunMessageOnly ( __has_no_params ) {                               //
                                                                                            //
            MSG msg = { 0 };    /*  Структура, определяющая параметры сообщения */          //
                                                                                            //
            for ( ; IsWindow ( m_hWnd ) && \
                GetMessage ( &msg, NULL, NULL, NULL ); \
                    TranslateMessage ( &msg ), DispatchMessage ( &msg ) );

            return (INT_PTR) msg.wParam;
        }
            
        //  ‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡  //
            
        //  Inline-реализация обязательных методов интерфейса IUnknown                      //
                                                                                            //
        //  IUnknown :: QueryInterface                                                      //
        //  Запрос поддерживаемого объектом COM-интерфейса                                  //
                                                                                            //
        __inline STDMETHOD( QueryInterface )  /*  Поддержка только для IUnknown */ \
             ( __in REFIID iid, __out LPVOID * ppvObj )  /*  … другие - отклоняются */ \
                RETURNS( ppvObj == NULL ? E_POINTER : ( iid == IID_IUnknown ? \
                    ( *ppvObj = ( IUnknown * ) this, this -> AddRef(), S_OK ) : \
                ( *ppvObj = NULL, E_NOINTERFACE )) );                                       //
                                                                                            //
        //  IUnknown :: AddRef                                                              //
        //  Добавление ссылки на объект и увеличение счётчика                               //
                                                                                            //
        __inline STDMETHOD_( ULONG, AddRef ) ( __has_no_params )    /*  ThreadSafe … */ \
            RETURNS( InterlockedIncrement ( &m_cRef ) );   /*  … увеличение счётчика */     //
                                                                                            //
        //  IUnknown :: Release                                                             //
        //  Удаление ссылки на объект и уменьшение счётчика                                 //
                                                                                            //
        __inline STDMETHOD_( ULONG, Release ) ( __has_no_params ) \
            /*  Освобождение памяти при нулевом значении счётчика */
            RETURNS( InterlockedDecrement ( &m_cRef ) ? m_cRef : ( delete this, 0 ) );      //
    };                                                                                      //

    //  ▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪  //

