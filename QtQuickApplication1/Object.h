#pragma once

class Object
{
public:
	//std::vector<std::shared_ptr<Object>> children;
	
	std::string tag;
	
	virtual ~Object() = default;
};
