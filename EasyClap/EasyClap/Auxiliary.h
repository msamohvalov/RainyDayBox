

    //  ▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪  //

    #pragma once    //  Include-Guard подключаемого модуля                                  //
                                                                                            //
    //  Вспомогательный конструкции препроцессора для сокращения синтаксиса языка           //
                                                                                            //
    #define __has_no_params VOID        /*  Объявление функции без параметров */            //
    #define __no_args  __null           /*  Вызову не передаются аргументы */               //
                                                                                            //
    #define __EMPTY_FUNCTION_BODY_IMPLEMENTATION { NULL; }  /*  Пустое тело реализации */   //
                                                                                            //
    #define YIELD(field)    { return this -> field; }   /*  Возвращает значение поля */     //
    #define RETURNS(expr)   { return (expr); }          /*  Возвращает выражение */         //
    #define EMIT(expr)      return (expr);              /*  Оператор RETURN */              //
    #define APPLY(expr)     { expr; }                   /*  Блок операторов */              //
                                                                                            //
    //  Декорирование блоков SEH                                                            //
                                                                                            //
    //  Вариант №1                                                                          //
                                                                                            //
    #define __TRY_EXCEPT_FINALLY    __try { __try       //  Начало SEH-кадра                //
    #define __EXCEPT(cond)          __except (cond)     //  Блок обработки исключений       //
    #define __FINALLY               } __finally         //  Блок завершения SEH-кадра       //
                                                                                            //
    //  Вариант №2                                                                          //
                                                                                            //
    #define __TRY_FINALLY           __try { __try       //  Начало SEH-кадра                //
                                                                                            //
    /*  SEH-кадр - сопровождается только ( исключительно ) блоком Finally */                //
    #define __FINALY_BLOCK      __except (EXCEPTION_EXECUTE_HANDLER) { } } __finally        //
                                                                                            //
    /*  Принудительный выход из SEH-блока через генерацию исключения */                     //
    #define __LEAVE     ( RaiseException( ERROR_ASSERTION_FAILURE, 0, 0, NULL ), 1L )       //
                                                                                            //
    /*  Проверка хендла файла после попытки его открытия */                                 //
    #define BAD_FILE(h)     ( ((INT_PTR) h) ^ ( (INT_PTR) INVALID_HANDLE_VALUE ) ) == 0     //
                                                                                            //
    #define _En(...) ( __VA_ARGS__ )    /*  Подсчёт количества аргументов,    */            //
    #define _E1(x) x                    /*  переданных вариадическому макросу */            //
                                                                                            //
    //  Подсчёт только до 10 параметров ( можно расширить )                                 //
                                                                                            //
    #define VA_COUNT_VALUE( _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _ , ... ) _            //
                                                                                            //
    #define VA_ARGS_COUNT(...) \
        _E1(VA_COUNT_VALUE)_En( 0 __VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 )              //
                                                                                            //
    //  Динамическое создание объекта с «умным» указателем                                  //
                                                                                            //
    #define DYNCREATE_OBJ( ObjClass, ... ) \
        CComPtr< ObjClass > ( new ObjClass ( ##__VA_ARGS__ ) )                              //
                                                                                            //
    //  Макрос для перехвата ошибок, возникающих в конструкторе                             //
    //  динамически создаваемых объектов ( признак ошибки - NULL-указатель )                //
                                                                                            //
    #define GUIDED_OBJECT_CREATION(obj_create) { \
        __try { return ( obj_create ); } __except ( EXCEPTION_EXECUTE_HANDLER ) \
            { SetLastError ( _exception_code() ); return NULL; } }                          //
                                                                                            //
    //  Перехват ошибок деструктора динамически созданных объектов                          //
                                                                                            //
    #define GUIDED_OBJECT_DELETION(obj_delete) { \
        __try { return ( obj_delete ), TRUE; } __except ( EXCEPTION_EXECUTE_HANDLER ) \
            { SetLastError ( _exception_code() ); return FALSE; } }                         //

    //  ▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪▪  //

    