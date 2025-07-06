

    //  ▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪  //

    #pragma once    //  Include-Guard подключаемого модуля                                  //
                                                                                            //
    #undef WINVER   /*  Принудительное переопределение версии Win API */                    //
    #define WINVER _WIN32_WINNT_VISTA   /*  Windows Vista */                                //
                                                                                            //
    #undef _WIN32_WINNT   /*  Принудительное переопределение версии Native API */           //
    #define _WIN32_WINNT _WIN32_WINNT_VISTA  /*  Windows Vista */                           //
                                                                                            //
    #undef NTDDI_VERSION    /*  Поддержка системных механизмов разработки */                //
    #define NTDDI_VERSION NTDDI_VISTA   /*  Windows Vista */                                //
                                                                                            //
    #undef _WIN32_IE    /*  Принудительное переопределение версии IE */                     //
    #define _WIN32_IE _WIN32_IE_WIN8  /*  Internet Explorer 10.0 */                         //
                                                                                            //
    #include <Unknwn.h>     //  Описание интерфейса IUnknown                                //
    #include <AtlBase.h>    //  Функционал взаимодействия с подсистемой COM                 //
    #include <Windows.h>    //  Функционал Windows API                                      //
    #include <CommCtrl.h>   //  Функционал Common Controls                                  //
    #include <WindowsX.h>   //  Расширенный функционал Windows API                          //
                                                                                            //
    /*  Манифест для загрузки библиотеки Common Controls 6.0 */                             //
    #pragma comment(linker,"/manifestdependency:\"type='win32' " \
        "name='Microsoft.Windows.Common-Controls' version='6.0.0.0' " \
    "processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")           //
                                                                                            //
    #pragma comment(lib, "Comctl32.lib")  /*  Статическая компоновка Common Controls */     //
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
    /*  Распаковщик сообщения «WM_NOTIFYICON» */                                            //
    #define HANDLE_WM_NOTIFYICON( hwnd, wParam, lParam, fn ) \
        ((fn)((hwnd),(WPARAM)(wParam),(LPARAM)(lParam)), 1L)                                //
                                                                                            //
    #define FORWARD_WM_NOTIFYICON( hwnd, notify, guid, lparam, pfn ) \
        (( ( VOID (CALLBACK *) ( HWND, UINT, const GUID&, LPARAM ) ) (pfn) ) \
            ( hwnd, notify, guid, lparam ), 1L )                                            //
                                                                                            //

    //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //

    #define UsingImages( hinst,...) /*  Коллекция изображений для реализации GUI */ \
        TrayNotifyIcon :: ImageList ( hinst, ##__VA_ARGS__, 0L )                            //
                                                                                            //
    //  Класс-обёртка - реализует запуск приложения без его графической визуализации        //
    //  ( создания главного окна приложения ), предоставляет функционал добавления и        //
    //  управления иконкой приложения в системном трее, обеспечивая цикл его «жизни»        //
                                                                                            //
    class TrayNotifyIcon : public IUnknown {    /*  Имплементация IUnknown */               //
                                                                                            //
        public:    /*  Открытые свойства и методы класса */                                 //
                                                                                            //
        struct ImageList {  /*  Коллекция изображений, используемых трей-иконкой */         //
                                                                                            //
            //  Вариадический конструктор ( принимает строковые названия иконок )           //
                                                                                            //
            ImageList ( __in HINSTANCE hInstance = NULL, __in_opt ... ) : \
                hImageList ( NULL )  /*  Изначально список изображений - пуст … */ {        //
                                                                                            //
                va_list args;  /*  Вариадический маркер переданных аргументов */            //
                                                                                            //
                va_start ( args, hInstance );  /*  Начало списка аргументов … */            //
                                                                                            //
                /*  Назначение иконок в хранимый список */ \
                Assign ( hInstance ? hInstance : GetModuleHandle ( NULL ), args );          //
                                                                                            //
                va_end ( args );  /*  Завершение списка аргументов */                       //
            }                                                                               //
                                                                                            //
            ~ImageList ( __has_no_params ) {  /*  Деструктор объектов класса */             //
                                                                                            //
                ImageList_Destroy ( hImageList );  /*  Удаление списка изображений */       //
            }                                                                               //
                                                                                            //
            operator HIMAGELIST () const  /* Оператор приведения типа «HIMAGELIST» */ \
                RETURNS( ImageList_Duplicate ( hImageList ) );  /* Дубликат ! */            //

            //  ··········································································  //

            private:   /*  Закрытые свойства и методы класса */                             //
                                                                                            //
            HIMAGELIST hImageList = NULL;   /*  Построенный список изображений */           //
                                                                                            //
            BOOLEAN Assign  /*  Назначение иконок в список изображений … */ \
                ( __in_opt HINSTANCE hInstance, va_list Args /* Resource VaArgs */ ) {      //
                                                                                            //
                PTSTR pszResourceName = NULL;  /*  Название ресурса иконки */               //
                SIZE Size = {0};    /*  Размер предоставляемых иконок */                    //
                                                                                            //
                /*  Обход названий, переданных через параметры для построения списка … */ \
                for ( HICON hResIcon = NULL; pszResourceName = va_arg( Args, PTSTR ); \
                                                                                            //
                FAILED( LoadIconMetric  /*  Загрузка иконки из ресурсов … */ \
                    ( hInstance, pszResourceName, LIM_SMALL, &hResIcon) ) || \
                                                                                            //
                ( hImageList  /* Если список не существует - он будет создан … */ || \
                                                                                            //
                /*  Параметры списка выбираются из первой иконки … */                       //
                hResIcon && ( Size = GetIconSize ( hResIcon ), 1L ) && \
                                                                                            //
                ( hImageList = ImageList_Create  /* Создание списка хранения … */ \
                    ( Size.cx, Size.cy, ILC_COLOR32 | ILC_HIGHQUALITYSCALE, 0, 1 ) )), \
                                                                                            //
                ImageList_AddIcon ( hImageList, hResIcon )  /* Присоединение иконки … */, \
                                                                                            //
                hResIcon && DestroyIcon ( hResIcon ));  /*  … иконка больше не нужна */     //
                                                                                            //
                va_end( Args );   /*  Завершение работы со списком параметров */            //
                                                                                            //
                return hImageList != NULL;  /*  Признак успешного завершения метода */      //
            }                                                                               //

            //  ··········································································  //

            //  Запрос размерности иконки по её манипулятору                                //
                                                                                            //
            __inline static SIZE GetIconSize ( __in HICON hIcon ) {                         //
                                                                                            //
                ICONINFO ii = {0}; BITMAP bm = {0};  /*  Информационные структуры … */      //
                                                                                            //
                return GetIconInfo ( hIcon, &ii ) &&  /*  Запрос параметров … */ \
                    GetObject ( ii.hbmMask, sizeof(bm), &bm ) == sizeof ( bm ), \
                                                                                            //
                ii.hbmMask && DeleteObject ( ii.hbmMask ), /* Удаление служебных … */ \
                    ii.hbmColor && DeleteObject ( ii.hbmColor ),  /* … полей данных */ \
                                                                                            //
                /*  Возвращаются установленные размеры иконки … */                          //
                SIZE { bm.bmWidth, ii.hbmColor ? bm.bmHeight : bm.bmHeight >> 1 };          //
            }                                                                               //
        };                                                                                  //

        //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //

        //  Построение контекстного меню для значка в области уведомлений                   //
                                                                                            //
        #define CtxMenu(...)  /*  Представление контекстного меню */ \
            TrayNotifyIcon :: PopupMenu( ##__VA_ARGS__ , NULL )                             //
                                                                                            //
        #define SubMenu(...)  /*  Подменю во вложенном контекстном */ \
            TrayNotifyIcon :: PopupMenu( ##__VA_ARGS__ , NULL )                             //

        #define SubMenuWithIcon(icon, ...)  /*  Подменю во вложенном контекстном */ \
            TrayNotifyIcon :: PopupMenu( ##__VA_ARGS__ , NULL ).SetIcon(icon)               //
                                                                                            //
        #define Item(...)  /*  Элемент меню */ \
            TrayNotifyIcon :: MenuItem( ##__VA_ARGS__ )                                     //
                                                                                            //
        struct MenuItem {   /*  Абстракция «Элемент контекстного меню» */                   //
                                                                                            //
            private : /*  Закрытые свойства и методы класса */                              //
                                                                                            //
            PTSTR m_pszText;    /*  Строка представления элемента меню */                   //
            USHORT m_uImage;    /*  Номер картинки из списка IMAGELIST */                   //
            UINT m_uID;         /*  Идентификатор элемента меню */                          //
            UINT m_fState;      /*  Флаги состояния элемента меню */                        //
                                                                                            //
            public :  /*  Открытые свойства и методы класса */                              //
                                                                                            //
            __inline MenuItem ( __in UINT uID, __in USHORT uImage, \
                __in __nullterminated PTSTR pszText, __in_opt UINT fState = 0 ) \
                                                                                            //
                APPLY ( m_pszText = pszText; /* Инициализация полей структуры */ \
                    m_uID = uID; m_uImage = uImage; m_fState = fState );                    //
                                                                                            //
            operator PTSTR () const YIELD( m_pszText );  /*  Строка представления … */      //
            operator USHORT () const YIELD( m_uImage );  /*  Номер картинки из списка */    //
            operator UINT () const YIELD( m_uID );       /*  Идентификатор элемента */      //
            operator DWORD () const YIELD( m_fState );   /*  Флаги состояния элемента */    //
        };                                                                                  //

        //  ··············································································  //
        
        struct PopupMenu {  /*  Абстракция «Контекстное меню» или «Элемент подменю» … */    //
                                                                                            //
            private:  /*  Закрытые свойства и методы класса */                              //
                                                                                            //
            HMENU m_hMenu;      /*  Хэндл родительского меню ( для подменю ) */             //
            PTSTR m_pszName;    /*  Строка представления элемента ( текст ) */              //
            USHORT m_uImage;    /*  Номер картинки из списка IMAGELIST */                   //
                                                                                            //
            public:   /*  Открытые свойства и методы класса */                              //
                                                                                            //
            //  «hVariated» - вариативный параметр, если это «HIMAGELIST» - предполагается  //
            //  создание контекстного меню, если это «PTSTR» - создаётся элемент подменю    //
            //  для данного контекстного, а строка - обозначает его представление           //
                                                                                            //
            PopupMenu( HANDLE hVariated, ... ) :  m_pszName ( NULL ) /*  Название */ , \
                m_uImage ( -1 ) /* Иконка */, m_hMenu ( CreatePopupMenu() ) /* Хэндл */ {   //
                                                                                            //
                va_list Args;   /*  Маркер вариадических параметров конструктора */         //
                va_start ( Args, hVariated );   /*  Инициализация маркера параметров */     //
                                                                                            //
                BOOL fHandleIsImage = FALSE;    /*  Параметр - хэндл списка иконок */       //
                BOOL fHandleIsString = FALSE;   /*  Параметр - строка представления */      //
                                                                                            //
                fHandleIsImage = hVariated == NULL ||  /*  Проверка HIMAGELIST … */ \
                    ImageList_Destroy ( ImageList_Duplicate( (HIMAGELIST) hVariated ));     //
                                                                                            //
                fHandleIsString = !fHandleIsImage &&  /*  Проверка PTSTR … */ \
                    !IsBadStringPtr ( (PTSTR) hVariated, lstrlen( (PTSTR) hVariated ));     //
                                                                                            //
                fHandleIsString &&  /*  Представленная строка - сохраняется как … */ \
                    ( m_pszName = (PTSTR) hVariated );  /* … представление элемента */      //
                                                                                            //
                MENUINFO mi = { sizeof(mi), /* Параметры подложки контекстного меню … */ \
                    MIM_MENUDATA, 0, 0, NULL, 0, (ULONG_PTR) hVariated /* HIMAGELIST */ };  //
                                                                                            //
                fHandleIsImage && SetMenuInfo ( m_hMenu, &mi );  /*  Связывание … */        //
                                                                                            //
                MENUITEMINFO mii = { sizeof(mii), /*  Шаблон состояния элемента меню … */ \
                    MIIM_STRING | MIIM_BITMAP | MIIM_ID | MIIM_FTYPE | MIIM_DATA, \
                    MFT_STRING, 0, 0, NULL, NULL, NULL, 0, NULL, 0, HBMMENU_CALLBACK };     //
                                                                                            //
                MENUINFO dummy = { sizeof(dummy), MIM_STYLE };  /* Для проверки HMENU … */  //
                                                                                            //
                for ( PVOID pVariadic = NULL; /*  Обход вариадических параметров … */ \
                    pVariadic = va_arg( Args, PVOID ) /*  … пока не встречен «NULL» */; \
                                                                                            //
                (( SetLastError ( NOERROR ), 1L ) &&  /* Проверка представления … */ \
                    GetMenuInfo( (HMENU) * (PopupMenu*) pVariadic, &dummy) && \
                GetLastError() != ERROR_INVALID_MENU_HANDLE ) &&  /*  Это HMENU … */ \
                                                                                            //
                ( mii.fMask |= MIIM_SUBMENU, mii.fMask &= ~MIIM_STATE, /* Подменю … */ \
                    mii.dwTypeData = (PTSTR) * (PopupMenu *) pVariadic /* Текст */ , \
                mii.dwItemData = (USHORT) * (PopupMenu *)pVariadic /* Номер иконки */, \
                    mii.hSubMenu = (HMENU) * (PopupMenu *) pVariadic /* Хэндл меню */ , \
                mii.wID = 0  /* Пункты, являющиеся подменю - не имеют ID */, 1L ) || \
                                                                                            //
                ( mii.fMask &= ~MIIM_SUBMENU, mii.fMask |= MIIM_STATE, /* Элемент … */ \
                    mii.wID = (UINT) * (MenuItem *) pVariadic  /* Код элемента */, \
                mii.dwItemData = (USHORT) * (MenuItem *) pVariadic /* Номер иконки */, \
                    mii.dwTypeData = (PTSTR) * (MenuItem *) pVariadic /* Текст … */, \
                mii.fState = (UINT) (DWORD) * (MenuItem *) pVariadic, 1L ), \
                                                                                            //
                mii.cch = lstrlen(mii.dwTypeData)  /* Длина строки представления */, \
                                                                                            //
                InsertMenuItem ( m_hMenu, 0, TRUE, &mii) );  /* Вставка элемента … */       //
                                                                                            //
                va_end( Args );  /*  Конец маркера вариадических параметров */              //
            };                                                                              //

            __inline const PopupMenu & SetIcon ( __in USHORT uImage ) \
                APPLY( m_uImage = uImage; RETURNS(*this) );
                                                                                            //
            operator HMENU () const YIELD( m_hMenu );     /* Хэндл данного меню */          //
            operator PTSTR () const YIELD( m_pszName );   /* Строка его представления */    //
            operator USHORT () const YIELD( m_uImage );   /* Номер картинки из списка */    //
        };                                                                                  //

        //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //

        private:   /*  Закрытые свойства и методы класса */                                 //
                                                                                            //
        static const UINT WM_NOTIFYICON  = WM_USER + 0x100;  /*  ID сообщения */            //
        static const UINT WM_NOTIFYTIMER = WM_USER + 0x101;  /*  ID таймера */              //
                                                                                            //
        LONG m_cRef = 0;    //  Счётчик ссылок на объект класса ( IUnknown :: m_cRef )      //
                                                                                            //
        HINSTANCE m_hInstance;  //  Дескриптор экземпляра приложения                        //
        HANDLE m_hAppMutex;     //  Хендл уникального мьютекса экземпляра                   //
                                                                                            //
        HWND m_hWnd;    //  Дескриптор связанного MessageOnly-окна                          //
        HMENU m_hMenu;  //  Дескриптор контекстного меню TrayNotify-значка                  //
        GUID iconGuid;  //  Идентификатор «главной» иконки приложения                       //
                                                                                            //
        FARPROC m_pfnHandler;   //  Пользовательский обработчик сообщений                   //
                                                                                            //
        CRITICAL_SECTION * m_pSyncObject;   /*  Объект синхронизации доступа */             //

        //  ··············································································  //
        
        /*  Закрытый конструктор объектов класса ( напрямую не используется ) */            //
                                                                                            //
        TrayNotifyIcon ( __in HINSTANCE hInstance, __in FARPROC pfnMsgHandler, \
            __in __nullterminated PTSTR pszUnique, __in const ImageList & Images, \
                __in UINT uIconsDelayTimeout, __in HMENU hContextMenu ) : \
            m_hWnd ( NULL ), m_hMenu ( hContextMenu ), m_hInstance ( hInstance ), \
                /* Обработчик событий значка */ m_pfnHandler ( pfnMsgHandler ) {            //
                                                                                            //
            ( m_pSyncObject = ( CRITICAL_SECTION * ) /*  Объект синхронизации … */ \
                HeapAlloc ( RtlGetProcessHeap ( __no_args ), \
                    HEAP_ZERO_MEMORY, sizeof( CRITICAL_SECTION )) ) && \
                                                                                            //
            ( InitializeCriticalSection ( m_pSyncObject ), 1L )  /*  Инициализация */ ;     //
                                                                                            //
            DLLGETVERSIONPROC DllGetVersion = /*  Для запроса на проверку версии … */ \
                (DLLGETVERSIONPROC) LdrpFindProcedureAddress /* … «ComCtl32.dll» */ \
                    ( LdrpGetModuleHandle ( L"ComCtl32.dll" ), "DllGetVersion" );           //
                                                                                            //
            DLLVERSIONINFO dvi = { sizeof(dvi), 0L, 0L, 0L, 0L };   //  Версия  DLL         //
                                                                                            //
            WNDCLASSEX wc = { 0 };    /*  Структура WinAPI с описанием класса окна */       //
            HICON hBalloonIcon = NULL;  /*  Хэндл значка, для BalloonMessage */             //
            HICON hTipsIcon = NULL;     /*  Хэндл значка, для ToolTips */                   //
                                                                                            //
            SetLastError ( NOERROR );   /*  Сброс состояния последней ошибки */             //
                                                                                            //
            MENUINFO mi = { sizeof(mi), MIM_MENUDATA, 0, 0, NULL, 0, NULL };                //
            GetMenuInfo ( hContextMenu, &mi );  /* Запрос списка иконок для меню */         //
                                                                                            //
            /*  Допускается запуск - только одного единственного экземпляра приложения */   //
            ( m_hAppMutex = CreateMutex ( NULL, TRUE, pszUnique ) /* Проверка … */ ) && \
                GetLastError() != ERROR_ALREADY_EXISTS /* Экземпляр - уникален … */ && \
                                                                                            //
            /*  Запрос используемой приложением версии «ComCtl32.dll» */                    //
            DllGetVersion && SUCCEEDED( DllGetVersion (&dvi) ) &&  /* Windows Vista */ \
                dvi.dwMajorVersion >= SPVER(_WIN32_WINNT_VISTA)  /* … или старше */ && \
                                                                                            //
            /*  Заполнение «тривиальных» полей WNDCLASSEX-структуры … */                    //
            ( wc.cbSize = sizeof ( WNDCLASSEX ), wc.lpfnWndProc = MessageOnlyWndProc, \
                wc.hInstance = m_hInstance, wc.lpszClassName = _T("EasyClient"), \
                    RegisterClassEx ( &wc ) /*  … регистрация класса окна */) && \
                                                                                            //
            SUCCEEDED( CoCreateGuid ( &iconGuid ))  /* Создание идентификатора */ && \
                                                                                            //
            //  Работа клиентского приложения будет осуществляться без полноценной          //
            //  оконной визуализации, но в системном трее - будет создана служебная         //
            //  иконка, отображающая текущее состояние и принимающая команды                //
                                                                                            //
            ( m_hWnd = CreateWindowEx /*  Создание MessageOnly-окна … */ \
                ( 0, wc.lpszClassName, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, \
                    (PVOID) this /*  Экземпляр объекта связывается с окном */ )) && \
                                                                                            //
            ( SetProp ( m_hWnd, /*  Хэндл изображений для контекстного меню */ \
                _T("ContextMenu_ImageList"), (HANDLE) mi.dwMenuData ), 1L ) && \
                                                                                            //
            /*  Загрузка пользовательской иконки, отображаемой в BalloonMessage */          //
            ( SUCCEEDED( LoadIconWithScaleDown ( m_hInstance /* … c масштабированием */, \
                /* Иконка сообщения */ _T("Shell_Notify"), 64, 64, &hBalloonIcon )) && \
                                                                                            //
            /*  Сохранение пользовательской иконки в именованных свойствах  */              //
            SetProp ( m_hWnd, _T("BalloonIcon"), hBalloonIcon ), 1L) && \
                                                                                            //
            /*  Загрузка пользовательской иконки, отображаемой в ToolTips */                //
            ( SUCCEEDED( LoadIconWithScaleDown ( m_hInstance /* … c масштабированием */, \
                /* Иконка уведомления */ _T("Shell_Tips"), 32, 32, &hTipsIcon )) && \
                                                                                            //
            /*  Сохранение пользовательской иконки в именованных свойствах  */              //
            SetProp ( m_hWnd, _T("ToolTipsIcon"), hTipsIcon), 1L) && \
                                                                                            //
            /*  Регистрация события о добавлении главной иконки приложения … */             //
            m_pfnHandler && ( FORWARD_WM_NOTIFYICON( m_hWnd, /*  Создание … */ \
                WM_CREATE, iconGuid, NULL, m_pfnHandler ), 1L ) && \
                                                                                            //
            ( CreateTrayNotifyIcon  /* Добавление «главной» иконки … */ \
                ( iconGuid, Images, uIconsDelayTimeout ));  /*  Как минимум одна … */       //
        }                                                                                   //

        //  ··············································································  //

        //  Трансляция GUID-значения, привязанного к значку системного трея - в индекс      //
        //  представления ( относительно всех зарегистрированных значков )                  //
                                                                                            //
        static UINT TranslateGuid ( __in HWND hWnd, __in const GUID& iconGuid ) {           //
                                                                                            //
            TrayNotifyIcon * pAssocObj = GetWndObjAssoc ( hWnd ); /* Компаньон-объект */    //
                                                                                            //
            TCHAR chParam[32] = { 0 };   /*  Именованный параметр … */                      //
            UINT uIconIdx = 0, uFoundIdx = 0;   /*  Индекс соответствия */                  //
                                                                                            //
            PWSTR pszGuid = NULL, pszGuidToFind = NULL;  /* Для обработки GUID-значений */  //
                                                                                            //
            StringFromIID ( iconGuid, &pszGuidToFind );   /*  Искомый GUID */               //
                                                                                            //
            pAssocObj && pAssocObj -> m_pSyncObject &&  /*  Блок синхронизации … */ \
                ( EnterCriticalSection ( pAssocObj -> m_pSyncObject ), 1L );                //
                                                                                            //
            for ( SetLastError ( NO_ERROR ), uIconIdx = 1  /*  Начало поиска … */ ; \
                pszGuidToFind && GetLastError() != ERROR_FILE_NOT_FOUND; /* Условие … */ \
                                                                                            //
                wnsprintf ( chParam, /*  Хранимый именованный параметр … */ \
                    ARRAYSIZE( chParam ), _T("TrayIcon%d_GUID"), uIconIdx ) > 0 && \
                                                                                            //
                ( pszGuid = (PWSTR) GetProp ( hWnd, chParam ))  /* Строка … */ && \
                                                                                            //
                lstrcmpW ( pszGuid, pszGuidToFind ) == 0  /*  Поиск значения … */ && \
                                                                                            //
                /*  Освобождение выделенной памяти и выход из цикла, либо повтор … */       //
                ( uFoundIdx = uIconIdx, pszGuidToFind = NULL, 1L ) || ++uIconIdx );         //
                                                                                            //
            pAssocObj && pAssocObj -> m_pSyncObject &&  /*  Без синхронизации … */ \
                ( LeaveCriticalSection ( pAssocObj -> m_pSyncObject ), 1L );                //
                                                                                            //
            return uFoundIdx;   /*  Индекс иконки, соответствующей заданному GUID */        //
        }                                                                                   //
        
        //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //
        
        /*  Инициализация MessageOnly-окна при его создании ( WM_CREATE ) */                //
                                                                                            //
        static BOOL OnCreateMessageOnlyWnd \
            ( __in HWND hWnd, __in LPCREATESTRUCT lpCreateInfo ) {                          //
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
                SUCCEEDED( (( TrayNotifyIcon* ) lpCreateInfo -> lpCreateParams ) -> \
                    QueryInterface ( IID_IUnknown, (PVOID*) &pAssocObj ) ) && \
                                                                                            //
                SetWndObjAssoc ( hWnd, pAssocObj ); /*  Установка ассоциации */             //
        }                                                                                   //

        //  ··············································································  //
        
        //   «Обёртка» для перехвата и обработки сообщения «WM_NOTIFY»                      //
                                                                                            //
        static LRESULT OnNotifyMessageOnlyWnd  /*  Только для «TTN_GETDISPINFO» … */ \
            ( __in HWND hWnd, __in INT idFrom, __inout_opt __maybenull NMHDR * pnmhdr ) {   //
                                                                                            //
            TrayNotifyIcon * pAssocObj = GetWndObjAssoc ( hWnd );  /* Компаньон-объект */   //
                                                                                            //
            UINT uID = (UINT) (UINT_PTR) GetProp ( hWnd, _T("ToolTipsID") );  /* Индекс */  //
                                                                                            //
            GUID IconGuid = {0};    /*  GUID-значение для идентификатора иконки … */        //
            PWSTR pszGuid = NULL;   /*  Строковое представление для GUID-значения */        //
                                                                                            //
            TCHAR chParamName[32] = {0};    /*  Название параметра для запроса GUID */      //
                                                                                            //
            wnsprintf ( chParamName,  /*  Представление именованного параметра … */ \
                ARRAYSIZE( chParamName ), _T("TrayIcon%d_GUID"), uID ) > 0 && \
                                                                                            //
            ( pszGuid = (PWSTR) GetProp ( hWnd, chParamName ))  /*  Строка GUID … */ && \
                SUCCEEDED( IIDFromString ( pszGuid, &IconGuid ) );  /*  GUID-значение */    //
                                                                                            //
            pAssocObj && pnmhdr -> code == TTN_GETDISPINFO  /*  Запрос текста … */ && \
                                                                                            //
            FORWARD_WM_NOTIFYICON ( hWnd, /* Проброс на пользовательский обработчик */ \
                TTN_NEEDTEXT, IconGuid, (LPARAM) pnmhdr, pAssocObj -> m_pfnHandler ) && \
                                                                                            //
            ((NMTTDISPINFO *) pnmhdr) -> lParam  /*  Если передан заголовок … */ && \
                                                                                            //
            /*  Он будет вместе с иконкой добавлен к уведомлению … */ \
            SNDMSG( (HWND) GetProp( pAssocObj -> m_hWnd, _T("ToolTipsHandle")), \
                TTM_SETTITLE, (WPARAM) GetProp( pAssocObj -> m_hWnd, \
                    _T("ToolTipsIcon")), (LPARAM) ((NMTTDISPINFO*) pnmhdr) -> lParam );     //
                                                                                            //
            return TRUE;  /*  Признак обработанного сообщения … */                          //
        }                                                                                   //

        //  ··············································································  //

        //  Вызов стороннего обработчика для сообщений трей-иконки ( WM_NOTIFYICON )        //
                                                                                            //
        static VOID OnNotifyShellIcon ( __in HWND hWnd, \
            __in __reserved WPARAM wParam, __in __reserved LPARAM lParam ) {                //
                                                                                            //
            TrayNotifyIcon * pAssocObj = GetWndObjAssoc ( hWnd );  /* Компаньон-объект */   //
                                                                                            //
            struct {  /*  Представление параметров сообщения ( NOTIFYICON_VERSION_4 ) */    //
                                                                                            //
                USHORT uNotify;  //  Идентификатор уведомления ( NIN_*, WM_* )              //
                USHORT uID;      //  16-битный индекс зарегистрированного значка            //
                                                                                            //
                SHORT x, y;      //  (X,Y) экранные координаты мыши                         //
                                                                                            //
                GUID iconGuid;   //  GUID # XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX */         //
            }                                                                               //
              Params_V4 = { LOWORD(lParam), /*  Представление параметров … */ \
                HIWORD( lParam ), GET_X_LPARAM( wParam ), GET_Y_LPARAM( wParam ), {0} };    //
                                                                                            //
            TCHAR chParamName[32] = { 0 };  /*  Представление именованных параметров */     //
                                                                                            //
            PWSTR pszGuid = NULL;  /*  Строковое представление GUID-идентификатора */       //
                                                                                            //
            pAssocObj && (  /*  Проверка доступности компаньон-объекта … */ \
                                                                                            //
                ( pAssocObj -> m_pSyncObject &&  /*  Начало блока синхронизации … */ \
                    ( EnterCriticalSection ( pAssocObj -> m_pSyncObject ), 1L ), \
                                                                                            //
                wnsprintf ( chParamName, ARRAYSIZE( chParamName ), /* Именованный */ \
                    _T("TrayIcon%d_GUID"), Params_V4.uID ) > 0 &&     /* параметр */ \
                                                                                            //
                /*  Запрос именованного параметра и представление GUID-значения … */        //
                ( pszGuid = (PWSTR) GetProp ( pAssocObj -> m_hWnd, chParamName ) ) && \
                    IIDFromString ( (LPCOLESTR) pszGuid, (LPIID) &Params_V4.iconGuid ), \
                                                                                            //
                pAssocObj -> m_pSyncObject &&  /*  Завершение блока синхронизации … */ \
                    ( LeaveCriticalSection ( pAssocObj -> m_pSyncObject ), 1L ) ), \
                                                                                            //
                pAssocObj -> m_pfnHandler &&  /*  Проверка доступности обработчика … */ \
                                                                                            //
                ( Params_V4.uNotify == WM_CONTEXTMENU  /*  Вызов контекстного меню … */ && \
                    pAssocObj -> m_hMenu != NULL  /*  … если меню уже создано … */ && \
                                                                                            //
                ( OnMessageOnlyContextMenu ( hWnd,  /*  Стандартное открытие меню … */ \
                    pAssocObj -> m_hMenu, Params_V4.x, Params_V4.y ), 1L ) || \
                                                                                            //
                /*  В остальных ситуациях - вызов пользовательского обработчика … */ \
                ( FORWARD_WM_NOTIFYICON ( hWnd, Params_V4.uNotify, Params_V4.iconGuid, \
                    MAKELPARAM( Params_V4.x, Params_V4.y ), pAssocObj -> m_pfnHandler )) )  //
            );                                                                              //
        }                                                                                   //

        //  ··············································································  //

        //  Стандартное раскрытие всплывающего контекстного меню для значка                 //
                                                                                            //
        __inline static VOID OnMessageOnlyContextMenu \
            ( __in HWND hWnd, __in HMENU hContextMenu, __in UINT xPos, __in UINT yPos ) {   //
                                                                                            //
            SetForegroundWindow ( hWnd ),  /*  Отображение Popup-меню … */ \
                TrackPopupMenu ( hContextMenu, 0, xPos, yPos, 0, hWnd, NULL );              //
        }                                                                                   //

        //  ··············································································  //

        //  Запрос размерности элементов для осуществления OwnerDraw-операций               //
                                                                                            //
        static VOID OnMessageOnlyMeasure \
            ( __in HWND hWnd, __in __refparam MEASUREITEMSTRUCT * lpMeasureItem ) {         //
                                                                                            //
            HANDLE hImageList = NULL;     /*  Список иконок контекстного меню */            //
            INT cxIcon = 0, cyIcon = 0;   /*  Размерности иконок - ширина, высота */        //
                                                                                            //
            /*  Проверка и запрос списка иконок контекстного меню … */ \
            lpMeasureItem && lpMeasureItem -> CtlType == ODT_MENU /* Меню … */ && \
                ( hImageList = GetProp ( hWnd, _T("ContextMenu_ImageList") )) && \
                                                                                            //
            /*  Запрос размерности иконок для пунктов контекстного меню */ \
            ImageList_GetIconSize ( (HIMAGELIST) hImageList, &cxIcon, &cyIcon) && \
                                                                                            //
            //  Размерности иконок выбираются с учётом возможного Check-маркера             //
                                                                                            //
            ( lpMeasureItem -> itemWidth = /* Дополнение к ширине иконки */ \
                    cxIcon - GetSystemMetrics ( SM_CXMENUCHECK ) + 6, \
                                                                                            //
                lpMeasureItem -> itemHeight = cyIcon + 2 /* Точная высота иконки */ );      //
        }                                                                                   //

        //  ··············································································  //

        //  Непосредственное осуществление OwnerDraw-операций                               //
                                                                                            //
        static VOID OnMessageOnlyDraw ( __in HWND hWnd, \
            __in __refparam const DRAWITEMSTRUCT * lpDrawItem ) {                           //
                                                                                            //
            HANDLE hImageList = NULL;     /*  Список иконок контекстного меню */            //
            HICON hDrawIcon = NULL;       /*  Хэндл отображаемой иконки */                  //
            INT cxIcon = 0, cyIcon = 0;   /*  Размерности иконок - ширина, высота */        //
                                                                                            //
            /*  Проверка и запрос списка иконок контекстного меню … */ \
            lpDrawItem && lpDrawItem -> CtlType == ODT_MENU && \
                !( lpDrawItem -> itemState & ODS_CHECKED ) /* … без Check-маркера */ && \
                    ( hImageList = GetProp ( hWnd, _T("ContextMenu_ImageList") )) && \
                                                                                            //
            /*  Запрос хэндла актуальной иконки для прорисовки пункта меню */ \
            ( hDrawIcon = ImageList_GetIcon ( (HIMAGELIST) hImageList, \
                (INT) lpDrawItem -> itemData, ILD_IMAGE | ILD_TRANSPARENT ) ) && \
                                                                                            //
            /*  Получение размерности отображаемой иконки ( ширина и высота ) … */ \
            ( ImageList_GetIconSize ( (HIMAGELIST) hImageList, &cxIcon, &cyIcon) ) && \
                                                                                            //
            /*  Прорисовка иконки в активном или Disabled-состоянии … */ \
            DrawState ( lpDrawItem -> hDC, (HBRUSH) ( COLOR_GRAYTEXT + 1 ), \
                NULL, (LPARAM) hDrawIcon, NULL, 2, 2, cxIcon, cyIcon, DST_ICON | \
                    ( lpDrawItem -> itemState & ODS_DISABLED ? DSS_MONO : DSS_NORMAL ));    //
        }                                                                                   //

        //  ··············································································  //

        //  Ротация изображений для значка TrayNotify по таймеру ( WM_TIMER )               //
                                                                                            //
        static VOID OnMessageOnlyTimer ( __in HWND hWnd, __in UINT uTrayIndex ) {           //
                                                                                            //
            TrayNotifyIcon * pAssocObj = GetWndObjAssoc ( hWnd );  /* Компаньон-объект */   //
                                                                                            //
            pAssocObj && pAssocObj -> m_pSyncObject &&  /*  Блок синхронизации … */ \
                ( EnterCriticalSection ( pAssocObj -> m_pSyncObject ), 1L );                //
                                                                                            //
            TCHAR chParam[32] = {0};    /*  Именованный параметр для запросов */            //
                                                                                            //
            PWSTR pszGuid = NULL;    /*  Строковое представление Guid-идентификатора */     //
            GUID iconGuid = { 0 };   /*  GUID-значение идентификатора TrayIcon */           //
                                                                                            //
            HIMAGELIST hImageList = 0;  /*  Коллекция изображений TrayIcon-значка */        //
            UINT uIndexValue = -1;   /*  Индекс изображения отображаемого на значке */      //
            UINT uImageCount = 0;    /*  Количество изображений в коллекции значка */       //
                                                                                            //
            wnsprintf ( chParam, /*  Строковое представление GUID-идентификатора … */ \
                ARRAYSIZE( chParam ), _T("TrayIcon%d_GUID"), uTrayIndex ) > 0 && \
                                                                                            //
            /*  Запрос GUID-значения для идентификатора TrayNotify-иконки */ \
            ( pszGuid = (PWSTR) GetProp ( hWnd, chParam )) && \
                SUCCEEDED( IIDFromString( pszGuid, &iconGuid) );                            //
                                                                                            //
            wnsprintf ( chParam, /*  Именованный параметр для коллекции … */ \
                ARRAYSIZE( chParam ), _T("TrayIcon%d_ImageList"), uTrayIndex ) > 0 && \
                                                                                            //
            /*  Запрос коллекции изображений для текущего TrayIcon-значка */ \
            ( hImageList = (HIMAGELIST) GetProp ( hWnd, chParam )) && \
                ( uImageCount = ImageList_GetImageCount ( hImageList ) );                   //
                                                                                            //
            wnsprintf ( chParam, /*  Именованный параметр для индекса изображения … */ \
                ARRAYSIZE( chParam ), _T("TrayIcon%d_Index"), uTrayIndex ) > 0 && \
                                                                                            //
            /*  Запрос индекса изображения для текущего TrayIcon-значка */ \
            ( uIndexValue = (UINT)(UINT_PTR) GetProp ( pAssocObj -> m_hWnd, chParam ));     //
                                                                                            //
            NOTIFYICONDATA nid = { 0 };   /*  Параметры изменяемой иконки */                //
                                                                                            //
            nid.cbSize = NOTIFYICONDATA_V3_SIZE;    //  Системный маркер размера            //
                                                                                            //
            nid.uFlags = NIF_GUID | NIF_ICON;  /*  Валидация полей ( GUID и иконка ) */     //
            nid.hWnd = hWnd;                /*  Хэндл MessageOnly окна-владельца */         //
            nid.guidItem = iconGuid;    /*  GUID-идентификатор иконки */                    //
                                                                                            //
            ( nid.hIcon = /*  Запрос актуального изображения из списка */ \
                ImageList_GetIcon ( hImageList, uIndexValue, ILD_TRANSPARENT )) && \
                  Shell_NotifyIcon ( NIM_MODIFY, &nid );  /*  Установка значка */           //
                                                                                            //
            nid.hIcon && DestroyIcon ( nid.hIcon );  /* Уничтожение созданной иконки */     //
                                                                                            //
            uImageCount && ( ++uIndexValue %= uImageCount );  /* Ротация изображения … */   //
                                                                                            //
            /*  Ротация изображения ( сдвиг индекса изображения по таймеру ) */ \
            SetProp ( pAssocObj -> m_hWnd, chParam, (HANDLE) (UINT_PTR) uIndexValue );      //
                                                                                            //
            pAssocObj && pAssocObj -> m_pSyncObject &&  /*  Выход из блока … */ \
                ( LeaveCriticalSection ( pAssocObj -> m_pSyncObject ), 1L );                //
        }

        //  ··············································································  //

        /*  Очистка ресурсов окна при его уничтожени ( WM_DESTROY ) */                      //
                                                                                            //
        static VOID OnDestroyMessageOnlyWnd ( __in HWND hWnd ) {                            //
                                                                                            //
            TrayNotifyIcon * pAssocObj = NULL;   /*  Ассоциированный объект */              //
            ULONG uThisRefCount = -1;   /*  Счётчик ссылок при уничтожении объекта */       //
                                                                                            //
            ( pAssocObj = GetWndObjAssoc ( hWnd ) ) &&   /*  Запрос указателя */  \
                                                                                            //
            //  Вызов «PostQuitMessage» - обеспечит завершение очереди сообщений окна;      //
            //  последующий вызов « IUnknown :: Release » - освободит удерживаемую ссылку,  //
            //  что приведёт к вызову деструктора компаньон-объекта                         //
                                                                                            //
            /*  Установка кода завершения для очереди сообщений MessageOnly-окна */         //
            ( PostQuitMessage ( (INT) (INT_PTR) GetProp ( hWnd, _T("LoopExitCode") )), \
                uThisRefCount = pAssocObj -> Release()  /*  Освобождение ссылки */, \
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
        static LRESULT CALLBACK MessageOnlyWndProc \
            ( __in HWND hWnd, __in UINT uMsg, __in WPARAM wParam, __in LPARAM lParam ) {    //
                                                                                            //
            switch ( uMsg ) {   /*  Тривиальная обработка сообщений */                      //
                                                                                            //
                HANDLE_MSG ( hWnd, WM_CREATE, OnCreateMessageOnlyWnd );       /* CREATE */  //
                HANDLE_MSG ( hWnd, WM_NOTIFYICON, OnNotifyShellIcon );   /* NOTIFY ICON */  //
                HANDLE_MSG ( hWnd, WM_NOTIFY, OnNotifyMessageOnlyWnd );       /* NOTIFY */  //
                HANDLE_MSG ( hWnd, WM_MEASUREITEM, OnMessageOnlyMeasure );   /* MEASURE */  //
                HANDLE_MSG ( hWnd, WM_DRAWITEM, OnMessageOnlyDraw );            /* DRAW */  //
                HANDLE_MSG ( hWnd, WM_TIMER, OnMessageOnlyTimer );             /* TIMER */  //
                HANDLE_MSG ( hWnd, WM_DESTROY, OnDestroyMessageOnlyWnd );    /* DESTROY */  //
            }                                                                               //
                                                                                            //
            return DefWindowProc ( hWnd, uMsg, wParam, lParam );  /* … по-умолчанию … */    //
        }                                                                                   //
        
        //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //

        public :    /*  Открытые свойства и методы класса */                                //
                                                                                            //
        virtual ~TrayNotifyIcon ( __has_no_params ) {   /*  Деструктор объекта */           //
                                                                                            //
            /*  Регистрация события об уничтожении главной иконки приложения … */           //
            m_pfnHandler && ( FORWARD_WM_NOTIFYICON ( m_hWnd, /*  Удаление … */ \
                WM_DESTROY, iconGuid, NULL, m_pfnHandler ), 1L);                            //
                                                                                            //
            //  При уничтожении MessageOnly-окна - необходимо удалить все созданные им      //
            //  иконки TrayNotify + освободить все задействованные под них ресурсы          //
                                                                                            //
            PTSTR pszPropNames[] = { _T("BalloonIcon"), _T("ToolTipsIcon") };               //
                                                                                            //
            for ( UINT i=0; i < ARRAYSIZE(pszPropNames);  /*  Удаление свойств … */ \
                DestroyIcon ( (HICON) GetProp ( m_hWnd, pszPropNames[i] )), \
                    RemoveProp ( m_hWnd, pszPropNames[i] ), i++);                           //
                                                                                            //
            HIMAGELIST hCtxMenuImgList =  /* Список иконок контекстного меню */ \
                (HIMAGELIST) GetProp ( m_hWnd, _T("ContextMenu_ImageList") );               //
                                                                                            //
            /*  Уничтожение списка иконок контекстного меню … */ \
            hCtxMenuImgList && ImageList_Destroy ( hCtxMenuImgList ) && \
                RemoveProp ( m_hWnd, _T("ContextMenu_ImageList") );                         //
                                                                                            //
            UINT uIconIdx = 0;          //  Индекс поиска добавленных иконок                //
            TCHAR chParam[32] = {0};    //  Представление именованного параметра            //
            PWSTR pszIconGuid = NULL;   //  Строковое представление GUID-иконки             //
            GUID IconGuid = {0};        //  GUID-значение идентификатора иконки             //
                                                                                            //
            //  Удаление всех Tray-значков, ассоциированных с данным MessageOnly-окном      //
                                                                                            //
            for ( SetLastError ( NO_ERROR ), uIconIdx = 1; /* Начало с первой иконки … */   //
                                                                                            //
                wnsprintf ( chParam,  /*  Строка GUID-идентификатора значка … */ \
                    ARRAYSIZE( chParam ), _T("TrayIcon%d_GUID"), uIconIdx ) > 0 && \
                                                                                            //
                /*  GUID-значение идентификатора значка, полученное из строки … */ \
                m_hWnd && ( pszIconGuid = (PWSTR) GetProp ( m_hWnd, chParam ) ) && \
                    SUCCEEDED ( IIDFromString ( pszIconGuid, &IconGuid )) && \
                                                                                            //
                /* … будут освобождены - память строки GUID и список изображений … */ \
                RemoveTrayNotifyIcon ( IconGuid )  /* Запрос на удаление значка … */, \
                                                                                            //
                GetLastError() != ERROR_FILE_NOT_FOUND /* Обход всех */ ; uIconIdx++ ) {    //
                                                                                            //
                /*  Удаление добавленных «свойств», шаг необязательный, но полезный … */    //
                                                                                            //
                RemoveProp ( m_hWnd, chParam );  /*  Удаление «TrayIcon%d_GUID» */          //
                                                                                            //
                wnsprintf ( chParam,  /*  Хранимый хендл коллекции изображений … */ \
                    ARRAYSIZE( chParam ), _T("TrayIcon%d_ImageList"), uIconIdx) > 0 && \
                                                                                            //
                RemoveProp ( m_hWnd, chParam );  /*  Удаление «TrayIcon%d_ImageList» */     //
                                                                                            //
                wnsprintf ( chParam,  /*  Индекс отображаемого значка … */ \
                    ARRAYSIZE( chParam ), _T("TrayIcon%d_Index"), uIconIdx) > 0 && \
                                                                                            //
                RemoveProp ( m_hWnd, chParam );  /*  Удаление «TrayIcon%d_Index» */         //
            }                                                                               //
                                                                                            //
            m_hAppMutex &&  /*  Закрытие мьютекса экземпляра … */ \
                CloseHandle ( m_hAppMutex );  /*  … если он был успешно открыт */           //
                                                                                            //
            //  Удаление свойств, связанных с работой MessageOnly-окна                      //
                                                                                            //
            RemoveProp ( m_hWnd, _T("ToolTipsHandle") ), /* Хэндл подложки … */ \
            RemoveProp ( m_hWnd, _T("ToolTipsID") ),  /*  ID уведомления … */ \
            RemoveProp ( m_hWnd, _T("TrayItemsCounter") );  /*  Счётчик иконок … */         //
                                                                                            //
            m_pSyncObject &&  /*  Удаление объекта синхронизации … */ \
                HeapFree ( RtlGetProcessHeap ( __no_args ), 0, m_pSyncObject );             //
        }                                                                                   //
        
        //  ··············································································  //
        
        //  Подготовка рабочего состояния MessageOnly-окна с добавлением «главного»         //
        //  значка приложения + назначение обработчика событий этого значка                 //
                                                                                            //
        __inline static TrayNotifyIcon * CreateInstance \
            ( __in HINSTANCE hInstance, __in __nullterminated PTSTR pszUnique, \
                __in FARPROC pfnMsgHandler, __in const ImageList & Images, \
            __in UINT uIconsDelayTimeout, __in HMENU hIconContextMenu = NULL ) \
                                                                                            //
            /*  Добавление «главной» иконки приложения … */ \
            RETURNS ( DYNCREATE_OBJ ( TrayNotifyIcon, hInstance, pfnMsgHandler, \
                pszUnique, Images, uIconsDelayTimeout, hIconContextMenu ));                 //
        
        //  ··············································································  //
        
        /*  Создание SystemTray-иконки для MessageOnly-окна  */                             //
                                                                                            //
        __inline BOOL CreateTrayNotifyIcon ( __in const GUID & iconGuid, \
            __in const ImageList & Images, __in USHORT uDelayTimeout ) {                    //
                                                                                            //
            NOTIFYICONDATA nid = { 0 };  /*  Параметры создаваемой иконки */                //
                                                                                            //
            nid.cbSize = NOTIFYICONDATA_V3_SIZE;    //  Системный маркер размера            //
            nid.uVersion = NOTIFYICON_VERSION_4;    //  Формат обработки обращений          //
                                                                                            //
            nid.uFlags = NIF_MESSAGE | NIF_GUID | NIF_TIP;  /*  Значимые поля … */          //
                                                                                            //
            m_pSyncObject &&  /*  Начало блока синхронизации … */ \
                ( EnterCriticalSection ( m_pSyncObject ), 1L );                             //
                                                                                            //
            /*  Индекс по количеству зарегистрированных за приложением иконок */            //
            nid.uID = (USHORT) (UINT_PTR) GetProp ( m_hWnd, _T("TrayItemsCounter")) + 1;    //
                                                                                            //
            m_pSyncObject &&  /*  Завершение блока синхронизации … */ \
                ( LeaveCriticalSection ( m_pSyncObject ), 1L );                             //
                                                                                            //
            nid.hWnd = m_hWnd;          /*  Хэндл MessageOnly окна-владельца */             //
            nid.guidItem = iconGuid;    /*  GUID-идентификатор иконки */                    //
                                                                                            //
            nid.uCallbackMessage = WM_NOTIFYICON;   /*  Идентификатор уведомлений */        //
                                                                                            //
            lstrcpy( nid.szTip, _T("DummyText"));  /*  Для Tips-уведомлений … */            //
                                                                                            //
            LPWSTR pwszGuid = NULL;     /*  Строковое представление GUID */                 //
            TCHAR chParam[32] = {0};    /*  Именованный параметр … */                       //
                                                                                            //
            /*  Представление именованного свойства с описанием GUID-идентификатора */      //
            wnsprintf ( chParam, ARRAYSIZE( chParam ), _T("TrayIcon%d_GUID"), nid.uID );    //
                                                                                            //
            /*  Попытка добавления иконки в системный трей … */ \
            return Shell_NotifyIcon ( NIM_ADD, &nid ) &&  /* При успешном добавлении … */ \
                                                                                            //
                ( m_pSyncObject &&  /*  Начало блока синхронизации … */ \
                    ( EnterCriticalSection ( m_pSyncObject ), 1L ), 1L ) && \
                                                                                            //
                ( SetProp ( m_hWnd, /*  Регистрация счётчика индексов … */                  //
                    _T("TrayItemsCounter"), (HANDLE) (ULONG_PTR) nid.uID ), \
                                                                                            //
                /*  Регистрация GUID-идентификатора добавленной иконки … */ \
                ( SUCCEEDED( StringFromIID ( nid.guidItem, &pwszGuid )) && \
                    SetProp( m_hWnd, chParam, (VOID*) pwszGuid ), 1L ), 1L ) && \
                                                                                            //
                ( m_pSyncObject &&  /*  Завершение блока синхронизации … */ \
                    ( LeaveCriticalSection ( m_pSyncObject ), 1L ), 1L ) && \
                                                                                            //
                /*  Установка версии соглашения о предоставлении уведомлений */             //
                Shell_NotifyIcon ( NIM_SETVERSION, &nid /* V4 */ ) && \
                                                                                            //
                /* Замена изображений на значке TrayNotify … */                             //
                TrayNotifyIconChangeImages ( iconGuid, Images, uDelayTimeout );             //
        }                                                                                   //

        //  ··············································································  //

        /*  «Переназначение» списка изображений, связанных со значком в трее */             //
                                                                                            //
        __inline BOOL TrayNotifyIconChangeImages ( __in const GUID & iconGuid, \
            __in HIMAGELIST hImgIconsList, __in USHORT uDelayTimeout ) {                    //
                                                                                            //
            NOTIFYICONDATA nid = { 0 };   /*  Параметры изменяемой иконки */                //
                                                                                            //
            nid.cbSize = NOTIFYICONDATA_V3_SIZE;    //  Системный маркер размера            //
                                                                                            //
            nid.uFlags = NIF_GUID | NIF_ICON;   /*  Валидация полей ( GUID и иконка ) */    //
            nid.hWnd = m_hWnd;          /*  Хэндл MessageOnly окна-владельца */             //
            nid.guidItem = iconGuid;    /*  GUID-идентификатор иконки */                    //
                                                                                            //
            /*  Как минимум будет выполнена попытка поставить иконку с индексом #0 */       //
            nid.hIcon = ImageList_GetIcon ( hImgIconsList, 0, ILD_TRANSPARENT );            //
                                                                                            //
            nid.uID = TranslateGuid ( m_hWnd, iconGuid ); /*  Соответствие GUID <-> ID */   //
                                                                                            //
            m_pSyncObject &&  /*  Начало блока синхронизации … */ \
                ( EnterCriticalSection ( m_pSyncObject ), 1L );                             //
                                                                                            //
            TCHAR chParam[32] = {0};  /*  Представление именованного параметра */           //
                                                                                            //
            /*  Попытка установки изображения для значка системного трея … */ \
            BOOL fResult = nid.hIcon && Shell_NotifyIcon ( NIM_MODIFY, &nid ) && \
                                                                                            //
                /*  Сброс  ранее установленного таймера ротации изображений … */
                nid.uID > 0 && ( KillTimer ( m_hWnd, nid.uID ), 1L ) && ( \
                                                                                            //
                wnsprintf ( chParam, /* Именованный параметр для хендла списка … */ \
                    ARRAYSIZE( chParam ), _T("TrayIcon%d_ImageList"), nid.uID ) > 0 && \
                                                                                            //
                /*  «Старый» список изображений - уничтожается … */ \
                ImageList_Destroy ( (HIMAGELIST) GetProp ( m_hWnd, chParam ) ), \
                    /*  … и далее - заменяется «новым» ( пользовательским ) … */ \
                    SetProp ( m_hWnd, chParam, hImgIconsList ), 1L ) && \
                                                                                            //
                wnsprintf ( chParam, /*  Именованный параметр для индекса … */ \
                    ARRAYSIZE( chParam ), _T("TrayIcon%d_Index"), nid.uID ) > 0 && \
                                                                                            //
                /*  Назначение нового индекса текущей ( отображаемой ) иконки */ \
                ( SetProp ( m_hWnd, chParam, 0 ), 1L )  /*  … отсчёт от нуля */ && \
                                                                                            //
                /*  Назначение нового используемого списка изображений */ \
                ImageList_GetImageCount ( hImgIconsList ) > 0 && \
                    SetTimer ( m_hWnd, nid.uID, uDelayTimeout, NULL );  /* Новый … */       //
                                                                                            //
            m_pSyncObject &&  /*  Завершение блока синхронизации … */ \
                ( LeaveCriticalSection ( m_pSyncObject ), 1L );                             //
                                                                                            //
            nid.hIcon && DestroyIcon ( nid.hIcon );  /* Уничтожение созданной иконки */     //
                                                                                            //
            return fResult; /*  Результирующее значение вызова */                           //
        }                                                                                   //

        //  ··············································································  //

        //  Отображение уведомления в стиле сообщений Windows 10                            //
                                                                                            //
        __inline BOOL TrayNotifyIconMessage \
            ( __in const GUID& iconGuid, __in __nullterminated PTSTR pszTitle, \
                __in __nullterminated PTSTR pszMessage ) {                                  //
                                                                                            //
            NOTIFYICONDATA nid = { 0 };  /*  Параметры управления иконкой */                //
                                                                                            //
            nid.cbSize = NOTIFYICONDATA_V3_SIZE;    //  Системный маркер размера            //
                                                                                            //
            nid.uFlags = NIF_GUID | NIF_INFO;  /*  Валидация полей структуры */             //
            nid.dwInfoFlags = NIIF_USER | NIIF_LARGE_ICON | NIIF_NOSOUND;                   //
                                                                                            //
            nid.guidItem = iconGuid;    /*  GUID-идентификатор иконки */                    //
                                                                                            //
            /*  Пользовательскя иконка, отображаемая в уведомлении */                       //
            nid.hBalloonIcon = (HICON) GetProp ( m_hWnd, _T("BalloonIcon") );               //
                                                                                            //
            lstrcpy ( nid.szInfo, pszMessage );     /*  Текст уведомления … */              //
            lstrcpy ( nid.szInfoTitle, pszTitle );  /*  Заголовок уведомления */            //
                                                                                            //
            return Shell_NotifyIcon ( NIM_MODIFY, &nid );  /* Отображение уведомления */    //
        }                                                                                   //

        //  ··············································································  //
        
        //  Отображение  «пузырька» уведомлений ( в стиле Windows XP )                      //
                                                                                            //
        BOOL TrayNotifyIconBalloonShow \
            ( __in const GUID & iconGuid, __in __maybenull PTSTR pszTipTitle, \
                __in __maybenull PTSTR pszTipMessage, __in_opt BOOLEAN fSelfClose = 0 ) {   //
                                                                                            //
            EnterCriticalSection ( m_pSyncObject );  /*  Синхронизация доступа … */         //
                                                                                            //
            /*  Хэндл окна уведомления ( «пузырь» уведомления - будет только один ) */      //
            HWND hWndToolTips = (HWND) GetProp ( m_hWnd, _T("ToolTipsHandle") );            //
                                                                                            //
            /*  Закрытие существующего «пузырька» уведомлений … */                          //
            hWndToolTips && TrayNotifyIconBalloonHide ( __no_args );                        //
                                                                                            //
            TOOLINFO ti = {0};  /*  Настройка «пузырька» уведомлений */                     //
            ti.cbSize = sizeof(ti), ti.hwnd = m_hWnd, ti.uId = (UINT_PTR) m_hWnd;           //
                                                                                            //
            NOTIFYICONIDENTIFIER niid = { sizeof(niid), /* NOTIFYICONIDENTIFIER */\
                m_hWnd, TranslateGuid ( m_hWnd, iconGuid ), iconGuid};  /* Tray ID */       //
                                                                                            //
            Shell_NotifyIconGetRect ( &niid, &ti.rect );  /* Прямоугольник размещения … */  //
            SetProp ( m_hWnd, _T("ToolTipsID"), (HANDLE) (UINT_PTR) niid.uID );  /* ID */   //
                                                                                            //
            /*  Создание окна-подложки для «пузырька» уведомлений … */                      //
            ( hWndToolTips = CreateWindowEx ( WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL, \
                WS_POPUP | TTS_NOPREFIX | TTS_BALLOON | ( fSelfClose ? TTS_CLOSE : 0 ), \
                    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, \
                        NULL, (HMENU) NULL, m_hInstance, NULL) ) && \
                                                                                            //
            /*  Установка хранимого свойства ( хэндл «пузырька» уведомлений … )*/           //
            ( SetProp ( m_hWnd, _T("ToolTipsHandle"), hWndToolTips ), \
                                                                                            //
            pszTipTitle && /*  Если строка заголовка - задана пользователем … */ \
                SNDMSG ( hWndToolTips, TTM_SETTITLE, /*  … назначение заголовка */ \
                (WPARAM) GetProp ( m_hWnd, _T("ToolTipsIcon") ), (LPARAM) pszTipTitle ), \
                                                                                            //
            /*  Служебные флаги и текст сообщения «пузырька» */                             //
            ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS | TTF_TRANSPARENT | TTF_TRACK, \
                                                                                            //
            /*  Если текст не указан - определяется Callback-вызов для его зароса */        //
            ti.lpszText = pszTipMessage ? pszTipMessage : LPSTR_TEXTCALLBACK, \
                                                                                            //
            /*  Регистрация и отображение «пузырька» уведомлений … */                       //
            SNDMSG ( hWndToolTips, TTM_ADDTOOL, 0, (LPARAM) &ti), \
                                                                                            //
                SNDMSG ( hWndToolTips, TTM_TRACKPOSITION, 0,  /*  Перенос позиции … */\
                    MAKELPARAM( ( ti.rect.left + ti.rect.right ) >> 1, ti.rect.top )), \
                                                                                            //
                /*  Отображение «пузырька» уведомления в заданной позиции … */              //
                SNDMSG ( hWndToolTips, TTM_TRACKACTIVATE, 1, (LPARAM) &ti ) );              //
                                                                                            //
            LeaveCriticalSection ( m_pSyncObject );  /* Выход из блока синхронизации … */   //
                                                                                            //
            return hWndToolTips != NULL;  /*  Признак успешного выполнения операции */      //
        }                                                                                   //

        //  ··············································································  //

        //  Уничтожение отображаемого уведомления в ( BalloonToolTip )                      //
                                                                                            //
        __inline BOOL TrayNotifyIconBalloonHide ( __has_no_params ) {                       //
                                                                                            //
            EnterCriticalSection ( m_pSyncObject );  /*  Синхронизация доступа … */         //
                                                                                            //
            /*  Хэндл отображаемого уведомления ( если оно существует ) */                  //
            HWND hWndToolTips = (HWND) GetProp ( m_hWnd, _T("ToolTipsHandle") );            //
                                                                                            //
            TOOLINFO ti = {0};  /*  Настройка параметров … */                               //
            ti.cbSize = sizeof(ti), ti.hwnd = m_hWnd, ti.uId = (UINT_PTR) m_hWnd;           //
                                                                                            //
            BOOL fResult = !IsWindow ( hWndToolTips ) ? FALSE : \
                                                                                            //
                /*  Проверка хэндла «подложки» уведомления + его закрытие … */ \
                ( SNDMSG (hWndToolTips, TTM_TRACKACTIVATE, 0, (LPARAM) &ti), \
                                                                                            //
                SetProp ( m_hWnd, _T("ToolTipsHandle"), NULL ), /* «Забой» свойств … */ \
                    SetProp ( m_hWnd, _T("ToolTipsID"), 0 ), TRUE );                        //
                                                                                            //
            LeaveCriticalSection ( m_pSyncObject ); /*  Выход из блока синхронизации … */   //
                                                                                            //
            return fResult;  /*  Результат выполнения … */                                  //
        }                                                                                   //
        
        //  ··············································································  //

        /*  Удаление указанной SystemTray-иконки для MessageOnly-окна  */                   //
                                                                                            //
        __inline BOOL RemoveTrayNotifyIcon ( __in const GUID& iconGuid ) {                  //
                                                                                            //
            NOTIFYICONDATA nid = { 0 };   /*  Параметры удаляемой иконки */                 //
                                                                                            //
            nid.cbSize = NOTIFYICONDATA_V3_SIZE;    //  Системный маркер размера            //
                                                                                            //
            nid.uFlags = NIF_GUID;      /*  Валидация полей ( только GUID) */               //
            nid.hWnd = m_hWnd;          /*  Хэндл MessageOnly окна-владельца */             //
            nid.guidItem = iconGuid;    /*  GUID-идентификатор иконки */                    //
                                                                                            //
            /*  Попытка удаления иконки из системного трея … */ \
            BOOL fResult = Shell_NotifyIcon ( NIM_DELETE, &nid );                           //
                                                                                            //
            //  После удаления иконки - необходимо освободить память, выделенную под        //
            //  хранение строкового представления GUID, осуществляется поиск строки         //
                                                                                            //
            TCHAR chParam[32] = { 0 };  /*  Именованный параметр … */                       //
            PWSTR pszGuid = NULL;   /*  Строка для освобождения памяти */                   //
            HIMAGELIST hImageList = NULL;   /*  Список изображений для удаления */
                                                                                            //
            ( nid.uID = TranslateGuid ( m_hWnd, iconGuid ) /* Поиск индекса */ ) > 0 && \
                                                                                            //
                ( wnsprintf ( chParam, /*  Хранимый именованный параметр … */ \
                    ARRAYSIZE( chParam ), _T("TrayIcon%d_GUID"), nid.uID ) > 0 && \
                                                                                            //
            ( pszGuid = (PWSTR) GetProp ( m_hWnd, chParam ) ) /* Захват строки … */ && \
                ( CoTaskMemFree ( pszGuid ), 1L ) /* Освобождение блока памяти */ && \
                                                                                            //
            SetProp ( m_hWnd, chParam, NULL ),  /*  «Забой» значения параметра … */ \
                                                                                            //
                wnsprintf ( chParam, /*  Хранимый именованный параметр … */ \
                    ARRAYSIZE( chParam ), _T("TrayIcon%d_ImageList"), nid.uID ) > 0 && \
                                                                                            //
            ( hImageList = (HIMAGELIST) GetProp ( m_hWnd, chParam ) ) /* Список … */ && \
                ( ImageList_Destroy ( hImageList ), 1L) /* Уничтожение списка … */ && \
                                                                                            //
            SetProp ( m_hWnd, chParam, NULL ));  /* «Забой» значения параметра … */ \
                                                                                            //
            return fResult; /*  Результирующее значение вызова */                           //
        }                                                                                   //

        //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //

        //  Запуск очереди сообщений MessageOnly-окна                                       //
                                                                                            //
        __inline INT RunMessageOnly ( __has_no_params ) {                                   //
                                                                                            //
            MSG msg = { 0 };    /*  Структура, определяющая параметры сообщения */          //
                                                                                            //
            for ( ; IsWindow ( m_hWnd ) &&  /*  Успешное создание окна … */ \
                GetMessage ( &msg, NULL, NULL, NULL );  /*  Обработка сообщений … */ \
                    TranslateMessage ( &msg ), DispatchMessage ( &msg ) );                  //
                                                                                            //
            return (INT) msg.wParam;    /*  Установленное возвращаемое значение */          //
        }                                                                                   //

        //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //

        //  Завершение очереди сообщений MessageOnly-окна                                   //
                                                                                            //
        __inline VOID DestroyMessageOnly ( __in INT nExitCode ) {                           //
                                                                                            //
            /*  Установка переданного кода завершения для MessageOnly-окна */               //
            SetProp ( m_hWnd, _T("LoopExitCode"), (HANDLE) (INT_PTR) nExitCode );           //
            DestroyWindow ( m_hWnd );   /*  Завершение очереди сообщений */                 //
        }                                                                                   //
            
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

