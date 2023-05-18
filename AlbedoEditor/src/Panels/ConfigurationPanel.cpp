#include "AlbedoPreCompiledHeader.h"

#include "ConfigurationPanel.h"

#include <imgui/imgui.h>

namespace Albedo {

	ConfigurationPanel::ConfigurationPanel()
	{
	}

	void ConfigurationPanel::OnImGuiRender()
	{
		ImGui::Begin("Configuration");

		static float padding = 20.0f;
		static float thumbnailSize = 35.0f;
		float cellSize = thumbnailSize + padding;

		//float panelWidth = ImGui::GetContentRegionAvail().x;
		//int columnCount = (int)(panelWidth / cellSize);
		//if (columnCount < 1)
		//	columnCount = 1;

		ImGui::End();
	}

}