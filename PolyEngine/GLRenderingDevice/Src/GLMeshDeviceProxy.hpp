#pragma once

#include <IRenderingDevice.hpp>
#include <GL\glew.h>
#include <EnumUtils.hpp>

namespace Poly
{
	class GLMeshDeviceProxy : public IMeshDeviceProxy
	{
	public:
		enum class eBufferType {
			VERTEX_BUFFER,
			TEXCOORD_BUFFER,
			NORMAL_BUFFER,
			INDEX_BUFFER,
			_COUNT
		};

		GLMeshDeviceProxy();
		virtual ~GLMeshDeviceProxy();

		void SetContent(const Mesh& mesh);

	private:
		void EnsureVBOCreated(eBufferType type);

		GLuint VAO = 0;
		EnumArray<GLuint, eBufferType> VBO;

		friend class GLRenderingDevice;
	};
}
