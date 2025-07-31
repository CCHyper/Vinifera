/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DEVELOPER_WND_HELPER.CPP
 *
 *  @author        CCHyper, and AI used for mass duplication
 *
 *  @brief         
 *
 *  @license       Vinifera is free software: you can redistribute it and/or
 *                 modify it under the terms of the GNU General Public License
 *                 as published by the Free Software Foundation, either version
 *                 3 of the License, or (at your option) any later version.
 *
 *                 Vinifera is distributed in the hope that it will be
 *                 useful, but WITHOUT ANY WARRANTY; without even the implied
 *                 warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *                 PURPOSE. See the GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public
 *                 License along with this program.
 *                 If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include "developer_wnd_helper.h"
#include <imgui.h>

#include "debughandler.h"
#include "asserthandler.h"

#include "tibsun_globals.h"
#include "tibsun_functions.h"
#include "vinifera_defines.h"
#include "vinifera_globals.h"
#include "aircrafttype.h"
#include "buildingtype.h"
#include "infantrytype.h"
#include "unittype.h"
#include "animtype.h"
#include "overlaytype.h"
#include "terraintype.h"
#include "bullettype.h"
#include "warheadtype.h"
#include "weapontype.h"
#include "housetype.h"
#include "isotiletype.h"
#include "unit.h"
#include "aircraft.h"
#include "building.h"
#include "infantry.h"
#include "house.h"
#include "rules.h"
#include "scenario.h"
#include "session.h"

#include "testlocomotion.h"

#include "shapeset.h"
#include "drawshape.h"
#include "dsurface.h"
#include "bsurface.h"
#include "convert.h"

#include <vector>
#include <unordered_map>
#include <d3d11.h>


extern ID3D11Device * _D3DDevice;
extern float _DrawScale;


/**
 * 
 *  Utility
 * 
 */
namespace ccstd {

template<typename T>
std::string to_string(T value)
{
    char buffer[32];
    if constexpr (std::is_same_v<T, int>)
        snprintf(buffer, sizeof(buffer), "%d", value);
    else if constexpr (std::is_same_v<T, unsigned int>)
        snprintf(buffer, sizeof(buffer), "%u", value);
    else if constexpr (std::is_same_v<T, float>)
        snprintf(buffer, sizeof(buffer), "%.3f", value);
    else if constexpr (std::is_same_v<T, double>)
        snprintf(buffer, sizeof(buffer), "%.3f", value);
    else if constexpr (std::is_same_v<T, size_t>)
        snprintf(buffer, sizeof(buffer), "%zu", value);
    else
        static_assert(sizeof(T) == 0, "Unsupported type in to_string_fallback");

    return std::string(buffer);
}

} // ccstd namespace end










typedef enum DebugImageSetType
{
    IMAGE_TYPE_CAMEO,
    IMAGE_TYPE_SHAPE,
    IMAGE_TYPE_ALPHA_SHAPE,
    IMAGE_TYPE_VOXEL,

    IMAGE_TYPE_UNKNOWN = -1
};

struct DebugImageSetKey
{
    std::string name = "";          // IniName
    int32_t id = -1;                // HeapID
    RTTIType type = RTTI_NONE;      // Class type
    DebugImageSetType imageKind = IMAGE_TYPE_UNKNOWN;

    bool operator == (const DebugImageSetKey & other) const
    {
        return name == other.name && id == other.id && type == other.type && imageKind == other.imageKind;
    }
};

struct DebugImageSet
{
    int Get_Frame_Count() const { return static_cast<int>(frames.size()); }
    int Get_Width() const { return width; }
    int Get_Height() const { return height; }

    DebugImageSet() = default;
    DebugImageSet(int w, int h) :
        width(w),
        height(h)
    {
    }

    ID3D11ShaderResourceView * Get_Frame(int index) const
    {
        if (index >= 0 && index < frames.size()) {
            return frames[index];
        }
        return nullptr;
    }

    void Add_Frame(ID3D11ShaderResourceView * srv)
    {
        frames.push_back(srv);
    }

    void Release_All()
    {
        for (ID3D11ShaderResourceView* srv : frames) {
            if (srv) {
                srv->Release();
            }
        }
        frames.clear();
    }

private:
    std::vector<ID3D11ShaderResourceView*> frames;

    int width = 0;
    int height = 0;
};

namespace std
{
    template <>
    struct hash<RTTIType> {
        std::size_t operator()(const RTTIType& type) const noexcept {
            return std::hash<int>{}(static_cast<int>(type));
        }
    };

    template <>
    struct hash<DebugImageSetType> {
        size_t operator()(const DebugImageSetType& kind) const noexcept {
            return std::hash<int>{}(static_cast<int>(kind));
        }
    };

    template <>
    struct hash<DebugImageSetKey>
    {
        size_t operator()(const DebugImageSetKey& key) const
        {
            size_t h1 = std::hash<std::string>{}(key.name);
            size_t h2 = std::hash<int32_t>{}(key.id);
            size_t h3 = std::hash<RTTIType>{}(key.type);
            size_t h4 = std::hash<DebugImageSetType>{}(key.imageKind);

            // Combine using XOR and shifts (simple hash mixing)
            return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
        }
    };
}

static std::unordered_map<DebugImageSetKey, DebugImageSet> DebugImageSetCache;












static bool Upload_Shapes_As_ImageSet_Textures(DebugImageSetKey &uniqueKey, ConvertClass &drawer, const ShapeSet *shapeSet)
{
    if (!_D3DDevice || !shapeSet) {
        return false;
    }

    // Avoid duplicate uploads
    if (DebugImageSetCache.find(uniqueKey) != DebugImageSetCache.end()) {
        return true;
    }

    const int frameCount = shapeSet->Get_Count();
    const int width = shapeSet->Get_Width();
    const int height = shapeSet->Get_Height();

    DebugImageSet imageSet(width, height);

    for (int frame = 0; frame < frameCount; ++frame) {
        const Rect shape_rect = shapeSet->Get_Rect(frame);
        if (!shape_rect.IsValid) {
            imageSet.Add_Frame(nullptr);
            continue;
        }

        BSurface shape_surface(width, height, 2);
        void* surface_pixels = shape_surface.Lock();
        if (!surface_pixels) {
            imageSet.Add_Frame(nullptr);
            continue;
        }

        Draw_Shape(shape_surface, drawer, shapeSet, frame, Point2D(0, 0), shape_rect);

        const unsigned short* surface565 = static_cast<const unsigned short*>(surface_pixels);
        std::vector<uint8_t> rgba(width * height * 4);
        for (int i = 0; i < width * height; ++i) {
            unsigned short value = surface565[i];
            uint8_t r = ((value & 0xF800) >> 11) * 255 / 31;
            uint8_t g = ((value & 0x07E0) >> 5)  * 255 / 63;
            uint8_t b = ((value & 0x001F))       * 255 / 31;
            rgba[i * 4 + 0] = r;
            rgba[i * 4 + 1] = g;
            rgba[i * 4 + 2] = b;
            rgba[i * 4 + 3] = 255;
        }

        shape_surface.Unlock();

        // Create texture
        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = rgba.data();
        initData.SysMemPitch = width * 4;

        ID3D11Texture2D* texture = nullptr;
        if (FAILED(_D3DDevice->CreateTexture2D(&desc, &initData, &texture))) {
            imageSet.Add_Frame(nullptr);
            continue;
        }

        ID3D11ShaderResourceView* srv = nullptr;
        if (FAILED(_D3DDevice->CreateShaderResourceView(texture, nullptr, &srv))) {
            texture->Release();
            imageSet.Add_Frame(nullptr);
            continue;
        }

        texture->Release();  // SRV now owns the texture
        imageSet.Add_Frame(srv);
    }

    DebugImageSetCache[uniqueKey] = std::move(imageSet);

    return true;
}

static bool Get_Texture_Dimensions(ID3D11ShaderResourceView* srv, int& outWidth, int& outHeight)
{
    if (!srv) {
        return false;
    }

    ID3D11Resource* resource = nullptr;
    srv->GetResource(&resource);
    if (!resource) {
        return false;
    }

    ID3D11Texture2D* texture = nullptr;
    HRESULT hr = resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&texture);
    resource->Release();  // Done with ID3D11Resource*

    if (FAILED(hr) || !texture) {
        return false;
    }

    D3D11_TEXTURE2D_DESC desc;
    texture->GetDesc(&desc);
    texture->Release();  // Done with ID3D11Texture2D*

    outWidth = static_cast<int>(desc.Width);
    outHeight = static_cast<int>(desc.Height);

    return true;
}















static void ShowImagePanel(const char* panelLabel, const DebugImageSetKey& uniqueKey, int& currentFrame)
{
    auto it = DebugImageSetCache.find(uniqueKey);
    if (it == DebugImageSetCache.end()) {
        ImGui::Begin(panelLabel);
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "No image data available.");
        ImGui::End();
        DEBUG_INFO("[DevWnd] Missing image set for key: %s\n", uniqueKey.name.c_str());
        return;
    }

    //DEBUG_INFO("[DevWnd] Displaying: %s kind=%d\n", uniqueKey.name.c_str(), static_cast<int>(uniqueKey.imageKind));

    const DebugImageSet& imageSet = it->second;
    const int frameCount = imageSet.Get_Frame_Count();
    const int imageWidth = imageSet.Get_Width();
    const int imageHeight = imageSet.Get_Height();

    ImGui::Begin(panelLabel);

    // -- Image size
    ImGui::Text("Image Size: %d x %d", imageWidth, imageHeight);

    // -- Image (centered)
    ID3D11ShaderResourceView* texture = imageSet.Get_Frame(currentFrame);
    if (texture) {
        ImVec2 avail = ImGui::GetContentRegionAvail();
        ImVec2 imageSize((float)imageWidth * _DrawScale, (float)imageHeight * _DrawScale);

        ImVec2 cursorPos = ImGui::GetCursorPos();
        if (avail.x > imageSize.x) {
            cursorPos.x += (avail.x - imageSize.x) * 0.5f;
            ImGui::SetCursorPos(cursorPos);
        }

        ImGui::Image((ImTextureID)texture, imageSize, ImVec2(0, 0), ImVec2(1, 1));
    } else {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Missing texture for %s frame %d", uniqueKey.name.c_str(), currentFrame);
    }

    // -- Frame slider
    if (frameCount > 1) {
        ImGui::SliderInt("Frame", &currentFrame, 0, frameCount - 1);
    } else {
        ImGui::BeginDisabled();
        ImGui::SliderInt("Frame", &currentFrame, 0, 0);
        ImGui::EndDisabled();
    }

    // -- Metadata
    ImGui::Separator();
    ImGui::Text("Metadata:");
    ImGui::BulletText("Frame Count: %d", frameCount);
    ImGui::BulletText("Current Frame: %d", currentFrame);
    ImGui::BulletText("Object Name: %s", uniqueKey.name.c_str());

    ImGui::End();
}










void ImGui_Draw_AbstractClass(AbstractClass * ptr);
void ImGui_Draw_AbstractTypeClass(AbstractTypeClass * ptr, bool skip_base = false);
void ImGui_Draw_ObjectTypeClass(ObjectTypeClass * ptr, bool skip_base = false);
void ImGui_Draw_TechnoTypeClass(TechnoTypeClass * ptr, bool skip_base = false);
void ImGui_Draw_AircraftTypeClass(AircraftTypeClass * ptr);
void ImGui_Draw_AnimTypeClass(AnimTypeClass * ptr);
void ImGui_Draw_BuildingTypeClass(BuildingTypeClass * ptr);
void ImGui_Draw_BulletTypeClass(BulletTypeClass * ptr);
void ImGui_Draw_HouseTypeClass(HouseTypeClass * ptr);
void ImGui_Draw_InfantryTypeClass(InfantryTypeClass * ptr);
void ImGui_Draw_IsometricTileTypeClass(IsometricTileTypeClass * ptr);
void ImGui_Draw_OverlayTypeClass(OverlayTypeClass * ptr);
void ImGui_Draw_ParticleTypeClass(ParticleTypeClass * ptr);
void ImGui_Draw_ParticleSystemTypeClass(ParticleSystemTypeClass * ptr);
void ImGui_Draw_ScriptTypeClass(ScriptTypeClass * ptr);
void ImGui_Draw_SmudgeTypeClass(SmudgeTypeClass * ptr);
void ImGui_Draw_SuperWeaponTypeClass(SuperWeaponTypeClass * ptr);
void ImGui_Draw_TeamTypeClass(TeamTypeClass * ptr);
void ImGui_Draw_TerrainTypeClass(TerrainTypeClass * ptr);
void ImGui_Draw_TriggerTypeClass(TriggerTypeClass * ptr);
void ImGui_Draw_TagTypeClass(TagTypeClass * ptr);
void ImGui_Draw_VoxelAnimTypeClass(VoxelAnimTypeClass * ptr);
void ImGui_Draw_UnitTypeClass(UnitTypeClass * ptr);
void ImGui_Draw_WeaponTypeClass(WeaponTypeClass * ptr);
void ImGui_Draw_WarheadTypeClass(WarheadTypeClass * ptr);
void ImGui_Draw_AITriggerTypeClass(AITriggerTypeClass * ptr);







// TODO: Function that sweeps though a vector to reapply changes to the main class (for stealth etc)

/*

for each TechnoType
   if class == ttype.Class
     reapply stuff.


*/

// Should be called at the end of a main class draw func

void Reapply_Some_Properties()
{
}






template<typename T>
void ImGui_Draw_ObjectPointer(const char* label,
                              T*& ptr,
                              const DynamicVectorClass<T *> & options,
                              const std::function<std::string(T*)>& getLabel,
                              const std::function<void(T*)>& drawDetail = nullptr)
{
    ImGui::Text("%s: %p", label, static_cast<void*>(ptr));

    std::string viewId = "View Type";
    std::string pickId = "Pick Type";
    std::string viewPopupId = std::string("ViewPopup##") + label;
    std::string pickPopupId = std::string("PickPopup##") + label;

    ImGui::SameLine();
    if (ImGui::Button(viewId.c_str())) {
        ImGui::OpenPopup(viewPopupId.c_str());
    }

    ImGui::SameLine();
    if (ImGui::Button(pickId.c_str())) {
        ImGui::OpenPopup(pickPopupId.c_str());
    }

    // View details popup
    if (ImGui::BeginPopup(viewPopupId.c_str())) {
        if (ptr && drawDetail) {
            drawDetail(ptr);
        } else {
            ImGui::TextDisabled("No detail available.");
        }
        ImGui::EndPopup();
    }

    // Picker popup
if (ImGui::BeginPopup(pickPopupId.c_str())) {
    for (int i = 0; i < options.Count(); ++i) {
        T* candidate = options[i];
        std::string itemLabel = getLabel ? getLabel(candidate) : ccstd::to_string(reinterpret_cast<std::uintptr_t>(candidate));
        if (ImGui::Selectable(itemLabel.c_str(), candidate == ptr)) {
            ptr = candidate;
            ImGui::CloseCurrentPopup();
        }
    }
    ImGui::EndPopup();
}
}

