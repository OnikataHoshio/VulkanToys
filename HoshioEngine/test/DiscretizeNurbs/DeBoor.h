#ifndef _DE_BOOR_H_
#define _DE_BOOR_H_

#include "Engine/Panel/Editor/EditorPanel.h"

using namespace HoshioEngine;

class DeBoor :public EditorPanel {
private:
	void DrawConfigPannel();
	void DrawPaintingPannel();
	Eigen::Vector2f CalDeboor(float u);
	std::vector<Eigen::Vector2f> InterpolationBSplines();
	void UpdateNodeParams();

public:
	DeBoor() = default;
	~DeBoor() = default;


	void Render() override;

	std::vector<Eigen::Vector2f> control_pos;
	std::vector<float> node_params;
	std::vector<Eigen::Vector2f> u_pos;
	bool update = false;
	int degree = 1;
	float u_step = 0.01f;



};


#endif // !_DE_BOOR_H_
