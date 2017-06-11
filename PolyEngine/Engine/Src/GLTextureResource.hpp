#pragma once

#include "ResourceBase.hpp"

typedef unsigned int GLuint;

namespace Poly 
{
	class ENGINE_DLLEXPORT GLTextureResource : public ResourceBase
	{
	public:
		GLTextureResource(const String& path);
		~GLTextureResource() override;

		unsigned char* GetImage() const { return Image; }
		int GetWidth() const { return Width; }
		int GetHeight() const { return Height; }
		int GetChannels() const { return Channels; }

		ITextureDeviceProxy* GetTextureProxy() { return TextureProxy.get(); }
		const ITextureDeviceProxy* GetTextureProxy() const { return TextureProxy.get(); }

	private:
		std::unique_ptr<ITextureDeviceProxy> TextureProxy;
		unsigned char* Image;
		int Width;
		int Height;
		int Channels;
	};
}

