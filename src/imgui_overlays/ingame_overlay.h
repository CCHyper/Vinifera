#pragma once

#include "always.h"


class XSurface;


namespace InGameOverlay
{

typedef enum OverlayMode
{
    INGAME_OVERLAY_NONE,

    INGAME_OVERLAY_DEVELOPER,
    INGAME_OVERLAY_SCENARIO_EDITOR,
} OverlayMode;

extern bool IsInitialised;
extern OverlayMode Mode;

bool Init();
void Shutdown();

void Process();
void Render(XSurface *surface);
void Render_To_Surface(XSurface *surface);

};
