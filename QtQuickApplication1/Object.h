#pragma once

class Object
{
public:
	Object* parent = nullptr;
	std::vector<std::shared_ptr<Object>> children;

	void addChild(std::shared_ptr<Object> obj)
	{
		children.push_back(obj);
		obj->parent = this;
	}
	
	std::string tag;
	
	virtual ~Object() = default;
};
