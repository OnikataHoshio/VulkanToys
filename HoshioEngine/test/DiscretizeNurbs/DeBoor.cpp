#include "DeBoor.h"

ImVec2 g_canvas_pos_lu = { 0.0f, 0.0f };
ImVec2 g_canvas_pos_rb = { 0.0f, 0.0f };
Eigen::Vector2f g_last_mouse_pos = {0.0f, 0.0f};
bool is_dragging = false;

void PlotCanvasGrid(float spacing, ImDrawList* draw_list, ImU32 line_col) {
	for (float temp = spacing + g_canvas_pos_lu.x; temp < g_canvas_pos_rb.x; temp += spacing) {
		draw_list->AddLine({ temp, g_canvas_pos_lu.y }, { temp, g_canvas_pos_rb.y }, line_col);
	}

	for (float temp = g_canvas_pos_rb.y - spacing; temp > g_canvas_pos_lu.y; temp -= spacing) {
		draw_list->AddLine({ g_canvas_pos_lu.x, temp }, { g_canvas_pos_rb.x, temp }, line_col);
	}
}

void PlotLineSegments(const std::vector<Eigen::Vector2f>& poss, ImDrawList* draw_list, ImU32 line_col, ImU32 point_col, bool plot_dot = true) {
	//draw line segments
	for (size_t i = 1; i < poss.size(); i++) {
		draw_list->AddLine({ poss[i - 1].x() + g_canvas_pos_lu.x, g_canvas_pos_rb.y - poss[i - 1].y() },
			{ poss[i].x() + g_canvas_pos_lu.x, g_canvas_pos_rb.y - poss[i].y() }, line_col, 2.0f);
	}
	//draw point
	if (plot_dot) {
		for (auto& pos : poss) {
			draw_list->AddCircleFilled({ pos.x() + g_canvas_pos_lu.x, g_canvas_pos_rb.y - pos.y() }, 5.0f, point_col);
			draw_list->AddCircle({ pos.x() + g_canvas_pos_lu.x, g_canvas_pos_rb.y - pos.y() }, 5.0f, point_col);
		}
	}
}

void DeBoor::DrawConfigPannel()
{
	if (ImGui::Begin("Config")) {
		if (ImGui::TreeNode("Global-Configuration")) {
			if (ImGui::DragInt("Degree", &degree, 1.0f, 1, 5)) {
				update = true;
			}
			if (ImGui::SliderFloat("Step", &u_step, 0.01f, 0.5f)) {
				update = true;
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Control-Points-List")) {
			for (int i = 0; i < control_pos.size(); i++) {
				ImGui::Text("P%d : (x: %.2f, y: %.2f)", i, control_pos[i].x(), control_pos[i].y());
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Node-List")) {
			for (int i = 0; i < node_params.size(); i++)
			{
				ImGui::Text("u%d : %.2f", i, node_params[i]);
			}
			ImGui::TreePop();
		}

		ImGui::End();
	}
}

void DeBoor::DrawPaintingPannel()
{
	if (ImGui::Begin("Canvas"))
	{
		g_canvas_pos_lu = ImGui::GetCursorScreenPos();
		ImVec2 canvas_size = ImGui::GetContentRegionAvail();
		if (canvas_size.x < 50.f)
			canvas_size.x = 50.0f;
		if (canvas_size.y < 50.0f)
			canvas_size.x = 50.0f;
		g_canvas_pos_rb = ImVec2(g_canvas_pos_lu.x + canvas_size.x, g_canvas_pos_lu.y + canvas_size.y);

		//get the imgui io to fetch the input 
		ImGuiIO& io = ImGui::GetIO();
		//get the imgui window drawlist to draw item in the canvas
		ImDrawList* drawlist = ImGui::GetWindowDrawList();
		//paint the canvas to make it visible
		drawlist->AddRectFilled(g_canvas_pos_lu, g_canvas_pos_rb, IM_COL32(50, 50, 50, 255));
		PlotCanvasGrid(80.0f, drawlist, IM_COL32(120, 120, 120, 255));
		drawlist->AddRect(g_canvas_pos_lu, g_canvas_pos_rb, IM_COL32(80, 80, 80, 255), 0.0f, 0, 10.0f);

		//create a invisible button and add hover test for it
		ImGui::InvisibleButton("canvas", canvas_size);
		const bool is_hovered = ImGui::IsItemHovered();

		//if mouse hover on the canvas and left mouse button Clicked
		if (is_hovered) {
			Eigen::Vector2f mouse_pos = { io.MousePos.x - g_canvas_pos_lu.x, g_canvas_pos_rb.y - io.MousePos.y };
			float min_dist = std::numeric_limits<float>::max();
			float min_index = 0;
			for (size_t i = 0; i < control_pos.size(); i++) {
				float dist = (mouse_pos - control_pos[i]).squaredNorm();
				if (dist < min_dist) {
					if (dist < min_dist) {
						min_dist = dist;
						min_index = i;
					}
				}
			}
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
				g_last_mouse_pos = { io.MousePos.x - g_canvas_pos_lu.x, g_canvas_pos_rb.y - io.MousePos.y };
				update = true;
			}
			else if (is_hovered && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				if (min_dist <= 100.0f || is_dragging)
				{
					is_dragging = true;
					Eigen::Vector2f current_mouse_pos = { io.MousePos.x - g_canvas_pos_lu.x, g_canvas_pos_rb.y - io.MousePos.y };
					Eigen::Vector2f mouse_offset = current_mouse_pos - g_last_mouse_pos;
					control_pos[min_index] = control_pos[min_index] + mouse_offset;
					g_last_mouse_pos = current_mouse_pos;
					update = true;
				}
			}
			else if (is_hovered && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
				if (!is_dragging)
				{
					control_pos.emplace_back(io.MousePos.x - g_canvas_pos_lu.x, g_canvas_pos_rb.y - io.MousePos.y);
					update = true;
				}
				is_dragging = false;
			}
			else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
				if (min_dist <= 100.0f)
				{
					control_pos.erase(control_pos.begin() + min_index, control_pos.begin() + min_index + 1);
					update = true;
				}
			}


		}


		if (update) {
			UpdateNodeParams();
			u_pos = InterpolationBSplines();
			update = false;
		}

		PlotLineSegments(control_pos, drawlist, IM_COL32(255, 50, 50, 255), IM_COL32(255, 80, 80, 255));
		PlotLineSegments(u_pos, drawlist, IM_COL32(127, 255, 80, 255), IM_COL32(127, 255, 80, 255), false);

		ImGui::End();
	}
}

