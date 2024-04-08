
#pragma once

#ifndef SUBTITLEMANAGER_H
#define SUBTITLEMANAGER_H

#include "always.h"
#include "vector.h"
#include "wstring.h"

class SubTitleClass;
class FontClass;
class Surface;

class SubTitleManagerClass
{
	public:
		// Instantiate a subtitle manager
		static SubTitleManagerClass * Create(const Wstring &filename, const Wstring &subtitlefilename, FontClass * font);

		// Destroy subtitle manager
		~SubTitleManagerClass();

		// Check if there are subtitles.
		bool Has_Sub_Titles(void) const { return (SubTitles != NULL); }

		// Reset subtitles to start
		void Reset(void);

		// Process subtitles
		bool Process(unsigned long movieTime);
		void Render();

	private:
		// Prevent direct creation
		SubTitleManagerClass();

		void Set_Font(FontCharsClass* font);

		bool Load_Sub_Titles(const char* moviename, const char* subtitlefilename);
		void Draw_Sub_Title(const SubTitleClass* subtitle);

		DynamicVectorClass<class SubTitleClass*>* SubTitles;
		int SubTitleIndex;
		SubTitleClass* ActiveSubTitle;
		Render2DSentenceClass Renderer;
};

#endif