template<typename T>
void ImGui_Draw_DynamicVector(const char* label, DynamicVectorClass<T>& list)
{
    if (ImGui::TreeNode(label)) {
        for (int i = 0; i < list.Count(); ++i) {
            std::string itemLabel = std::string("Item[") + ccstd::to_string(i) + "]";
            ImGui::InputInt(itemLabel.c_str(), reinterpret_cast<int*>(&list[i]));
        }
        ImGui::TreePop();
    }
}


/**
 *  Draws an ImGui combo box for selecting an enum value, displaying items in uppercase.
 *  This templated function works with any enum type by supplying a mapping function
 *  from enum to name. Updates the enum value in-place when changed.
 */
template <typename EnumType>
bool ImGui_Draw_EnumCombo(
    const char* label,
    EnumType& value,
    int count,
    const char* (*NameFromEnum)(EnumType))
{
    int index = static_cast<int>(value);

    // Preallocate to avoid reallocations
    std::vector<std::string> itemStrings;
    itemStrings.reserve(count);
    std::vector<const char*> items;
    items.reserve(count);

    for (int i = 0; i < count; ++i) {
        const char* original = NameFromEnum(static_cast<EnumType>(i));
        std::string upperStr;
        for (char c : std::string(original)) {
            upperStr += static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        }
        itemStrings.push_back(std::move(upperStr));
        items.push_back(itemStrings.back().c_str());
    }

    if (ImGui::Combo(label, &index, items.data(), count)) {
        value = static_cast<EnumType>(index);
        return true;
    }

    return false;
}






