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
	void removeChild(std::shared_ptr<Object> obj)
	{
		children.erase(std::remove(children.begin(), children.end(), obj), children.end());
		obj->parent = nullptr;
	}
	std::string tag;
	
	virtual ~Object() = default;
};
