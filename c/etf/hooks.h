/** \file hooks.h
    \brief Hook interface for modified dynamic linker (glibc-2.2.5)
*/
/*!
    \mainpage Dynamic Dynamic Loader's hook interface
    \section intro_sec Introduction
    The GNU dynamic loader has been modified to provide the basic functionality
    of DDL-framework: events. As mentioned earlier, events are nothing but
    dynamic link requests generated by application binary or the externally
    linked shared objects it is dependent on. It includes system C library
    as well as programmer defined shared libraries (.so files) The modifications
    to dynamic linker are known as hooks. Hooks are nothing but hardcoded
    external function calls with definite behavior. DDL-framework gets access
    to link resolution information through these hooks. This information
    constitutes of symbol getting resolved, GOT entry of caller library,
    definition symbol, its address etc. Modified dynamic linker, (we call it
    DDL (Dynamic Dynamic Linker)) is itself a reusable peace of software as
    every hook has fixed behavior and the hook interface has been defined here.

    \section implementation_sec Hook interface
        \li hooks.h defines modified linker's hook interface

    \htmlinclude footer.html


*/
#ifndef __HOOKS_H
#define __HOOKS_H

/** \brief A constant representing library lookup (force) event.

    We can force linker to load a symbol from a particular library. This
    event is generated when linker comes in do-lookup.h to resolve a symbol
    and we actually force it to load from particular library. This event is
    invisible to tools.*/
const int _EVENT_FORCE_LIBLOOKUP=-10;
/** \brief A constant representing event of calling _dl_redirect_lookup hook.

    Just before calling \e _dl_redirect_lookup hook, \e _dl_redirect_isactive
    hook is called with \e EVENT_LOOKUP as a parameter. This event is
    visible to tools.*/
const int EVENT_LOOKUP=10;
/** \brief A constant representing event of calling _dl_redirect_definition hook.

    Just before calling \e _dl_redirect_definition hook, \e _dl_redirect_isactive
    hook is called with \e EVENT_DEFINITION as a parameter. This event is
    visible to tools.*/
const int EVENT_DEFINITION=20;
/** \brief A constant representing event of calling _dl_redirect_offset hook.

    Just before calling \e _dl_redirect_offset hook, \e _dl_redirect_isactive
    hook is called with \e EVENT_OFFSET as a parameter. This event is
    visible to tools.*/
const int EVENT_OFFSET=30;
/** \brief A constant representing event of calling _dl_redirect_symdef hook.

    Just before calling \e _dl_redirect_symdef hook, \e _dl_redirect_isactive
    hook is called with \e EVENT_SYMDEF as a parameter. This event is
    visible to tools.*/
const int EVENT_SYMDEF=40;

//! To initialise hook implementation library.
/*! Hook implementation library must export a function symbol
    having the same signature. It is a \b callback function
    called by DDL(Dynamic Dynamic Linker). This hook is called only once
    to provide a chance to initialise hook implementation library.
\return Nothing.
\sa _dl_redirect_isactive(), _dl_redirect_lookup(), _dl_redirect_offset()
\sa _dl_redirect_definition(), _dl_redirect_symdef()
*/
void  _dl_redirect_init(void);

//! To check whether hook implementation library is 'active' or not.
/*! Hook implementation library must export a function symbol
    having the same signature. It is a \b callback function called
    by DDL(Dynamic Dynamic Linker). This hook is called everytime
    prior to invoking any other hook. If it returns zero, next immediate
    hook is not invoked by DDL.
\param eventtype It represents next immediate hook to be invoked by
       DDL. Invocation of hooks is considered as events.
\return One if next hook callback is to be made, Zero otherwise.
\sa _dl_redirect_init(), _dl_redirect_lookup(), _dl_redirect_offset()
\sa _dl_redirect_definition(), _dl_redirect_symdef()
*/
int _dl_redirect_isactive(int eventtype);

