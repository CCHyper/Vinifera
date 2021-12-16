#include "developer_overlay.h"
#include "tibsun_globals.h"
#include "scenario.h"
#include "imgui.h"


/**
 *  ImGui globals
 */
//static ImVec4 ImGui_ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


bool Scenario_Show = false;
bool Special_Show = false;

bool Object_Show = false;


void Developer_Overlay_Process()
{
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Scenario");

#if 1
    //ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

    #define LIGHTING_SLIDER_FLOAT(name, scen_val, float_val) \
        { \
            float float_val = float(scen_val/1000); \
            if (ImGui::SliderFloat(name, &float_val, -5.0f, 5.0f)) { \
                scen_val = float_val*1000.0; \
                Scen->IsAmbientChanged = true; \
            } \
        }

    LIGHTING_SLIDER_FLOAT("AmbientOriginal", Scen->AmbientOriginal, _AmbientOriginal);
    LIGHTING_SLIDER_FLOAT("AmbientCurrent", Scen->AmbientCurrent, _AmbientCurrent);
    LIGHTING_SLIDER_FLOAT("AmbientTarget", Scen->AmbientTarget, _AmbientTarget);

    LIGHTING_SLIDER_FLOAT("Red", Scen->Red, _Red);
    LIGHTING_SLIDER_FLOAT("Green", Scen->Green, _Green);
    LIGHTING_SLIDER_FLOAT("Blue", Scen->Blue, _Blue);
    LIGHTING_SLIDER_FLOAT("Ground", Scen->Ground, _Ground);
    LIGHTING_SLIDER_FLOAT("Level", Scen->Level, _Level);

    LIGHTING_SLIDER_FLOAT("IonAmbient", Scen->IonAmbient, _IonAmbient);
    LIGHTING_SLIDER_FLOAT("IonRed", Scen->IonRed, _IonRed);
    LIGHTING_SLIDER_FLOAT("IonGreen", Scen->IonGreen, _IonGreen);
    LIGHTING_SLIDER_FLOAT("IonBlue", Scen->IonBlue, _IonBlue);
    LIGHTING_SLIDER_FLOAT("IonGround", Scen->IonGround, _IonGround);
    LIGHTING_SLIDER_FLOAT("IonLevel", Scen->IonLevel, _IonLevel);

    ImGui::Checkbox("InputLocked", &Scen->UserInputLocked);
#endif

    //if (ImGui::Button("Button")) {                          // Buttons return true when clicked (most widgets return true when edited/activated)
    //    counter++;
    //}
    //ImGui::SameLine();
    //ImGui::Text("counter = %d", counter);

    //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();
}