#include "methodsx86.h"

MethodsX86::MethodsX86()
{

}

void MethodsX86::Assemble(Assembler *as, AbstractASTDispatcher *dispatcher)
{
    m_dispatcher = dispatcher;
/*    if (Command("Writeln")) {
        as->Writeln();

        m_node->m_params[0]->Accept(dispatcher);
        as->EndWriteln();
    }
*/
    if (Command("waitforverticalblank"))
        WaitForVerticalBlank(as);

    if (Command("init_cga_scanlines"))
        AddInitMethod(as,"init_cga_scanlines","init_cga_scanlines.asm");

    if (Command("setscreenmode")) {
        LoadVar(as,0);
        as->Asm("mov ah,0");
        as->Asm("int $10");
    }
    if (Command("VGAclearScreen16")) {
        ClearScreen(as,16);
    }
    if (Command("VGAclearScreen32")) {
        ClearScreen(as,32);
    }
    if (Command("scrollx")) {
        ScrollX(as);
    }
    if (Command("setpalette")) {
        as->Asm("mov dx,3c8h");
        LoadVar(as,0);
        as->Asm("out dx,al");
        as->Asm("inc dx");
        LoadVar(as,1);
        as->Asm("out dx,al");
        LoadVar(as,2);
        as->Asm("out dx,al");
        LoadVar(as,3);
        as->Asm("out dx,al");
        as->Asm("Sti");
    }
    if (Command("setpixel")) {
/*        Procedure Putpixel (X,Y : Integer; Col : Byte; P :Pointer);
        BEGIN
          If (X>0) and (X<=319) and (Y>0) and (Y<=200) then
          Asm*/
            as->Asm("Cli");
//            as->Asm("mov    ax,$a000");
            LoadVar(as,0);

            as->Asm("mov    es,ax");
            as->Asm("mov    di,0");
//            as->Asm("Les     Di,$a000");
            LoadVar(as,1);
            as->Asm("mov     bx,ax");
            LoadVar(as,2);
            as->Asm("mov     dx,ax");
            as->Asm("Add     di,bx");
            as->Asm("mov     bx, dx ");
            as->Asm("shl     dx, 8");
            as->Asm("shl     bx, 6");
            as->Asm("add     dx, bx");
            as->Asm("add     di, dx");
            as->Asm("Dec     Di");
 //           as->Asm("mov     al, [Col]");
            LoadVar(as,3);
            as->Asm("stosb");
            as->Asm("Sti ");
//                    End;
//        END;

    }
    if (Command("setpixelcga")) {
        LoadVar(as,3);
//        as->Asm("mov al,ah");
        as->Asm("mov [table_cga_scanlines_x],al");
//        as->Asm("push ah");
        LoadVar(as,0);
//        as->Asm("mov ax,0");
//        as->Asm("sta ds,ax");
        as->Asm("mov  es, ax");
//        as->Asm("mov  ax, x");
        LoadVar(as,1); // Load x into ax
        as->Asm("mov  di, ax");
        as->Asm("shr  di, 1");
        as->Asm("shr  di, 1");
//        as->Asm("mov  bx, y");
        dynamic_cast<ASTDispatcherX86*>(m_dispatcher)->PushX();
        LoadVar(as,2); // Load x into bx
        dynamic_cast<ASTDispatcherX86*>(m_dispatcher)->PopX();
        as->Asm("shl  bx, 1");
        as->Asm("add  di, [bx + table_cga_scanlines]");
/*        as->Asm("push ax");
        as->Asm("mov ax, 80");
        as->Asm("mul bx");
        as->Asm("add  di, ax");
        as->Asm("pop ax");
*/
        as->Asm("and  ax, 3");
        as->Asm("mov  bx, table_cga_masks");
        as->Asm("xlat");
//        as->Asm("mov  ah, 1"); // Fix colour later
//        as->Asm("pop ah"); // Get colour back
        as->Asm("mov ah,[table_cga_scanlines_x]");
//        as->Asm("mov ah,$FF");

        as->Asm("and  ah, al");
        as->Asm("not  al");
        as->Asm("and  al, [es:di]");
        as->Asm("or   al, ah");
        as->Asm("stosb");

    }

}

bool MethodsX86::Command(QString name)
{
    return m_node->m_procName.toLower() == name.toLower();

}

void MethodsX86::LoadVar(Assembler *as, int paramNo)
{
    m_node->m_params[paramNo]->Accept(m_dispatcher);

}

void MethodsX86::ClearScreen(Assembler *as, int bits)
{
    if (bits==16) {
        as->Asm("mov ax, $A000");
        as->Asm("mov es, ax");
        LoadVar(as,0);

        as->Asm("mov bh,al");
        as->Asm("mov bl,al");
        as->Asm("mov ax, bx");
        as->Asm("xor di, di");
        as->Asm("mov cx, 32000");
        as->Asm("rep stosw");

    }
    if (bits==32) {
        as->Asm("mov ax, $A000");
        as->Asm("mov es, ax");
        LoadVar(as,0);

        as->Asm("mov bh,al");
        as->Asm("mov bl,al");
        as->Asm("shl ebx,16");
        as->Asm("mov bh,al");
        as->Asm("mov bl,al");
        as->Asm("mov eax, ebx");
        as->Asm("xor di, di");
        as->Asm("mov cx, 16000");
        as->Asm("rep stosd");
    }

}

void MethodsX86::WaitForVerticalBlank(Assembler *as)
{
    QString l1 = as->NewLabel("vblank1");
    QString l2 = as->NewLabel("vblank2");

    as->Asm("mov dx, $3da");
    as->Label(l1);
    as->Asm("in al, dx");
    as->Asm("test al, 8");
    as->Asm("je "+l1);
    as->Label(l2);
    as->Asm("in al, dx");
    as->Asm("test al, 8");
    as->Asm("jne "+l2);

    as->PopLabel("vblank2");
    as->PopLabel("vblank1");
}

void MethodsX86::ScrollX(Assembler *as)
{
    LoadVar(as,0);
    as->Asm("push ax");
    as->Asm("mov dx,$3d4");
    as->Asm("mov al,$0d");
    as->Asm("out dx,al");
    as->Asm("inc dx");
    as->Asm("pop ax");
    as->Asm("out dx,al");
    as->Asm("dec dx");
    as->Asm("mov al,$00");
    as->Asm("out dx,al");
    as->Asm("inc dx");
    as->Asm("mov al,ah");
    as->Asm("out dx,al");

}

void MethodsX86::AddInitMethod(Assembler *as, QString name, QString file)
{
    if (m_node->m_isInitialized[name])
        return;

    file = ":resources/code/x86/"+file;

    as->IncludeFile(file);
    m_node->m_isInitialized[name] = true;


}