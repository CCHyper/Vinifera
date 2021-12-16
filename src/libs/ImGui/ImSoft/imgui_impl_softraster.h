#pragma once

struct texture_base_t;
struct ImDrawData;

#include "softraster/softraster_texture.h"

IMGUI_IMPL_API bool     ImGui_ImplSoftraster_Init(texture_base_t *screen);
IMGUI_IMPL_API void     ImGui_ImplSoftraster_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplSoftraster_NewFrame();
IMGUI_IMPL_API void     ImGui_ImplSoftraster_RenderDrawData(ImDrawData* draw_data);
