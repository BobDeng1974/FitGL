#pragma once
namespace NeiVu {
  class Loader;
};
#include <string>
#include <Image.h>
#include <VuNode.h>


class NeiVu::Loader {
public:
	// Scene
	static NodeShared scene(std::string const & fileName);
	static bool generateOnImageNotFound;
};