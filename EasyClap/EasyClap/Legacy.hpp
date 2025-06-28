

    //  ▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪  //

    #pragma once    //  Include-Guard подключаемого модуля                                  //
                                                                                            //
    #include "Auxiliary.h"  //  Вспомогательные конструкции препроцессора                   //
                                                                                            //
    //  Реализация системных функций в контексте LDRP API                                   //
    //  «Library Load & Reference Processing»                                               //
                                                                                            //
    #include <WTypes.h>     //  Базовые типы среды WinAPI                                   //
    #include <TChar.h>      //  Представление ANSI/UNICODE строк                            //
                                                                                            //
    //  Смещения, используемые в относительной адресации для TEB / PEB                      //
                                                                                            //
    #ifdef _WIN64   /*  Для 64-разрядных систем */                                          //
                                                                                            //
        #define PcPeb       0x60    //  TEB -> ProcessEnvironmentBlock                      //
        #define PcLdrLock   0x110   //  PEB -> LoaderLock                                   //
        #define PcLdrData   0x18    //  PEB -> LdrData                                      //
        #define PcProcHeap  0x30    //  PEB -> ProcessHeap                                  //
                                                                                            //
    #else   /*  Для 32-разрядных систем */                                                  //
                                                                                            //
        #define PcPeb       0x30    //  TEB -> ProcessEnvironmentBlock                      //
        #define PcLdrLock   0xA0    //  PEB -> LoaderLock                                   //
        #define PcLdrData   0x0C    //  PEB -> LdrData                                      //
        #define PcProcHeap  0x18    //  PEB -> ProcessHeap                                  //
                                                                                            //
    #endif                                                                                  //
                                                                                            //
    /*  Строка Native API */                                                                //
                                                                                            //
    #pragma pack(8)                 //  Выравнивание полей структуры 2 байта                //
    typedef struct _NT_STRING {     //  Unicode-строка в представлении Native API           //
                                                                                            //
        USHORT Length, MaximumLength;   //  Актуальная / максимальная длина строки          //
                                                                                            //
        __field_bcount_part(MaximumLength, Length) \
            __nullterminated WCHAR * Buffer;        //  Символьный буфер строки             //
    }                                                                                       //
      NT_STRING, * PNT_STRING;      //  Псевдоним имени типа                                //
                                                                                            //
    #pragma pack()                                                                          //
                                                                                            //
    /*  Таблица модулей, инициализируемая загрузчиком */                                    //
                                                                                            //
    #pragma pack(4)                         //  Выравнивание полей структуры 4 байта        //
    typedef struct _PEB_LDR_DATATABLE {     //  Таблица данных о модулях                    //
                                                                                            //
        ULONG Length;           //  Общий размер таблицы данных загрузчика ( байт )         //
        BOOLEAN fInitialized;   //  Признак инициализации таблицы                           //
        PVOID Reserved;         //  Зарезервировано системой                                //
                                                                                            //
        //  Указатели начала списков с информацией о загруженных модулях                    //
                                                                                            //
        LIST_ENTRY InLoadOrderModuleList;       //  По очерёдности загрузки                 //
        LIST_ENTRY InMemoryOrderModuleList;     //  По очерёдности размещения               //
        LIST_ENTRY InInitOrderModuleList;       //  По очерёдности инициалиизации           //
    }                                                                                       //
      PEB_LDR_DATA, * PPEB_LDR_DATA;                                                        //
                                                                                            //
    /*  Элемент таблицы загрузчика */                                                       //
                                                                                            //
    #pragma pack(2)                          //  Выравнивание полей структуры 2 байта       //
    typedef struct _LDR_DATATABLE_ENTRY {    //  Информация о загруженном модуле            //
                                                                                            //
        //  Указатели списков модулей, загруженных в адресное пространство процесса         //
                                                                                            //
        LIST_ENTRY InLoadOrderModuleLinks;      //  По очерёдности загрузки                 //
        LIST_ENTRY InMemoryOrderModuleLinks;    //  По очерёдности размещения               //
        LIST_ENTRY InInitOrderModuleLinks;      //  По очерёдности инициалиизации           //
                                                                                            //
        PVOID BaseAddress;          //  Базовый адрес загрузки модуля                       //
        PVOID EntryPoint;           //  Точка входа данного модуля                          //
        ULONG_PTR SizeOfImage;      //  Объём памяти, занимаемой образом                    //
                                                                                            //
        NT_STRING FullModuleName;   //  Полный путь к модулю                                //
        NT_STRING BaseModuleName;   //  Имя файла модуля                                    //
    }                                                                                       //
      LDR_MODULE, * PLDR_MODULE;                                                            //
                                                                                            //
    #pragma pack()                                                                          //
                                                                                            //
    #define OFFSET_PTR(from, with_offset)  /*  Взятие указателя со смещением … */ \
        ( (UINT_PTR) (from) + (UINT_PTR) (with_offset) )  /* … Win32 / Win64 */             //
                                                                                            //
    #define TAKE_PTR(addr)  /*  Получение хранимого по указаному адресу значения … */ \
        ( (UINT_PTR) ( *(UINT_PTR*) ( (UINT_PTR)(addr) )) )  /* … HANDLE / POINTER */       //
    
    //  ‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡  //
    
    //  Объект синхронизации, используемый загрузчиком модулей                              //
                                                                                            //
    __inline RTL_CRITICAL_SECTION * LdrpPebLoaderLock ( __has_no_params ) {                 //
                                                                                            //
        UINT_PTR pPeb = TAKE_PTR( OFFSET_PTR( NtCurrentTeb(), PcPeb ));                     //
        UINT_PTR pLoaderLock = TAKE_PTR( OFFSET_PTR( pPeb, PcLdrLock ));                    //
                                                                                            //
        return reinterpret_cast< RTL_CRITICAL_SECTION * > ( pLoaderLock );                  //
                                                                                            //
        //  Альтернативная алгебра указателей для Win32                                     //
                                                                                            //
        //  __asm   mov eax, fs:[30h]           //  PEB ( Process Environment Block )       //
        //  __asm   mov eax, ds:[eax + 0A0h]    //  LoaderLock                              //
        //                                                                                  //
        //  … или - __readfsdword( 0x30 ) + 0xA0                                            //
                                                                                            //
        //  … для Win64                                                                     //
        //                                                                                  //
        //  __asm   mov rax, gs:[60h]           //  PEB ( Process Environment Block )       //
        //  __asm   mov rax, ds:[rax + 110h]    //  LoaderLock                              //
        //                                                                                  //
        //  … или - __readgsqword( 0x60 ) + 0x110                                           //
    }                                                                                       //
    
    //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //
    
    //  Возвращает указатель pLdrData из структуры PEB текущего процесса                    //
                                                                                            //
    __inline struct _PEB_LDR_DATATABLE * LdrpPebLdrData ( __has_no_params ) {               //
                                                                                            //
        UINT_PTR pPeb = TAKE_PTR( OFFSET_PTR( NtCurrentTeb(), PcPeb ));                     //
        UINT_PTR pLdrData = TAKE_PTR( OFFSET_PTR( pPeb, PcLdrData ));                       //
                                                                                            //
        return reinterpret_cast< _PEB_LDR_DATATABLE * > ( pLdrData );                       //
                                                                                            //
        //  __asm   mov eax, fs:[30h]           //  PEB ( Process Environment Block )       //
        //  __asm   mov eax, ds:[eax + 0Ch]     //  pLdrData                                //
        //                                                                                  //
        //  … или - __readfsdword( 0x30 ) + 0x0C                                            //
                                                                                            //
        //  … для Win64                                                                     //
        //                                                                                  //
        //  __asm   mov rax, gs:[60h]           //  PEB ( Process Environment Block )       //
        //  __asm   mov rax, ds:[rax + 18h]     //  pLdrData                                //
        //                                                                                  //
        //  … или - __readgsqword( 0x60 ) + 0x18                                            //
    }                                                                                       //
    
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙ //
    
                                                                                            //
    //  Очерёдность представления элементов в списках модулей из pLdrData                   //
                                                                                            //
    //  LoadOrder   - модули в порядке их загрузки в адресное пространство процесса         //
    //  MemoryOrder - модули в порядке возрастания их базовых адресов загрузки              //
    //  InitOrder   - модули в порядке их инициализации по зависимостям                     //
                                                                                            //
    //  ( InitOrder / LoadOrder / MemoryOrder )                                             //
                                                                                            //
    #undef LoadOrder        //  LoadOrder :     pLdrData -> InInitOrderModuleList           //
    #undef MemoryOrder      //  MemoryOrder :   pLdrData -> InMemoryOrderModuleList         //
    #undef InitOrder        //  InitOrder :     pLdrData -> InLoadOrderModuleList           //
                                                                                            //
    //  Обобщённые макросы доступа к элементам связного списка ( LIST_ENTRY )               //
                                                                                            //
    #define LIST_ENTRY( Entry, Field, Type )    /*  Текущий элемент */ \
        ( (Type)((BYTE *)(Entry) - (DWORD_PTR)( &((Type) 0) -> Field )) )                   //
                                                                                            //
    #define ENTRY_FLINK(Entry,Order,Scope)  /*  Следующий элемент */ \
        ( Entry -> In##Order##Module##Scope##.Flink )                                       //
                                                                                            //
    //  Доступ к элементам LDR_MODULE из связных списков PEB_LDR_DATA                       //
                                                                                            //
    //  Первый модуль из заданного списка загрузчика                                        //
                                                                                            //
    #define LDR_FIRST_MODULE( LdrTable, Order ) /*  LdrTable - > LdrModule */   \
        LIST_ENTRY( LdrTable -> In##Order##ModuleList.Flink, \
            In##Order##ModuleLinks, PLDR_MODULE )                                           //
                                                                                            //
    //  Следующий модуль из заданного списка загрузчика                                     //
                                                                                            //
    #define LDR_NEXT_MODULE( LdrModule, Order ) /*  LdrModule - > LdrModule.Flink */ \
        LIST_ENTRY( LdrModule -> In##Order##ModuleLinks.Flink, \
            In##Order##ModuleLinks, PLDR_MODULE )                                           //
                                                                                            //
    //  Последний модуль из заданного списка загрузчика                                     //
                                                                                            //
    #define LDR_LAST_MODULE( LdrTable, LdrModule, Order ) \
        /*  LdrTable.Flink == LdrModule.Flink */ \
        ( ENTRY_FLINK(LdrTable,Order,List) == ENTRY_FLINK(LdrModule,Order,Links) )          //
                                                                                            //
    //  Примитивы синхронизации для потокобезопасного обращения к данным загрузчика         //
                                                                                            //
    #define LdrLockLoaderLock()   /*  Блокировка таблицы загрузчика */ \
        EnterCriticalSection ( LdrpPebLoaderLock() );                                       //
                                                                                            //
    #define LdrUnlockLoaderLock()   /*  Разблокирование таблицы загрузчика */ \
        LeaveCriticalSection ( LdrpPebLoaderLock() );                                       //
                                                                                            //
    //  Перечисление всех модулей в заданном списке таблицы загрузчика, не является         //
    //  потокобезопасной операцией, т.к. не использует примитивы синхронизации доступа      //
    //  ( LdrLockLoaderLock / LdrUnlockLoaderLock ) при обращении к таблице                 //
                                                                                            //
    #define LDR_ENUM_DATATABLE_ENTRIES( Order ) \
        _PEB_LDR_DATATABLE * Table = LdrpPebLdrData();  /*  Таблица загрузчика */ \
        for ( LDR_MODULE * Entry = LDR_FIRST_MODULE( Table, Order ); /*  Перечисление */ \
            Entry -> BaseAddress && !LDR_LAST_MODULE( Table, Entry, Order ); \
                Entry = LDR_NEXT_MODULE( Entry, Order ) )                                   //
                                                                                            //
    //  Информационные атрибуты, используемые в перечислении модулей таблицы загрузчика     //
                                                                                            //
    #define IMAGE_BASE      ( Entry -> BaseAddress )    /*  Базовый адрес модуля */         //
    #define IMAGE_SIZE      ( Entry -> SizeOfImage )    /*  Размер образа модуля */         //
                                                                                            //
    #define IMAGE_BORDER    /*  Базовый адрес + размер образа */ \
        ( (PVOID)( (LONG_PTR) IMAGE_BASE + IMAGE_SIZE ) )   /*  Граница образа модуля */    //
                                                                                            //
    #define ENTRY_POINT     ( Entry -> EntryPoint )     /*  Точка входа модуля */           //
                                                                                            //
    #define MODULE_NAME     ( Entry -> BaseModuleName.Buffer )  /*  Базовое имя */          //
    #define MODULE_PATH     ( Entry -> FullModuleName.Buffer )  /*  Полный путь */          //
                                                                                            //
    //  Осуществляет поиск в таблице модулей, загруженных в адресное пространство           //
    //  текущего процесса, и возвращает указатель (LDR_MODULE *) на запись с информацией    //
    //  о нём, если установлено соответствие заданным информационным атрибутам, является    //
    //  потокобезопасной операцией                                                          //
                                                                                            //
    #define LDR_FIND_DATATABLE_ENTRY( OutPtr, Order, From, Clause ) { \
        _PEB_LDR_DATATABLE * Table = LdrpPebLdrData(); LDR_MODULE * Beg = From; \
            OutPtr = NULL; LdrLockLoaderLock(); /* Блокировка данных загрузчика */ \
        for ( LDR_MODULE * Entry = ( !Beg ? LDR_FIRST_MODULE( Table, Order ) : Beg ); \
            Entry -> BaseAddress && !LDR_LAST_MODULE( Table, Entry, Order ) && \
        !( (Clause) && ( OutPtr = Entry ) ); Entry = LDR_NEXT_MODULE( Entry, Order )); \
            LdrUnlockLoaderLock();  /*  Разблокирование данных загрузчика */ }              //
    
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙ //
    
    //  Осуществляет обход таблицы модулей, загруженных в адресное пространство             //
    //  текущего процесса, и возвращает указатель (LDR_MODULE *) на запись с информацией    //
    //  о нём, если установлено соответствие указателя адресному пространству модуля,       //
    //  или же NULL, если соответствие не определено                                        //
                                                                                            //
    __inline __success(return != NULL) \
        LDR_MODULE * NTAPI LdrFindEntryForAddress ( __in __valid PVOID pvAddress ) {        //
                                                                                            //
        LDR_MODULE * pLdrEntry = NULL;  /*  Указатель искомой записи LDR_MODULE */          // 
                                                                                            //
        LDR_FIND_DATATABLE_ENTRY( pLdrEntry, LoadOrder, /*  Поиск записи */ \
            NULL, pvAddress >= IMAGE_BASE && pvAddress <= IMAGE_BORDER );                   //
                                                                                            //
        return pLdrEntry;   /*  Указатель соответствия для записи LDR_MODULE */             //
    }                                                                                       //
    
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙ //
    
    //  Потокобезопасный обход таблицы модулей, загруженных в адресное пространство         //
    //  текущего процесса, для поиска записи (LDR_MODULE *) соответствующей заданному       //
    //  имени модуля, или же NULL, если соответствие не определено                          //
                                                                                            //
    __inline __success(return != NULL) LDR_MODULE * NTAPI LdrFindEntryForName \
        ( __in __readonly __nullterminated PCWSTR pszModuleName ) {                         //
                                                                                            //
        LDR_MODULE * pLdrEntry = NULL;  /*  Указатель искомой записи LDR_MODULE */          // 
                                                                                            //
        LDR_FIND_DATATABLE_ENTRY( pLdrEntry, LoadOrder, /*  Поиск записи */ \
            NULL, lstrcmpiW ( MODULE_NAME , pszModuleName ) == 0 );                         //
                                                                                            //
        return pLdrEntry;   /*  Указатель соответствия для записи LDR_MODULE */             //
    }                                                                                       //
    
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙ //
    
    //  Запрос имени ранее загруженного модуля по ссылке на его адресное пространство       //
                                                                                            //
    __inline __success(return != NULL) \
        PCWSTR NTAPI LdrpGetModuleNameByAddress ( __in __valid PVOID pvAddress ) {          //
                                                                                            //
        /*  Поиск соответствующей записи LDR_MODULE в таблице загрузчика */                 //
        LDR_MODULE * pLdrEntry = LdrFindEntryForAddress ( pvAddress );                      //
                                                                                            //
        /*  Возвращается указатель символьного буфера с именем модуля */                    //
        return pLdrEntry ? pLdrEntry -> BaseModuleName.Buffer : NULL;                       //
    }                                                                                       //
    
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙ //
    
    //  Запрос хэндла экземпляра для загруженного модуля по его имени                       //
                                                                                            //
    __inline __success(return != NULL) \
        __typefix(PIMAGE_DOS_HEADER) HINSTANCE NTAPI LdrpGetModuleHandle \
            ( __in __readonly __nullterminated PCWSTR pszModuleName ) {                     //
                                                                                            //
        /*  Поиск соответствующей записи LDR_MODULE в таблице загрузчика */                 //
        LDR_MODULE * pLdrEntry = LdrFindEntryForName ( pszModuleName );                     //
                                                                                            //
        /*  Возвращается базовый адрес загрузки искомого модуля */                          //
        return pLdrEntry ? (HINSTANCE) pLdrEntry -> BaseAddress : NULL;                     //
    }                                                                                       //
    
    //  ÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷  //
    
    //  Приведение относительного адреса ( смещения, заданного в байтах относительно        //
    //  базы загруженного модуля, RVA - Relative Virtual Address ) к линейному адресу,      //
    //  определяющему указатель в виртуальном адресном пространстве процесса                //
                                                                                            //
    #define RvaToVa( base, rva )  /*  Смещение относительно базы */ \
        ( (VOID*) ( (BYTE *) (DWORD_PTR)(base) + (DWORD_PTR)(rva) ) )                       //
                                                                                            //
    //  Типизированая версия макроса «RvaToVa»                                              //
                                                                                            //
    #define VA_FROM_RVA( Type, mod_base, rva ) \
        ( (Type) RvaToVa( mod_base, (rva) ) )                                               //
        
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙ //
    
    //  Получение линейного адреса ( указателя ) на PE-заголовок (IMAGE_NT_HEADERS)         //
    //  для модуля, загруженного в адресное пространство данного процесса по указанному     //
    //  базовому адресу загрузки                                                            //
                                                                                            //
    __inline __success(return != NULL) PIMAGE_NT_HEADERS NTAPI LdrpImageNtHeader \
        ( __in __valid __readonly __typefix(PIMAGE_DOS_HEADER) PVOID pModuleBase ) {        //
                                                                                            //
        return (PIMAGE_NT_HEADERS) ( pModuleBase && /*  Проверка указателя */ \
            !IsBadReadPtr( pModuleBase, sizeof ( IMAGE_DOS_HEADER ) ) && \
                                                                                            //
        /*  Проверка MZ-сигнатуры, определяющей заголовок «IMAGE_DOS_HEADER» */ \
        PIMAGE_DOS_HEADER( pModuleBase ) -> e_magic == IMAGE_DOS_SIGNATURE && \
                                                                                            //
        /*  Проверка выравнивания значения на границу двойного слова для заданного */       //
        /*  RAW-смещения, определяющего расположение заголовка «IMAGE_NT_HEADERS» */        //
        /*  относительно заданного базового адреса загрузки модуля */                       //
                                                                                            //
        PIMAGE_DOS_HEADER( pModuleBase ) -> e_lfanew % sizeof(DWORD) == 0 && \
                                                                                            //
        /*  Проверка PE-сигнатуры искомого заголовока «IMAGE_NT_HEADERS» */ \
        PIMAGE_NT_HEADERS( RvaToVa( pModuleBase, \
            PIMAGE_DOS_HEADER( pModuleBase ) -> e_lfanew ) ) -> Signature == \
                IMAGE_NT_SIGNATURE  /*  Совпадение PE-сигнатуры */ ? \
                                                                                            //
        /*  Получение указателя по заданному RAW-смещению относительно базы */ \
        RvaToVa( pModuleBase, PIMAGE_DOS_HEADER( pModuleBase ) -> e_lfanew ) : \
            /*  Значение, обозначающее ошибку при чтении заголовков */ NULL );              //
    }                                                                                       //
    
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙ //
    
    //  Получение линейного адреса для информационной директории, обозначаемой заданным     //
    //  индексом - для PE-образа, определяемого указанным заголовком «IMAGE_NT_HEADERS»     //
                                                                                            //
    __inline __success(return != NULL) \
        PIMAGE_DATA_DIRECTORY NTAPI LdrpImageDirectoryEntry \
        ( __in __valid __readonly PIMAGE_NT_HEADERS pImageNtHdr, \
            /* IMAGE_DIRECTORY_ENTRY_EXPORT … IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR */       //
                __in __valid __in_range(0, 14) WORD wDirEntryIdx ) {                        //
                                                                                            //
        #ifdef _WIN64   /*  Реализация зависит от разрядности приложения */                 //
                                                                                            //
        return pImageNtHdr && /*  Проверка корректности переданного указателя */ \
            !IsBadReadPtr( pImageNtHdr, sizeof ( IMAGE_NT_HEADERS ) ) && \
                                                                                            //
            /*  Проверка сигнатуры опционального заголовка */ \
            PIMAGE_NT_HEADERS( pImageNtHdr ) -> /*  Для 32-разрядного приложения */ \
                OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC && \
                                                                                            //
            /*  Проверка разрядности компоновки приложения */ \
            PIMAGE_NT_HEADERS( pImageNtHdr ) -> FileHeader.Characteristics & \
                ( IMAGE_FILE_MACHINE_IA64 | IMAGE_FILE_MACHINE_ALPHA64 | \
                    IMAGE_FILE_MACHINE_AMD64 ) ? PIMAGE_NT_HEADERS( pImageNtHdr ) -> \
                    /*  Линейный указатель, или NULL - как признак ошибки */ \
                    OptionalHeader.DataDirectory + wDirEntryIdx : NULL;                     //
                                                                                            //
        #else   /*  Реализация для 32-х разрядных систем */                                 //
                                                                                            //
        return pImageNtHdr && /*  Проверка корректности переданного указателя */ \
            !IsBadReadPtr( pImageNtHdr, sizeof ( IMAGE_NT_HEADERS ) ) && \
                                                                                            //
            /*  Проверка сигнатуры опционального заголовка */ \
            PIMAGE_NT_HEADERS( pImageNtHdr ) -> /*  Для 32-разрядного приложения */ \
                OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC && \
                                                                                            //
            /*  Проверка разрядности компоновки приложения */ \
            PIMAGE_NT_HEADERS( pImageNtHdr ) -> FileHeader.Characteristics & \
                IMAGE_FILE_32BIT_MACHINE ? PIMAGE_NT_HEADERS( pImageNtHdr ) -> \
                    /*  Линейный указатель, или NULL - как признак ошибки */ \
                    OptionalHeader.DataDirectory + wDirEntryIdx : NULL;                     //
                                                                                            //
        #endif                                                                              //
    }                                                                                       //
    
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙ //
    
    //  Получение линейного адреса заголовка каталога данных, обозначенного индексом        //
                                                                                            //
    __inline __success(return != NULL) PVOID NTAPI LdrpImageDirectoryEntryToData \
        ( __in __valid __readonly PIMAGE_NT_HEADERS pImageNtHdr, \
            /* IMAGE_DIRECTORY_ENTRY_EXPORT … IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR */       //
                __valid __in_range(0, 14) __in_range(0, 14)__in WORD wDirEntryIdx ) {       //
                                                                                            //
        register PIMAGE_DATA_DIRECTORY pImageDataDir =  /*  Информационная директория */ \
            LdrpImageDirectoryEntry ( pImageNtHdr, wDirEntryIdx );
                                                                                            //
        return pImageDataDir  /*  Проверка допустимости указателя */ && \
            !IsBadReadPtr( pImageDataDir, sizeof ( IMAGE_DATA_DIRECTORY ) ) && \
                                                                                            //
            wDirEntryIdx < /*  Проверка существования запрошенной секции */ \
                PIMAGE_NT_HEADERS( pImageNtHdr ) -> OptionalHeader.NumberOfRvaAndSizes && \
                                                                                            //
            /*  Проверка того, что секция содержит данные */ \
            pImageDataDir -> Size && pImageDataDir -> VirtualAddress ? \
                RvaToVa ( PIMAGE_NT_HEADERS( pImageNtHdr ) -> \
                    OptionalHeader.ImageBase, pImageDataDir -> VirtualAddress ) : NULL;     //
    }                                                                                       //
    
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙ //
    
    //  Непосредственное обращение к данным секции экспорта загруженного образа             //
                                                                                            //
    #define LdrpImageExportDirectory( hModBase ) \
        ( ( PIMAGE_EXPORT_DIRECTORY ) LdrpImageDirectoryEntryToData \
            ( LdrpImageNtHeader ( hModBase ), IMAGE_DIRECTORY_ENTRY_EXPORT ) )              //
                                                                                            //
    //  Макросы для обращения к полям структуры, описывающей каталог экспорта               //
                                                                                            //
    #define EXP_COUNT( exp )  /*  Количество символов экспорта */ \
        ( (INT) exp -> NumberOfNames )                                                      //
                                                                                            //
    //  Получение линейного адреса массива Ordinal-значений, применяемых для индексации     //
    //  в массивах RVA-значений адресов и имён экспортируемых символов                      //
                                                                                            //
    #define EXP_ORDINALS( mod_base, exp ) \
        ( (WORD *) RvaToVa ( mod_base, exp -> AddressOfNameOrdinals ) )                     //
                                                                                            //
    //  Получение линейного адреса массива RVA-значений для адресов экспортируемых          //
    //  символов ( индексация массива осуществляется через Ordinal-индекс )                 //
                                                                                            //
    #define EXP_FUNCTIONS_RVA( mod_base, exp ) \
        ( (DWORD * ) RvaToVa ( mod_base, exp -> AddressOfFunctions ) )                      //
                                                                                            //
    //  Получение линейного адреса массива RVA-значений для имён экспортируемых             //
    //  символов ( индексация массива осуществляется через Ordinal-индекс )                 //
                                                                                            //
    #define EXP_SYMBOLS_RVA( mod_base, exp ) \
        ( (DWORD *) RvaToVa ( mod_base, exp -> AddressOfNames ) )                           //    
            
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙ //

    __inline INT NTAPI _lstrcmp /* Посимвольное (лексическое) сравнение ANSI строк */ \
        (__in __nullterminated PCSTR _S1, __in __nullterminated PCSTR _S2) {                //
                                                                                            //
        INT ret = 0; for (; !( ret = *(CHAR *) _S1 - *(CHAR *) _S2 ) \
            && *_S2; ++_S1, ++_S2 ); return ret;                                            //
    }                                                                                       //

    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙ //
    
    //  Вспомогательный макрос, осуществляющий поиск экспортируемого модулем символа        //
    //  по его ASCIIZ-имени, предполагается, что список экспорта модуля отсортирован        //
    //  в алфавитном порядке, использует метод бинарного поиска                             //
                                                                                            //
    #define LDR_FIND_EXPORT_SYMBOL_BY_NAME( out_ptr, mod_base, name_ascii ) { \
        /*  Запрос информации каталога экспорта для заданного модуля */ \
        PIMAGE_EXPORT_DIRECTORY _Exp = LdrpImageExportDirectory ( mod_base ); \
        WORD * _PO = NULL /*  Массив Ordinal-индексов экспортируемых символов */; \
        DWORD * _PNRv = NULL /* RVA их имён … */, * _PFRv = NULL /*  RVA их адресов … */; \
        _Exp && ( _PO = EXP_ORDINALS( mod_base, _Exp ) /*  Ordinals */, \
            _PNRv = EXP_SYMBOLS_RVA( mod_base, _Exp )  /*  RVA-имён */, \
        _PFRv = EXP_FUNCTIONS_RVA( mod_base, _Exp ) /*  RVA-адресов */ ); \
            out_ptr = NULL; LdrLockLoaderLock(); /* Блокировка данных загрузчика */ \
        for ( INT _L = 0, _R = _Exp ? ~- EXP_COUNT( _Exp ) : ~0 /*  Границы поиска … */, \
            _M = 0, _X = _PFRv && _PNRv && _PO ? ~0 : 0 /*  Проверка наличия экспорта */; \
        _L <= _R && _X && !out_ptr; /* Условие завершения поиска */ \
            _M = ( ( _L ^ _R ) >> 1 ) + ( _L & _R ) /*  Индекс сравнения */, \
        /*  Лексическое сравнение ASCIIZ-имени экспортируемого символа */ \
        _X = _lstrcmp ( VA_FROM_RVA( PCSTR, mod_base, _PNRv[ _M ] ), name_ascii ), \
            _X > 0 && ( _R = ~-_M ) || _X < 0 && ( _L = -~_M ) || /*  Выбор границ … */ \
        ( out_ptr = VA_FROM_RVA( FARPROC, mod_base, _PFRv[ *(_PO + _M) ] )) ); \
            LdrUnlockLoaderLock();  /*  Разблокирование данных загрузчика */ }              //

    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙ //
    
    //  Осуществляет поиск символа, экспортируемого модулем, загруженным в адресное         //
    //  пространство текущего процесса, возвращает NULL - как признак ошибки                //
                                                                                            //
    __inline __success(return != NULL) FARPROC NTAPI LdrpFindProcedureAddress \
        ( __in __valid __typefix(PIMAGE_DOS_HEADER) HMODULE hModuleBase, \
            __in __nullterminated __readonly PCSTR pszProcName ) {                          //
                                                                                            //
        FARPROC pfnExportProc = NULL;   /*  Возвращаемое значение */                        //
                                                                                            //
        /*  Осуществляется поиск экспортируемого модулем символа */                         //
        LDR_FIND_EXPORT_SYMBOL_BY_NAME( pfnExportProc, hModuleBase, pszProcName );          //
                                                                                            //
        return pfnExportProc;   /*  Возвращается найденный указатель */                     //
    }                                                                                       //

    //  ÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷  //

    //  Указатель кучи памяти, принадлежащей процессу                                       //
                                                                                            //
    __inline HANDLE RtlGetProcessHeap ( __has_no_params ) {                                 //
                                                                                            //
        UINT_PTR pPeb = TAKE_PTR( OFFSET_PTR( NtCurrentTeb(), PcPeb ));                     //
        UINT_PTR hProcessHeap = TAKE_PTR( OFFSET_PTR( pPeb, PcProcHeap ));                  //
                                                                                            //
        return reinterpret_cast< HANDLE > ( hProcessHeap );                                 //
                                                                                            //
        //  __asm   mov eax, fs:[30h]           //  PEB ( Process Environment Block )       //
        //  __asm   mov eax, ds:[eax + 18h]     //  ProcessHeap                             //
        //                                                                                  //
        //  … или - __readfsdword( 0x30 ) + 0x18                                            //
                                                                                            //
        //  … для Win64                                                                     //
        //                                                                                  //
        //  __asm   mov rax, gs:[60h]           //  PEB ( Process Environment Block )       //
        //  __asm   mov rax, ds:[rax + 30h]     //  ProcessHeap                             //
        //                                                                                  //
        //  … или - __readgsqword( 0x60 ) + 0x30                                            //
    }                                                                                       //

    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙ //

    #ifndef _WIN64                                                                          //
                                                                                            //
    //  Осуществление вызова через указатель - в соглашении STDCALL, в котором действует    //
    //  большинство разработанных API для 32-х битных ОС Windows                            //
                                                                                            //
    //  Этот подход справедлив только для 32-х битных систем и только STDCALL-вызовов,      //
    //  попытки осуществить вызов функции в другом соглашении - приведут к ошибкам          //
    //  выполнения, повреждению стека или исключениям времени выполнения приложения         //
                                                                                            //
    //  Для осуществления вызова функций в других соглашениях, в том числе и функций в      //
    //  соглашении FASTCALL ( применяемом для 64-х битных систем ) - используется макрос    //
    //  «CallPtr», определяющий для компилятора используемое соглашение вызова              //
                                                                                            //
    __declspec(naked) __inline ULONG __cdecl RtlInvokeStdCallPtr \
        ( __in __notnull FARPROC /* call_ptr */ , __in UINT /* cnt_args */ , \
            __in_opt __refparam va_list /* args */ ) {                                      //
                                                                                            //
        //  Для функций, объявленных как «naked» - автоматическое управление фреймом стека  //
        //  а также - генерируемые компилятором пролог и эпилог - опускаются, извлечение    //
        //  аргументов, передаваемых через стек - осуществляется вызываемой стороной        //
                                                                                            //
        //  Для осуществления STDCALL-вызова - необходимо корректно построить фрейм стека   //
        //  и скопировать на его вершину актуальные значения аргументов вызова              //
                                                                                            //
        //  На вершине стека ( со смещением 00h ) - размещён адрес возврата, далее …        //
                                                                                            //
        __asm   mov   eax, [esp + 04h]  //  «call_ptr» - адрес вызова                       //
        __asm   mov   ecx, [esp + 08h]  //  «cnt_args» - количество аргументов              //
        __asm   mov   esi, [esp + 0Ch]  //  «args» - область значений аргументов            //
                                                                                            //
        //  Для 32-битных приложений - стек оперирует величинами кратности 4 байта          //
        //  ( двойное машинное слово ), в ECX - загружается значение счётчика байт,         //
        //  передаваемых через стек ( длина области значений всех аргументов )              //
                                                                                            //
        __asm   shl   ecx, 2    /*  cnt_args * 4 - полное количество байт */                //
                                                                                            //
        __asm   sub   esp, ecx    /*  Резервируется вычисленное количество байт */          //
        __asm   mov   edi, esp    /*  Указатель для переноса данных в область стека */      //
                                                                                            //
        __asm   rep   movsb   /*  Копирование данных из аргументов функции - на стек */     //
        __asm   call  eax     /*  Стек подготовлен - осуществляется вызов */                //
        __asm   ret           /*  Возврат управления вызывающей стороне */                  //
    }                                                                                       //

    #define StdCallPtr( call_ptr, ... ) \
        ( ( call_ptr ) ( VA_ARGS_COUNT(##__VA_ARGS__), ##__VA_ARGS__ ) )
                                                                                            //
    #endif                                                                                  //

    #define CallPtr( call_ptr, type, ... ) \
        ( ( (type) (FARPROC) call_ptr ) ( ##__VA_ARGS__ ) )

    //  ▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪  //

