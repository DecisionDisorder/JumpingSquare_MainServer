#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <sstream>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "Vector3.h"

std::vector<std::string> split(std::string str, char Delimiter);
int GetCurrentTimeInMilliSeconds();
bool CheckBoundary1D(float playerPosition, float targetPosition, float targetBoundary);
bool CheckBoundary3D(Vector3 playerPosition, const Vector3 targetPosition, const Vector3 targetBoundary);
std::string GetJsonString(rapidjson::Document& doc, int& size);