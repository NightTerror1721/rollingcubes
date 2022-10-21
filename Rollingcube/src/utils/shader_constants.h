#pragma once

#include <string_view>

#define DEFINE_SHADER_UNIFORM_CONSTANT(_FieldName, _FieldValue) \
inline consteval std::string_view _FieldName() { return _FieldValue; }

#define SHADER_UNIFORM_TO_STRING(_Value) #_Value
#define SHADER_UNIFORM_CONSTANT_ARRAY_BEGIN(_Index, _MaxIndex) switch(_Index) {
#define SHADER_UNIFORM_CONSTANT_ARRAY_END() default: return "<invalid>"; }
#define SHADER_UNIFORM_CONSTANT_INDEX(_Index, _FiledValuePrefix, _FieldValuePostfix) \
case _Index: return SHADER_UNIFORM_TO_STRING(_FiledValuePrefix##[##_Index##]##_FieldValuePostfix);

#define DEFINE_SHADER_UNIFORM_CONSTANT_ARRAY8(_FieldName, _FiledValuePrefix, _FieldValuePostfix)	\
inline constexpr std::string_view _FieldName(const int index)								\
{																									\
	SHADER_UNIFORM_CONSTANT_ARRAY_BEGIN(index, 8)													\
		SHADER_UNIFORM_CONSTANT_INDEX(0, _FiledValuePrefix, _FieldValuePostfix)						\
		SHADER_UNIFORM_CONSTANT_INDEX(1, _FiledValuePrefix, _FieldValuePostfix)						\
		SHADER_UNIFORM_CONSTANT_INDEX(2, _FiledValuePrefix, _FieldValuePostfix)						\
		SHADER_UNIFORM_CONSTANT_INDEX(3, _FiledValuePrefix, _FieldValuePostfix)						\
		SHADER_UNIFORM_CONSTANT_INDEX(4, _FiledValuePrefix, _FieldValuePostfix)						\
		SHADER_UNIFORM_CONSTANT_INDEX(5, _FiledValuePrefix, _FieldValuePostfix)						\
		SHADER_UNIFORM_CONSTANT_INDEX(6, _FiledValuePrefix, _FieldValuePostfix)						\
		SHADER_UNIFORM_CONSTANT_INDEX(7, _FiledValuePrefix, _FieldValuePostfix)						\
	SHADER_UNIFORM_CONSTANT_ARRAY_END()																\
}																		




namespace constants
{
	namespace attributes
	{
		static constexpr GLuint vertices_array_attrib_index = 0;
		static constexpr GLuint uvs_array_attrib_index = 1;
		static constexpr GLuint normals_array_attrib_index = 2;
		static constexpr GLuint tangents_array_attrib_index = 3;
		static constexpr GLuint bitangents_array_attrib_index = 4;
		static constexpr GLuint colors_array_attrib_index = 5;
	}

	namespace uniform
	{
		namespace flags
		{
			DEFINE_SHADER_UNIFORM_CONSTANT(useNormalMapping, "useNormalMapping")
		}

		namespace material
		{
			DEFINE_SHADER_UNIFORM_CONSTANT(ambientColor, "material.color.ambient")
			DEFINE_SHADER_UNIFORM_CONSTANT(diffuseColor, "material.color.diffuse")
			DEFINE_SHADER_UNIFORM_CONSTANT(specularColor, "material.color.specular")
			DEFINE_SHADER_UNIFORM_CONSTANT(diffuseTexture, "material.diffuse")
			DEFINE_SHADER_UNIFORM_CONSTANT(specularTexture, "material.specular")
			DEFINE_SHADER_UNIFORM_CONSTANT(normalsTexture, "material.normals")
			DEFINE_SHADER_UNIFORM_CONSTANT(shininess, "material.shininess")
			DEFINE_SHADER_UNIFORM_CONSTANT(opacity, "material.opacity")
		}

		namespace static_lights
		{
			DEFINE_SHADER_UNIFORM_CONSTANT(count, "pointLightsCount")
			DEFINE_SHADER_UNIFORM_CONSTANT_ARRAY8(position, pointLights, .position)
			DEFINE_SHADER_UNIFORM_CONSTANT_ARRAY8(ambientColor, pointLights, .color.ambient)
			DEFINE_SHADER_UNIFORM_CONSTANT_ARRAY8(diffuseColor, pointLights, .color.diffuse)
			DEFINE_SHADER_UNIFORM_CONSTANT_ARRAY8(specularColor, pointLights, .color.specular)
			DEFINE_SHADER_UNIFORM_CONSTANT_ARRAY8(constant, pointLights, .constant)
			DEFINE_SHADER_UNIFORM_CONSTANT_ARRAY8(linear, pointLights, .linear)
			DEFINE_SHADER_UNIFORM_CONSTANT_ARRAY8(quadratic, pointLights, .quadratic)
			DEFINE_SHADER_UNIFORM_CONSTANT_ARRAY8(intensity, pointLights, .intensity)
		}

		namespace directional_light
		{
			DEFINE_SHADER_UNIFORM_CONSTANT(direction, "dirLight.direction")
			DEFINE_SHADER_UNIFORM_CONSTANT(ambientColor, "dirLight.color.ambient")
			DEFINE_SHADER_UNIFORM_CONSTANT(diffuseColor, "dirLight.color.diffuse")
			DEFINE_SHADER_UNIFORM_CONSTANT(specularColor, "dirLight.color.specular")
			DEFINE_SHADER_UNIFORM_CONSTANT(intensity, "dirLight.intensity")
		}

		namespace model_data
		{
			DEFINE_SHADER_UNIFORM_CONSTANT(model, "model")
			DEFINE_SHADER_UNIFORM_CONSTANT(modelNormal, "modelNormal")
		}

		namespace camera
		{
			DEFINE_SHADER_UNIFORM_CONSTANT(viewProjection, "viewProjection")
			DEFINE_SHADER_UNIFORM_CONSTANT(viewPos, "viewPos")
		}
	}


	namespace shader
	{
		struct ShaderName
		{
			std::size_t index;
			std::string_view name;
		};

		inline constexpr const ShaderName lightning = { 0, "lightning" };
		inline constexpr const ShaderName freetype_font = { 1, "freetype_font" };
		inline constexpr const ShaderName sky = { 2, "sky" };
		inline constexpr const ShaderName lines = { 3, "lines" };

		namespace internals
		{
			struct ShaderFiles
			{
				std::string_view id;
				std::string_view vertex;
				std::string_view fragment;
				std::string_view geometry = "";
			};

			inline constexpr const ShaderFiles shaders[]
			{
				{ lightning.name, "internal/lightning.vert", "internal/lightning.frag" },
				{ freetype_font.name, "internal/freetype_font.vert", "internal/freetype_font.frag" },
				{ sky.name, "internal/sky.vert", "internal/sky.frag" },
				{ lines.name, "internal/lines.vert", "internal/lines.frag" }
			};
			inline constexpr const std::size_t count = sizeof(internals::shaders) / sizeof(ShaderFiles);

			inline constexpr const ShaderFiles& shaderFiles(const ShaderName& name) { return shaders[name.index]; }
		}
	}
}

#undef DEFINE_SHADER_UNIFORM_CONSTANT
#undef SHADER_UNIFORM_TO_STRING
#undef SHADER_UNIFORM_CONSTANT_ARRAY_BEGIN
#undef SHADER_UNIFORM_CONSTANT_ARRAY_END
#undef SHADER_UNIFORM_CONSTANT_INDEX
#undef DEFINE_SHADER_UNIFORM_CONSTANT_ARRAY8
