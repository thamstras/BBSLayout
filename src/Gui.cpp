#include "Common.hpp"

namespace ImGui
{
	// These 2 functions 'borrowed' from https://gist.github.com/dougbinks/ef0962ef6ebe2cadae76c4e9f0586c69
	inline void AddUnderLine(ImColor col_)
	{
		ImVec2 min = ImGui::GetItemRectMin();
		ImVec2 max = ImGui::GetItemRectMax();
		min.y = max.y;
		ImGui::GetWindowDrawList()->AddLine(min, max, col_, 1.0f);
	}

	inline void TextHyperlink(const char* label, const char* url)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
		ImGui::Text(label);
		ImGui::PopStyleColor();

		if (ImGui::IsItemHovered())
		{
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				// PROBABLY A HACK -- only works on WINDOWS!
				std::string command = std::string("start ");
				command.append(url);
				system(command.c_str());
			}
			AddUnderLine(ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
			ImGui::SetTooltip(/*ICON_FA_LINK*/ "  Open in browser\n%s", url);
		}
		else
		{
			AddUnderLine(ImGui::GetStyle().Colors[ImGuiCol_Button]);
		}
	}
}

void gui_MenuBar()
{
	bool bExit = false;
	bool bAbout = false;

	if (ImGui::BeginMainMenuBar())
	{

		if (ImGui::BeginMenu("File"))
		{

			ImGui::MenuItem("Open...");
			ImGui::MenuItem("Close");
			ImGui::Separator();
			ImGui::MenuItem("Exit", nullptr, &bExit);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem("About...", nullptr, &bAbout);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (bAbout) ImGui::OpenPopup("AboutModal");
	if (ImGui::BeginPopupModal("AboutModal"))
	{
		ImGui::Text("BBS Layout Viewer");
		ImGui::Text("Created by Thamstras");
		ImGui::Text("Special thanks: OpenKH Project");
		ImGui::Separator();
		ImGui::TextHyperlink("BBSLayout GitHub", "https://github.com/thamstras/KHBBSLayout");
		ImGui::SameLine();
		ImGui::TextHyperlink("OpenKH GitHub", "https://github.com/Xeeynamo/OpenKh");
		ImGui::Separator();
		if (ImGui::Button("Close"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

}

void gui_main()
{
	gui_MenuBar();

}