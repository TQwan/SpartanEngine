/*
Copyright(c) 2016-2018 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

//= INCLUDES ====
#include <string>
//===============

enum IconProvider_Icon
{
	Icon_Component_Options,
	Icon_Component_AudioListener,
	Icon_Component_AudioSource,
	Icon_Component_Camera,
	Icon_Component_Collider,
	Icon_Component_Light,
	Icon_Component_Material,
	Icon_Component_MeshCollider,
	Icon_Component_MeshFilter,
	Icon_Component_MeshRenderer,
	Icon_Component_RigidBody,
	Icon_Component_Script,
	Icon_Component_Transform,
	Icon_Console_Info,
	Icon_Console_Warning,
	Icon_Console_Error,
	Icon_File_Default,
	Icon_Folder,
	Icon_File_Audio,
	Icon_File_Scene,
	Icon_File_Model,
	Icon_Button_Play
};

namespace Directus { class Context; }

// An image
#define ICON_PROVIDER_IMAGE(icon_enum, size)	\
	ImGui::Image(								\
	IconProvider::GetShaderResource(icon_enum),	\
	ImVec2(size, size),							\
	ImVec2(0, 0),								\
	ImVec2(1, 1),								\
	ImColor(255, 255, 255, 255),				\
	ImColor(255, 255, 255, 0))					\

// An image button by enum
#define ICON_PROVIDER_IMAGE_BUTTON_ENUM(icon_enum, size) ImGui::ImageButton(ICON_PROVIDER(icon_enum), ImVec2(size, size))
// An image button by enum, with a specific ID
#define ICON_PROVIDER_IMAGE_BUTTON_ENUM_ID(id, icon_enum, size) IconProvider::ImageButton_enum_id(id, icon_enum, size)
// An image button by filepath
#define ICON_PROVIDER_IMAGE_BUTTON_FILEPATH(filepath, size) ImGui::ImageButton(ICON_PROVIDER(filepath), ImVec2(size, size))
// An icon shader resource pointer by enum or filePath
#define ICON_PROVIDER(variant) IconProvider::GetShaderResource(variant)

class IconProvider
{
public:
	static void Initialize(Directus::Context* context);

	//= SHADER RESOURCE ========================================
	static void* GetShaderResource(IconProvider_Icon icon);
	static void* GetShaderResource(const std::string& filePath);
	//==========================================================

	//= ImGui::ImageButton =============================================================
	static bool ImageButton_enum_id(const char* id, IconProvider_Icon icon, float size);
	static bool ImageButton_filepath(const std::string& filepath, float size);
	//==================================================================================
};