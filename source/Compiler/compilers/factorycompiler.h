#ifndef FACTORYCOMPILER_H
#define FACTORYCOMPILER_H

#include "compiler6502.h"
#include "compilerm68k.h"
#include "compilerx86.h"

class FactoryCompiler
{
public:
    FactoryCompiler();
    static Compiler* CreateCompiler(CIniFile* ini, CIniFile* pIni);
};

#endif // FACTORYCOMPILER_H