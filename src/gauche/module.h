/*
 * module.h - Modules
 *
 *   Copyright (c) 2000-2009  Shiro Kawai  <shiro@acm.org>
 * 
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 * 
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   3. Neither the name of the authors nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  $Id: gauche.h,v 1.508 2008-06-02 01:13:13 shirok Exp $
 */

#ifndef GAUCHE_MODULE_H
#define GAUCHE_MODULE_H

/*
 * A module keeps "toplevel environment", which maps names of free
 * variables (symbols) to a location (GLOCs).
 */

struct ScmModuleRec {
    SCM_HEADER;
    ScmObj name;                /* symbol or #f */
    ScmObj imported;            /* list of imported modules.  each element
                                   may be #<module> or (#<module> . prefix) */
    ScmObj exported;            /* list of exported symbols */
    int    exportAll;           /* TRUE if (export-all) */
    ScmObj parents;             /* direct parent modules */
    ScmObj mpl;                 /* module precedence list */
    ScmObj depended;            /* list of modules that are depended by this
                                   module for compilation */
    ScmHashTable *table;        /* binding table */
};

#define SCM_MODULE(obj)       ((ScmModule*)(obj))
#define SCM_MODULEP(obj)      SCM_XTYPEP(obj, SCM_CLASS_MODULE)

SCM_CLASS_DECL(Scm_ModuleClass);
#define SCM_CLASS_MODULE     (&Scm_ModuleClass)

SCM_EXTERN ScmObj Scm_MakeModule(ScmSymbol *name, int error_if_exists);

/* Flags for Scm_FindBinding and Scm_GlobalVariableRef */
enum {
    /* do not search parent/imported */
    SCM_BINDING_STAY_IN_MODULE = (1L<<0)
};

SCM_EXTERN ScmGloc *Scm_FindBinding(ScmModule *module, ScmSymbol *symbol,
				    int flags);
SCM_EXTERN ScmObj Scm_GlobalVariableRef(ScmModule *module,
                                        ScmSymbol *symbol,
                                        int flags);

SCM_EXTERN ScmObj Scm_Define(ScmModule *module, ScmSymbol *symbol,
			     ScmObj value);
SCM_EXTERN ScmObj Scm_DefineConst(ScmModule *module, ScmSymbol *symbol,
                                  ScmObj value);

SCM_EXTERN ScmObj Scm_ExtendModule(ScmModule *module, ScmObj supers);
SCM_EXTERN ScmObj Scm_ImportModule(ScmModule *module, ScmObj imported,
                                   ScmObj prefix, u_long flags);
SCM_EXTERN ScmObj Scm_ImportModules(ScmModule *module, ScmObj list);/*deprecated*/
SCM_EXTERN ScmObj Scm_ExportSymbols(ScmModule *module, ScmObj list);
SCM_EXTERN ScmObj Scm_ExportAll(ScmModule *module);
SCM_EXTERN ScmModule *Scm_FindModule(ScmSymbol *name, int flags);
SCM_EXTERN ScmObj Scm_AllModules(void);
SCM_EXTERN void   Scm_SelectModule(ScmModule *mod);

/* Flags for Scm_FindModule
   NB: Scm_FindModule's second arg has been changed since 0.8.6;
   before, it was just a boolean value to indicate whether a new
   module should be created (TRUE) or not (FALSE).  We added a
   new flag value to make Scm_FindModule raises an error if the named
   module doesn't exist.  This change should be transparent as far
   as the caller's using Gauche's definition of TRUE. */
enum {
    SCM_FIND_MODULE_CREATE = 1, /* Create if there's no named module */
    SCM_FIND_MODULE_QUIET  = 2  /* Do not signal an error if there's no
                                   named module, but return NULL instead. */
};

#define SCM_FIND_MODULE(name, flags) \
    Scm_FindModule(SCM_SYMBOL(SCM_INTERN(name)), flags)

SCM_EXTERN ScmObj Scm_ModuleNameToPath(ScmSymbol *name);
SCM_EXTERN ScmObj Scm_PathToModuleName(ScmString *path);

SCM_EXTERN ScmModule *Scm_NullModule(void);
SCM_EXTERN ScmModule *Scm_SchemeModule(void);
SCM_EXTERN ScmModule *Scm_GaucheModule(void);
SCM_EXTERN ScmModule *Scm_UserModule(void);
SCM_EXTERN ScmModule *Scm_CurrentModule(void);

#define SCM_DEFINE(module, cstr, val)           \
    Scm_Define(SCM_MODULE(module),              \
               SCM_SYMBOL(SCM_INTERN(cstr)),    \
               SCM_OBJ(val))

/* OBSOLETED */
#define Scm_SymbolValue(m, s) Scm_GlobalVariableRef(m, s, FALSE)
/* OBSOLETED */
#define SCM_SYMBOL_VALUE(module_name, symbol_name)                      \
    Scm_SymbolValue(SCM_FIND_MODULE(module_name, 0),                    \
                    SCM_SYMBOL(SCM_INTERN(symbol_name)))


#endif /*GAUCHE_MODULE_H*/

