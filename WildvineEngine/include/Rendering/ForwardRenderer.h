#pragma once
#include "Prerequisites.h"
#include "Buffer.h"
#include "DepthStencilState.h"
#include "Rendering/RenderScene.h"
#include "Rendering/RenderTypes.h"

class Device;
class DeviceContext;
class Camera;
class EditorViewportPass;
class Material;

/**
 * @class ForwardRenderer
 * @brief Renderer principal usando Forward Rendering.
 *
 * @details
 *  Este es el corazón del pipeline de render (modo forward).
 *
 *  Básicamente aquí:
 *   - Se organizan los objetos en colas (opaque / transparent)
 *   - Se actualizan los constant buffers
 *   - Se hacen los distintos passes de render
 *   - Se decide cómo se dibuja cada objeto
 *
 *  Piensa en esto como el "director de orquesta" del render.
 */
class
	ForwardRenderer {
public:

	/**
	 * @brief Inicializa el renderer.
	 *
	 * @details
	 *  Aquí normalmente crearías:
	 *   - Blend states
	 *   - Depth states
	 *   - Buffers globales
	 */
	HRESULT
		init(Device& device);


	/**
	 * @brief Se llama cuando cambia el tamaño de la ventana.
	 *
	 * @details
	 *  Ideal para recrear:
	 *   - Render targets
	 *   - Depth buffers
	 *
	 *  Ahorita está vacío, pero aquí debería ir eso.
	 */
	void
		resize(Device& device, unsigned int width, unsigned int height) {

	}


	/**
	 * @brief Actualiza datos que cambian cada frame.
	 *
	 * @details
	 *  Aquí se actualizan cosas como:
	 *   - Cámara
	 *   - Luces globales
	 *   - Tiempo
	 *
	 *  Esto se manda al GPU mediante constant buffers.
	 */
	void
		updatePerFrame(const Camera& camera,
			const RenderScene& scene,
			DeviceContext& deviceContext);


	/**
	 * @brief Función principal de render.
	 *
	 * @details
	 *  Flujo típico:
	 *   1. Construir colas (opaque / transparent)
	 *   2. Render opaque
	 *   3. Render skybox
	 *   4. Render transparent
	 */
	void
		render(DeviceContext& deviceContext,
			const Camera& camera,
			RenderScene& scene,
			EditorViewportPass& viewportPass);


	/**
	 * @brief Libera recursos del renderer.
	 */
	void
		destroy() {

	}

private:

	/**
	 * @brief Organiza los objetos en colas de render.
	 *
	 * @details
	 *  Divide los objetos en:
	 *   - Opaque (se renderizan primero)
	 *   - Transparent (después, ordenados)
	 *
	 *  Esto es clave para evitar problemas de blending.
	 */
	void
		buildQueues(RenderScene& scene, const Camera& camera);


	/**
	 * @brief Renderiza objetos opacos.
	 *
	 * @details
	 *  Estos se dibujan primero porque:
	 *   - No necesitan blending
	 *   - Es más eficiente (early-z)
	 */
	void
		renderOpaquePass(DeviceContext& deviceContext);


	/**
	 * @brief Renderiza objetos transparentes.
	 *
	 * @details
	 *  Se renderizan después porque:
	 *   - Necesitan blending
	 *   - Deben respetar orden de profundidad
	 */
	void
		renderTransparentPass(DeviceContext& deviceContext);


	/**
	 * @brief Renderiza el skybox.
	 *
	 * @details
	 *  Generalmente:
	 *   - Se dibuja después del opaque
	 *   - Antes del transparente
	 */
	void
		renderSkyboxPass(DeviceContext& deviceContext,
			RenderScene& scene);


	/**
	 * @brief Renderiza un objeto individual.
	 *
	 * @param object Objeto a renderizar.
	 * @param passType Tipo de pass (opaque/transparente/etc).
	 *
	 * @details
	 *  Aquí es donde realmente pasa la magia:
	 *   - Bind de buffers
	 *   - Bind de material
	 *   - Draw call
	 */
	void
		renderObject(DeviceContext& deviceContext,
			const RenderObject& object,
			RenderPassType passType);


	/**
	 * @brief Crea los distintos estados de blending.
	 *
	 * @details
	 *  Ejemplos:
	 *   - Alpha blending
	 *   - Additive
	 *   - Premultiplied
	 */
	HRESULT
		createBlendStates(Device& device);


	/**
	 * @brief Resuelve qué blend state usar según el material.
	 *
	 * @details
	 *  Ahorita está dummy (return nullptr),
	 *  pero aquí deberías decidir:
	 *   - Transparente → alpha blend
	 *   - Partículas → additive
	 *   - UI → premultiplied
	 */
	ID3D11BlendState*
		resolveBlendState(const Material* material) const {
		return nullptr;
	}

private:

	/** @brief Constant buffer que cambia cada frame (cámara, luz, etc). */
	Buffer m_perFrameBuffer;

	/** @brief Constant buffer por objeto (world matrix, etc). */
	Buffer m_perObjectBuffer;

	/** @brief Constant buffer por material (color, roughness, etc). */
	Buffer m_perMaterialBuffer;


	/** @brief Depth stencil especial para transparencias. */
	DepthStencilState m_transparentDepthStencil;


	/** @brief Estados de blending */
	ID3D11BlendState* m_alphaBlendState = nullptr;
	ID3D11BlendState* m_opaqueBlendState = nullptr;
	ID3D11BlendState* m_additiveBlendState = nullptr;
	ID3D11BlendState* m_premultipliedBlendState = nullptr;

	/** @brief Factor de mezcla (usado por DirectX en algunos casos). */
	float m_blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };


	/** @brief Datos que se mandan al GPU */
	CBPerFrame m_cbPerFrame{};
	CBPerObject m_cbPerObject{};
	CBPerMaterial m_cbPerMaterial{};


	/** @brief Cola de objetos opacos */
	std::vector<const RenderObject*> m_opaqueQueue;

	/** @brief Cola de objetos transparentes */
	std::vector<const RenderObject*> m_transparentQueue;
};