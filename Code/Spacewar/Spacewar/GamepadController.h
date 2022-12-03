#pragma once

#include "IController.h"

class CGamepadController : public IController
{
public:

	CGamepadController(int idx) : m_idx(idx) {}

	virtual EControllerType GetType() const override { return Gamepad; }
	virtual void Update() override;

private:

	int m_idx = 0;
};