template<typename T>
void ImGui_Show_Vector_ListWindow(
    const char* windowLabel,
    const char* itemPrefix,
    DynamicVectorClass<T*>& list,
    std::function<void(T*)> drawDetails)
{
    static T* selected = nullptr;

    ImGui::Begin(windowLabel);

    if (list.Count() == 0) {
        ImGui::TextDisabled("No items available.");
        ImGui::End();
        return;
    }

    // --- Step 1: Calculate widest label width
    float maxLabelWidth = 0.0f;
    for (int i = 0; i < list.Count(); ++i) {
        T* item = list[i];
        if (!item) continue;

        std::string label = "[" + ccstd::to_string(i) + "] " + itemPrefix + " " + item->IniName;
        float labelWidth = ImGui::CalcTextSize(label.c_str()).x;
        maxLabelWidth = std::max(maxLabelWidth, labelWidth);
    }

    const float buttonWidth = ImGui::CalcTextSize("View").x + ImGui::GetStyle().FramePadding.x * 4;
    float leftPanelWidth = maxLabelWidth + buttonWidth + 24.0f;  // Add some spacing

    // --- Step 2: Left panel
    ImGui::BeginChild("ItemList", ImVec2(leftPanelWidth, 0), true);
    for (int i = 0; i < list.Count(); ++i) {
        T* item = list[i];
        if (!item) {
            ImGui::TextDisabled("Item[%d] = nullptr", i);
            continue;
        }

        std::string label = "[" + ccstd::to_string(i) + "] " + itemPrefix + " " + item->IniName;
        ImGui::TextUnformatted(label.c_str());

        ImGui::SameLine();

        std::string buttonId = "View##" + ccstd::to_string(i);
        if (ImGui::Button(buttonId.c_str())) {
            selected = item;
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // Right panel: details for selected item
    ImGui::BeginChild("ItemDetails", ImVec2(0, 0), true);
    if (selected) {
        drawDetails(selected);
    } else {
        ImGui::Text("Select an item to view its details.");
    }
    ImGui::EndChild();

    ImGui::End();
}













#define DRAW_BASE_CLASS(_name, _ptr, _skip) \
    if (ImGui::TreeNode(#_name)) { \
        /*ImGui::SeparatorText(#_name);*/ \
        ImGui_Draw_##_name(static_cast<_name*>(_ptr), true); \
        ImGui::TreePop(); \
    }

void ImGui_Draw_AbstractClass(AbstractClass * ptr)
{
    ASSERT(ptr != nullptr);

    ImGui::BeginDisabled();
        ImGui::InputInt("ID", &ptr->ID);
        ImGui::InputInt("RefCount", &ptr->RefCount);
        ImGui::Checkbox("Dirty", &ptr->Dirty);
    ImGui::EndDisabled();
}

void ImGui_Draw_AbstractTypeClass(AbstractTypeClass * ptr, bool skip_base)
{
    ASSERT(ptr != nullptr);

    //// Draw base classes first
    //if (skip_base) {
    //    //DRAW_BASE_CLASS(AbstractClass, ptr);
    //    ImGui::PushStyleColor(ImGuiCol_Header,         ImVec4(0.2f, 0.4f, 0.6f, 1.0f));
    //    ImGui::PushStyleColor(ImGuiCol_HeaderHovered,  ImVec4(0.3f, 0.5f, 0.7f, 1.0f));
    //    ImGui::PushStyleColor(ImGuiCol_HeaderActive,   ImVec4(0.4f, 0.6f, 0.8f, 1.0f));
    //    ImGui::PushStyleColor(ImGuiCol_Text,           ImVec4(1.0f, 1.0f, 0.9f, 1.0f)); // Only affects header
    //    bool node_opened = ImGui::TreeNodeEx("AbstractClass");
    //    ImGui::PopStyleColor(4); // Pop immediately after TreeNodeEx()
    //    if (node_opened) {
    //        //ImGui::SeparatorText("AbstractClass");
    //        ImGui_Draw_AbstractClass(ptr);
    //        ImGui::TreePop();
    //    }
    //}

    // This class
    ImGui::InputText("IniName", ptr->IniName, sizeof(ptr->IniName));
    ImGui::InputText("FullName", ptr->FullName, sizeof(ptr->FullName));
}

void ImGui_Draw_ObjectTypeClass(ObjectTypeClass * ptr, bool skip_base)
{
    ASSERT(ptr != nullptr);

    // Draw base classes first
    if (!skip_base) {
        DRAW_BASE_CLASS(AbstractTypeClass, ptr);
        ImGui::SeparatorText("ObjectTypeClass");
    }

    // RadialColor
    ImVec4 colorVec = ImVec4(
        ptr->RadialColor.R / 255.0f,
        ptr->RadialColor.G / 255.0f,
        ptr->RadialColor.B / 255.0f,
        1.0f
    );
    if (ImGui::ColorEdit3("RadialColor", (float*)&colorVec)) {
        ptr->RadialColor.R = static_cast<uint8_t>(colorVec.x * 255.0f);
        ptr->RadialColor.G = static_cast<uint8_t>(colorVec.y * 255.0f);
        ptr->RadialColor.B = static_cast<uint8_t>(colorVec.z * 255.0f);
    }

    ImGui_Draw_EnumCombo<ArmorType>("Armor", ptr->Armor, ARMOR_COUNT, Name_From_Armor);

    ImGui::InputScalar("MaxStrength", ImGuiDataType_U32, &ptr->MaxStrength);

    ImGui::InputText("GraphicName", ptr->GraphicName, sizeof(ptr->GraphicName));
    ImGui::InputText("AlphaGraphicName", ptr->AlphaGraphicName, sizeof(ptr->AlphaGraphicName));

    // Booleans
    ImGui::Checkbox("IsTheater", &ptr->IsTheater);
    ImGui::Checkbox("IsCrushable", &ptr->IsCrushable);
    ImGui::Checkbox("IsStealthy", &ptr->IsStealthy);
    ImGui::Checkbox("IsSelectable", &ptr->IsSelectable);
    ImGui::Checkbox("IsLegalTarget", &ptr->IsLegalTarget);
    ImGui::Checkbox("IsInsignificant", &ptr->IsInsignificant);
    ImGui::Checkbox("IsImmune", &ptr->IsImmune);
    ImGui::Checkbox("IsSentient", &ptr->IsSentient);
    ImGui::Checkbox("IsFootprint", &ptr->IsFootprint);
    ImGui::Checkbox("IsVoxel", &ptr->IsVoxel);
    ImGui::Checkbox("IsNewTheater", &ptr->IsNewTheater);
    ImGui::Checkbox("IsHasRadialIndicator", &ptr->IsHasRadialIndicator);
    ImGui::Checkbox("IsIgnoresFirestorm", &ptr->IsIgnoresFirestorm);

    ImGui::InputInt("MaxDimension", (int*)&ptr->MaxDimension);
    ImGui::InputInt("CrushSound", (int*)&ptr->CrushSound);

    // VoxelIndex objects
    if (ImGui::TreeNode("VoxelIndex")) {
        //ImGui::InputInt("Voxel", &ptr->VoxelIndex.Index);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("AuxVoxelIndex")) {
        //ImGui::InputInt("Aux", &ptr->AuxVoxelIndex.Index);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("ShadowVoxelIndex")) {
        //ImGui::InputInt("Shadow", &ptr->ShadowVoxelIndex.Index);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("AuxVoxel2Index")) {
        //ImGui::InputInt("Aux2", &ptr->AuxVoxel2Index.Count());
        ImGui::TreePop();
    }

    if (ptr->Image) {
        if (ImGui::Button("View Image")) {
            ImGui::OpenPopup("Image");
        }
        if (ImGui::BeginPopupModal("Image", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            DebugImageSetKey key = { ptr->IniName, ptr->ID, ptr->RTTI, IMAGE_TYPE_SHAPE };
            auto textures_uploaded = Upload_Shapes_As_ImageSet_Textures(key, *NormalDrawer, ptr->Image);
            ASSERT(textures_uploaded);
            static int _current_frame = 0;
            ShowImagePanel("Image", key, _current_frame);
            if (ImGui::Button("Close")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    } else {
        ImGui::TextDisabled("Image: NULL");
    }
    
    if (ptr->AlphaImage) {
        if (ImGui::Button("View AlphaImage")) {
            ImGui::OpenPopup("AlphaImage");
        }
        if (ImGui::BeginPopupModal("AlphaImage", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            DebugImageSetKey key = { ptr->IniName, ptr->ID, ptr->RTTI, IMAGE_TYPE_ALPHA_SHAPE };
            auto textures_uploaded = Upload_Shapes_As_ImageSet_Textures(key, *NormalDrawer, ptr->AlphaImage);
            ASSERT(textures_uploaded);
            static int _current_frame = 0;
            ShowImagePanel("AlphaImage", key, _current_frame);
            if (ImGui::Button("Close")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    } else {
        ImGui::TextDisabled("AlphaImage: NULL");
    }

    // Same for VoxelObject/AuxVoxel/etc, unless you're ready to implement editors for them.
}

void ImGui_Draw_TechnoTypeClass(TechnoTypeClass * ptr, bool skip_base)
{
    ASSERT(ptr != nullptr);

    // Draw base classes first
    if (!skip_base) {
        DRAW_BASE_CLASS(AbstractTypeClass, ptr);
        DRAW_BASE_CLASS(AbstractTypeClass, ptr);
        DRAW_BASE_CLASS(ObjectTypeClass, ptr);
        ImGui::SeparatorText("TechnoTypeClass");
    }

    ImGui::InputFloat("CollateralDamageCoefficient", &ptr->CollateralDamageCoefficient);
    ImGui::InputFloat("field_128", &ptr->field_128);
    ImGui::InputInt("WalkRate", &ptr->WalkRate);

    // Veteran and Elite Abilities
    ImGui::Text("VeteranAbilities:");
    //ImGui_Draw_AbilitiesStruct("Veteran", &ptr->VeteranAbilities);

    ImGui::Text("EliteAbilities:");
    //ImGui_Draw_AbilitiesStruct("Elite", &ptr->EliteAbilities);

    ImGui::InputDouble("SpecialThreatValue", &ptr->SpecialThreatValue);
    ImGui::InputDouble("MyEffectivenessCoefficient", &ptr->MyEffectivenessCoefficient);
    ImGui::InputDouble("TargetEffectivenessCoefficient", &ptr->TargetEffectivenessCoefficient);
    ImGui::InputDouble("TargetSpecialThreatCoefficient", &ptr->TargetSpecialThreatCoefficient);
    ImGui::InputDouble("TargetStrengthCoefficient", &ptr->TargetStrengthCoefficient);
    ImGui::InputDouble("TargetDistanceCoefficient", &ptr->TargetDistanceCoefficient);
    ImGui::InputDouble("ThreatAvoidanceCoefficient", &ptr->ThreatAvoidanceCoefficient);
    ImGui::InputInt("SlowdownDistance", &ptr->SlowdownDistance);
    ImGui::InputDouble("DeaccelerationFactor", &ptr->DeaccelerationFactor);
    ImGui::InputDouble("AccelerationFactor", &ptr->AccelerationFactor);
    ImGui::InputInt("CloakingSpeed", &ptr->CloakingSpeed);

    //ImGui_Show_Vector_ListWindow("DebrisTypes", "VoxelAnim", ptr->DebrisTypes, ImGui_Draw_VoxelAnimTypeClass);

    if (ImGui::TreeNode("DebrisMaximums")) {
        for (int i = 0; i < ptr->DebrisMaximums.Count(); ++i) {
            ImGui::PushID(i); // To avoid ID collisions
            ImGui::InputInt("Value", &ptr->DebrisMaximums[i]);
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    static struct LocomotorInfo {
        std::string name;
        CLSID id;
    } AvailableLocomotors[] = {
        { "Test Locomotor", __uuidof(TestLocomotionClass) }
    };

    // Find current name (fallback to hex string if not found)
    const char* currentLabel = nullptr;
    for (const auto& entry : AvailableLocomotors) {
        if (entry.id == ptr->Locomotor) {
            currentLabel = entry.name.c_str();
            break;
        }
    }
    char fallback[16];
    if (!currentLabel) {
        std::snprintf(fallback, sizeof(fallback), "%08X", ptr->Locomotor);
        currentLabel = fallback;
    }

    if (ImGui::BeginCombo("Locomotor", currentLabel)) {
        for (int i = 0; i < IM_ARRAYSIZE(AvailableLocomotors); ++i) {
            bool isSelected = (AvailableLocomotors[i].id == ptr->Locomotor);
            if (ImGui::Selectable(AvailableLocomotors[i].name.c_str(), isSelected)) {
                ptr->Locomotor = AvailableLocomotors[i].id;
            }
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::InputDouble("field_1F8", &ptr->field_1F8);
    ImGui::InputDouble("field_200", &ptr->field_200);
    ImGui::InputDouble("Weight", &ptr->Weight);
    ImGui::InputDouble("PhysicalSize", &ptr->PhysicalSize);

    ImGui_Draw_EnumCombo("InitialMission", ptr->InitialMission, MISSION_COUNT, MissionClass::Mission_Name);

    ImGui::InputDouble("RollAngle", &ptr->RollAngle);
    ImGui::InputDouble("PitchSpeed", &ptr->PitchSpeed);
    ImGui::InputDouble("PitchAngle", &ptr->PitchAngle);
    ImGui::InputInt("BuildLimit", &ptr->BuildLimit);
    //ImGui_Draw_EnumCombo("Category", ptr->Category, CATEGORY_COUNT, Name_From_CategoryType);

    ImGui::InputInt("field_240", &ptr->field_240);
    ImGui::InputDouble("DeployTime", &ptr->DeployTime);
    ImGui::InputInt("FireAngle", &ptr->FireAngle);
    //ImGui_Draw_EnumCombo("PipScale", ptr->PipScale, PIPSCALE_COUNT, Name_From_PipScaleType);

    //ImGui_Show_Vector_ListWindow("Dock", "DockBuilding", ptr->Dock, ImGui_Draw_BuildingTypeClass);
    //ImGui_PointerPicker("DeploysInto", ptr->DeploysInto, GameTypes::BuildingTypes, "View Deploy", [](auto p) { return p->IniName; }, ImGui_Draw_UnitTypeClass);
    //ImGui_PointerPicker("UndeploysInto", ptr->UndeploysInto, GameTypes::UnitTypes, "View Undeploy", [](auto p) { return p->IniName; }, ImGui_Draw_UnitTypeClass);

    //ImGui_Show_Vector_ListWindow("VoiceSelect", "Voice", ptr->VoiceSelect, nullptr);
    //ImGui_Show_Vector_ListWindow("VoiceMove", "Voice", ptr->VoiceMove, nullptr);
    //ImGui_Show_Vector_ListWindow("VoiceAttack", "Voice", ptr->VoiceAttack, nullptr);
    //ImGui_Show_Vector_ListWindow("VoiceDie", "Voice", ptr->VoiceDie, nullptr);
    //ImGui_Show_Vector_ListWindow("VoiceFeedback", "Voice", ptr->VoiceFeedback, nullptr);

    ImGui::InputInt("AuxSound1", (int*)&ptr->AuxSound1);
    ImGui::InputInt("AuxSound2", (int*)&ptr->AuxSound2);
    //ImGui_Draw_EnumCombo("MZone", ptr->MZone, MZONE_COUNT, Name_From_MZoneType);

    ImGui::InputInt("ThreatRange", &ptr->ThreatRange);
    ImGui::InputInt("MaxDebris", &ptr->MaxDebris);
    ImGui::InputInt("MaxPassengers", &ptr->MaxPassengers);
    ImGui::InputInt("SightRange", &ptr->SightRange);
    ImGui::InputInt("Cost", &ptr->Cost);
    ImGui::InputInt("FlightLevel", &ptr->FlightLevel);
    ImGui::InputInt("TechLevel", (int*)&ptr->TechLevel);

    //ImGui_Show_Vector_ListWindow("Prerequisite", "Prereq", ptr->Prerequisite, nullptr);

    ImGui::InputInt("Risk", &ptr->Risk);
    ImGui::InputInt("Reward", &ptr->Reward);
    ImGui::InputInt("MaxSpeed", (int*)&ptr->MaxSpeed);
    ImGui::InputInt("Speed", (int*)&ptr->Speed);
    ImGui::InputInt("MaxAmmo", &ptr->MaxAmmo);
    ImGui::InputScalar("Ownable", ImGuiDataType_U32, &ptr->Ownable, nullptr, nullptr, "%08X");

    ImGui::Checkbox("IsAllowedToStartInMultiplayer", &ptr->IsAllowedToStartInMultiplayer);
    ImGui::InputText("CameoFilename", ptr->CameoFilename, sizeof(ptr->CameoFilename));

    ImGui::TextDisabled("CameoData: %p", ptr->CameoData);  // Can hook to image preview

    ImGui::InputInt("Rotation", &ptr->Rotation);
    ImGui::InputInt("ROT", &ptr->ROT);
    ImGui::InputInt("TurretOffset", &ptr->TurretOffset);
    ImGui::InputInt("Points", &ptr->Points);

    //ImGui_Show_Vector_ListWindow("Explosion", "Anim", ptr->Explosion, ImGui_Draw_AnimTypeClass);
    //ImGui_PointerPicker("NaturalParticleSystem", ptr->NaturalParticleSystem, GameTypes::ParticleSystemTypes, "View", [](auto p) { return p->Get_Name(); }, ImGui_Draw_ParticleSystemTypeClass);
    ImGui::InputInt3("NaturalParticleSystemLocation", &ptr->NaturalParticleSystemLocation.X);

    //ImGui_Show_Vector_ListWindow("DamageParticleSystems", "DmgPSys", ptr->DamageParticleSystems, ImGui_Draw_ParticleSystemTypeClass);
    ImGui::InputInt3("DamageSmokeOffset", &ptr->DamageSmokeOffset.X);
    ImGui::InputInt("ShadowIndex", &ptr->ShadowIndex);
    ImGui::InputInt("Storage", &ptr->Storage);

    // Weapons
    if (ImGui::TreeNode("Weapons")) {
        for (int i = 0; i < WEAPON_SLOT_COUNT; ++i) {
            char label[32];
            std::snprintf(label, sizeof(label), "Weapon %d", i);
            if (ImGui::TreeNode(label)) {
                //ImGui_Draw_WeaponInfoStruct(&ptr->Weapons[i]);
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }

    // All booleans
    ImGui::Checkbox("IsTypeImmune", &ptr->IsTypeImmune);
    ImGui::Checkbox("MoveToShroud", &ptr->MoveToShroud);
    ImGui::Checkbox("IsTrainable", &ptr->IsTrainable);
    ImGui::Checkbox("DamageSparks", &ptr->DamageSparks);
    ImGui::Checkbox("TargetLaser", &ptr->TargetLaser);
    ImGui::Checkbox("IsImmuneToVeins", &ptr->IsImmuneToVeins);
    ImGui::Checkbox("IsTiberiumHeal", &ptr->IsTiberiumHeal);
    ImGui::Checkbox("CloakStop", &ptr->CloakStop);
    ImGui::Checkbox("IsTrain", &ptr->IsTrain);
    ImGui::Checkbox("IsDropship", &ptr->IsDropship);
    ImGui::Checkbox("ToProtect", &ptr->ToProtect);
    ImGui::Checkbox("Disableable", &ptr->Disableable);
    ImGui::Checkbox("Unbuildable_or_CanBuild", &ptr->Unbuildable_or_CanBuild);
    ImGui::Checkbox("IsDoubleOwned", &ptr->IsDoubleOwned);
    ImGui::Checkbox("IsInvisible", &ptr->IsInvisible);
    ImGui::Checkbox("IsRadarVisible", &ptr->IsRadarVisible);
    ImGui::Checkbox("IsLeader", &ptr->IsLeader);
    ImGui::Checkbox("IsScanner", &ptr->IsScanner);
    ImGui::Checkbox("IsNominal", &ptr->IsNominal);
    ImGui::Checkbox("IsTurretEquipped", &ptr->IsTurretEquipped);
    ImGui::Checkbox("IsRepairable", &ptr->IsRepairable);
    ImGui::Checkbox("IsCrew", &ptr->IsCrew);
    ImGui::Checkbox("IsRemappable", &ptr->IsRemappable);
    ImGui::Checkbox("IsCloakable", &ptr->IsCloakable);
    ImGui::Checkbox("IsSelfHealing", &ptr->IsSelfHealing);
    ImGui::Checkbox("IsExploding", &ptr->IsExploding);
    ImGui::Checkbox("IsNoAutoFire", &ptr->IsNoAutoFire);
    ImGui::Checkbox("IsTurretSpins", &ptr->IsTurretSpins);
    ImGui::Checkbox("IsRegulated", &ptr->IsRegulated);
    ImGui::Checkbox("IsManualReload", &ptr->IsManualReload);
    ImGui::Checkbox("IsVisibleLoad", &ptr->IsVisibleLoad);
    ImGui::Checkbox("IsLightningRod", &ptr->IsLightningRod);
    ImGui::Checkbox("IsHunterSeeker", &ptr->IsHunterSeeker);
    ImGui::Checkbox("IsCrusher", &ptr->IsCrusher);
    ImGui::Checkbox("IsTiltsWhenCrushes", &ptr->IsTiltsWhenCrushes);
    ImGui::Checkbox("IsSubterranean", &ptr->IsSubterranean);
    ImGui::Checkbox("IsAutoCrush", &ptr->IsAutoCrush);
    ImGui::Checkbox("IsAccelerates", &ptr->IsAccelerates);

    ImGui::InputInt("ZFudgeCliff", &ptr->ZFudgeCliff);
    ImGui::InputInt("ZFudgeColumn", &ptr->ZFudgeColumn);
    ImGui::InputInt("ZFudgeTunnel", &ptr->ZFudgeTunnel);
    ImGui::InputInt("ZFudgeBridge", &ptr->ZFudgeBridge);
}

void ImGui_Draw_AircraftTypeClass(AircraftTypeClass * ptr)
{
    ASSERT(ptr != nullptr);

    // Draw base classes first
    DRAW_BASE_CLASS(AbstractTypeClass, ptr);
    DRAW_BASE_CLASS(ObjectTypeClass, ptr);
    DRAW_BASE_CLASS(TechnoTypeClass, ptr);

    if (ImGui::TreeNode("AircraftTypeClass")) {

        ImGui::Separator();

        ImGui::BeginDisabled();
            ImGui::InputInt("HeapID", (int*)&ptr->HeapID);
        ImGui::EndDisabled();

        ImGui::Checkbox("IsCarryall", &ptr->IsCarryall);
        ImGui::Checkbox("IsRotorEquipped", &ptr->IsRotorEquipped);
        ImGui::Checkbox("IsRotorCustom", &ptr->IsRotorCustom);
        ImGui::Checkbox("IsLandable", &ptr->IsLandable);

        ImGui::TreePop();
    }
}

void ImGui_Draw_AnimTypeClass(AnimTypeClass * ptr)
{
    ASSERT(ptr != nullptr);

    // Draw base classes first
    DRAW_BASE_CLASS(AbstractTypeClass, ptr);
    DRAW_BASE_CLASS(ObjectTypeClass, ptr);

    ImGui::Text("AnimTypeClass");

    ImGui::Separator();
    
    ImGui::BeginDisabled();
        ImGui::InputInt("HeapID", (int*)&ptr->HeapID);
    ImGui::EndDisabled();

    ImGui::InputInt("Biggest", &ptr->Biggest);
    ImGui::InputDouble("Damage", &ptr->Damage);
    ImGui::InputInt("Delay", &ptr->Delay);
    ImGui::InputInt("Start", &ptr->Start);
    ImGui::InputInt("LoopStart", &ptr->LoopStart);
    ImGui::InputInt("LoopEnd", &ptr->LoopEnd);
    ImGui::InputInt("Stages", &ptr->Stages);
    ImGui::InputInt("Loops", (int*)&ptr->Loops);
    ImGui::InputInt("Sound (VocType)", (int*)&ptr->Sound);
    ImGui::Text("ChainTo: %p", ptr->ChainTo);
    ImGui::InputInt("DetailLevel", &ptr->DetailLevel);
    ImGui::InputInt("TranslucencyDetailLevel", &ptr->TranslucencyDetailLevel);
    ImGui::InputInt("RandomLoopDelayMin", &ptr->RandomLoopDelayMin);
    ImGui::InputInt("RandomLoopDelayMax", &ptr->RandomLoopDelayMax);
    ImGui::InputInt("RandomRateMin", &ptr->RandomRateMin);
    ImGui::InputInt("RandomRateMax", &ptr->RandomRateMax);
    ImGui::InputInt("Translucency", &ptr->Translucency);
    ImGui::Text("Spawns: %p", ptr->Spawns);
    ImGui::InputInt("SpawnCount", &ptr->SpawnCount);
    ImGui::InputInt("StartSound", (int*)&ptr->StartSound);
    ImGui::InputInt("BounceSound", (int*)&ptr->BounceSound);
    ImGui::InputInt("ExpireSound", (int*)&ptr->ExpireSound);
    ImGui::Text("BounceAnim: %p", ptr->BounceAnim);
    ImGui::Text("ExpireAnim: %p", ptr->ExpireAnim);
    ImGui::Text("TrailerAnim: %p", ptr->TrailerAnim);
    ImGui::InputInt("TrailerSeperation", &ptr->TrailerSeperation);
    ImGui::InputDouble("Elasticity", &ptr->Elasticity);
    ImGui::InputDouble("MinZVel", &ptr->MinZVel);
    ImGui::InputDouble("MaxZVel", &ptr->MaxZVel);
    ImGui::InputDouble("MaxXYVel", &ptr->MaxXYVel);
    ImGui::Text("Warhead: %p", ptr->Warhead);
    ImGui::InputInt("DamageRadius", &ptr->DamageRadius);
    ImGui::Text("TiberiumSpawnType: %p", ptr->TiberiumSpawnType);
    ImGui::InputInt("TiberiumSpreadRadius", &ptr->TiberiumSpreadRadius);
    ImGui::InputInt("YSortAdjust", &ptr->YSortAdjust);
    ImGui::InputInt("YDrawOffset", &ptr->YDrawOffset);
    ImGui::InputInt("RunningFrames", &ptr->RunningFrames);

    // Booleans
    ImGui::Checkbox("IsFlamingGuy", &ptr->IsFlamingGuy);
    ImGui::Checkbox("IsVeins", &ptr->IsVeins);
    ImGui::Checkbox("IsMeteor", &ptr->IsMeteor);
    ImGui::Checkbox("IsTiberiumChainReaction", &ptr->IsTiberiumChainReaction);
    ImGui::Checkbox("IsTiberium", &ptr->IsTiberium);
    ImGui::Checkbox("IsBouncer", &ptr->IsBouncer);
    ImGui::Checkbox("IsTiled", &ptr->IsTiled);
    ImGui::Checkbox("IsShouldUseCellDrawer", &ptr->IsShouldUseCellDrawer);
    ImGui::Checkbox("IsUseNormalLight", &ptr->IsUseNormalLight);
    ImGui::Checkbox("IsDemandLoad", &ptr->IsDemandLoad);
    ImGui::Checkbox("IsFreeAfterPlaying", &ptr->IsFreeAfterPlaying);
    ImGui::Checkbox("IsAnimatedTiberium", &ptr->IsAnimatedTiberium);
    ImGui::Checkbox("IsAltPalette", &ptr->IsAltPalette);
    ImGui::Checkbox("IsNormalized", &ptr->IsNormalized);
    ImGui::Checkbox("IsGroundLayer", &ptr->IsGroundLayer);
    ImGui::Checkbox("IsFlat", &ptr->IsFlat);
    ImGui::Checkbox("IsTranslucent", &ptr->IsTranslucent);
    ImGui::Checkbox("IsScorcher", &ptr->IsScorcher);
    ImGui::Checkbox("IsFlameThrower", &ptr->IsFlameThrower);
    ImGui::Checkbox("IsCraterForming", &ptr->IsCraterForming);
    ImGui::Checkbox("IsSticky", &ptr->IsSticky);
    ImGui::Checkbox("IsPingPong", &ptr->IsPingPong);
    ImGui::Checkbox("IsReverse", &ptr->IsReverse);
    ImGui::Checkbox("IsShouldFogRemove", &ptr->IsShouldFogRemove);
}

void ImGui_Draw_BuildingTypeClass(BuildingTypeClass * ptr)
{
    ASSERT(ptr != nullptr);

    // Draw base classes first
    DRAW_BASE_CLASS(AbstractTypeClass, ptr);
    DRAW_BASE_CLASS(ObjectTypeClass, ptr);
    DRAW_BASE_CLASS(TechnoTypeClass, ptr);

    ImGui::Text("BuildingTypeClass");

    ImGui::Separator();
    
    ImGui::BeginDisabled();
        ImGui::InputInt("HeapID", (int*)&ptr->HeapID);
    ImGui::EndDisabled();

    ImGui::Text("OccupyList: %p", ptr->OccupyList);
    ImGui::Text("BuildupData: %p", ptr->BuildupData);

    ImGui::InputInt3("HalfDamageSmokeLocation1", (int*)&ptr->HalfDamageSmokeLocation1);
    ImGui::InputInt3("HalfDamageSmokeLocation2", (int*)&ptr->HalfDamageSmokeLocation2);

    ImGui::InputDouble("GateCloseDelay", &ptr->GateCloseDelay);
    ImGui::InputInt("LightVisibility", &ptr->LightVisibility);
    ImGui::InputInt("LightIntensity", &ptr->LightIntensity);
    ImGui::InputInt("LightRedTint", &ptr->LightRedTint);
    ImGui::InputInt("LightGreenTint", &ptr->LightGreenTint);
    ImGui::InputInt("LightBlueTint", &ptr->LightBlueTint);

    ImGui::InputInt2("PrimaryFirePixelOffset", (int*)&ptr->PrimaryFirePixelOffset);
    ImGui::InputInt2("SecondaryFirePixelOffset", (int*)&ptr->SecondaryFirePixelOffset);

    ImGui::Text("ToOverlay: %p", ptr->ToOverlay);
    ImGui::Text("ToTile: %p", ptr->ToTile);
    ImGui::InputText("BuildupFilename", ptr->BuildupFilename, sizeof(ptr->BuildupFilename));
    ImGui::InputText("PowersUpBuilding", ptr->PowersUpBuilding, sizeof(ptr->PowersUpBuilding));

    ImGui::Text("FreeUnit: %p", ptr->FreeUnit);
    ImGui::InputInt("FoundationFace", (int*)&ptr->FoundationFace);
    ImGui::InputInt("Adjacent", &ptr->Adjacent);
    ImGui::InputInt("ToBuild (RTTI)", (int*)&ptr->ToBuild);

    ImGui::InputInt("ExitCoordinate.X", &ptr->ExitCoordinate.X);
    ImGui::InputInt("ExitCoordinate.Y", &ptr->ExitCoordinate.Y);
    ImGui::InputInt("ExitCoordinate.Z", &ptr->ExitCoordinate.Z);

    ImGui::Text("ExitList: %p", ptr->ExitList);
    ImGui::InputInt("StartFace", (int*)&ptr->StartFace);
    ImGui::InputInt("Power", &ptr->Power);
    ImGui::InputInt("Drain", &ptr->Drain);
    ImGui::InputInt("Size", (int*)&ptr->Size);
    ImGui::InputInt("ZHeight", &ptr->ZHeight);
    ImGui::InputInt("MidPoint", &ptr->MidPoint);
    ImGui::InputInt("DoorStages", &ptr->DoorStages);

    if (ImGui::TreeNode("Anims"))
    {
        for (int i = 0; i < BSTATE_COUNT; ++i) {
            ImGui::PushID(i);
            ImGui::Text("State %d", i);
            ImGui::InputInt("Start", &ptr->Anims[i].Start);
            ImGui::InputInt("Count", &ptr->Anims[i].Count);
            ImGui::InputInt("Rate", &ptr->Anims[i].Rate);
            ImGui::PopID();
            ImGui::Separator();
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("field_580")) {
        for (int i = 0; i < BANIM_COUNT; ++i) {
            ImGui::PushID(i);
            ImGui::InputText("Anim", ptr->field_580[i].Anim, sizeof(ptr->field_580[i].Anim));
            ImGui::InputText("AnimDamaged", ptr->field_580[i].AnimDamaged, sizeof(ptr->field_580[i].AnimDamaged));
            ImGui::InputInt2("Location", (int*)&ptr->field_580[i].Location);
            ImGui::InputScalar("ZAdjust", ImGuiDataType_S8, &ptr->field_580[i].ZAdjust);
            ImGui::InputScalar("YSort", ImGuiDataType_S8, &ptr->field_580[i].YSort);
            ImGui::Checkbox("Powered", &ptr->field_580[i].Powered);
            ImGui::Checkbox("PoweredLight", &ptr->field_580[i].PoweredLight);
            ImGui::PopID();
            ImGui::Separator();
        }
        ImGui::TreePop();
    }

    ImGui::InputInt("Upgrades", &ptr->Upgrades);
    ImGui::Text("DeployingAnim: %p", ptr->DeployingAnim);
    ImGui::Text("UnderDoorAnim: %p", ptr->UnderDoorAnim);
    ImGui::Text("DoorAnim: %p", ptr->DoorAnim);
    ImGui::Text("SpecialZOverlay: %p", ptr->SpecialZOverlay);
    ImGui::InputInt("SpecialZOverlayZAdjust", &ptr->SpecialZOverlayZAdjust);
    ImGui::Text("BibShape: %p", ptr->BibShape);
    ImGui::InputInt("NormalZAdjust", &ptr->NormalZAdjust);
    ImGui::InputInt("AntiAirValue", &ptr->AntiAirValue);
    ImGui::InputInt("AntiArmorValue", &ptr->AntiArmorValue);
    ImGui::InputInt("AntiInfantryValue", &ptr->AntiInfantryValue);
    ImGui::InputInt2("ZShapePointMove", (int*)&ptr->ZShapePointMove);
    ImGui::InputInt4("field_7F0", (int*)&ptr->field_7F0.X);

    ImGui::InputInt("ExtraLight", (int*)&ptr->ExtraLight);

    if (ImGui::TreeNode("Flags")) {
        ImGui::Checkbox("IsCanTogglePower", &ptr->IsCanTogglePower);
        ImGui::Checkbox("IsHasSpotlight", &ptr->IsHasSpotlight);
        ImGui::Checkbox("IsTemple", &ptr->IsTemple);
        ImGui::Checkbox("IsPlug", &ptr->IsPlug);
        ImGui::Checkbox("IsHoverPad", &ptr->IsHoverPad);
        ImGui::Checkbox("IsBase", &ptr->IsBase);
        ImGui::Checkbox("IsBibbed", &ptr->IsBibbed);
        ImGui::Checkbox("IsWall", &ptr->IsWall);
        ImGui::Checkbox("IsCaptureable", &ptr->IsCaptureable);
        ImGui::Checkbox("IsPowered", &ptr->IsPowered);
        ImGui::Checkbox("IsUnsellable", &ptr->IsUnsellable);
        ImGui::Checkbox("IsRadar", &ptr->IsRadar);
        ImGui::Checkbox("IsHasChargeAnim", &ptr->IsHasChargeAnim);
        ImGui::Checkbox("IsSiloDamage", &ptr->IsSiloDamage);
        ImGui::Checkbox("CanUnitRepair", &ptr->CanUnitRepair);
        ImGui::Checkbox("CanUnitReload", &ptr->CanUnitReload);
        ImGui::Checkbox("IsFlat", &ptr->IsFlat);
        ImGui::Checkbox("IsDockUnload", &ptr->IsDockUnload);
        ImGui::Checkbox("IsRecoilless", &ptr->IsRecoilless);
        ImGui::Checkbox("IsHasStupidGuardMode", &ptr->IsHasStupidGuardMode);
        ImGui::Checkbox("IsBridgeRepairHut", &ptr->IsBridgeRepairHut);
        ImGui::Checkbox("IsGate", &ptr->IsGate);
        ImGui::Checkbox("IsSAM", &ptr->IsSAM);
        ImGui::Checkbox("IsConstructionYard", &ptr->IsConstructionYard);
        ImGui::Checkbox("IsNukeSilo", &ptr->IsNukeSilo);
        ImGui::Checkbox("IsRefinery", &ptr->IsRefinery);
        ImGui::Checkbox("IsWeeder", &ptr->IsWeeder);
        ImGui::Checkbox("IsWeaponsFactory", &ptr->IsWeaponsFactory);
        ImGui::Checkbox("IsLaserFencePost", &ptr->IsLaserFencePost);
        ImGui::Checkbox("IsLaserFence", &ptr->IsLaserFence);
        ImGui::Checkbox("IsFirestormWall", &ptr->IsFirestormWall);
        ImGui::Checkbox("IsHospital", &ptr->IsHospital);
        ImGui::Checkbox("IsArmory", &ptr->IsArmory);
        ImGui::Checkbox("IsEMPulseCannon", &ptr->IsEMPulseCannon);
        ImGui::Checkbox("IsTickTank", &ptr->IsTickTank);
        ImGui::Checkbox("IsTurretAnimAVoxel", &ptr->IsTurretAnimAVoxel);
        ImGui::Checkbox("IsCloakGenerator", &ptr->IsCloakGenerator);
        ImGui::Checkbox("IsSensorArray", &ptr->IsSensorArray);
        ImGui::Checkbox("IsICBMLauncher", &ptr->IsICBMLauncher);
        ImGui::Checkbox("IsArtillary", &ptr->IsArtillary);
        ImGui::Checkbox("IsHelipad", &ptr->IsHelipad);
        ImGui::Checkbox("IsGDIBarracks", &ptr->IsGDIBarracks);
        ImGui::Checkbox("IsNODBarracks", &ptr->IsNODBarracks);
        ImGui::Checkbox("IsLimpetMine", &ptr->IsLimpetMine);
        ImGui::Checkbox("IsMobileWar", &ptr->IsMobileWar);
        ImGui::Checkbox("IsMobileStealth", &ptr->IsMobileStealth);
        ImGui::Checkbox("IsJuggernaut", &ptr->IsJuggernaut);
        ImGui::Checkbox("IsCoreDefender", &ptr->IsCoreDefender);
        ImGui::Checkbox("IsBarrelAnimAVoxel", &ptr->IsBarrelAnimAVoxel);
        ImGui::Checkbox("IsTurretAnimExclusive", &ptr->IsTurretAnimExclusive);
        ImGui::Checkbox("field_897", &ptr->field_897);
        ImGui::Checkbox("IsDamagedDoor", &ptr->IsDamagedDoor);
        ImGui::Checkbox("IsInvisibleInGame", &ptr->IsInvisibleInGame);
        ImGui::Checkbox("IsTerrainPalette", &ptr->IsTerrainPalette);
        ImGui::Checkbox("IsCanPlaceAnywhere", &ptr->IsCanPlaceAnywhere);
        ImGui::Checkbox("IsExtraDamageStage", &ptr->IsExtraDamageStage);
        ImGui::Checkbox("CanAIBuildThis", &ptr->CanAIBuildThis);
        ImGui::Checkbox("IsBaseDefense", &ptr->IsBaseDefense);
        ImGui::Checkbox("IsDemandLoad", &ptr->IsDemandLoad);
        ImGui::Checkbox("IsDemandLoadBuildup", &ptr->IsDemandLoadBuildup);
        ImGui::Checkbox("IsFreeBuildup", &ptr->IsFreeBuildup);
        ImGui::Checkbox("IsThreatRatingNode", &ptr->IsThreatRatingNode);

        ImGui::TreePop();
    }

    ImGui::InputInt("SuperWeapon", (int*)&ptr->SuperWeapon);
    ImGui::InputInt("SuperWeapon2", (int*)&ptr->SuperWeapon2);
    ImGui::InputText("VoxelBarrelFile", ptr->VoxelBarrelFile, sizeof(ptr->VoxelBarrelFile));
    ImGui::InputDouble("VoxelBarrelScale", &ptr->VoxelBarrelScale);

    ImGui::InputInt3("VoxelBarrelOffsetToPitchPivotPoint", (int*)&ptr->VoxelBarrelOffsetToPitchPivotPoint);
    ImGui::InputInt3("VoxelBarrelOffsetToRotatePivotPoint", (int*)&ptr->VoxelBarrelOffsetToRotatePivotPoint);
    ImGui::InputInt3("VoxelBarrelOffsetToBuildingPivotPoint", (int*)&ptr->VoxelBarrelOffsetToBuildingPivotPoint);
    ImGui::InputInt3("VoxelBarrelOffsetToBarrelEnd", (int*)&ptr->VoxelBarrelOffsetToBarrelEnd);

    ImGui::InputInt("TurretChargeAnimRate", &ptr->TurretChargeAnimRate);
    ImGui::InputInt("StartPitch", (int*)&ptr->StartPitch);
    ImGui::InputInt("CloakRadiusInCells", (int*)&ptr->CloakRadiusInCells);
    ImGui::InputText("TheaterImageFile", ptr->TheaterImageFile, sizeof(ptr->TheaterImageFile));
}

void ImGui_Draw_BulletTypeClass(BulletTypeClass * ptr)
{
    ASSERT(ptr != nullptr);

    // Draw base classes first
    DRAW_BASE_CLASS(AbstractTypeClass, ptr);
    DRAW_BASE_CLASS(ObjectTypeClass, ptr);

    ImGui::Text("BulletTypeClass");

    ImGui::Separator();

    // Editable booleans
    ImGui::Checkbox("IsAirburst", &ptr->IsAirburst);
    ImGui::Checkbox("IsFloater", &ptr->IsFloater);
    ImGui::Checkbox("IsHigh", &ptr->IsHigh);
    ImGui::Checkbox("IsVeryHigh", &ptr->IsVeryHigh);
    ImGui::Checkbox("IsShadow", &ptr->IsShadow);
    ImGui::Checkbox("IsArcing", &ptr->IsArcing);
    ImGui::Checkbox("IsDropping", &ptr->IsDropping);
    ImGui::Checkbox("IsInvisible", &ptr->IsInvisible);
    ImGui::Checkbox("IsProximityArmed", &ptr->IsProximityArmed);
    ImGui::Checkbox("IsFueled", &ptr->IsFueled);
    ImGui::Checkbox("IsFaceless", &ptr->IsFaceless);
    ImGui::Checkbox("IsInaccurate", &ptr->IsInaccurate);
    ImGui::Checkbox("IsAntiAircraft", &ptr->IsAntiAircraft);
    ImGui::Checkbox("IsAntiGround", &ptr->IsAntiGround);
    ImGui::Checkbox("IsDegenerate", &ptr->IsDegenerate);
    ImGui::Checkbox("IsBouncy", &ptr->IsBouncy);
    ImGui::Checkbox("IsAnimPalette", &ptr->IsAnimPalette);
    ImGui::Checkbox("IsSplits", &ptr->IsSplits);
    ImGui::Checkbox("IsAntiVehicle", &ptr->IsAntiVehicle);

    // Editable integers and floats
    ImGui::InputInt("Cluster", &ptr->Cluster);
    ImGui::InputDouble("Elasticity", &ptr->Elasticity);
    ImGui::InputInt("Acceleration", &ptr->Acceleration);
    ImGui::InputInt("RetargetAccuracy", &ptr->RetargetAccuracy);
    ImGui::InputInt("Arming", &ptr->Arming);
    ImGui::InputInt("ROT", (int*)&ptr->ROT);

    // Enums and raw values
    ImGui::InputInt("Color (ColorSchemeType)", (int*)&ptr->Color);
    ImGui::InputInt("AnimLow", (int*)&ptr->AnimLow);
    ImGui::InputInt("AnimHigh", (int*)&ptr->AnimHigh);
    ImGui::InputInt("AnimRate", (int*)&ptr->AnimRate);

    //ImGui_Draw_ObjectPointer<WeaponTypeClass>(
    //    "AirburstWeapon", 
    //    ptr->AirburstWeapon,
    //    WeaponTypes,
    //    [](WeaponTypeClass* a) { return a->IniName; },
    //    ImGui_Draw_WeaponTypeClass
    //);

    ImGui_Draw_ObjectPointer<AnimTypeClass>(
        "Trailer", 
        ptr->Trailer,
        AnimTypes,
        [](AnimTypeClass* a) { return a->IniName; },
        ImGui_Draw_AnimTypeClass
    );
}

void ImGui_Draw_HouseTypeClass(HouseTypeClass * ptr)
{
    ASSERT(ptr != nullptr);

    // Draw base classes first
    DRAW_BASE_CLASS(AbstractTypeClass, ptr);

    ImGui::Text("HouseTypeClass");

    ImGui::Separator();

    // Enums and IDs
    ImGui::InputInt("HeapID", reinterpret_cast<int*>(&ptr->HeapID));
    ImGui::InputInt("House", reinterpret_cast<int*>(&ptr->House));
    ImGui::InputInt("Side", reinterpret_cast<int*>(&ptr->Side));
    ImGui::InputInt("RemapColor", reinterpret_cast<int*>(&ptr->RemapColor));

    // Bias sliders
    MyImGui::SliderScalarAuto("FirepowerBias", &ptr->FirepowerBias, 0.0, 3.0);
    MyImGui::SliderScalarAuto("GroundspeedBias", &ptr->GroundspeedBias, 0.0, 3.0);
    MyImGui::SliderScalarAuto("AirspeedBias", &ptr->AirspeedBias, 0.0, 3.0);
    MyImGui::SliderScalarAuto("ArmorBias", &ptr->ArmorBias, 0.0, 3.0);
    MyImGui::SliderScalarAuto("ROFBias", &ptr->ROFBias, 0.0, 3.0);
    MyImGui::SliderScalarAuto("CostBias", &ptr->CostBias, 0.0, 3.0);
    MyImGui::SliderScalarAuto("BuildSpeedBias", &ptr->BuildSpeedBias, 0.0, 3.0);

    // Strings
    char suffixBuf[sizeof(ptr->Suffix) + 1] = {};
    std::memcpy(suffixBuf, ptr->Suffix, sizeof(ptr->Suffix));
    if (ImGui::InputText("Suffix", suffixBuf, sizeof(suffixBuf))) {
        std::memcpy(ptr->Suffix, suffixBuf, sizeof(ptr->Suffix));
    }

    char prefixBuf[2] = { ptr->Prefix, 0 };
    if (ImGui::InputText("Prefix", prefixBuf, sizeof(prefixBuf), ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue)) {
        ptr->Prefix = prefixBuf[0];
    }

    // Bools
    ImGui::Checkbox("IsMultiplay", &ptr->IsMultiplay);
    ImGui::Checkbox("IsMultiplayPassive", &ptr->IsMultiplayPassive);
    ImGui::Checkbox("IsWallOwner", &ptr->IsWallOwner);
    ImGui::Checkbox("IsSmartAI", &ptr->IsSmartAI);
}

void ImGui_Draw_InfantryTypeClass(InfantryTypeClass * ptr)
{
    ASSERT(ptr != nullptr);

    // Draw base classes first
    DRAW_BASE_CLASS(AbstractTypeClass, ptr);
    DRAW_BASE_CLASS(ObjectTypeClass, ptr);
    DRAW_BASE_CLASS(TechnoTypeClass, ptr);

    ImGui::Text("InfantryTypeClass");
    ImGui::Separator();

    // Identifiers
    ImGui::InputInt("HeapID", reinterpret_cast<int*>(&ptr->HeapID));
    ImGui::InputInt("Pip", reinterpret_cast<int*>(&ptr->Pip));

    // DoInfoStruct pointer
    ImGui::Text("DoControls: %p", static_cast<void*>(ptr->DoControls));
    // Optional button for detail or picker
    if (ImGui::Button("View DoControls")) {
        ImGui::OpenPopup("DoControlsPopup");
    }

    if (ImGui::BeginPopup("DoControlsPopup")) {
        ImGui::Text("Viewing DoInfoStruct (TODO: Implement viewer)");
        // You could call ImGui_Draw_DoInfoStruct(ptr->DoControls) here if implemented
        ImGui::EndPopup();
    }

    // Fire values
    ImGui::InputInt("FireLaunch", &ptr->FireLaunch);
    ImGui::InputInt("ProneLaunch", &ptr->ProneLaunch);

    // VoiceComment list (TypeList<VocType>)
    ImGui_Draw_DynamicVector("VoiceComment", ptr->VoiceComment);

    // Booleans
    ImGui::Checkbox("IsCyborg", &ptr->IsCyborg);
    ImGui::Checkbox("IsFearless", &ptr->IsFearless);
    ImGui::Checkbox("IsCrawling", &ptr->IsCrawling);
    ImGui::Checkbox("IsCapture", &ptr->IsCapture);
    ImGui::Checkbox("IsFraidyCat", &ptr->IsFraidyCat);
    ImGui::Checkbox("IsTiberiumProof", &ptr->IsTiberiumProof);
    ImGui::Checkbox("IsCivilian", &ptr->IsCivilian);
    ImGui::Checkbox("IsBomber", &ptr->IsBomber);
    ImGui::Checkbox("IsEngineer", &ptr->IsEngineer);
    ImGui::Checkbox("IsDisguised", &ptr->IsDisguised);
    ImGui::Checkbox("IsAgent", &ptr->IsAgent);
    ImGui::Checkbox("IsThief", &ptr->IsThief);
    ImGui::Checkbox("IsVehicleThief", &ptr->IsVehicleThief);
    ImGui::Checkbox("IsDoggie", &ptr->IsDoggie);
    ImGui::Checkbox("IsJumpJet", &ptr->IsJumpJet);
    ImGui::Checkbox("IsWebImmune", &ptr->IsWebImmune);
}

void ImGui_Draw_IsometricTileTypeClass(IsometricTileTypeClass * ptr)
{
    ASSERT(ptr != nullptr);

    // Draw base classes first
    DRAW_BASE_CLASS(AbstractTypeClass, ptr);
    DRAW_BASE_CLASS(ObjectTypeClass, ptr);

    ImGui::Text("IsometricTileTypeClass");
    ImGui::Separator();

    ImGui::InputInt("HeapID", reinterpret_cast<int*>(&ptr->HeapID));
    ImGui::InputInt("MarbleMadness", reinterpret_cast<int*>(&ptr->MarbleMadness));
    ImGui::InputInt("NonMarbleMadness", reinterpret_cast<int*>(&ptr->NonMarbleMadness));
    ImGui::InputInt("field_130", &ptr->field_130);

    // field_134: DynamicVectorClass<unsigned short*>
    if (ImGui::TreeNode("field_134 (DynamicVectorClass<unsigned short*>)")) {
        for (int i = 0; i < ptr->field_134.Count(); ++i) {
            ImGui::Text("Item[%d]: %p", i, (void*)ptr->field_134[i]);
        }
        ImGui::TreePop();
    }

    // Pointer field
    ImGui::Text("NextTileTypeInSet: %p", static_cast<void*>(ptr->NextTileTypeInSet));

    ImGui::InputInt("ToSnowTheater", reinterpret_cast<int*>(&ptr->ToSnowTheater));
    ImGui::InputInt("ToTemperateTheater", reinterpret_cast<int*>(&ptr->ToTemperateTheater));
    ImGui::InputInt("Anim", reinterpret_cast<int*>(&ptr->Anim));

    ImGui::InputInt("XOffset", &ptr->XOffset);
    ImGui::InputInt("YOffset", &ptr->YOffset);
    ImGui::InputInt("AttachesTo", &ptr->AttachesTo);
    ImGui::InputInt("ZAdjust", &ptr->ZAdjust);
    ImGui::InputInt("field_16C", &ptr->field_16C);

    ImGui::Checkbox("IsMorphable", &ptr->IsMorphable);
    ImGui::Checkbox("IsShadowCaster", &ptr->IsShadowCaster);
    ImGui::Checkbox("IsAllowToPlace", &ptr->IsAllowToPlace);
    ImGui::Checkbox("IsRequiredForRMG", &ptr->IsRequiredForRMG);

    ImGui::InputInt("TileBlockHeight", &ptr->TileBlockHeight);
    ImGui::InputInt("TileBlockWidth", &ptr->TileBlockWidth);
    ImGui::InputInt("field_17C", &ptr->field_17C);
    ImGui::InputInt("TilesInSequence", &ptr->TilesInSequence);

    ImGui::Checkbox("IsFileLoaded", &ptr->IsFileLoaded);

    // Filename edit
    char filenameBuf[sizeof(ptr->Filename) + 1] = {};
    std::memcpy(filenameBuf, ptr->Filename, sizeof(ptr->Filename));
    if (ImGui::InputText("Filename", filenameBuf, sizeof(filenameBuf))) {
        std::memcpy(ptr->Filename, filenameBuf, sizeof(ptr->Filename));
    }

    ImGui::Checkbox("IsAllowBurrowing", &ptr->IsAllowBurrowing);
    ImGui::Checkbox("IsAllowTiberium", &ptr->IsAllowTiberium);

    ImGui::InputInt("field_198", &ptr->field_198);
}

void ImGui_Draw_OverlayTypeClass(OverlayTypeClass * ptr)
{
    ASSERT(ptr != nullptr);

    // Draw base classes first
    DRAW_BASE_CLASS(AbstractTypeClass, ptr);
    DRAW_BASE_CLASS(ObjectTypeClass, ptr);

    ImGui::Text("OverlayTypeClass");

    ImGui::Separator();

    ImGui::InputInt("HeapID", reinterpret_cast<int*>(&ptr->HeapID));
    ImGui::InputInt("Land", reinterpret_cast<int*>(&ptr->Land));

    ImGui_Draw_ObjectPointer<AnimTypeClass>(
        "CellAnim", 
        ptr->CellAnim,
        AnimTypes,
        [](AnimTypeClass* a) { return a->IniName; },
        ImGui_Draw_AnimTypeClass
    );

    ImGui::InputInt("DamageLevels", &ptr->DamageLevels);
    ImGui::InputInt("DamagePoints", &ptr->DamagePoints);

    // Booleans
    ImGui::Checkbox("IsWall", &ptr->IsWall);
    ImGui::Checkbox("IsHigh", &ptr->IsHigh);
    ImGui::Checkbox("IsTiberium", &ptr->IsTiberium);
    ImGui::Checkbox("IsCrate", &ptr->IsCrate);
    ImGui::Checkbox("IsCrateTrigger", &ptr->IsCrateTrigger);
    ImGui::Checkbox("NoUseTileLandType", &ptr->NoUseTileLandType);
    ImGui::Checkbox("IsVeinholeMonster", &ptr->IsVeinholeMonster);
    ImGui::Checkbox("IsVeins", &ptr->IsVeins);
    ImGui::Checkbox("IsDemandLoad", &ptr->IsDemandLoad);
    ImGui::Checkbox("IsExplosive", &ptr->IsExplosive);
    ImGui::Checkbox("IsChainReactive", &ptr->IsChainReactive);
    ImGui::Checkbox("IsPriority", &ptr->IsPriority);
    ImGui::Checkbox("DrawFlat", &ptr->DrawFlat);
    ImGui::Checkbox("IsARock", &ptr->IsARock);
}

void ImGui_Draw_ParticleTypeClass(ParticleTypeClass * ptr)
{

}

void ImGui_Draw_ParticleSystemTypeClass(ParticleSystemTypeClass * ptr)
{

}

void ImGui_Draw_ScriptTypeClass(ScriptTypeClass * ptr)
{

}

void ImGui_Draw_SmudgeTypeClass(SmudgeTypeClass * ptr)
{

}

void ImGui_Draw_SuperWeaponTypeClass(SuperWeaponTypeClass * ptr)
{

}

void ImGui_Draw_TeamTypeClass(TeamTypeClass * ptr)
{

}

void ImGui_Draw_TerrainTypeClass(TerrainTypeClass * ptr)
{

}

void ImGui_Draw_TriggerTypeClass(TriggerTypeClass * ptr)
{

}

void ImGui_Draw_TagTypeClass(TagTypeClass * ptr)
{

}

void ImGui_Draw_VoxelAnimTypeClass(VoxelAnimTypeClass * ptr)
{

}

void ImGui_Draw_UnitTypeClass(UnitTypeClass * ptr)
{
    ASSERT(ptr != nullptr);

    // Draw base classes first
    DRAW_BASE_CLASS(AbstractTypeClass, ptr);
    DRAW_BASE_CLASS(ObjectTypeClass, ptr);
    DRAW_BASE_CLASS(TechnoTypeClass, ptr);

    //ImGui::SeparatorText("UnitTypeClass");

    if (ImGui::TreeNode("UnitTypeClass")) {

        ImGui::BeginDisabled();
            ImGui::InputInt("HeapID", (int*)&ptr->HeapID);
        ImGui::EndDisabled();

        ImGui_Draw_EnumCombo<LandType>(
            "MovementRestrictedTo",
            ptr->MovementRestrictedTo,
            LAND_COUNT,
            Name_From_Land
        );

        ImGui::InputInt3("HalfDamageSmokeLocation", &ptr->HalfDamageSmokeLocation.X);

        ImGui::Checkbox("IsPassive", &ptr->IsPassive);
        ImGui::Checkbox("IsCrateGoodie", &ptr->IsCrateGoodie);
        ImGui::Checkbox("IsToHarvest", &ptr->IsToHarvest);
        ImGui::Checkbox("IsToVeinHarvest", &ptr->IsToVeinHarvest);
        ImGui::Checkbox("IsFireAnim", &ptr->IsFireAnim);
        ImGui::Checkbox("IsLockTurret", &ptr->IsLockTurret);
        ImGui::Checkbox("IsNoFireWhileMoving", &ptr->IsNoFireWhileMoving);
        ImGui::Checkbox("IsDeployToFire", &ptr->IsDeployToFire);
        ImGui::Checkbox("IsTilter", &ptr->IsTilter);
        ImGui::Checkbox("UseTurretShadow", &ptr->UseTurretShadow);
        ImGui::Checkbox("IsTooBigToFitUnderBridge", &ptr->IsTooBigToFitUnderBridge);
        ImGui::Checkbox("IsSmallVisceroid", &ptr->IsSmallVisceroid);
        ImGui::Checkbox("IsLargeVisceroid", &ptr->IsLargeVisceroid);
        ImGui::Checkbox("IsCarriesCrate", &ptr->IsCarriesCrate);
        ImGui::Checkbox("IsNonVehicle", &ptr->IsNonVehicle);
        ImGui::Checkbox("IsJellyfish", &ptr->IsJellyfish);
        ImGui::Checkbox("IsLimpetDrone", &ptr->IsLimpetDrone);
        ImGui::Checkbox("IsMobileEMP", &ptr->IsMobileEMP);
        ImGui::Checkbox("IsCoreDefender", &ptr->IsCoreDefender);

        if (ptr->AltImage) {
            if (ImGui::Button("View AltImage")) {
                ImGui::OpenPopup("AltImage");
            }
            if (ImGui::BeginPopupModal("AltImage", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                DebugImageSetKey key = { ptr->IniName, ptr->ID, ptr->RTTI, IMAGE_TYPE_SHAPE };
                auto textures_uploaded = Upload_Shapes_As_ImageSet_Textures(key, *NormalDrawer, ptr->AltImage);
                ASSERT(textures_uploaded);
                static int _current_frame = 0;
                ShowImagePanel("AltImage", key, _current_frame);
                if (ImGui::Button("Close")) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        } else {
            ImGui::TextDisabled("AltImage: NULL");
        }

        ImGui::InputText("AltImageFile", ptr->AltImageFile, sizeof(ptr->AltImageFile));

        ImGui::InputScalar("StandingFrames", ImGuiDataType_U8, &ptr->StandingFrames);
        ImGui::InputScalar("DeathFrames", ImGuiDataType_U8, &ptr->DeathFrames);
        ImGui::InputScalar("DeathFrameRate", ImGuiDataType_U8, &ptr->DeathFrameRate);

        ImGui::InputInt("MaxCharge", &ptr->MaxCharge);
        ImGui::InputInt("StartCharge", &ptr->StartCharge);

        if (ImGui::TreeNode("FiringSyncFrame")) {
            for (int i = 0; i < UnitTypeClass::FIRING_SYNC_FRAME_MAX; ++i) {
                ImGui::PushID(i);
                ImGui::InputInt("Frame", &ptr->FiringSyncFrame[i]);
                ImGui::PopID();
            }
            ImGui::TreePop();
        }

        ImGui::InputInt("StartStandFrame", &ptr->StartStandFrame);
        ImGui::InputInt("StartWalkFrame", &ptr->StartWalkFrame);
        ImGui::InputInt("StartFiringFrame", &ptr->StartFiringFrame);
        ImGui::InputInt("StartDeathFrame", &ptr->StartDeathFrame);
        ImGui::InputInt("MaxDeathCounter", &ptr->MaxDeathCounter);
        ImGui::InputInt("Facings", &ptr->Facings);
        ImGui::InputScalar("WalkFrames", ImGuiDataType_U8, &ptr->WalkFrames);
        ImGui::InputScalar("FiringFrames", ImGuiDataType_U8, &ptr->FiringFrames);

        ImGui::TreePop();

        Reapply_Some_Properties();
    }
}

void ImGui_Draw_WeaponTypeClass(WeaponTypeClass * ptr)
{

}

void ImGui_Draw_WarheadTypeClass(WarheadTypeClass * ptr)
{

}

void ImGui_Draw_AITriggerTypeClass(AITriggerTypeClass * ptr)
{

}
























/**
 *  Dispatcher function for drawing the type classes
 */
void ImGui_Draw_TypeClass(AbstractTypeClass * ptr)
{
    ASSERT(ptr != nullptr);

    switch (ptr->RTTI) {
        case RTTI_AIRCRAFTTYPE: ImGui_Draw_AircraftTypeClass(reinterpret_cast<AircraftTypeClass*>(ptr)); break;
        case RTTI_ANIMTYPE: ImGui_Draw_AnimTypeClass(reinterpret_cast<AnimTypeClass*>(ptr)); break;
        case RTTI_BUILDINGTYPE: ImGui_Draw_BuildingTypeClass(reinterpret_cast<BuildingTypeClass*>(ptr)); break;
        case RTTI_BULLETTYPE: ImGui_Draw_BulletTypeClass(reinterpret_cast<BulletTypeClass*>(ptr)); break;
        case RTTI_HOUSETYPE: ImGui_Draw_HouseTypeClass(reinterpret_cast<HouseTypeClass*>(ptr)); break;
        case RTTI_INFANTRYTYPE: ImGui_Draw_InfantryTypeClass(reinterpret_cast<InfantryTypeClass*>(ptr)); break;
        case RTTI_ISOTILETYPE: ImGui_Draw_IsometricTileTypeClass(reinterpret_cast<IsometricTileTypeClass*>(ptr)); break;
        case RTTI_OVERLAYTYPE: ImGui_Draw_OverlayTypeClass(reinterpret_cast<OverlayTypeClass*>(ptr)); break;
        case RTTI_PARTICLETYPE: ImGui_Draw_ParticleTypeClass(reinterpret_cast<ParticleTypeClass*>(ptr)); break;
        case RTTI_PARTICLESYSTEMTYPE: ImGui_Draw_ParticleSystemTypeClass(reinterpret_cast<ParticleSystemTypeClass*>(ptr)); break;
        case RTTI_SCRIPTTYPE: ImGui_Draw_ScriptTypeClass(reinterpret_cast<ScriptTypeClass*>(ptr)); break;
        case RTTI_SMUDGETYPE: ImGui_Draw_SmudgeTypeClass(reinterpret_cast<SmudgeTypeClass*>(ptr)); break;
        case RTTI_SUPERWEAPONTYPE: ImGui_Draw_SuperWeaponTypeClass(reinterpret_cast<SuperWeaponTypeClass*>(ptr)); break;
        case RTTI_TEAMTYPE: ImGui_Draw_TeamTypeClass(reinterpret_cast<TeamTypeClass*>(ptr)); break;
        case RTTI_TERRAINTYPE: ImGui_Draw_TerrainTypeClass(reinterpret_cast<TerrainTypeClass*>(ptr)); break;
        case RTTI_TRIGGERTYPE: ImGui_Draw_TriggerTypeClass(reinterpret_cast<TriggerTypeClass*>(ptr)); break;
        case RTTI_TAGTYPE: ImGui_Draw_TagTypeClass(reinterpret_cast<TagTypeClass*>(ptr)); break;
        case RTTI_UNITTYPE: ImGui_Draw_UnitTypeClass(reinterpret_cast<UnitTypeClass*>(ptr)); break;
        case RTTI_VOXELANIMTYPE: ImGui_Draw_VoxelAnimTypeClass(reinterpret_cast<VoxelAnimTypeClass*>(ptr)); break;
        case RTTI_WEAPONTYPE: ImGui_Draw_WeaponTypeClass(reinterpret_cast<WeaponTypeClass*>(ptr)); break;
        case RTTI_WARHEADTYPE: ImGui_Draw_WarheadTypeClass(reinterpret_cast<WarheadTypeClass*>(ptr)); break;
        case RTTI_AITRIGGERTYPE: ImGui_Draw_AITriggerTypeClass(reinterpret_cast<AITriggerTypeClass*>(ptr)); break;

        default:
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Unsupported RTTI: %d", ptr->RTTI);
            break;
    }
}

void ImGui_Draw_Class(AbstractClass * ptr)
{
    ASSERT(ptr != nullptr);

    switch (ptr->RTTI) {
    }
}
















/**
 * 
 * Pickers
 * 
 */

void MyImGui::ShowUnitTypePicker(UnitTypeClass ** value, const char * label)
{
    // Display current selection as a button
    const char * currentName = (*value)->IniName;
    if (ImGui::Button(currentName, ImVec2(180, 0))) {
        ImGui::OpenPopup(("##pick_" + std::string(label)).c_str());
    }

    // Same line label
    ImGui::SameLine();
    ImGui::TextUnformatted(label);

    // Popup listing all UnitTypes
    if (ImGui::BeginPopup(("##pick_" + std::string(label)).c_str())) {

        if (ImGui::Selectable("<none>", *value == nullptr)) {
            *value = nullptr;
        }

        //  Enumerate global list (replace with your container)
        for (int i = 0; i < UnitTypes.Count(); ++i) {

            UnitTypeClass* ut = UnitTypes[i];
            if (!ut) {
                continue;
            }

            bool selected = (ut == *value);
            if (ImGui::Selectable(ut->IniName, selected)) {
                *value = ut;
            }

            if (selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndPopup();
    }
}

/**
 * 
 * Windows
 * 
 */
void MyImGui::DoCheatsWindow(bool *pOpen)
{
    bool disabled = false;
    if (Session.Players.Count() > 1) {
        disabled = true;
    }

    // Don't continue if flag pointer is NULL
    if (!pOpen) {
        return;
    }

    // Open a window with a close button in the corner
    if (!ImGui::Begin("Cheats", pOpen)) {
        ImGui::End(); // required to balance ImGui::Begin()
        return;
    }

    ImGui::Columns(2, nullptr, false);  // Two columns, no vertical separator

    // Row 1
    {
    ImGui::Text("Instant Build (Player)");
    ImGui::NextColumn();
    ImGui::Checkbox("##InstantBuildPlayer", &Vinifera_Developer_InstantBuild);
    ImGui::NextColumn();
    }

    // Row 2
    {
    ImGui::Text("Instant Build (Computer)");
    ImGui::NextColumn();
    ImGui::Checkbox("##InstantBuildPlayer", &Vinifera_Developer_AIInstantBuild);
    ImGui::NextColumn();
    }

    // Row 3
    {
    ImGui::Text("Outcome");
    ImGui::NextColumn();

    float totalWidth = ImGui::GetContentRegionAvail().x;
    float spacing = ImGui::GetStyle().ItemSpacing.x;
    float buttonWidth = (totalWidth - 2 * spacing) / 3;

    if (ImGui::Button("Win", ImVec2(buttonWidth, 0))) {
        /**
         *  Player wins.
         */
        PlayerPtr->Flag_To_Win();
    }
    ImGui::SameLine();
    if (ImGui::Button("Lose", ImVec2(buttonWidth, 0))) {
        /**
         *  Player loses.
         */
        PlayerPtr->Flag_To_Lose();
    }
    ImGui::SameLine();
    if (ImGui::Button("Die", ImVec2(buttonWidth, 0))) {
        /**
         *  Player dies.
         */
        PlayerPtr->Flag_To_Die();
    }
    ImGui::NextColumn();
    }

    // Row 4
    {
    ImGui::Text("Give $10,000 credits to player");
    ImGui::NextColumn();
    if (ImGui::Button("Grant")) {
    /**
     *  Give 10,000 credits to the player.
     */
    PlayerPtr->Refund_Money(10000);
    }
    ImGui::NextColumn();
    }

    // Row 5
    {
    ImGui::Text("Give $10,000 credits to player");
    ImGui::NextColumn();
    if (ImGui::Button("Grant")) {
    /**
     *  Give 10,000 credits to the player.
     */
    PlayerPtr->Refund_Money(10000);
    }
    ImGui::NextColumn();
    }

    // End columns and window
    ImGui::Columns(1);

    ImGui::End();
}

void MyImGui::DoDebugWindow(bool *pOpen)
{
    bool disabled = false;
    if (Session.Players.Count() > 1) {
        disabled = true;
    }

    // Don't continue if flag pointer is NULL
    if (!pOpen) {
        return;
    }

    // Open a window with a close button in the corner
    if (!ImGui::Begin("Debug", pOpen)) {
        ImGui::End(); // required to balance ImGui::Begin()
        return;
    }

    ImGui::Columns(2, nullptr, false);  // Two columns, no vertical separator

    // Row 1
    {
    ImGui::Text("Instant Build (Player)");
    ImGui::NextColumn();
    ImGui::Checkbox("##InstantBuildPlayer", &Vinifera_Developer_InstantBuild);
    ImGui::NextColumn();
    }

    ImGui::NextColumn();

    // End columns and window
    ImGui::Columns(1);

    ImGui::End();
}

void MyImGui::DoRulesEditor(bool *pOpen)
{
    bool disabled = false;
    if (Session.Players.Count() > 1) {
        disabled = true;
    }

    // Don't continue if flag pointer is NULL
    if (!pOpen) {
        return;
    }

    // Open a window with a close button in the corner
    if (!ImGui::Begin("Rules Editor", pOpen)) {
        ImGui::End(); // required to balance ImGui::Begin()
        return;
    }

    /*  GROUP: simple fields */
    if (ImGui::CollapsingHeader("Basic Flags & Values", ImGuiTreeNodeFlags_DefaultOpen)) {

        /* --- ints --- */
        ImGui::InputInt("AmmoCrateDamage",               &Rule->AmmoCrateDamage);
        ImGui::InputInt("AttackingAircraftSightRange",   &Rule->AttackingAircraftSightRange);
        ImGui::InputInt("WaypointAnimationSpeed",        &Rule->WaypointAnimationSpeed);
        ImGui::InputInt("FlashFrameTime",                &Rule->FlashFrameTime);
        ImGui::InputInt("RadarCombatFlashTime",          &Rule->RadarCombatFlashTime);
        ImGui::InputInt("MaxWaypointPathLength",         &Rule->MaxWaypointPathLength);
        ImGui::InputInt("VeinholeMonsterStrength",       &Rule->VeinholeMonsterStrength);
        ImGui::InputInt("MaxVeinholeGrowth",             &Rule->MaxVeinholeGrowth);
        ImGui::InputInt("VeinholeGrowthRate",            &Rule->VeinholeGrowthRate);
        ImGui::InputInt("VeinholeShrinkRate",            &Rule->VeinholeShrinkRate);
        ImGui::InputInt("VeinDamage",                    &Rule->VeinDamage);
        ImGui::InputInt("MaximumQueuedObjects",          &Rule->MaximumQueuedObjects);
        ImGui::InputInt("AircraftFogReveal",             &Rule->AircraftFogReveal);
        ImGui::InputInt("BridgeVoxelMax",                &Rule->BridgeVoxelMax);
        ImGui::InputInt("CloakingStages",                &Rule->CloakingStages);
        ImGui::InputInt("RevealTriggerRadius",           &Rule->RevealTriggerRadius);
        ImGui::InputInt("SpotlightMovementRadius",       &Rule->SpotlightMovementRadius);
        ImGui::InputInt("SpotlightLocationRadius",       &Rule->SpotlightLocationRadius);
        ImGui::InputInt("SpotlightRadius",               &Rule->SpotlightRadius);

        /* --- bools --- */
        ImGui::Checkbox("IsFreeMCV",                     &Rule->IsFreeMCV);
        ImGui::Checkbox("IsBerzerkAllowed",              &Rule->IsBerzerkAllowed);
        ImGui::Checkbox("IsComputerParanoid",            &Rule->IsComputerParanoid);
        ImGui::Checkbox("IsCurleyShuffle",               &Rule->IsCurleyShuffle);
        ImGui::Checkbox("IsBlendedFog",                  &Rule->IsBlendedFog);
        ImGui::Checkbox("IsCompEasyBonus",               &Rule->IsCompEasyBonus);
        ImGui::Checkbox("IsFineDifficulty",              &Rule->IsFineDifficulty);
        ImGui::Checkbox("IsExplosiveHarvester",          &Rule->IsExplosiveHarvester);
        ImGui::Checkbox("IsHealthBar",                   &Rule->IsHealthBar);
        ImGui::Checkbox("IsAllyReveal",                  &Rule->IsAllyReveal);
        ImGui::Checkbox("IsSeparate",                    &Rule->IsSeparate);
        /* …add remaining bools the same way … */
    }

    /*  GROUP: floats & doubles */
    if (ImGui::CollapsingHeader("Rates / Coefficients"))
    {
        SliderScalarAuto("TunnelSpeed",                      &Rule->TunnelSpeed);
        SliderScalarAuto("TiberiumHeal",                     &Rule->TiberiumHeal);
        SliderScalarAuto("RadarEventColorSpeed",             &Rule->RadarEventColorSpeed);
        SliderScalarAuto("RadarEventSpeed",                  &Rule->RadarEventSpeed);
        SliderScalarAuto("RadarEventRotationSpeed",          &Rule->RadarEventRotationSpeed);
        SliderScalarAuto("AITriggerSuccessWeightDelta",      &Rule->AITriggerSuccessWeightDelta);
        SliderScalarAuto("AITriggerFailureWeightDelta",      &Rule->AITriggerFailureWeightDelta);
        SliderScalarAuto("AITriggerTrackRecordCoefficient",  &Rule->AITriggerTrackRecordCoefficient);
        SliderScalarAuto("JumpjetClimb",                     &Rule->JumpjetClimb);
        SliderScalarAuto("JumpjetAcceleration",              &Rule->JumpjetAcceleration);
        SliderScalarAuto("JumpjetWobblesPerSecond",          &Rule->JumpjetWobblesPerSecond);
        SliderScalarAuto("ZoomInFactor",                     &Rule->ZoomInFactor);
        /* …continue for every float/double… */
    }

    /*  GROUP: pointer pickers */
    if (ImGui::CollapsingHeader("TypeClass Pointers")) {

        ShowUnitTypePicker     (&Rule->LargeVisceroid,      "LargeVisceroid");
        ShowUnitTypePicker     (&Rule->SmallVisceroid,      "SmallVisceroid");
        ShowUnitTypePicker     (&Rule->UnloadingHarvester,  "UnloadingHarvester");
//        ShowAnimTypePicker     (&Rule->DropPodPuff,         "DropPodPuff");
//        ShowAnimTypePicker     (&Rule->BarrelExplode,       "BarrelExplode");
//        ShowParticleSysPicker  (&Rule->BarrelParticle,      "BarrelParticle");
//        ShowAnimTypePicker     (&Rule->Wake,                "Wake");
        /* …repeat pattern for all *TypeClass pointers… */
    }

    /*  GROUP: vector editors */
    if (ImGui::CollapsingHeader("Lists / Vectors")) {

//        ShowPointerVector("HSBuilding",              &Rule->HSBuilding);
//        ShowPointerVector("BarrelDebris",            &Rule->BarrelDebris);
//        ShowPointerVector("DropPod",                 &Rule->DropPod);
//        ShowPointerVector("DeadBodies",              &Rule->DeadBodies);
//        ShowPointerVector("MetallicDebris",          &Rule->MetallicDebris);
//        ShowPointerVector("BridgeExplosions",        &Rule->BridgeExplosions);
//        ShowIntVector    ("PrerequisitePower",       &Rule->PrerequisitePower);
//        ShowIntVector    ("PrerequisiteFactory",     &Rule->PrerequisiteFactory);
//        ShowIntVector    ("PrerequisiteBarracks",    &Rule->PrerequisiteBarracks);
//        ShowIntVector    ("PrerequisiteRadar",       &Rule->PrerequisiteRadar);
//        ShowIntVector    ("PrerequisiteTech",        &Rule->PrerequisiteTech);
        /* …and so on for every TypeList… */
    }

    ImGui::End();
}

void MyImGui::DoScenarioEditor(bool *pOpen)
{
    bool disabled = false;
    if (Session.Players.Count() > 1) {
        disabled = true;
    }

    // Don't continue if flag pointer is NULL
    if (!pOpen) {
        return;
    }

    // Open a window with a close button in the corner
    if (!ImGui::Begin("Scenario Editor", pOpen)) {
        ImGui::End(); // required to balance ImGui::Begin()
        return;
    }

    // Paths and Strings
    ImGui::InputText("NextScenarioName", Scen->NextScenarioName, sizeof(Scen->NextScenarioName));
    ImGui::InputText("AltNextScenarioName", Scen->AltNextScenarioName, sizeof(Scen->AltNextScenarioName));
    ImGui::InputText("ScenarioName", Scen->ScenarioName, sizeof(Scen->ScenarioName));
    ImGui::InputText("Description", Scen->Description, sizeof(Scen->Description));

    // Briefing text
    static bool editingBriefing = false;

    if (editingBriefing) {
        if (ImGui::Button("Save Briefing")) editingBriefing = false;

        ImGui::InputTextMultiline("##BriefingEdit",
            Scen->BriefingText, sizeof(Scen->BriefingText),
            ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 8));
    } else {
        if (ImGui::Button("Edit Briefing")) editingBriefing = true;

        ImGui::PushTextWrapPos();
        ImGui::TextWrapped("%s", Scen->BriefingText);
        ImGui::PopTextWrapPos();
    }

    // Integers
    ImGui::InputInt("Home", &Scen->Home);
    ImGui::InputInt("AltHome", &Scen->AltHome);

    // UniqueID should not be modified!
    ImGui::BeginDisabled();
        ImGui::InputInt("UniqueID", &Scen->UniqueID);
    ImGui::EndDisabled();

    ImGui::InputInt("Scenario", &Scen->Scenario);
    ImGui::InputInt("CarryOverCap", &Scen->CarryOverCap);
    ImGui::InputInt("Percent", &Scen->Percent);
    ImGui::InputInt("BridgeCount", &Scen->BridgeCount);
    ImGui::InputInt("StartingDropships", &Scen->StartingDropships);
    ImGui::InputInt("AmbientLight", &Scen->AmbientLight);
    ImGui::InputInt("CurrentAmbientLight", &Scen->CurrentAmbientLight);
    ImGui::InputInt("DesiredAmbientLight", &Scen->DesiredAmbientLight);
    ImGui::InputInt("RedTint", &Scen->RedTint);
    ImGui::InputInt("GreenTint", &Scen->GreenTint);
    ImGui::InputInt("BlueTint", &Scen->BlueTint);
    ImGui::InputInt("GroundLight", &Scen->GroundLight);
    ImGui::InputInt("LevelLight", &Scen->LevelLight);
    ImGui::InputInt("IonAmbientLight", &Scen->IonAmbientLight);
    ImGui::InputInt("IonRedTint", &Scen->IonRedTint);
    ImGui::InputInt("IonGreenTint", &Scen->IonGreenTint);
    ImGui::InputInt("IonBlueTint", &Scen->IonBlueTint);
    ImGui::InputInt("IonGroundLight", &Scen->IonGroundLight);
    ImGui::InputInt("IonLevelLight", &Scen->IonLevelLight);
    ImGui::InputInt("InitTime", &Scen->InitTime);
    ImGui::InputInt("Stage", reinterpret_cast<int*>(&Scen->Stage)); // ushort cast

    // Booleans
    ImGui::Checkbox("IsFreeRadar", &Scen->IsFreeRadar);
    ImGui::Checkbox("IsTrainCrate", &Scen->IsTrainCrate);
    ImGui::Checkbox("IsTibGrowth", &Scen->IsTibGrowth);
    ImGui::Checkbox("IsVeinGrowth", &Scen->IsVeinGrowth);
    ImGui::Checkbox("IsIceGrowth", &Scen->IsIceGrowth);
    ImGui::Checkbox("IsBridgeChanged", &Scen->IsBridgeChanged);
    ImGui::Checkbox("IsFlagChanged", &Scen->IsFlagChanged);
    ImGui::Checkbox("IsAmbientChanged", &Scen->IsAmbientChanged);
    ImGui::Checkbox("IsEndOfGame", &Scen->IsEndOfGame);
    ImGui::Checkbox("IsInheritTimer", &Scen->IsInheritTimer);
    ImGui::Checkbox("IsSkipScore", &Scen->IsSkipScore);
    ImGui::Checkbox("IsOneTimeOnly", &Scen->IsOneTimeOnly);
    ImGui::Checkbox("IsNoMapSel", &Scen->IsNoMapSel);
    ImGui::Checkbox("IsTruckCrate", &Scen->IsTruckCrate);
    ImGui::Checkbox("IsMoneyTiberium", &Scen->IsMoneyTiberium);
    ImGui::Checkbox("IsTiberiumDeathToVisceroid", &Scen->IsTiberiumDeathToVisceroid);
    ImGui::Checkbox("IsIgnoreGlobalAITriggers", &Scen->IsIgnoreGlobalAITriggers);
    ImGui::Checkbox("IsGDI", &Scen->IsGDI);
    ImGui::Checkbox("IsMultiplayerOnly", &Scen->IsMultiplayerOnly);
    ImGui::Checkbox("IsRandom", &Scen->IsRandom);
    ImGui::Checkbox("IsCratePickup", &Scen->IsCratePickup);
    ImGui::Checkbox("field_1E38", &Scen->field_1E38);
    ImGui::Checkbox("InputLock", &Scen->InputLock);

    // Floats
    MyImGui::SliderScalarAuto("CarryOverPercent", &Scen->CarryOverPercent, 0.0f, 100.0f);

    // Enums — placeholder combos
    const char* campaign_names[] = { "None", "GDI", "Nod", "Custom" };  // Replace with actual
    ImGui::Combo("Campaign", reinterpret_cast<int*>(&Scen->Campaign), campaign_names, IM_ARRAYSIZE(campaign_names));

    const char* theater_names[] = { "Temperate", "Snow", "Urban", "Desert", "Lunar" };  // Replace
    ImGui::Combo("Theater", reinterpret_cast<int*>(&Scen->Theater), theater_names, IM_ARRAYSIZE(theater_names));

    const char* house_names[] = { "GDI", "Nod", "Neutral", "Civilian" };  // Replace
    ImGui::Combo("PlayerHouse", reinterpret_cast<int*>(&Scen->PlayerHouse), house_names, IM_ARRAYSIZE(house_names));

    const char* speech_side_names[] = { "GDI", "Nod", "Mutant", "Random" };  // Replace
    ImGui::Combo("SpeechSide", reinterpret_cast<int*>(&Scen->SpeechSide), speech_side_names, IM_ARRAYSIZE(speech_side_names));

    // Movie types (VQType enums)
    ShowVQPicker(&Scen->IntroMovie, "IntroMovie");
    ShowVQPicker(&Scen->BriefMovie, "BriefMovie");
    ShowVQPicker(&Scen->WinMovie, "WinMovie");
    ShowVQPicker(&Scen->LoseMovie, "LoseMovie");
    ShowVQPicker(&Scen->ActionMovie, "ActionMovie");
    ShowVQPicker(&Scen->PostScoreMovie, "PostScoreMovie");
    ShowVQPicker(&Scen->PreMapSelectMovie, "PreMapSelectMovie");

    // Lists (TypeList support)
//    ShowPointerVector("AllowableUnits", &Scen->AllowableUnits);
//    ShowIntVector("AllowableUnitMaximums", &Scen->AllowableUnitMaximums);
//    ShowIntVector("AllowableUnitUsage", &Scen->AllowableUnitUsage);

    // Custom structs (placeholders)
    if (ImGui::CollapsingHeader("Special")) {
        DrawSpecialEditor(nullptr);  // Or &open flag
    }

    if (ImGui::CollapsingHeader("Global Flags")) {
        for (int i = 0; i < ARRAYSIZE(Scen->GlobalFlags); ++i) {
            ImGui::Checkbox(("[" + ccstd::to_string(i) + "] " + Scen->GlobalFlags[i].Name).c_str(), (bool*)&Scen->GlobalFlags[i].Value);
        }
    }

    if (ImGui::CollapsingHeader("Local Flags")) {
        for (int i = 0; i < ARRAYSIZE(Scen->LocalFlags); ++i) {
            ImGui::Checkbox(("[" + ccstd::to_string(i) + "] " + Scen->LocalFlags[i].Name).c_str(), (bool*)&Scen->LocalFlags[i].Value);
        }
    }


    ImGui::End();
}

void MyImGui::DoSpecialEditor(bool *pOpen)
{
    bool disabled = false;
    if (Session.Players.Count() > 1) {
        disabled = true;
    }


}

void MyImGui::DoGlobalsEditor(bool *pOpen)
{
    bool disabled = false;
    if (Session.Players.Count() > 1) {
        disabled = true;
    }

    // Don't continue if flag pointer is NULL
    if (!pOpen) {
        return;
    }

    // Open a window with a close button in the corner
    if (!ImGui::Begin("Globals Editor", pOpen)) {
        ImGui::End(); // required to balance ImGui::Begin()
        return;
    }

    // --------------------------------------
    if (ImGui::CollapsingHeader("Debug Options", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Windowed", &Debug_Windowed);
        ImGui::Checkbox("ScenarioFilenameSet", &Debug_ScenarioFilenameSet);
        ImGui::InputText("ScenarioFilename", Debug_ScenarioFilename, 128);

        ImGui::Checkbox("MotionCapture", &Debug_MotionCapture);
        ImGui::Checkbox("AllowModeToggle", &Debug_AllowModeToggle);
        ImGui::Checkbox("Quiet", &Debug_Quiet);
        ImGui::Checkbox("Remap", &Debug_Remap);
        ImGui::Checkbox("Icon", &Debug_Icon);
        ImGui::Checkbox("Flag", &Debug_Flag);
        ImGui::Checkbox("Lose", &Debug_Lose);
        ImGui::Checkbox("Win", &Debug_Win);
        ImGui::Checkbox("Map", &Debug_Map);
        ImGui::Checkbox("Passable", &Debug_Passable);
        ImGui::Checkbox("Unshroud", &Debug_Unshroud);
        ImGui::Checkbox("Threat", &Debug_Threat);
        ImGui::Checkbox("Find_Path", &Debug_Find_Path);
        ImGui::Checkbox("Check_Map", &Debug_Check_Map);
        ImGui::Checkbox("Playtest", &Debug_Playtest);
        ImGui::Checkbox("Trap_Check_Heap", &Debug_Trap_Check_Heap);
        ImGui::Checkbox("Modem_Dump", &Debug_Modem_Dump);
        ImGui::Checkbox("Print_Events", &Debug_Print_Events);
    }

    if (ImGui::CollapsingHeader("Cheat Flags", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Cheat", &Debug_Cheat);
        ImGui::Checkbox("Cheat_Pengo", &Cheat_Pengo);
        ImGui::Checkbox("Cheat_TheTeam", &Cheat_TheTeam);
    }

    if (ImGui::CollapsingHeader("Scenario / Player State", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("UserInputLocked", &UserInputLocked);
        ImGui::InputInt("ScenarioInit", &ScenarioInit);
        ImGui::Checkbox("AllowVoice", &AllowVoice);
        ImGui::Checkbox("TacticalViewActive", &TacticalViewActive);
        ImGui::Checkbox("ScenarioStarted", &ScenarioStarted);
        ImGui::Checkbox("GameInFocus", &GameInFocus);
        ImGui::Checkbox("GameActive", &GameActive);
        ImGui::Checkbox("PlayerWins", &PlayerWins);
        ImGui::Checkbox("PlayerLoses", &PlayerLoses);
        ImGui::Checkbox("PlayerRestarts", &PlayerRestarts);
        ImGui::Checkbox("PlayerAborts", &PlayerAborts);
        ImGui::Checkbox("ScoresPresent", &ScoresPresent);
    }

    if (ImGui::CollapsingHeader("Timers", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::TextDisabled("Timers are referenced, not editable directly:");
        ImGui::Text("NetFrameTimer:  %d", NetFrameTimer.Value());
        ImGui::Text("TickCount:      %d", TickCount.Value());
        ImGui::Text("FrameTimer:     %d", FrameTimer.Value());
        ImGui::Text("ActionLineTimer:%d", ActionLineTimer.Value());
    }

    if (ImGui::CollapsingHeader("Frame Control", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::InputInt("NewMaxAheadFrame1", &NewMaxAheadFrame1);
        ImGui::InputInt("NewMaxAheadFrame2", &NewMaxAheadFrame2);
        ImGui::InputInt("FramesPerSecond", (int*)&FramesPerSecond);
    }

    // --------------------------------------
    if (ImGui::CollapsingHeader("Team State", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::InputInt("TeamEvent", &TeamEvent);
        ImGui::InputInt("TeamNumber", &TeamNumber);
    }

    // --------------------------------------
    if (ImGui::CollapsingHeader("Pointers / Objects", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("PlayerPtr:        %p", PlayerPtr);
        ImGui::Text("MasterParticle:   %p", MasterParticle);
        ImGui::Text("EndGame:          %p", &EndGame);
        ImGui::Text("SpecialDialog:    %d", (int)SpecialDialog); // Enum as int
        ImGui::Text("Whom (HouseType): %d", (int)Whom); // Enum as int
    }

    // --------------------------------------
    if (ImGui::CollapsingHeader("Seed & Random", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::InputScalar("Seed", ImGuiDataType_U32, &Seed);
        ImGui::InputScalar("CustomSeed", ImGuiDataType_U32, &CustomSeed);
    }

    // --------------------------------------
    if (ImGui::CollapsingHeader("UI Layout", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("RedrawSidebar", &RedrawSidebar);

        ImGui::Text("SidebarRect");
        ImGui::InputInt4("##Sidebar", &SidebarRect.X);

        ImGui::Text("TacticalRect");
        ImGui::InputInt4("##Tactical", &TacticalRect.X);

        ImGui::Text("ScreenRect");
        ImGui::InputInt4("##Screen", &ScreenRect.X);
    }

    if (ImGui::CollapsingHeader("Misc", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::InputInt("BuildLevel", &BuildLevel);
        ImGui::InputInt("RequiredAddon", &RequiredAddon);
        ImGui::InputInt("NewINIFormat", &NewINIFormat);
    }

    ImGui::End();
}

void MyImGui::DoTypesEditor(bool *pOpen)
{
    bool disabled = false;
    if (Session.Players.Count() > 1) {
        disabled = true;
    }

    if (!ImGui::Begin("Types", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::End();
        return;
    }

    // Macro to emit one line (label, size, button)
#define TYPE_LINE(label, vec)                             \
    ImGui::Text("%s", label);                             \
    ImGui::SameLine(200);                                 \
    ImGui::Text("%d", vec.Count());                       \
    ImGui::SameLine(260);                                 \
    if (ImGui::Button(("View##" label)))                  \
        ImGui::OpenPopup(label);                          \
    //ShowVectorViewer(label, vec);

    // ---- GROUP: Abstract / Object / Techno ----------------
    //if (ImGui::CollapsingHeader("Core Types", ImGuiTreeNodeFlags_DefaultOpen))
    //{
        //TYPE_LINE("AbstractTypes",     AbstractTypes);
        //TYPE_LINE("ObjectTypes",       ObjectTypes);
        //TYPE_LINE("TechnoTypes",       TechnoTypes);
        ImGui_Show_Vector_ListWindow<BuildingTypeClass>("BuildingTypes List", "Building", BuildingTypes, ImGui_Draw_BuildingTypeClass);
        ImGui_Show_Vector_ListWindow<UnitTypeClass>("UnitTypes List", "Unit", UnitTypes, ImGui_Draw_UnitTypeClass);
        ImGui_Show_Vector_ListWindow<InfantryTypeClass>("InfantryTypes List", "Infantry", InfantryTypes, ImGui_Draw_InfantryTypeClass);
        ImGui_Show_Vector_ListWindow<AircraftTypeClass>("AircraftTypes List", "Aircraft", AircraftTypes, ImGui_Draw_AircraftTypeClass);
        //TYPE_LINE("VoxelAnimTypes",    VoxelAnimTypes);
        //TYPE_LINE("WeaponTypes",       WeaponTypes);
        //TYPE_LINE("BulletTypes",       BulletTypes);
        //TYPE_LINE("WarheadTypes",      WarheadTypes);
        //TYPE_LINE("OverlayTypes",      OverlayTypes);
    //}

    // ---- GROUP: Game World Objects ------------------------
    if (ImGui::CollapsingHeader("World Objects"))
    {
        TYPE_LINE("Objects", Objects);
        TYPE_LINE("Technos", Technos);
        TYPE_LINE("Foots", Foots);
        TYPE_LINE("Buildings", Buildings);
        TYPE_LINE("Units", Units);
        TYPE_LINE("Infantry", Infantry);
        TYPE_LINE("Aircrafts", Aircrafts);
        TYPE_LINE("Bullets", Bullets);
        TYPE_LINE("VoxelAnims", VoxelAnims);
        TYPE_LINE("ParticleSystems", ParticleSystems);
        TYPE_LINE("Particles", Particles);
        TYPE_LINE("IsoTiles", IsoTiles);
        TYPE_LINE("Terrains", Terrains);
        TYPE_LINE("Smudges", Smudges);
        TYPE_LINE("Overlays", Overlays);
    }

    // ---- GROUP: AI / Teams / Scripts ----------------------
    if (ImGui::CollapsingHeader("AI & Teams"))
    {
        TYPE_LINE("AITriggerTypes", AITriggerTypes);
        TYPE_LINE("TeamTypes", TeamTypes);
        TYPE_LINE("Teams", Teams);
        TYPE_LINE("TaskForces", TaskForces);
        TYPE_LINE("ScriptTypes", ScriptTypes);
        TYPE_LINE("Scripts", Scripts);
        TYPE_LINE("TEvents", TEvents);
        TYPE_LINE("TActions", TActions);
    }

    // ---- GROUP: Houses / Sides ----------------------------
    if (ImGui::CollapsingHeader("Houses & Sides"))
    {
        TYPE_LINE("HouseTypes", HouseTypes);
        TYPE_LINE("Houses", Houses);
        TYPE_LINE("Sides", Sides);
        TYPE_LINE("ColorSchemes", ColorSchemes);
    }

    // ---- GROUP: Audio / Video -----------------------------
    if (ImGui::CollapsingHeader("Audio / Video"))
    {
        TYPE_LINE("Vocs", Vocs);
        TYPE_LINE("IngameVQ", IngameVQ);
    }

    // ---- GROUP: Misc --------------------------------------
    if (ImGui::CollapsingHeader("Misc"))
    {
        TYPE_LINE("Converts", Converts);
        TYPE_LINE("TileDrawers", TileDrawers);
        TYPE_LINE("LightSources", LightSources);
        TYPE_LINE("ParticleSystemTypes", ParticleSystemTypes);
        TYPE_LINE("Spotlights", Spotlights);
        TYPE_LINE("LaserDraws", LaserDraws);
        TYPE_LINE("Empulses", Empulses);
        TYPE_LINE("AlphaShapes", AlphaShapes);
        TYPE_LINE("FoggedObjects", FoggedObjects);
        TYPE_LINE("VeinholeMonsters", VeinholeMonsters);
    }

#undef  TYPE_LINE
    ImGui::End();
}


void __cdecl Developer_Window_Helper_At_Exit()
{
    for (auto& pair : DebugImageSetCache) {
        pair.second.Release_All();
    }

    DebugImageSetCache.clear();
}
