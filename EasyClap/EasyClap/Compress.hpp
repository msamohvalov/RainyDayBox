

    //  ▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪  //

    #pragma once    //  Include-Guard подключаемого модуля                                  //
                                                                                            //
    #include "Legacy.hpp"   //  Системные функции в контексте LDRP API                      //
    #include "Auxiliary.h"  //  Вспомогательные конструкции препроцессора                   //
                                                                                            //
    //  Данный модуль определяет некоторые «единицы абстрации» в разработке приложения,     //
    //  реализуя концепцию декомпозиции, многие «техники», использованные в нём - лишь      //
    //  показывают доступные возможности языка, компилятора, препроцессорной обработки      //
                                                                                            //
    //  В общей компоновке проекта могут быть использованы различные подходы, и наиболее    //
    //  правильным - является использование заголовочных H-файлов с объявлениями классов    //
    //  вместе с CPP-файлами реализации их функциональности, этот модуль реализован как     //
    //  единственный заголовочный файл, только для того, чтобы не перегружать проект        //
    //  общим количеством файлов, его подключение выполняется только один раз               //
                                                                                            //
    //  В этом модуле продемонстрированы техники работы с образом исполняемого модуля,      //
    //  загруженным в память, процедуры импорта зависимостей из внешних модулей, вызовы     //
    //  функций - в различных соглашениях, поддерживаемых компилятором и ОС                 //
                                                                                            //
    //  Не воспринимайте использованный здесь подход - как одобрение этих техник            //
    //  Даже лучше - в ситуациях, когда это возможно - избегайте предложенных здесь         //
    
    //  Константы, используемые библиотекой сжатия                                          //
                                                                                            //
    static const USHORT FormatLZNT1       = 0x0002;  /* Алгоритм (формат) LZNT1 */          //
    static const USHORT FormatXPress      = 0x0003;  /* … XPress 4K */                      //
    static const USHORT FormatXPressHuff  = 0x0004;  /* … XPress-Huff */                    //
                                                                                            //
    #define USED_FORMAT( p_compres, fmt )   /*  Проверка используемого формата */ \
        ( ( p_compres -> Params.uFormatAndEngine & fmt ) == fmt )                           //
                                                                                            //
    #define CHECK( fmt, check )     ( ( fmt & check ) == check )                            //
                                                                                            //
    static const USHORT EngineStandard    = 0x0000;  /* Сжатие по-умолчанию */              //
    static const USHORT EngineMaximum     = 0x0100;  /* Максимальное сжатие */              //
                                                                                            //
    /*  Сканирование пути исходного файла для поиска расширения … */                        //
                                                                                            //
    #define FIND_EXTENSION( out, in ) /*  Локальный блок кода … */ \
        { for ( PTSTR symb = in + lstrlen(in) - 1; /*  Обработка строки с конца  … */ \
            symb - in > 0 /* Обход повторяется - пока остаются значимые символы */ && \
        /*  Если встречаются элементы каталогов - стоп */ * symb != _T('\\') && \
        *symb != _T('/') && /*  На всякий случай - для разницы представления … */\
            ( *symb != _T('.') /*  Точка - признак расширения … */ || \
        ( symb != in /* … только, если она часть имени файла */ && \
            * (symb - 1) != _T('\\') && *(symb - 1) != _T('/')) && \
        ( out = symb + 1 /* Фиксация указателя */, 0L)); \
            symb-- /* Обход строки справа налево */); }                                     //

    //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //

    //  Класс «Compressor», реализует функционал сжатия файлов с использованием алгоритма   //
    //  LZNT1 ( и его вариации - XPress / XPress-Huff), данный алгоритм применяется в ОС    //
    //  Windows семейства NT - в реализации сжатых файлов в рамках файловой системы NTFS    //
                                                                                            //
    class Compressor {  /*  Класс, реализующий функционал алгоритма сжатия */               //
                                                                                            //
        struct CompressorParameters {   /*  Рабочие параметры компрессора файлов */         //
                                                                                            //
            USHORT uFormatAndEngine = 0U;       /*  Применяемый формат и метод сжатия */    //
            ULONG uCompressBufferSize = 0U;     /*  Размерность буфера сжатия ( байт ) */   //
            ULONG uCompressFragmentSize = 0U;   /*  Размерность бфуера распаковки */        //
        }                                                                                   //
          Params;   /*  Переменные рабочего пространства алгоритма сжатия данных */         //
                                                                                            //
        struct Dependencies {   /*  «Внешние» зависимости класса «Compressor» */            //
                                                                                            //
        //  В реализации алгоритма сжатия - используются «закрытые» системные вызовы,       //
        //  относящиеся к сторонним библиотекам ОС ( существует внешняя зависимость )       //
                                                                                            //
        //  Объявленные ниже механизмы «MODULE_IMPORT» и «MODULE_REFS» - позволяют          //
        //  осуществить разрешение «внешних» зависимостей класса, посредством захвата       //
        //  дескриптора модуля и выполнения процедуры динамического импорта                 //
                                                                                            //
        //    #  Способы описания зависимостей по внешним вызовам                           //
                                                                                            //
        //  Модуль, определяющий зависимость и список внешних ( импортируемых ) вызовов     //
        //  объявляются совместно и однократно, посредством макроса «COMPLY_DEPENDENCY»     //
                                                                                            //
        //    ◊  Пример :                                                                   //
                                                                                            //
        //    COMPLY_DEPENDENCY( Compressor, Dependencies, \
        //        MODULE_IMPORT, SysNtLayer, NTDLL.DLL, MODULE_REFS, ExternalCalls, \
        //          PROC(RtlGetCompressionWorkSpaceSize), PROC(RtlCompressBuffer) );        //
                                                                                            //
        //  Позднее список внешних вызовов «ExternalCalls» - может быть расширен            //
        //  через связку макросов  «IMPLY_DEPENDENCY» / «RELATE_MODULE_REFS»                //

        //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //
        
        //  Модуль, определяющий зависимость, и список внешних ( импортируемых ) вызовов    //
        //  из этого модуля - объявляются раздельно, через макросы «IMPLY_DEPENDENCY» и     //
        //  «COMPLY_MODULE_REFS» - соответственно                                           //
                                                                                            //
        //  Список внешних вызовов «ExternalCalls» - позднее также может быть расширен      //
        //  через связку макросов  «IMPLY_DEPENDENCY» / «RELATE_MODULE_REFS»                //
                                                                                            //
        //    ◊  Пример :                                                                   //
                                                                                            //
        //    IMPLY_DEPENDENCY ( Compressor, Dependencies, \
        //        MODULE_IMPORT, SysNtLayer, NTDLL.DLL);                                    //
                                                                                            //
        //    COMPLY_MODULE_REFS( Compressor, Dependencies, \
        //        SysNtLayer, MODULE_REFS, ExternalCalls, \
        //          PROC(RtlGetCompressionWorkSpaceSize), PROC(RtlCompressBuffer) );        //
    
        //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //
    
        //  Модуль (модули), определяющие зависимости - определяются раздельно, макросом    //
        //  «IMPLY_DEPENDENCY», общий список внешних вызовов ( пустой ) - определяется      //
        //  через макрос «IMPLY_MODULE_REFS», реализация списка внешних вызовов -           //
        //  осуществляется позднее в коде проекта, через вызовы «RELATE_MODULE_REFS»        //
                                                                                            //
        //    ◊  Пример :                                                                   //
                                                                                            //
        //    IMPLY_DEPENDENCY( Compressor, Dependencies, \
        //        MODULE_IMPORT, SysNtLayer, NTDLL.DLL);                                    //
                                                                                            //
        //    IMPLY_MODULE_REFS( Compressor, Dependencies, \
        //        MODULE_REFS, ExternalCalls );                                             //
                                                                                            //
        //    RELATE_MODULE_REFS( Compressor, Dependencies, \
        //        SysNtLayer, ExternalCalls, \
        //      PROC( RtlGetCompressionWorkSpaceSize ), PROC( RtlCompressBuffer ) );        //

        //  ‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡  //
                                                                                            //
        /*  Макросы для «разрешения» зависимостей класса по внешним вызовам */              //
                                                                                            //
        #define PROC( proc_name ) ( #proc_name )  /*  Маркер «внешнего» вызова */           //
                                                                                            //
        /*  Внедрение зависимости в связке: «модуль» / «список внешних вызовов» */          //
        #define COMPLY_DEPENDENCY( subj, deps, imp, layer, mod, refs, calls, ... ) \
            subj :: deps :: imp subj :: deps :: layer ( L###mod ); subj :: deps :: \
                refs subj :: deps :: calls ( &##layer, ##__VA_ARGS__ , NULL )               //
                                                                                            //
        /*  Внедрение зависимости по указанному модулю */                                   //
        #define IMPLY_DEPENDENCY( subj, deps, imp, layer, mod ) \
            subj :: deps :: imp subj :: deps :: layer ( L###mod )                           //
                                                                                            //
        /*  Только объявление списка внешних ( импортируемых ) вызовов */                   //
        #define IMPLY_MODULE_REFS( subj, deps, refs, calls ) \
            subj :: deps :: refs subj :: deps :: calls                                      //
                                                                                            //
        /*  Реализация списка внешних вызовов по внедрённой зависимости */                  //
        #define COMPLY_MODULE_REFS( subj, deps, layer, refs, calls, ... ) \
            subj :: deps :: refs subj :: deps :: calls ( &##layer, \
                ##__VA_ARGS__ , NULL )                                                      //
                                                                                            //
        /*  Импорт указателей внешних вызовов ( по внедрённой зависимости ) */              //
        #define RELATE_MODULE_REFS( subj, deps, layer, calls, ... ) \
            subj :: deps :: calls.EXTEND_REFS ( &subj :: deps :: layer, \
                ##__VA_ARGS__ , NULL )                                                      //
                                                                                            //
        //  Вспомогательные конструкции при вызове через указатель                          //
                                                                                            //
        #define  __UseRef       Dependencies ::         /*  Коллекция ссылок */             //
        #define  __RefItem(i)   [##i]                   /*  Индекс в коллекции */           //
        #define  __Assign(a)    Dependencies :: ##a     /*  Соглашение вызова */            //
                                                                                            //
        #define  __Params       /*  Условный маркер списка аргументов вызова */             //
                                                                                            //
        //  Члены данных «SysNtLayer» и «ExternalCalls» - объявлены как статические,        //
        //  поэтому захват дескриптора для каждого из модулей, перечисленных в списке       //
        //  зависимостей и выполнение процедуры динамическго импорта - осуществляются       //
        //  однократно ( статическим инициализатором класса, при запуске приложения )       //
                                                                                            //
        static struct MODULE_IMPORT {   /* Механизм импорта внешних функций */              //
                                                                                            //
            private:  /*  Закрытый функционал объектов этого класса */                      //
                                                                                            //
            HMODULE const hThisComponentHadle;  /*  Дескриптор модуля */                    //
                                                                                            //
            public:  /*  Общедоступный функционал объектов класса «MODULE_IMPORT» */        //
                                                                                            //
            //  Оператор приведения типа объекта к «HMODULE»                                //
                                                                                            //
            __inline operator const HMODULE ( __has_no_params ) \
                const YIELD( hThisComponentHadle );  /*  Возвращает дескриптор */           //
                                                                                            //
            __inline __success(return != NULL)  /* Операция взятия индекса */ \
                FARPROC operator [] ( __in __nullterminated PSTR pszProcName ) const \
                    RETURNS( LdrpFindProcedureAddress( *(this), pszProcName ) );            //
                                                                                            //
            //  Реализация конструктора для объектов класса                                 //
                                                                                            //
            MODULE_IMPORT( __in __nullterminated PCWSTR pszModuleName ) : \
                hThisComponentHadle( LdrpGetModuleHandle( pszModuleName ) ) \
                    __EMPTY_FUNCTION_BODY_IMPLEMENTATION;  /*  Без реализации */            //
        }                                                                                   //
          SysNtLayer;  /* Зависимость по библиотеке системного окружения «NTDLL.DLL» */     //

        //  ÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷  //

        struct CALLEE_PTR {  /*  Класс-обёртка для осуществления внешних вызовов */         //
                                                                                            //
            private:  /*  Закрытый функционал объектов этого класса */                      //
                                                                                            //
            FARPROC pCallRef;   /*  Указатель внешнего вызова */                            //
                                                                                            //
            /*  Конструктор по-умолчанию ( без параметров ) */                              //
            CALLEE_PTR( __has_no_params ) : /* Является закрытым */ \
                pCallRef ( NULL ) __EMPTY_FUNCTION_BODY_IMPLEMENTATION;                     //
                                                                                            //
            public:  /*  Общедоступный функционал объектов класса «CALLEE_PTR» */           //
                                                                                            //
            //  Оператор приведения типа объекта к «FARPROC»                                //
                                                                                            //
            __inline operator const FARPROC( __has_no_params ) \
                const YIELD( pCallRef );  /*  Возвращает указатель внешнего вызова */       //
                                                                                            //
            CALLEE_PTR( const FARPROC pExternal ) : /* Открытый конструктор */ \
                pCallRef( pExternal ) __EMPTY_FUNCTION_BODY_IMPLEMENTATION;                 //
                                                                                            //
            CALLEE_PTR( const CALLEE_PTR & ) = default;  /*  «Неглубокое» копирование */    //

            //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //
                
            #ifndef _WIN64  /*  Оператор вызова () - реализован только в среде Win32 */     //
                                                                                            //
            //  Оператор «вызова» по указателю объявлен - как вариадический, с переменным   //
            //  количеством аргументов, применим только к STDCALL-указателям функций        //
                
            __inline ULONG __cdecl operator() ( __in UINT cnt_args, ... ) const {           //
                                                                                            //
                va_list args;   /*  Вариадический маркер аргументов функции */              //
                va_start( args, cnt_args);  /*  Инициализация маркера */                    //
                                                                                            //
                ULONG uRetValue = -1;   /*  Результирующее значение вызова */               //
                                                                                            //
                pCallRef &&  /* Вызов указателя ( STDCALL ), если он реализован … */        //
                    ( uRetValue = RtlInvokeStdCallPtr( *this, cnt_args, args), 1L ) || \
                                                                                            //
                ( SetLastError ( ERROR_CALL_NOT_IMPLEMENTED ), 0L );  /*  Ошибка */         //
                                                                                            //
                return va_end( args ) /* Сброс маркера */ , uRetValue;  /*  Результат */    //
            }                                                                               //
                                                                                            //
            #endif  /*  ! _WIN64  | Блок определён только для среды Win32 */                //
        };                                                                                  //

        //  ÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷  //

        //  Коллекция указателей «внешних» вызовов ( импортированных функций )              //
                                                                                            //
        static struct MODULE_REFS {  /*  Контейнер импортируемых ссылок */                  //
                                                                                            //
            private:  /*  Закрытый функционал объектов этого класса */                      //
                                                                                            //
            PCRITICAL_SECTION pSync = NULL;   /*  Примитив синхронизации */                 //
                                                                                            //
            FARPROC * rgn_refs = NULL;  /*  Массив хранимых ссылок */                       //
            UINT refs_count = 0;        /*  Количество элементов массива */                 //
            
            //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //
            
            VOID IMPORT_REFS  /* Служебный метод ( импорт внешних вызовов ) */ \
                ( __in __notnull const MODULE_IMPORT * const pModuleImport, \
                    __in_opt __refparam va_list args ) {                                    //
                                                                                            //
                /*  Хэндл кучи памяти, используемой текущим процессом */                    //
                HANDLE hProcessHeap = RtlGetProcessHeap();  /*  … получен из PEB */         //
                                                                                            //
                //  Временный указатель, позволяет избежать повреждения основного при       //
                //  при невозможности перераспределения блоков памяти на куче               //
                                                                                            //
                VOID * ptr_array = rgn_refs;  /*  Временный указатель */                    //
                FARPROC ptr_import = NULL;    /*  Импортируемая ссылка */                   //
                                                                                            //
                EnterCriticalSection( pSync );  /*  Обеспечение потокобезопасности */       //
                LdrLockLoaderLock ();   /*  Потокобезопасность данных загрузчика */         //
                                                                                            //
                for ( PSTR pszProcName = NULL;  /* Вариадическое значение */ \
                                                                                            //
                /*  Имена импортируемых функций - собираются по маркеру */                  //
                ( pszProcName = va_arg( args, PSTR ) ) /* параметр */ && \
                                                                                            //
                pModuleImport  /* Проверка зависимости по модулю … */ && \
                                                                                            //
                /*  Проверка существования импортируемого символа … */
                ( ptr_import = ( *pModuleImport )[pszProcName] ) && \
                                                                                            //
                /*  Если память не была выделена ранее - она выделяется */                  //
                ( !refs_count && ( ptr_array = HeapAlloc ( hProcessHeap, \
                    HEAP_ZERO_MEMORY, ( refs_count + 1 )*sizeof( FARPROC ) )) || \
                                                                                            //
                //  Ранее выделенный блок памяти - перераспределяется к размеру,            //
                //  достаточному для размещения нового указателя                            //
                                                                                            //
                ( ptr_array = HeapReAlloc ( hProcessHeap, HEAP_ZERO_MEMORY, \
                    ptr_array, (refs_count + 1) * sizeof(FARPROC) )) ); \
                                                                                            //
                /*  Если символ импорта существует - он сохраняется в массиве */            //
                *( (FARPROC *) ptr_array + refs_count ) = ptr_import, \
                    refs_count++ /* Увеличение счётчика ссылок */ );                        //
                                                                                            //
                ptr_array &&  /* Только при удачном перераспределении памяти … */ \
                    ( rgn_refs = (FARPROC *) ptr_array );  /* … замещение указателя */      //
                                                                                            //
                LdrUnlockLoaderLock ();  /* Выход из критической секции загрузчика */       //
                LeaveCriticalSection( pSync );  /*  Выход общей критической секции */       //
            }                                                                               //
                
            //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //
                
            public:  /*  Общедоступный функционал объектов класса «MODULE_REFS» */          //
                                                                                            //
            MODULE_REFS( __has_no_params ) : /*  Конструктор по-умолчанию */ \
                rgn_refs( NULL ), refs_count(0), pSync ( new CRITICAL_SECTION {0} ) \
                     { pSync && ( InitializeCriticalSection( pSync ), 1L ); }               //
                                                                                            //
            //  Вариадический конструктор, которому передаются «внешние» ссылки             //
                                                                                            //
            MODULE_REFS( __in __notnull const MODULE_IMPORT * const pModuleImport, \
                /*  Список аргументов */ ... ) : MODULE_REFS ( __no_args ) {                //
                                                                                            //
                va_list args;   /* Вариадический маркер для доступа к параметрам */         //
                va_start( args, pModuleImport );   /*  Инициализация маркера */             //
                                                                                            //
                IMPORT_REFS( pModuleImport, args );  /*  Импорт и инициализация */          //
                                                                                            //
                va_end( args );  /*  Сброс маркера вариадических параметров */              //
            }                                                                               //
                                                                                            //
            //  При завершении работы приложения - уничтожение единственного объекта,       //
            //  объявленного как статический - должно освобождать блок динамической         //
            //  памяти, используемой для хранения указателей вызова                         //
                                                                                            //
            ~MODULE_REFS( __has_no_params ) {   //  Деструктор объектов класса              //
                                                                                            //
                /*  Хэндл кучи памяти, используемой текущим процессом */                    //
                HANDLE hProcessHeap = RtlGetProcessHeap();  /*  … получен из PEB */         //
                                                                                            //
                EnterCriticalSection( pSync );  /*  Обеспечение потокобезопасности */       //
                                                                                            //
                rgn_refs &&  /*  Если блок памяти был распределён … */ \
                    HeapFree ( hProcessHeap, 0, rgn_refs );  /* … он освобождается */       //
                                                                                            //
                LeaveCriticalSection( pSync );  /*  Выход из критической секции */          //
                                                                                            //
                /*  Удаление критической секции и её блока памяти */                        //
                pSync && ( DeleteCriticalSection( pSync ), delete ( pSync ), 1L );          //
            }                                                                               //

            //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //
            
            /*  Запрос объекта-обёртки для указателя внешнего вызова */                     //
            __inline const CALLEE_PTR operator [] /* Операция взятия индекса */ \
                ( __valid __in_range(0, refs_count-1) __in UINT ref_idx ) const {           //
                                                                                            //
                EnterCriticalSection( pSync );  /*  Обеспечение потокобезопасности */       //
                                                                                            //
                /*  Получение указателя из хранимого массива - по его индексу */            //
                FARPROC pRetPtr = ref_idx < refs_count ? rgn_refs[ref_idx] : NULL ;         //
                                                                                            //
                LeaveCriticalSection( pSync );  /*  Выход из критической секции */          //
                                                                                            //
                //  Реализация конструктора копирования для CALLEE_PTR в данном контексте   //
                //  не нужна, оператор индексации - тривиальный, и объявлен как «inline»,   //
                //  поэтому, объект содаётся на стеке вызывающей стороны, в точке вызова    //
                                                                                            //
                //  В ситуации, если он потребуется - будет достаточно реализации и         //
                //  конструктора неглубокого копирования, предоставляемого компилятором     //
                                                                                            //
                return CALLEE_PTR( pRetPtr );   /*  Указатель внешнего вызова */            //
            }                                                                               //
                                                                                            //
            //  Вариадический метод, для добавления «внешних» ссылок к уже имеющимся        //
                                                                                            //
            VOID EXTEND_REFS /*  Применяется для существуюшего объекта */ \
                ( __in __notnull const MODULE_IMPORT * const pModuleImport, \
                    /*  Список с переменным количеством аргументов */ ... ) {               //
                                                                                            //
                va_list args;   /* Вариадический маркер для доступа к параметрам */         //
                va_start( args, pModuleImport);   /*  Инициализация маркера */              //
                                                                                            //
                IMPORT_REFS( pModuleImport, args );  /*  Импорт и добавление */             //
                                                                                            //
                va_end( args );  /*  Сброс маркера вариадических параметров */              //
            }                                                                               //
                                                                                            //
            //  Оператор приведения типа объекта к «FARPROC»                                //
                                                                                            //
            __inline UINT Length ( __has_no_params ) const \
                YIELD( refs_count );  /*  Возвращает указатель внешнего вызова */           //
        } \
          ExternalCalls;  /*  Указатели импортированных ( «внешних» ) функций */            //

        //  ÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷  //

        /*  Соглашения «внешних» ( импортируемых из «NTDLL.DLL» ) вызовов */                //
                                                                                            //
        /*  «RtlGetCompressionWorkSpaceSize» - инициализация алгоритма */                   //
        typedef NTSTATUS ( NTAPI * RtlGetCompressionWorkSpaceSize ) \
            ( __in USHORT uFormatAndEngine, __out ULONG * pBufferWorkSpaceSize, \
                __out ULONG * pFragmentWorkSpaceSize );                                     //
                                                                                            //
        /*  «RtlCompressBuffer» - сжатие блока данных */                                    //
        typedef NTSTATUS ( NTAPI * RtlCompressBuffer ) \
            ( __in USHORT uFormatAndEngine, __in PBYTE pUncompressedBuffer, \
                __in ULONG UncompressedBufferSize, __out PBYTE pCompressedBuffer, \
            __in ULONG CompressedBufferSize, __in ULONG UncompressedChunkSize, \
            __out PULONG pFinalCompressedSize, __in PVOID pWorkSpace );                     //
                                                                                            //
        /*  «RtlDecompressBufferEx» - распаковка блока данных */                            //
        typedef NTSTATUS ( NTAPI * RtlDecompressBufferEx ) \
            ( __in USHORT uFormatAndEngine, __out PBYTE pUncompressedBuffer, \
                __in ULONG UncompressedBufferSize, __in PBYTE pCompressedBuffer, \
            __in ULONG CompressedBufferSize, __out PULONG pFinalUncompressedSize, \
                __in PVOID pWorkSpace );                                                    //
                                                                                            //
        /*  «MD5Init» - инициализация структуры контекста алгоритма MD5 */                  //
        typedef VOID ( NTAPI * MD5Init ) \
            ( __in __typefix( struct MD5_CTX * ) PVOID p_ctx );                             //
                                                                                            //
        /*  «MD5Update» - вычисление частичного MD5 хеш-значения для блока данных */        //
        typedef VOID ( NTAPI * MD5Update ) \
            ( __inout __typefix( struct MD5_CTX * ) PVOID p_ctx, \
                __in_bcount(size) BYTE* buf, __in ULONG size );                             //
                                                                                            //
        /*  «MD5Final» - генерация окончательного хеш-значения MD5 */                       //
        typedef VOID ( NTAPI * MD5Final ) \
            ( __inout __typefix( struct MD5_CTX * ) PVOID p_ctx);                           //
                                                                                            //
        };  /*  Конец блока «Dependencies» для класса «Compressor» */                       //

        //  ÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷  //

        public:  /*  Общедоступный функционал объектов класса «Compressor» */               //
                                                                                            //
        /*  Абстракция «буфер данных», используется классом «Compressor» */                 //
        /*  Данный класс - реализует «правило пяти» в стандартах C++ */                     //
                                                                                            //
        class Buffer {  /* Реализация буфера данных, используемого методами класса  */      //
                                                                                            //
            BYTE * ptr = NULL;  /*  Байтовый массив данных, хранимых буфером */             //
            ULONG size = 0UL;   /*  Размерность хранимого массива данных */                 //
                                                                                            //
            public :  /*  Общедоступный функционал объектов класса «Buffer» */              //
                                                                                            //
            /*  Оператор приведения данных к типу «BYTE *» */                               //
            __inline operator BYTE * (__has_no_params) const YIELD( ptr );                  //
                                                                                            //
            /*  Открытый конструктор класса ( без инициализации данных ) */                 //
            __inline Buffer ( __in_opt ULONG size = 0UL ) \
                APPLY( CropOrExpand( size ) ); /* Задаётся только размер буфера */          //
                                                                                            //
            /*  Конструктор с инициализацией данных из источника */                         //
            __inline Buffer ( __in PBYTE src, __in ULONG b_cnt ) : Buffer( b_cnt ) \
                APPLY( WriteChunk ( 0, b_cnt, src ) );  /* Перенос данных из источника */   //
                                                                                            //
            ~Buffer( __has_no_params )  /*  Деструктор объектов класса */ \
                APPLY( CropOrExpand( 0 ) );  /*  Удаление хранимых данных */                //
                                                                                            //
            /*  Конструктор чистого ( «глубокого» ) копирования данных экземпляра */        //
            Buffer ( const Buffer & src ) : Buffer( src.size ) /* Установка размера */ \
                /*  Перенос хранимых данных из буфера-источника в буфер назначения … */ \
                APPLY(( this -> WriteChunk ( 0, src.size, src ) ));                         //
                                                                                            //
            /*  Оператор присваивания «копированием» */                                     //
            Buffer & operator = ( const Buffer & rhs ) \
                APPLY( this != &rhs && /*  Предотвращение «самоприсваивания» */ \
                    ( this -> CropOrExpand ( rhs.size ), /*  Инициализация буфера … */ \
                this -> WriteChunk ( 0, rhs.size, rhs ) /* Перенос данных экземпляра */ ); \
                    EMIT( *this ); /*  Объект для замыкания операции */ );                  //
                                                                                            //
            /*  Конструктор «перемещения» данных экземпляра класса */                       //
            Buffer ( Buffer && src )  /* «Сырое» перемещение к «this» объекту … */          //
                APPLY(( this -> ptr = src.ptr, this -> size = src.size, \
                    src.ptr = NULL, src.size = 0 /* «Сброс» состояния src-объекта */ ));    //
                                                                                            //
            /*  Оператор взятия индекса ( доступ к элементу буфера ) */                     //
            __inline BYTE& operator [] ( __in ULONG idx ) YIELD( ptr[ idx % size ] );       //
                                                                                            //
            /*  Оператор присваивания «перемещением» */                                     //
            Buffer & operator = ( Buffer && rhs ) \
                APPLY( this != &rhs && /*  Предотвращение «самоприсваивания» */ \
                    ( this -> CropOrExpand ( 0 ), /*  Сброс буфера для lhs-объекта … */ \
                this -> ptr = rhs.ptr, this -> size = rhs.size, /* Перемещение из rhs … */ \
                    rhs.ptr = NULL, rhs.size = 0 /* «Сброс» состояния src-объекта */ );     //
                        EMIT( *this )  /*  Объект для замыкания операции */ );              //
                                                                                            //
            /*  Возвращается действительный размер буфера ( количество байт ) */            //
            __inline ULONG Size( __has_no_params ) const YIELD( size );                     //
                                                                                            //
            /*  Оператор сравнения объектов класса */
            __inline BOOLEAN operator == ( const Buffer & rhs ) const \
                RETURNS( this -> size == rhs.size && \
                    !memcmp( (BYTE*) *this, (BYTE*) rhs, this -> size ) );                  //
            
            //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //

            //  Расширение буфера ( с сохранением данных ) или его удаление                 //
                                                                                            //
            VOID CropOrExpand ( __in ULONG size ) {  /*  Удаление или расширение буфера */  //
                                                                                            //
                this -> size == size /*  Размер уже актуален */ && ( 1L ) || \
                                                                                            //
                this -> size > 0 && size > 0 &&  /* Буфер и новый размер - заданы … */ \
                                                                                            //
                ( this -> ptr = (BYTE *) HeapReAlloc  /* Перераспределение памяти … */ \
                    ( RtlGetProcessHeap ( __no_args ), HEAP_FREE_CHECKING_ENABLED | \
                HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, this -> ptr, size )) && \
                    ( this -> size = size /*  Актуальная длина буфера */ ) || \
                                                                                            //
                this -> size == 0 && size > 0 &&  /* Буфер «пуст», выделение памяти … */ \
                                                                                            //
                ( this -> ptr = (BYTE *) HeapAlloc ( RtlGetProcessHeap ( __no_args ), \
                    HEAP_GENERATE_EXCEPTIONS | HEAP_FREE_CHECKING_ENABLED | \
                        HEAP_ZERO_MEMORY, size )) && ( this -> size = size ) || \
                                                                                            //
                /*  Нулевой размер буфера - означает его «удаление» … */                    //
                size == 0 && ( this -> ptr && this -> size &&  /*  Буфер задан … */ \
                    /*  Освобождение выделенного ранее блока динамической памяти … */       //
                    HeapFree ( RtlGetProcessHeap ( __no_args ), 0L, this -> ptr ), \
                        ( this -> ptr = NULL, this -> size = 0 )  /*  Сброс значений */ );  //
            }                                                                               //

            //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //

            /*  Чтение фрагмента буфера из данных текущего */                               //
                                                                                            //
            __inline Buffer ReadChunk ( __in ULONG pos, __in ULONG b_cnt ) const {          //
                                                                                            //
                return Buffer ( this -> ptr + pos, /*  Возвращается буфер-фрагмент */ \
                    this -> size <= pos + b_cnt ? b_cnt : 0  /* … или пустой буфер */ );    //
            }                                                                               //
            
            /*  Перезапись фрагмента буфера из указанного источника данных */               //
                                                                                            //
            BOOL WriteChunk ( __in ULONG pos, __in ULONG b_cnt, __in PBYTE src ) {          //
                                                                                            //
                /*  Проверка и осуществление переноса данных из источника */                //
                                                                                            //
                return this -> size <= pos + b_cnt &&  /*  «Вместимость» буфера … */ \
                    !IsBadReadPtr ( src, b_cnt ) && /*  Допустимость источника … */ \
                RtlCopyMemory ( ptr, src, b_cnt );  /*  Копирование / инициализация … */    //
            }                                                                               //

            //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //
            
            VOID RLE_InplaceEncode ( __has_no_params ) {  /* RLE-кодирование данных */      //
                                                                                            //
                Buffer Dummy ( this -> size );  /*  Вспомогательный буффер */               //
                                                                                            //
                for ( ULONG i = 0 /* Позиция чтения */ , k = 0 /* Позиция записи */ ; \
                                                                                            //
                i < this -> size  /*  Обработка по длине исходного массива … */ || \
                                                                                            //
                /*  При завершении обхода - перенос данных в текущий буфер */ \
                ( this -> CropOrExpand( k ) /* Размер данных RLE-кодирования */, \
                    this -> WriteChunk ( 0, k, Dummy ) /* Перенос RLE-данных … */, 0L ); \
                                                                                            //
                i++  /* Последовательный обход данных исходного буфера … */ )               //
                                                                                            //
                for ( BYTE runLength = 1  /* Вычисление текущего RLE-фрагмента */ ; \
                                                                                            //
                runLength < 0xFF /*  Максимальная длина фрагмента - 255 байт */  && \
                    i + 1 < this -> size /* Пока обход данных не завершён … */ && \
                                                                                            //
                (*this)[i] == (*this)[i + 1]  /*  Критерий повторяемости … */ || \
                                                                                            //
                /*  Вычисление RLE-фрагмента завершено, необходимо его размещение … */      //
                                                                                            //
                /*  Проверка возможности размещения RLE-фрагмента … */                      //
                ( k >= Dummy.size - 2 /* … помещается только один RLE-фрагмент */ && \
                    this -> size - i > 1 /* необработанных - осталось больше … */ && \
                                                                                            //
                /*  Расширение буфера c «запасом» на оставшиеся RLE-фрагменты … */          //
                ( Dummy.CropOrExpand ( Dummy.size << 1 ), 1L ), \
                                                                                            //
                /*  Размещение RLE-фрагмента в результирующем буфере … */                   //
                Dummy[k++] = runLength, Dummy[k++] = (*this)[i], 0L );                      //
                                                                                            //
                    runLength++, i++ /*  Обход данных при вычислении RLE-фрагмента */ );    //
            }                                                                               //

            //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //

            VOID RLE_InplaceDecode ( __has_no_params ) {  /* RLE-декодирование данных */    //
                                                                                            //
                Buffer Dummy( this -> size );   /*  Вспомогательный буффер */               //
                                                                                            //
                for ( ULONG i = 1 /* Порядковый номер RLE-фрагмента */, \
                    k = 0 /* Позиция записи раскодированных данных */ ; \
                        i < this -> size /*  Обработка по длине исходного массива … */ || \
                                                                                            //
                /*  При завершении обхода - перенос данных в текущий буфер */ \
                ( this -> CropOrExpand( k ) /* Размер исходного буфера */, \
                    this -> WriteChunk ( 0, k, Dummy ) /* Данные буфера … */, 0L );         //
                                                                                            //
                i += 2  /*  Обход данных RLE-фрагментов осуществляется - парами */ )        //
                                                                                            //
                for ( BYTE runLength = (*this)[i-1]  /*  Текущий RLE-фрагмент … */; \
                                                                                            //
                runLength > 0  /*  Пока фрагмент не обработан полностью … */ ; \
                                                                                            //
                /*  Проверка возможности размещения раскодированных данных */               //
                k >= Dummy.size  /*  Буфер размещения данных - закончился … */ && \
                                                                                            //
                /*  Увеличение буфера - блочное, по размеру исходного RLE-буфера */         //
                ( Dummy.CropOrExpand ( Dummy.size + this -> size ), 1L ), \
                                                                                            //
                Dummy[k++] = (*this)[i] /*  Перенос данных RLE-фрагмента … */, \
                                                                                            //
                runLength--  /* Обход по всей длине RLE-фрагмента */ );                     //
            }                                                                               //
        };                                                                                  //

        //  ÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷  //

        /*  Абстракция «подпись сообщения», используется классом «Compressor» */            //
        /*  для проверки целостности данных при их сжатии / распаковке */                   //
                                                                                            //
        class MessageDigest {  /* Реализация алгоритма MD5 ( хеш-значение 128-бит ) */      //
                                                                                            //
            private:   /*  Закрытый функционал объектов класса «MessageDigest» */           //
                                                                                            //
            struct MD5_CTX {  /*  Структура контекста для вычисления хеш-значения MD5 */    //
                                                                                            //
                union {   /*  Представление битового счётчика метода MD5 */                 //
                                                                                            //
                    struct {    /*  Компоненты 64-битного счётчика */                       //
                                                                                            //
                        ULONG low;    /*  Младшее двойное слово значения */                 //
                        ULONG high;   /*  Старшее двойное слово значения */                 //
                    }                                                                       //
                      part;  /*  Старшее и младшее двойное машинное слово значения */       //
                                                                                            //
                    ULONG64 value;  /*  Полное 64-битное значение ( LSB-first ) */          //
                }                                                                           //
                  count;    /*  Счётчик количества бит ( по модулю 2^64 ) */                //
                                                                                            //
                union {   /*  Представление регистров состояния метода MD5 */               //
                                                                                            //
                    struct {    /* Блок регистров состояния */                              //
                                                                                            //
                        DWORD A, B, C, D;  /*  Сдвиговые регистры состояния */              //
                    }                                                                       //
                      registers;  /*  32-х битные регистры состояния */                     //
                                                                                            //
                    DWORD state[4];  // MD5 «Magic» consts                                  //
                };                                                                          //
                                                                                            //
                BYTE  buffer[64];   /* Буфер хранимых данных алгоритма */                   //
                BYTE  value[16];    /* Вычисленное MD5 хеш-значение */                      //
                                                                                            //
                //  Конструктор копирования и операция присваивания для «MD5_CTX»           //
                                                                                            //
                MD5_CTX ( __has_no_params ) /*  Конструктор по-умолчанию */ \
                    APPLY( RtlZeroMemory( this, sizeof( MD5_CTX )) );                       //
                                                                                            //
                MD5_CTX ( const MD5_CTX & that ) /*  Конструктор копирования */ \
                    APPLY( RtlCopyMemory( this, &that, sizeof( MD5_CTX )) );                //
                                                                                            //
                MD5_CTX & operator = ( const MD5_CTX & rhs ) /*  Оператор присваивания */ \
                    APPLY( RtlCopyMemory( this, &rhs, sizeof( MD5_CTX )) );                 //
            }                                                                               //
              * p_ctx = NULL;   /*  Промежуточный контекст вычислений алгоритма */          //

            //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //

            /*  Закрытый служебный метод, применяется повторными вызовами … */              //
                                                                                            //
            //  Инициализация MD5-контекста ( для текущего объекта ) посредством вызова     //
            //  «MD5Init», для дальнейшего использования при вычислении хэш-значения        //
                                                                                            //
            __inline VOID Init ( __has_no_params ) \
                APPLY( CallPtr ( __UseRef ExternalCalls __RefItem(3), \
                    __Assign( MD5Init ), __Params p_ctx ) );  /* Вызов «MD5Init» */         //
                                                                                            //
            public:  /*  Общедоступный функционал объектов класса «MessageDigest» */        //
                                                                                            //
            MessageDigest ( __has_no_params ) : /*  Конструктор по-умолчанию */ \
                p_ctx ( new MD5_CTX( __no_args ) ) APPLY( Init( __no_args ) );              //
                                                                                            //
            MessageDigest ( const Buffer & data ) : /*  Конструктор с инициализацией */ \
                MessageDigest ( __no_args ) APPLY( Append( data ) );  /* Буфер данных */    //
                                                                                            //
            ~MessageDigest ( __has_no_params )  /*  Деструктор объектов */ \
                APPLY( p_ctx && ( delete p_ctx, 1L ));  /*  Уничтожение MD5-контекста … */  //
                                                                                            //
            /*  Сброс и повторная инициализация MD5-механизма */                            //
            VOID Reset ( __has_no_params ) APPLY( Init( __no_args ) );                      //
                                                                                            //
            //  Вычисление промежуточного хэш-значения для блока входных данных             //
            //  ( результат хранится в контексте вычислений текущего объекта )              //
                                                                                            //
            VOID Append ( const Buffer & data ) \
                APPLY( CallPtr ( __UseRef ExternalCalls __RefItem(4), \
                    __Assign( MD5Update ), __Params p_ctx, data, data.Size() ) );           //
                                                                                            //
            //  Вычисление «окончательного» хеш-значения, выходной буфер содержит полные    //
            //  128-бит «накопленного» результата MD5, т.к. вызов использует временный      //
            //  контекст при генерации результата - вычисления могут быть продолжены        //
                                                                                            //
            __inline const Buffer Value ( __has_no_params ) const {                         //
                                                                                            //
                MD5_CTX cur_ctx ( * this -> p_ctx );  /*  Временный MD5-контекст */         //
                                                                                            //
                /*  Запись результата с обнулением временного контекста */                  //
                /*  ( вычисления на хранимом контексте - могут быть продолжены ) */         //
                                                                                            //
                ( cur_ctx.count.value != 0 ) &&  /*  Если есть добавленные блоки … */ \
                                                                                            //
                    ( CallPtr ( __UseRef ExternalCalls __RefItem(5), \
                        __Assign( MD5Final ), __Params &cur_ctx ), 1L );                    //
                                                                                            //
                return Buffer( cur_ctx.value, 16 );  /*  Результирующий буфер */            //
            }                                                                               //
        };                                                                                  //

        //  ÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷  //

        private :   /*  Закрытый функционал объектов этого класса */                        //
                                                                                            //
        Buffer Workspace;   /*  Буфер рабочей области для операций с блоками */             //
                                                                                            //
        /*  Закрытый конструктор класса «Compressor» - напрямую не вызывается */            //
                                                                                            //
        Compressor ( __in USHORT uFormatAndEngine ) \
            noexcept(false) /*  Генерирует исключения при инициализации */ {                //
                                                                                            //
            /*  Проверка счётчика импорта ссылок ( «внешних» вызовов ) */                   //
            Dependencies :: ExternalCalls.Length ( __no_args ) == 6 || \
                                                                                            //
            /*  Вызов деструктора, освобождение блока динамической памяти */                //
            ( delete this, RaiseException( ERROR_DLL_INIT_FAILED, 0, 0, NULL ), 1L );       //
                                                                                            //
            /*  Попытка проведения инициализации ( через системный вызов ) */               //
            SUCCEEDED( CallPtr ( __UseRef ExternalCalls __RefItem(0), \
                __Assign( RtlGetCompressionWorkSpaceSize ), __Params uFormatAndEngine, \
                    &Params.uCompressBufferSize, &Params.uCompressFragmentSize ) ) && \
                                                                                            //
                /*  Инициализация буфера рабочего пространства для работы с блоками */      //
                ( Workspace.CropOrExpand ( Params.uCompressBufferSize ), 1L ) && \
                                                                                            //
                ( Params.uFormatAndEngine = uFormatAndEngine, 1L ) /* Успех … */ || \
                                                                                            //
            /*  Вызов деструктора, освобождение блока динамической памяти */                //
            ( delete this, RaiseException( ERROR_NOT_SUPPORTED, 0, 0, NULL ), 1L );         //
        }                                                                                   //
                                                                                            //
        ~Compressor ( __has_no_params ) {   /*  Закрытый деструктор объектов класса */      //
                                                                                            //
            //DebugBreak();
        }                                                                                   //

        //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //

        static USHORT GetArchFileInfo \
            ( __in __nullterminated const PTSTR pszFilePathSrc, \
                __out USHORT * puFormatAndEngine, __out __refparam Buffer & FileExt, \
                    __out UINT * puFileSize, __out __refparam Buffer & hash ) {
                                                                                            //
            USHORT uResult = 0;  /*  Результирующее значение ( размер заголовка ) */        //
                                                                                            //
            HANDLE hSrcFile = CreateFile ( pszFilePathSrc, GENERIC_READ, \
                FILE_SHARE_READ, ( _SECURITY_ATTRIBUTES *) NULL, OPEN_EXISTING, \
                    FILE_ATTRIBUTE_NORMAL, NULL );  /*  … открытие файла */                 //
                                                                                            //
            __TRY_FINALLY {   /*  Блок контроля файловых операций ввода / вывода */         //
                                                                                            //
                BAD_FILE( hSrcFile ) && __LEAVE;  /*  Выход при ошибке доступа */           //
                                                                                            //
                DWORD bytes = 0;    /*  Счётчик считанных байт */                           //
                                                                                            //
                USHORT Sign = 0;    /*  Значение сигнатуры архива ( формат и метод ) */     //
                                                                                            //
                /*  Проверка возможности записи по указателю */                             //
                !IsBadWritePtr ( puFormatAndEngine, sizeof(USHORT) ) && /*  Чтение … */ \
                    ReadFile ( hSrcFile, &Sign, sizeof(USHORT), &bytes, NULL ) && \
                                                                                            //
                bytes == sizeof(USHORT) /* Проверка размерности поля */ && \
                    !( Sign & 0xFEF8 ) /* Валидация значенимых бит поля */ && \
                                                                                            //
                /*  Проверка значимых бит, определяющих формат архива и степень сжатия */   //
                ( CHECK( Sign, FormatXPressHuff ) || CHECK( Sign, FormatXPress ) || \
                    CHECK( Sign, FormatLZNT1 ) ) || __LEAVE /*  Выход при ошибках */;       //
                                                                                            //
                BYTE ExtLength = 0;   /*  Попытка определения длины расширения … */         //
                                                                                            //
                for( BYTE val = 0, cnt = 0; /*  Считывание до нуль-терминатора … */ \
                    ReadFile ( hSrcFile, &val, sizeof(BYTE), &bytes, NULL ) && \
                    val != 0 && cnt < 0xFF || !val && ( ExtLength = cnt, 0 ) ; cnt++ );     //
                                                                                            //
                Buffer ExtData ( ExtLength + 1 ); /* Расширение ( с нуль-терминатором ) */  //
                                                                                            //
                /*  Если расширение имени файла указано - оно будет считано … */            //
                ExtLength && ( SetFilePointer ( hSrcFile, /* Выполняется смещение … */ \
                    - ( (LONG) ExtLength + 1 ), NULL, FILE_CURRENT ) != 0xFFFFFFFF && \
                                                                                            //
                ReadFile ( hSrcFile, /*  Считывание … */ (BYTE*) ExtData, ExtLength + 1, \
                    &bytes, NULL ) && bytes == ExtLength + 1 /* Проверка результата */ || \
                        __LEAVE /* Выход при возникновении ошибок … */ );                   //
                                                                                            //
                UINT uFileSize = 0;  /*  Считывание размера исходного файла … */            //
                                                                                            //
                /*  Проверка возможности записи по указателю … */                           //
                !IsBadWritePtr ( puFileSize, sizeof(UINT) ) && /*  Чтение … */ \
                    ReadFile ( hSrcFile, &uFileSize, sizeof(UINT), &bytes, NULL ) && \
                        bytes == sizeof(UINT) /* Проверка */ || __LEAVE; /* Выход … */      //
                                                                                            //
                BYTE md5[16] = {0};  /*  Рассчётное хеш-значение MD5 */                     //
                                                                                            //
                /*  Считывание 16-и байт ( 128 бит ) MD5 хеш-значения */
                ReadFile ( hSrcFile, md5, sizeof(BYTE[16]), &bytes, NULL ) && \
                    bytes == sizeof(BYTE[16])  /* Проверка */ || __LEAVE; /* Выход … */     //
                                                                                            //
                /*  Установка выходных параметров для вызывающей стороны */                 //
                                                                                            //
                *puFormatAndEngine = Sign;  /*  Сигнатура архива */                         //
                FileExt = ExtData;          /*  Расширение имени исходного файла */         //
                *puFileSize = uFileSize;    /*  Размер исходного файла */                   //
                                                                                            //
                hash.CropOrExpand( 16 ), hash.WriteChunk ( 0, 16, md5 );  /* MD5-хеш */     //
                                                                                            //
                /*  При успешном выполнении - возвращается размер заголовка */              //
                uResult = (USHORT) SetFilePointer ( hSrcFile, 0, NULL, FILE_CURRENT );      //
            }                                                                               //
                                                                                            //
            __FINALY_BLOCK  {   /*  Блок завершения контроля файловых операция */           //
                                                                                            //
                /*  Закрытие файловых манипуляторов «по необходимости» */                   //
                                                                                            //
                hSrcFile != INVALID_HANDLE_VALUE && CloseHandle ( hSrcFile );               //
            }                                                                               //
                                                                                            //
            return uResult;  /*  Размер заголовка архива или 0 - при ошибке */              //
        }                                                                                   //

        //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //

        public:  /*  Общедоступный функционал объектов класса «Buffer» */                   //
                                                                                            //
        /*  Статический метод для создания экземпляров класса */                            //
        __inline static __success( return != NULL ) \
            Compressor * CreateObject ( __in USHORT FormatAndEngine ) \
                GUIDED_OBJECT_CREATION( new Compressor( FormatAndEngine ) );                //
                                                                                            //
        /*  Статический метод для уничтожения экземпляров класса */                         //
        __inline static BOOLEAN DestroyObject \
            ( __in __refparam Compressor * pCompressorObj ) \
                GUIDED_OBJECT_DELETION( delete pCompressorObj );                            //

        //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //

        //  Методы уровня экземпляра, определяющие функционал сжатия / распаковки           //
                                                                                            //
        /*  Сжатие отдельного блока данных заданного размера */                             //
                                                                                            //
        Buffer CompressBuffer ( __in const Buffer & Uncompressed ) {                        //
                                                                                            //
            Buffer Compressed( Uncompressed.Size() + sizeof(ULONG) );  /* … упаковка */     //
                                                                                            //
            //  Размер блока упакованных данных - увеличен на длину значения «ULONG»,       //
            //  что необходимо для хранения величины, определяющей размер исходного         //
            //  блока данных - для обеспечения их корректной распаковки */                  //
                                                                                            //
            ULONG uKeepLength = 0;  /*  Актуальная длина данных после их упаковки */        //
                                                                                            //
            //  Изначально рассчитываем, что размер упакованных данных, как минимум,        //
            //  не превысит размер исходного буфера ( коэффициент сжатия ≥ 1)               //
                                                                                            //
            * (ULONG*) (BYTE*) Compressed = Uncompressed.Size();  /*  Хранимый размер */    //
                                                                                            //
            NTSTATUS ntStatus = (NTSTATUS) 0x00000000;  /* STATUS_SUCCESS */                //
                                                                                            //
            //  Обращение к Native API ( через указатель ) для упаковки блока данных        //
                                                                                            //
            for ( ; ntStatus = CallPtr ( __UseRef ExternalCalls __RefItem(1), \
                __Assign( RtlCompressBuffer ), __Params Params.uFormatAndEngine, \
                    Uncompressed, Uncompressed.Size(), (BYTE*) Compressed + sizeof(ULONG), \
                Compressed.Size() - sizeof(ULONG), 4096, &uKeepLength, (BYTE*) Workspace ), \
                                                                                            //
            //  В неблагоприятной ситуации, когда повторяемость данных в исходном массиве   //
            //  минимальна, а его длина достаточно мала ( коэффициент сжатия < 1 ) метод    //
            //  упаковки данных может быть неэффективным, при этом - длина упакованного     //
            //  массива будет превышать длину исходных данных                               //
                                                                                            //
            ntStatus != (NTSTATUS) 0x00000000  /* STATUS_SUCCESS */ && \
                ntStatus == (NTSTATUS) 0xC0000023  /* STATUS_BUFFER_TOO_SMALL */ ; \

            //  Если вызов «RtlCompressBuffer» завершится с кодом ошибки «0xC0000023»       //
            //  ( STATUS_BUFFER_TOO_SMALL ) - необходимо увеличить размерность буфера,      //
            //  представляющего упакованные данные ( буфер увеличивается в два раза )       //
                                                                                            //
            Compressed.CropOrExpand ( ( Compressed.Size() << 1 ) + sizeof( ULONG )) );      //
                                                                                            //
            //  Установка актуального размера упакованного буфера ( 0 - при ошибке )        //
                                                                                            //
            SUCCEEDED( ntStatus ) && ( uKeepLength += sizeof( ULONG ) ) || \
                ( uKeepLength ^= uKeepLength ); /* Сброс буфера при ошибке */               //
                                                                                            //
            Compressed.CropOrExpand( uKeepLength );  /* Актуализация размера буфера */      //
                                                                                            //
            return Compressed;  /*  Передача упакованного буфера вызывающей стороне */      //
        }                                                                                   //

        //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //

        //  Упаковка файла текущим компрессором ( определяется при создании объекта )       //
                                                                                            //
        BOOL CompressFile ( __in ULONG uWorkFrame /* Размер рабочего блока */ , \
            __in __nullterminated const PTSTR pszFilePathSrc /*  Файл источник */ ) {       //
                                                                                            //
            BOOL fResult = FALSE;  /*  Результирующее, возвращаемое значение */             //
                                                                                            //
            PTSTR pszFileExt = NULL;  /*  Указатель на расширение имени файла */            //
                                                                                            //
            FIND_EXTENSION( pszFileExt, pszFilePathSrc );   /* Поиск расширения … */        //
                                                                                            //
            //  Если pszFileExt == NULL - имя файла не содержит расширение                  //
                                                                                            //
            UINT cchNotable = pszFileExt == NULL ? /*  «Значимые» символы пути … */ \
                lstrlen ( pszFilePathSrc ) : (UINT) ( pszFileExt - pszFilePathSrc - 1 );    //
                                                                                            //
            /*  Выделение буфера, достаточного для хранения части пути + «\0x00» */         //
            Buffer FilePathDst ( ( cchNotable + 1 ) << ( sizeof(TCHAR) >> 1 ) );            //
            /*  Перенос фрагмента пути ( без расширения ), расширение добавим позже */      //
            lstrcpyn ( (PTSTR) (BYTE*) FilePathDst, pszFilePathSrc, cchNotable + 1 );       //
                                                                                            //
            TCHAR pszPackDotExt[] = { _T(".compact") };  /*  Расширение архива */           //
                                                                                            //
            FilePathDst.CropOrExpand  /*  Увеличение буфера для хранения пути */ \
                ( FilePathDst.Size() + sizeof( pszPackDotExt ) - sizeof(TCHAR) );           //
                                                                                            //
            /*  Добавление расширения к фрагменту пути ( теперь это полный путь ) */        //
            lstrcat ( (PTSTR) (BYTE*) FilePathDst, pszPackDotExt ); /* Путь к архиву */     //
                                                                                            //
            //  Архиватор должен восстанавливать исходное расширение в имени файла,         //
            //  при его распаковке, поэтому его нужно сохранить ( в ANSI-представлении )    //
                                                                                            //
            Buffer AnsiExt ( ( lstrlen ( pszFileExt ) + 1 ) << ( sizeof(TCHAR) >> 1 ) );    //
                                                                                            //
            #ifdef UNICODE  /*  В Unicode-компоновке расширение нужно декодировать */       //
                                                                                            //
                CHAR subs = '$';  /*  Символ-заменитель для потеряного представления */     //
                                                                                            //
                /*  При отсутствии расширения сохраняется только нуль-терминатор */         //
                AnsiExt.CropOrExpand ( pszFileExt == NULL ? 1 /* … нуль-терминатор */ : \
                    WideCharToMultiByte ( CP_ACP, 0, pszFileExt, /*  преобразование … */ \
                        -1, (PSTR) (BYTE*) AnsiExt, AnsiExt.Size(), &subs, NULL ) );        //
            #else                                                                           //
                                                                                            //
                /*  Для ANSI-компоновки - расширение копируется */                          //
                lstrcpy( (PSTR) (BYTE*) AnsiExt, pszFileExt );                              //
                                                                                            //
            #endif                                                                          //
                                                                                            //
            HANDLE hSrcFile = CreateFile ( pszFilePathSrc, GENERIC_READ, \
                FILE_SHARE_READ, ( _SECURITY_ATTRIBUTES *) NULL, OPEN_EXISTING, \
                    FILE_ATTRIBUTE_NORMAL, NULL );  /*  … открытие файла источника */       //
                                                                                            //
            HANDLE hDstFile = CreateFile ( (PTSTR) (BYTE*) FilePathDst, GENERIC_READ | \
                GENERIC_WRITE, 0, ( _SECURITY_ATTRIBUTES *) NULL, CREATE_ALWAYS, \
                    FILE_ATTRIBUTE_NORMAL, NULL );  /* … открытие файла назначения */       //
                                                                                            //
            __TRY_FINALLY {   /*  Блок контроля файловых операций ввода / вывода */         //
                                                                                            //
                /*  Выход при возникших ошибках доступа к файлам  */                        //
                ( BAD_FILE( hSrcFile ) || BAD_FILE( hDstFile ) ) && __LEAVE;                //
                                                                                            //
                Buffer ReadBuf( uWorkFrame ), WriteBuf( 0 );  /* Буфер чтения и записи */   //
                MessageDigest md5;  /*  Подсчёт контрольной суммы исходного файла */        //
                                                                                            //
                struct {  /*  Статистика файловых операций */                               //
                                                                                            //
                    DWORD Read = 0;     /*  Байтовый счётчик операции чтения */             //
                    DWORD Written = 0;  /*  Байтовый счётчик операции записи */             //
                    DWORD Total = 0;    /*  Количество байт для обработки */                //
                    DWORD Current = 0;  /*  Общее количество обработанных байт */           //
                }                                                                           //
                  BytesCount;   /*  Счётчики байт */                                        //
                                                                                            //
                /*  Запрос размера исходного файла ( пустые - будут пропущены ) */          //
                ( BytesCount.Total = GetFileSize ( hSrcFile, NULL ) ) && \
                    BytesCount.Total <= 0xFFFFFFFF  /*  Файлы до 2Гб */ || __LEAVE;         //
                                                                                            //
                /*  «Заголовок» архива - сигнатура + расширение + размер + MD5-хеш */       //
                                                                                            //
                WriteFile ( hDstFile, &Params.uFormatAndEngine, /*  Сигнатура … */ \
                    sizeof(USHORT), &BytesCount.Written, NULL ) || __LEAVE;                 //

                WriteFile ( hDstFile, (BYTE*) AnsiExt, /*  Расширение имени файла … */ \
                    AnsiExt.Size(), &BytesCount.Written, NULL ) || __LEAVE;                 //
                                                                                            //
                WriteFile ( hDstFile, &BytesCount.Total, /* Размер исходного файла … */ \
                    sizeof( BytesCount.Total ), &BytesCount.Written, NULL ) || __LEAVE;     //
                                                                                            //
                //  За полем размера - размещается MD5 хеш-значение исходного файла         //
                //  ( актуальное значение - будет установлено при завершении обработки )    //
                                                                                            //
                WriteFile ( hDstFile, (BYTE*) md5.Value(), 16 /* 128 бит */, \
                    &BytesCount.Written, NULL) || __LEAVE;  /*  Выход при ошибке … */       //
                                                                                            //
                ULONG uBlockSize = 0;  /*  Длина упакованного блока ( для записи ) */       //
                                                                                            //
                for ( ; /* Обход файла … */ BytesCount.Current < BytesCount.Total; \
                                                                                            //
                ReadFile ( hSrcFile, /* Чтение фрагмента исходного файла … */ \
                    (BYTE*) ReadBuf, ReadBuf.Size(), &BytesCount.Read, NULL) && ( \
                                                                                            //
                BytesCount.Read < uWorkFrame /* Прочитанный блок - неполон … */ && \
                    ( ReadBuf.CropOrExpand ( BytesCount.Read ) /* Хвост файла … */, 1L ), \
                                                                                            //
                md5.Append ( ReadBuf ), /*  Вычисление хеш-значения исходного блока … */ \
                    WriteBuf = this -> CompressBuffer ( ReadBuf )  /* Сжатие … */ ) && \
                                                                                            //
                ( uBlockSize = WriteBuf.Size() /* Успех считывания и сжатия … */ ) && \
                                                                                            //
                /*  Всё готово для записи - будут записаны длина блока и его данные */      //
                                                                                            //
                WriteFile ( hDstFile,  /*  Запись сжатого блока в файл-архива */ \
                    (BYTE*) &uBlockSize, sizeof(ULONG), &BytesCount.Written, NULL ) && \
                                                                                            //
                WriteFile ( hDstFile,  /*  Запись сжатого блока в файл-архива */ \
                    (BYTE*) WriteBuf, WriteBuf.Size(), &BytesCount.Written, NULL ) || \
                                                                                            //
                __LEAVE /* Выход из контрольного блока при ошибках чтения / записи */ , \
                                                                                            //
                /*  Увеличение счётчика байт и переход к следующему блоку */ \
                BytesCount.Current += BytesCount.Read /*, BytesCount.Read = 0 */ );         //
                                                                                            //
                //  Запись результирующего хеш-значения исходного файла                     //
                                                                                            //
                SetFilePointer  /*  Смещение в область заголовка, позицию MD5 … */ \
                    ( hDstFile, sizeof(USHORT) + AnsiExt.Size() + sizeof(DWORD), \
                        NULL, FILE_BEGIN ) != INVALID_SET_FILE_POINTER || __LEAVE;          //
                                                                                            //
                /*  Запись вычисленного по исходному файлу хеш-значения MD5 */              //
                WriteFile ( hDstFile, (BYTE*) md5.Value() , 16 /* 128 бит */, \
                    &BytesCount.Written, NULL ) || /* Выход при ошибке */ __LEAVE;          //
                                                                                            //
                fResult = TRUE;  /*  Признак успешного завершения прроцесса упаковки */     //
            }                                                                               //
                                                                                            //
            __FINALY_BLOCK  {   /*  Блок завершения контроля файловых операция */           //
                                                                                            //
                /*  Закрытие файловых манипуляторов «по необходимости» */                   //
                                                                                            //
                hSrcFile != INVALID_HANDLE_VALUE && CloseHandle ( hSrcFile );               //
                hDstFile != INVALID_HANDLE_VALUE && CloseHandle ( hDstFile );               //
                                                                                            //
                //  Если упаковка файла успешна - он удаляется, остаётся только архив,      //
                //  если возникли ошибки - удаляется возможно повреждённый архив            //

                fResult && ( DeleteFile ( (PTSTR) (BYTE*) pszFilePathSrc ), 1L ) || \
                    DeleteFile ( (PTSTR) (BYTE*) FilePathDst );                             //
            }                                                                               //
                                                                                            //
            return fResult;  /*  Результирующее значение вызова */                          //
        }                                                                                   //

        //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //
        
        /*  Распаковка отдельного блока данных, сжатого вызовом «CompressBuffer» */         //
                                                                                            //
        Buffer UncompressBuffer ( __in const Buffer & Compressed ) const {                  //
                                                                                            //
            ULONG UncompressedSize = * (ULONG*) (BYTE*) Compressed; /* Хранимый рамер */    //
            ULONG uKeepLength = 0;  /*  Актуальная длина распакованных данных */            //
                                                                                            //
            Buffer Uncompressed ( UncompressedSize );   /* Буфер для распаковки */          //
                                                                                            //
            //  Обращение через указатель к Native API для распаковки блока данных          //
                                                                                            //
            NTSTATUS ntStatus = CallPtr ( __UseRef ExternalCalls __RefItem(2), \
                __Assign( RtlDecompressBufferEx ), __Params Params.uFormatAndEngine, \
            Uncompressed, Uncompressed.Size(), (BYTE*) Compressed + sizeof(ULONG), \
                Compressed.Size() - sizeof(ULONG), &uKeepLength, (BYTE*) Workspace );       //
                                                                                            //
            //  Установка актуального размера распакованного буфера ( 0 - при ошибке )      //
            SUCCEEDED( ntStatus ) && ( uKeepLength == UncompressedSize) || \
                ( uKeepLength ^= uKeepLength ); /* Сброс буфера при ошибке */               //
                                                                                            //
            Uncompressed.CropOrExpand ( uKeepLength );  /* Актуализация размера буфера */   //
                                                                                            //
            return Uncompressed;  /* Передача распакованного буфера вызывающей стороне */   //
        }                                                                                   //

        //  ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙  //

        //  Распаковка файла назначенным компрессором ( «uFormatAndEngine» )                //
                                                                                            //
        BOOL UncompressFile ( __in __nullterminated const PTSTR pszFilePathSrc ) {          //
                                                                                            //
            BOOL fResult = FALSE;  /*  Результирующее, возвращаемое значение */             //
                                                                                            //
            PTSTR pszFileExt = NULL;  /*  Указатель на расширение имени файла */            //
                                                                                            //
            FIND_EXTENSION( pszFileExt, pszFilePathSrc );   /*  Поиск расширения */         //
                                                                                            //
            //  Если pszFileExt == NULL - имя файла не содержит расширение                  //
                                                                                            //
            UINT cchNotable = pszFileExt == NULL ? /*  «Значимые» символы пути … */ \
                lstrlen ( pszFilePathSrc ) : (UINT) ( pszFileExt - pszFilePathSrc - 1 );    //
                                                                                            //
            /*  Выделение буфера, достаточного для хранения части пути + «\0x00» */         //
            Buffer FilePathDst ( ( cchNotable + 1 ) << ( sizeof(TCHAR) >> 1 ) );            //
            /*  Перенос фрагмента пути ( без расширения ), расширение добавим позже */      //
            lstrcpyn ( (PTSTR) (BYTE*) FilePathDst, pszFilePathSrc, cchNotable + 1 );       //
                                                                                            //
            Buffer OriginalExt( 0 );  /*  Оригинальное ( сохранённое ) расширение файла */  //
                                                                                            //
            HANDLE hSrcFile = INVALID_HANDLE_VALUE;  /*  Хэндл сжатого файла */             //
            HANDLE hDstFile = INVALID_HANDLE_VALUE;  /*  Хэндл распакованного файла */      //
                                                                                            //
            __TRY_FINALLY {   /*  Блок контроля файловых операций ввода / вывода */         //
                                                                                            //
                struct ArchHeader {  /* Заголовок архива ( параметры для распаковки ) */    //
                                                                                            //
                    USHORT uFormatAndEngine = 0;   /*  Формат архива и степень сжатия */    //
                    Buffer FileExt;                 /*  ANSI-представление расширения */    //
                    UINT uFileSize = 0;             /*  Размер файла после распаковки */    //
                    Buffer hash;            /*  MD5 хеш-значение по содержимому файла */    //
                                                                                            //
                    UINT Beyond;     /* Смещение упакованных блоков ( за заголовком ) */    //
                }                                                                           //
                  Header;   /*  Параметры архива, полученные из прочитанного заголовка */   //
                                                                                            //
                ( Header.Beyond = GetArchFileInfo  /* Чтение заголовка архива */ \
                    ( pszFilePathSrc, &Header.uFormatAndEngine, Header.FileExt, \
                        &Header.uFileSize, Header.hash ) ) || __LEAVE;  /* Выход … */       //
                                                                                            //
                /*  Сравнение текущего упаковщика и применённого к архиву … */              //
                ( Header.uFormatAndEngine == this -> Params.uFormatAndEngine ) || \
                    __LEAVE  /* … выход, если текущий распаковщик не соответствует */;      //
                                                                                            //
                OriginalExt.CropOrExpand ( /* Установка размера буфера */ ( lstrlenA \
                    ( (PSTR) (BYTE*) Header.FileExt ) + 1 ) << ( sizeof(TCHAR) >> 1 ) );    //

                #ifdef UNICODE  /*  В Unicode-компоновке расширение нужно декодировать */   //
                                                                                            //
                    /*  При отсутствии расширения сохраняется только нуль-терминатор */     //
                    OriginalExt.Size() == 1 || /*  Иначе - преобразование … */ \
                        MultiByteToWideChar ( CP_ACP, 0, (CHAR*) (BYTE*) Header.FileExt, \
                            -1, (PWSTR) (BYTE*) OriginalExt, OriginalExt.Size() >> 1 );     //
                #else                                                                       //
                                                                                            //
                    OriginalExt.WriteChunk  /*  Для ANSI-компоновки - скопировать */ \
                        ( 0, Header.FileExt.Size(), (BYTE*) Header.FileExt );               //
                                                                                            //
                #endif                                                                      //
                                                                                            //
                FilePathDst.CropOrExpand /*  Подготовка размера буфера … */ \
                    ( FilePathDst.Size() + lstrlen( (PTSTR) (BYTE*) OriginalExt) + 1 );     //
                                                                                            //
                /*  Добавление расширения оригинального файла к фрагменту пути */           //
                lstrcat ( (PTSTR) (BYTE*) FilePathDst, _T(".") );  /*  Точка */             //
                lstrcat ( (PTSTR) (BYTE*) FilePathDst, (PTSTR) (BYTE*) OriginalExt );       //
                                                                                            //
                hSrcFile = CreateFile ( pszFilePathSrc, GENERIC_READ, \
                    FILE_SHARE_READ, ( _SECURITY_ATTRIBUTES *) NULL, OPEN_EXISTING, \
                        FILE_ATTRIBUTE_NORMAL, NULL );  /*  Открытие файла источника */     //
                                                                                            //
                hDstFile = CreateFile ( (PTSTR) (BYTE*) FilePathDst, \
                    GENERIC_READ | GENERIC_WRITE, 0, ( _SECURITY_ATTRIBUTES *) NULL, \
                        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ); /* … и назначения */  //
                                                                                            //
                /*  Выход при возникших ошибках доступа к файлам  */                        //
                ( BAD_FILE( hSrcFile ) || BAD_FILE( hDstFile ) ) && __LEAVE;                //
                                                                                            //
                Buffer ReadBuf( 0 ), WriteBuf( 0 );   /* Буфер чтения и записи */           //
                MessageDigest md5;  /*  Подсчёт контрольной суммы распакованного файла */   //
                                                                                            //
                struct {  /*  Статистика файловых операций */                               //
                                                                                            //
                    DWORD Read = 0;     /*  Байтовый счётчик операции чтения */             //
                    DWORD Written = 0;  /*  Байтовый счётчик операции записи */             //
                    DWORD Total = 0;    /*  Количество байт для обработки */                //
                    DWORD Current = 0;  /*  Общее количество обработанных байт */           //
                }                                                                           //
                  BytesCount;   /*  Счётчики байт */                                        //
                                                                                            //
                /*  Запрос размера исходного файла ( пустые - будут пропущены ) */          //
                ( BytesCount.Total = GetFileSize ( hSrcFile, NULL ) ) || __LEAVE;           //
                                                                                            //
                SetFilePointer ( hSrcFile, Header.Beyond, /*  Переход к 1-му блоку */ \
                    NULL, FILE_BEGIN ) != INVALID_SET_FILE_POINTER || __LEAVE ;             //
                                                                                            //
                BytesCount.Current += Header.Beyond;  /* Заголок считаем прочитанным */     //
                                                                                            //
                ULONG uBlockSize = 0;  /*  Длина упакованного блока ( для записи ) */       //
                                                                                            //
                for ( ; /* Обход файла … */ BytesCount.Current < BytesCount.Total; \
                                                                                            //
                ReadFile ( hSrcFile, /* Чтение длины упакованного блока … */ \
                    &uBlockSize, sizeof(ULONG), &BytesCount.Read, NULL) && \
                                                                                            //
                BytesCount.Read == sizeof(ULONG) /* Если успешно … */ && \
                    ( BytesCount.Current += BytesCount.Read ) /* + к счётчику байт */ && \
                ( ReadBuf.CropOrExpand ( uBlockSize ), 1L) /* Установка размера */ && \
                                                                                            //
                ReadFile ( hSrcFile, /* Чтение данных упакованного блока по размеру … */ \
                    (BYTE*) ReadBuf, ReadBuf.Size(), &BytesCount.Read, NULL) && \
                                                                                            //
                BytesCount.Read == uBlockSize &&  /* Блок считан полностью … */ \
                    ( BytesCount.Current += BytesCount.Read ) /* + к счётчику байт */ && \
                ( uBlockSize = * (ULONG*) (BYTE*) ReadBuf /* Ожидаемая длина … */ ) && \
                    ( WriteBuf = this -> UncompressBuffer(ReadBuf) ) /* Распаковка … */ && \
                WriteBuf.Size() == uBlockSize &&  /* Проверка успешности распаковки … */ \
                                                                                            //
                ( md5.Append ( WriteBuf ), /*  Хеш-значение распакованного блока … */ \
                                                                                            //
                WriteFile ( hDstFile, (BYTE*) WriteBuf, /* Запись распакованного блока */ \
                    WriteBuf.Size(), &BytesCount.Written, 0 ) ) || __LEAVE );
                                                                                            //
                FlushFileBuffers ( hDstFile );  /*  Сброс кеша записи … */                  //
                ULONG uSizeCreated = GetFileSize ( hDstFile, NULL ) /*  Новый размер */ ;   //
                                                                                            //
                fResult = uSizeCreated == Header.uFileSize &&  /* Проверка размера */ \
                    md5.Value() == Header.hash;  /*  Проверка хеш-значения MD5 */           //
            }                                                                               //
                                                                                            //
            __FINALY_BLOCK  {   /*  Блок завершения контроля файловых операция */           //
                                                                                            //
                /*  Закрытие файловых манипуляторов «по необходимости» */                   //
                                                                                            //
                hSrcFile != INVALID_HANDLE_VALUE && CloseHandle ( hSrcFile );               //
                hDstFile != INVALID_HANDLE_VALUE && CloseHandle ( hDstFile );               //
                                                                                            //
                //  Если распаковка файла успешна - архив удаляется, если возникли          //
                //  ошибки - удаляется возможно частично распакованный файл                 //
                                                                                            //
                fResult && ( DeleteFile ( (PTSTR) (BYTE*) pszFilePathSrc ), 1L ) || \
                    DeleteFile ( (PTSTR) (BYTE*) FilePathDst );                             //
            }                                                                               //
                                                                                            //
            return fResult;  /*  Результирующее значение вызова */                          //
        }                                                                                   //
    };                                                                                      //
    
    //  ÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷÷  //
    
    /*  Разрешение зависимостей по внешним вызовам ( библиотека «NTDLL.DLL» ) */            //
                                                                                            //
    COMPLY_DEPENDENCY( Compressor, Dependencies, \
        MODULE_IMPORT, SysNtLayer, NTDLL.DLL, MODULE_REFS, ExternalCalls, \
            PROC( RtlGetCompressionWorkSpaceSize ), \
                PROC( RtlCompressBuffer ), PROC( RtlDecompressBufferEx ), \
            PROC( MD5Init ), PROC( MD5Update ), PROC( MD5Final ) );                         //
    
    //  ▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪  //
    
    