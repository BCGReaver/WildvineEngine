#pragma once
#include "Prerequisites.h"
#include "Texture.h"
#include "RenderTargetView.h"
#include "DepthStencilView.h"

class Device;
class DeviceContext;

/**
 * @class EditorViewportPass
 * @brief Render pass dedicado al viewport del editor.
 *
 * @details
 *  Este compa básicamente funciona como un "framebuffer custom"
 *  donde renderizas la escena del editor (tipo Unreal/Unity viewport).
 *
 *  ¿Qué hace?
 *  - Crea un render target (color)
 *  - Crea un depth buffer
 *  - Permite renderizar ahí en lugar del backbuffer
 *  - Luego puedes mostrarlo en ImGui como textura
 */
class
	EditorViewportPass {
public:

	/**
	 * @brief Constructor default.
	 */
	EditorViewportPass() = default;

	/**
	 * @brief Destructor default.
	 */
	~EditorViewportPass() = default;

	/**
	 * @brief Inicializa el viewport pass.
	 *
	 * @param device Dispositivo de DirectX.
	 * @param width Ancho inicial.
	 * @param height Alto inicial.
	 * @return HRESULT indicando éxito o fallo.
	 */
	HRESULT init(Device& device, unsigned int width, unsigned int height);

	/**
	 * @brief Redimensiona los recursos del viewport.
	 *
	 * @details
	 *  Se llama cuando la ventana cambia de tamaño.
	 *  Básicamente recrea las texturas internas.
	 */
	HRESULT resize(Device& device, unsigned int width, unsigned int height);

	/**
	 * @brief Prepara el render pass.
	 *
	 * @details
	 *  Aquí haces:
	 *  - Bind del render target
	 *  - Clear del color buffer
	 *  - Clear del depth buffer
	 *
	 * @param deviceContext Contexto de DirectX.
	 * @param clearColor Color de limpieza (RGBA).
	 */
	void begin(DeviceContext& deviceContext, const float clearColor[4]);

	/**
	 * @brief Intercambia recursos con otro viewport pass.
	 *
	 * @details
	 *  Ahorita está vacío, pero normalmente se usa para:
	 *  - Double buffering
	 *  - Postprocesos
	 */
	void swap(EditorViewportPass& other) {

	}

	/**
	 * @brief Limpia el depth buffer.
	 *
	 * @details
	 *  Útil si quieres renderizar otra cosa sin arrastrar profundidad.
	 */
	void clearDepth(DeviceContext& deviceContext);

	/**
	 * @brief Configura el viewport en el pipeline.
	 *
	 * @details
	 *  Esto le dice a DirectX en qué área dibujar.
	 */
	void setViewport(DeviceContext& deviceContext);

	/**
	 * @brief Libera recursos.
	 *
	 * @details
	 *  Ahorita está vacío, pero debería liberar:
	 *  - Texturas
	 *  - RTV
	 *  - DSV
	 */
	void destroy() {

	}

	/**
	 * @brief Obtiene el Shader Resource View del color buffer.
	 *
	 * @details
	 *  Esto es CLAVE para ImGui:
	 *  puedes hacer:
	 *  ImGui::Image(...)
	 */
	ID3D11ShaderResourceView* getSRV() const { return m_colorSRV.m_textureFromImg; }

	/**
	 * @brief Devuelve el ancho actual.
	 */
	unsigned int getWidth() const { return m_width; }

	/**
	 * @brief Devuelve el alto actual.
	 */
	unsigned int getHeight() const { return m_height; }

	/**
	 * @brief Verifica si el viewport es válido.
	 *
	 * @details
	 *  Checa que todo esté correctamente creado:
	 *  - Color texture
	 *  - SRV
	 *  - Depth texture
	 */
	bool isValid() const
	{
		return m_colorTexture.m_texture != nullptr &&
			m_colorSRV.m_textureFromImg != nullptr &&
			m_depthTexture.m_texture != nullptr;
	}

private:

	/**
	 * @brief Crea los recursos internos.
	 *
	 * @details
	 *  Aquí se crean:
	 *  - Textura de color
	 *  - Shader Resource View
	 *  - Depth buffer
	 */
	HRESULT createResources(Device& device, unsigned int width, unsigned int height);

private:

	// --- COLOR BUFFER ---

	/** @brief Textura principal donde se renderiza el color */
	Texture m_colorTexture;

	/** @brief Vista para usar la textura en shaders (ImGui, postprocess, etc) */
	Texture m_colorSRV;

	/** @brief Render Target View (para bindear al pipeline) */
	RenderTargetView m_rtv;

	// --- DEPTH BUFFER ---

	/** @brief Textura de profundidad */
	Texture m_depthTexture;

	/** @brief Depth Stencil View */
	DepthStencilView m_dsv;

	// --- DIMENSIONES ---

	/** @brief Ancho actual del viewport */
	unsigned int m_width = 1;

	/** @brief Alto actual del viewport */
	unsigned int m_height = 1;
};