
#pragma once

#ifndef SUBTITLEPARSER_H
#define SUBTITLEPARSER_H

#include "always.h"
#include "vector.h"
#include "wstring.h"

class Straw;
class SubTitleClass;

class SubTitleParserClass
{
    public:
        SubTitleParserClass(Straw & input);
        ~SubTitleParserClass();

        DynamicVectorClass<SubTitleClass *> * Get_Sub_Titles(const char * moviename);

    private:
        enum { LINE_MAX = 1024 };

        typedef struct tagTokenHook
        {
            const wchar_t* Token;
            void (*Handler)(wchar_t * param, SubTitleClass * subTitle);
        } TokenHook;

        bool Find_Movie_Entry(const char * moviename);
        bool Parse_Sub_Title(wchar_t * string, SubTitleClass * subTitle);
        void Parse_Token(wchar_t * token, wchar_t * param, SubTitleClass * subTitle);
        Wstring Get_Next_Line(void);
        unsigned int Get_Line_Number(void) const { return LineNumber; }

        static TokenHook TokenHooks[];
        Straw & Input;
        char Buffer[LINE_MAX];
        unsigned int LineNumber;

    private:
        // Prevent copy construction
        SubTitleParserClass(const SubTitleParserClass&);
        const SubTitleParserClass operator=(const SubTitleParserClass&);
};

#endif