//! To allow possible redirection of symbol. Redirection is optional.
/*! It is a \b callback function called by DDL(Dynamic Dynamic Linker)
    as a lookup hook. It is invoked prior to actual symbol lookup is performed.
    This hook gives hook implementation library a chance to redirect symbol
    pointed by \e funcname. If the hook returns the same string, no redirection
    is performed but if hook returns different symbol, then DDL resolves
    the new symbol and thus we say redirection has been done.
\param funcname The function symbol DDL is going to resolve.
\param filename The library (shared object) which is calling \e funcname function.
                and therefore, wants the symbol to be resolved.
\param lookup_libname An out parameter pointing to a library name to force the
                      DDL to search and load the symbol from a specific library.
\return Pointer to the symbol which actually gets resolved. If this symbol is different
        than the parameter \e funcname, we say redirection has been done.
\sa _dl_redirect_isactive(), _dl_redirect_init(), _dl_redirect_offset()
\sa _dl_redirect_definition(), _dl_redirect_symdef()
*/
char* _dl_redirect_lookup(char* funcname,char *filename, char **lookup_libname);

//! To add an offset (optional) into resolved symbol address.
/*! It is a \b callback function invoked by DDL(Dynamic Dynamic Linker)
    to allow hook implementation library to add an offset into address of
    resolved symbol. This hook is invoked just before updating GOT entry of
    calling shared object. It is useful especially in table-based redirection.
\param funcname The function symbol dynamic linker resolved right now.
\param libname The library (shared object) which is calling \e funcname function.
               and therefore, wants the symbol to be resolved.
\return An integer value of offset to be added in the symbol address (if at all)
        Zero otherwise.
\sa _dl_redirect_isactive(), _dl_redirect_lookup(), _dl_redirect_offset()
\sa _dl_redirect_definition(), _dl_redirect_symdef()
*/
int   _dl_redirect_offset(const char *funcname, const char *libname);

//! Hook to access GOT address, resolved symbol's definition library.
/*! It is a callback function called by DDL (Dynamic Dynamic Linker) 
    to allow hook implementation library to access link related 
    information. It is a "read-only" hook. Hook library can read 
    information such as, address of GOT entry of caller shared object, 
    library containing definition of symbol, address of resolved symbol, 
    etc..
\param new_funcname The symbol actually resolved by dynamic linker.
\param new_libname The library containing definition of symbol.
\param new_func_address Address of definition of resolved symbol.
\param orig_funcname The symbol dynamic linker started to resolve at the beginning.
                     \e lookup hook might change this symbol to
                     different one to achive redirection.
\param orig_libname The library (shared object) which is calling \e funcname function.
                    and therefore, wants the symbol to be resolved.
\param orig_func_GOT_addr The address of GOT entry of \ orig_libname to be
       updated with the address of symbol resolved by dynamic linker.
\return Returns one if function finished successfully. Zero otherwise. Unused.
\sa _dl_redirect_isactive(), _dl_redirect_lookup(), _dl_redirect_offset()
\sa _dl_redirect_init(), _dl_redirect_symdef()
*/
int   _dl_redirect_definition(const char *new_funcname,
                              const char *new_libname,
                              const unsigned *new_func_address,
                              const char *orig_funcname,
                              const char *orig_libname,
                              const unsigned *orig_func_GOT_addr);

//! \b Callback function called by dynamic linker as a symdef hook.
/*! This hook is invoked just after resolving the symbol in rtld.c 
    source of dynamic linker.
\param symbol The symbol dynamic linker is resolving right now. (elf\\rtld.c)
\param libname Shared object in which definition of symbol was found.
\param address Address of symbol.
\param caller_libname Unused.
\return Returns 1 if no error occured. Unused.
\sa _dl_redirect_isactive(), _dl_redirect_lookup(), _dl_redirect_offset()
\sa _dl_redirect_init(), _dl_redirect_init()
*/
int _dl_redirect_symdef(const char *symbol, const char *libname,
                        unsigned *address, const char* caller_libname);


#endif /* __HOOKS_H  */
