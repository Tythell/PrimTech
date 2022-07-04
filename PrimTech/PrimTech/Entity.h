#pragma once
#include"../Graphics/Model.h"
class Entity
{
public:
	Entity();
	void InitModel(const std::string path);
private:
	Model m_model;

};