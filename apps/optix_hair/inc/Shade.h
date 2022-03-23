#pragma once

#ifndef SHADE_H
#define SHADE_H

#include "shaders/function_indices.h"

#include <string>

 // Host side GUI material parameters 
struct Shade
{
	std::string		BxDfType;
	FunctionIndex	BxDfIndex;
	int				HairType;
	int				shadeHT;
	int				shadeColorL;
	int				shadeColorA;
	int				shadeColorB;
	int				shadeColorRed;
	int				shadeColorGreen;
	int				shadeColorBlue;
};

#endif // SHADE_H