Eigen::Vector2f DeBoor::CalDeboor(float u) {
	int p = degree;
	int n = control_pos.size() - 1;

	// 找到区间 k，使得 u 属于 [u_k, u_{k+1})
	int k = p;
	for (int i = p; i <= n; i++) {
		if (u >= node_params[i] && u < node_params[i + 1]) {
			k = i;
			break;
		}
	}
	// 特殊处理最后一个点
	if (u >= node_params[n + 1]) k = n;

	// 使用一维数组替代二维 vector 减少内存分配
	std::vector<Eigen::Vector2f> d(p + 1);
	for (int i = 0; i <= p; i++) {
		d[i] = control_pos[k - p + i];
	}

	// 迭代计算
	for (int r = 1; r <= p; r++) {
		for (int i = p; i >= r; i--) {
			float alpha_num = u - node_params[i + k - p];
			float alpha_den = node_params[i + k - r + 1] - node_params[i + k - p];

			float alpha = (alpha_den == 0) ? 0.0f : alpha_num / alpha_den;
			d[i] = (1.0f - alpha) * d[i - 1] + alpha * d[i];
		}
	}

	return d[p];
}

std::vector<Eigen::Vector2f> DeBoor::InterpolationBSplines() {
	if (control_pos.size() < degree + 1) return {};

	std::vector<Eigen::Vector2f> u_poss;
	// 对于 Clamped 向量，范围就是 0 到 1
	for (float u = 0.0f; u <= 1.0f; u += u_step) {
		u_poss.push_back(CalDeboor(u));
	}
	// 确保包含最后一个点
	u_poss.push_back(CalDeboor(1.0f));

	return u_poss;
}

void DeBoor::UpdateNodeParams() {
	int n = control_pos.size() - 1;
	int p = degree;
	int m = n + p + 1;
	node_params.clear();

	for (int i = 0; i <= m; i++) {
		if (i <= p) node_params.push_back(0.0f);
		else if (i >= m - p) node_params.push_back(1.0f);
		else node_params.push_back((float)(i - p) / (n - p + 1));
	}
}

void DeBoor::Render()
{
	DrawConfigPannel();
	DrawPaintingPannel();
}
