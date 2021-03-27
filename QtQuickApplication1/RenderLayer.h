#pragma once
class RenderLayer
{
protected:
	virtual void onRender() = 0;

public:
	bool active = true;
	void render()
	{
		if(!active)
			return;
		
		onRender();
	}
};