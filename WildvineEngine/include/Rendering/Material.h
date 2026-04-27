#pragma once
#include "Prerequisites.h"
#include "Rendering/RenderTypes.h"

class ShaderProgram;
class RasterizerState;
class DepthStencilState;
class SamplerState;

/**
 * @class Material
 * @brief Representa cómo se ve un objeto al renderizarse.
 *
 * @details
 *  Este compa básicamente define TODO lo visual de un objeto:
 *
 *   - Qué shader usa
 *   - Cómo se rasteriza (wireframe, culling, etc)
 *   - Cómo se comporta la profundidad
 *   - Cómo se muestrean las texturas
 *   - Si es transparente o no
 *
 *  Piensa en esto como el "outfit gráfico" del objeto 😎
 *
 *  El mesh es la forma…
 *  El material es cómo se ve esa forma.
 */
class
	Material {
public:

	/**
	 * @brief Asigna el shader del material.
	 *
	 * @details
	 *  Este shader define:
	 *   - Cómo se procesan los vértices (VS)
	 *   - Cómo se pinta cada pixel (PS)
	 */
	void
		setShader(ShaderProgram* shader) { m_shader = shader; }


	/**
	 * @brief Define el estado de rasterización.
	 *
	 * @details
	 *  Controla cosas como:
	 *   - Culling (backface, frontface)
	 *   - Wireframe o sólido
	 */
	void
		setRasterizerState(RasterizerState* state) { m_rasterizerState = state; }


	/**
	 * @brief Define el comportamiento del depth buffer.
	 *
	 * @details
	 *  Aquí decides:
	 *   - Si escribe en depth
	 *   - Si hace test de profundidad
	 *
	 *  Clave para evitar glitches visuales.
	 */
	void
		setDepthStencilState(DepthStencilState* state) { m_depthStencilState = state; }


	/**
	 * @brief Define cómo se muestrean las texturas.
	 *
	 * @details
	 *  Ejemplos:
	 *   - Linear
	 *   - Point
	 *   - Anisotropic
	 */
	void
		setSamplerState(SamplerState* state) { m_samplerState = state; }


	/**
	 * @brief Define el dominio del material.
	 *
	 * @details
	 *  Ejemplo:
	 *   - Opaque
	 *   - Transparent
	 *   - UI
	 *   - Skybox
	 *
	 *  Esto ayuda al renderer a saber en qué pass dibujarlo.
	 */
	void
		setDomain(MaterialDomain domain) { m_domain = domain; }


	/**
	 * @brief Define el modo de blending.
	 *
	 * @details
	 *  Ejemplos:
	 *   - Opaque (sin mezcla)
	 *   - AlphaBlend (transparencia)
	 *   - Additive (efectos tipo fuego, magia)
	 */
	void
		setBlendMode(BlendMode blendMode) { m_blendMode = blendMode; }


	/**
	 * @brief Obtiene el shader del material.
	 */
	ShaderProgram*
		getShader() const { return m_shader; }


	/**
	 * @brief Obtiene el estado de rasterización.
	 */
	RasterizerState*
		getRasterizerState() const { return m_rasterizerState; }


	/**
	 * @brief Obtiene el estado de profundidad.
	 */
	DepthStencilState*
		getDepthStencilState() const { return m_depthStencilState; }


	/**
	 * @brief Obtiene el sampler.
	 */
	SamplerState*
		getSamplerState() const { return m_samplerState; }


	/**
	 * @brief Obtiene el dominio del material.
	 */
	MaterialDomain
		getDomain() const { return m_domain; }


	/**
	 * @brief Obtiene el modo de blending.
	 */
	BlendMode
		getBlendMode() const { return m_blendMode; }

private:

	/** @brief Shader principal del material */
	ShaderProgram* m_shader = nullptr;

	/** @brief Estado de rasterización */
	RasterizerState* m_rasterizerState = nullptr;

	/** @brief Estado de profundidad */
	DepthStencilState* m_depthStencilState = nullptr;

	/** @brief Estado de muestreo de textura */
	SamplerState* m_samplerState = nullptr;

	/** @brief Tipo de material (opaque, transparent, etc) */
	MaterialDomain m_domain = MaterialDomain::Opaque;

	/** @brief Tipo de blending */
	BlendMode m_blendMode = BlendMode::Opaque;
};