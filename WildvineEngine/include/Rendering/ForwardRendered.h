#pragma once

#include "Prerequisites.h"
#include "Buffer.h"
#include "DepthStencilState.h"
#include "DepthStencilView.h"
#include "RasterizerState.h"
#include "Rendering/RenderScene.h"
#include "Rendering/RenderTypes.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "EngineUtilities/Utilities/EditorViewportPass.h"

class Device;
class DeviceContext;
class Camera;
class Material;

/**
 * @brief Clase principal para el renderizado tradicional (Forward Rendering).
 * A diferencia del diferido, aquí dibujamos y calculamos la luz objeto por objeto.
 * Es más ligero si no tienes tantas luces en tu escena.
 */
class
  ForwardRenderer {
public:

  /**
   * @brief Arranca y prepara todos los recursos que va a usar el renderizador.
   * @param device Referencia a tu tarjeta gráfica para crear las cosas.
   * @return Un HRESULT que te dice si todo salió chido o si hubo algún error.
   */
  HRESULT
    init(Device& device);

  /**
   * @brief Ajusta los tamaños de las texturas cuando haces más grande o chica la ventana.
   * @param device Referencia al dispositivo gráfico.
   * @param width El nuevo ancho de la ventana.
   * @param height El nuevo alto de la ventana.
   */
  void
    resize(Device& device, unsigned int width, unsigned int height);

  /**
   * @brief Actualiza los datos que cambian en cada cuadro (frame), como la posición de la cámara.
   * @param camera La cámara desde donde estamos viendo.
   * @param scene La escena con todo lo que hay que dibujar.
   * @param deviceContext Contexto para mandar las órdenes a la gráfica.
   */
  void
    updatePerFrame(const Camera& camera, const RenderScene& scene, DeviceContext& deviceContext);

  /**
   * @brief La función estrella. Dibuja absolutamente toda la escena.
   * @param deviceContext Contexto del dispositivo para los comandos.
   * @param camera La cámara actual.
   * @param scene La escena con los objetos.
   * @param viewportPass Pase de render para dibujar la UI o cosas del editor.
   */
  void
    render(DeviceContext& deviceContext,
      const Camera& camera,
      RenderScene& scene,
      EditorViewportPass& viewportPass);

  /**
   * @brief Destruye todo y libera la memoria para que no tengas fugas de recursos.
   */
  void
    destroy();

  /**
   * @brief Te pasa la textura donde se guardaron las sombras.
   * @return Un puntero al SRV (Shader Resource View) de las sombras.
   */
  ID3D11ShaderResourceView*
    getShadowMapSRV() const { return m_shadowDepthSRV.m_textureFromImg; }

  /**
   * @brief Te da la vista de depuración de las sombras para el editor.
   * @return Un puntero al SRV del debug de sombras.
   */
  ID3D11ShaderResourceView*
    getPreShadowSRV() const { return m_preShadowDebugPass.getSRV(); }

private:

  /**
   * @brief Acomoda los objetos en dos filas: los sólidos (opacos) y los que tienen transparencia.
   * @param scene Tu escena actual.
   * @param camera La cámara que estás usando.
   */
  void
    buildQueues(RenderScene& scene, const Camera& camera);

  /**
   * @brief Dibuja un pase rápido para ver cómo andan las sombras antes de aplicarlas, útil para debug.
   * @param deviceContext Contexto del dispositivo.
   * @param scene Tu escena actual.
   */
  void
    renderPreShadowDebugPass(DeviceContext& deviceContext, RenderScene& scene);

  /**
   * @brief Se encarga de crear el mapa de sombras viendo la escena desde la luz.
   * @param deviceContext Contexto del dispositivo.
   */
  void
    renderShadowPass(DeviceContext& deviceContext);

  /**
   * @brief Dibuja todos los objetos macizos (que no se transparentan).
   * @param deviceContext Contexto del dispositivo.
   */
  void
    renderOpaquePass(DeviceContext& deviceContext);

  /**
   * @brief Dibuja los objetos como cristales o agua (que sí se transparentan).
   * @param deviceContext Contexto del dispositivo.
   */
  void
    renderTransparentPass(DeviceContext& deviceContext);

  /**
   * @brief Pinta el fondo o el cielo de tu escena.
   * @param deviceContext Contexto del dispositivo.
   * @param scene Escena actual para sacar la textura del cielo.
   */
  void
    renderSkyboxPass(DeviceContext& deviceContext, RenderScene& scene);

  /**
   * @brief Dibuja un objeto solito dependiendo del pase en el que andemos.
   * @param deviceContext Contexto del dispositivo.
   * @param object El objeto a dibujar.
   * @param passType El tipo de pase (opaco, transparente, etc.).
   */
  void
    renderObject(DeviceContext& deviceContext, const RenderObject& object, RenderPassType passType);

  /**
   * @brief Dibuja un objeto específico pero sólo para el mapa de sombras.
   * @param deviceContext Contexto del dispositivo.
   * @param object El objeto que va a proyectar sombra.
   */
  void
    renderShadowObject(DeviceContext& deviceContext, const RenderObject& object);

  /**
   * @brief Crea las texturas y configuraciones que necesitas para hacer sombras.
   * @param device Tu dispositivo gráfico.
   * @return HRESULT avisando si se creó bien o no.
   */
  HRESULT
    createShadowResources(Device& device);

  /**
   * @brief Calcula y actualiza las matemáticas (matrices) de la luz para proyectar las sombras.
   * @param camera Tu cámara principal.
   * @param scene Tu escena actual.
   */
  void
    updateLightMatrices(const Camera& camera, const RenderScene& scene);

  /**
   * @brief Crea las diferentes formas de mezclar colores (para transparencias y así).
   * @param device Tu dispositivo gráfico.
   * @return HRESULT avisando si todo salió bien.
   */
  HRESULT
    createBlendStates(Device& device);

  /**
   * @brief Checa qué tipo de mezcla de color necesita el material y te lo da.
   * @param material El material del objeto.
   * @return Puntero al estado de mezcla (Blend State) correcto.
   */
  ID3D11BlendState*
    resolveBlendState(const Material* material) const;

private:

  // ---------------------------------------------------------
  // VARIABLES DE CLASE (Miembros)
  // ---------------------------------------------------------

  Buffer m_perFrameBuffer;               ///< Guarda los datos que cambian por cada frame (ej. la cámara).
  Buffer m_perObjectBuffer;              ///< Guarda los datos individuales de cada objeto (ej. su posición).
  Buffer m_perMaterialBuffer;            ///< Guarda los datos de color o textura del material.

  DepthStencilState m_transparentDepthStencil; ///< Controla cómo se ocultan las cosas transparentes.

  ID3D11BlendState* m_alphaBlendState = nullptr;         ///< Mezcla normalita para cosas transparentes.
  ID3D11BlendState* m_opaqueBlendState = nullptr;        ///< Para dibujar en sólido sin mezclar nada.
  ID3D11BlendState* m_additiveBlendState = nullptr;      ///< Mezcla que suma luz (ideal para fuego o chispas).
  ID3D11BlendState* m_premultipliedBlendState = nullptr; ///< Mezcla para cuando la textura ya trae el alfa calculado.
  float m_blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };   ///< Un color base por si necesitas un valor constante al mezclar.

  Texture m_shadowDepthTexture;          ///< Textura donde dibujas la profundidad de la luz.
  Texture m_shadowDepthSRV;              ///< Vista para poder leer la textura de sombra en los shaders.
  DepthStencilView m_shadowDSV;          ///< Vista para escribir la profundidad de las sombras.
  ShaderProgram m_shadowShader;          ///< El programita que calcula la geometría para las sombras.
  RasterizerState m_shadowRasterizer;    ///< Configura cómo se rellenan los polígonos al hacer la sombra.
  unsigned int m_shadowMapSize = 2048;   ///< El tamaño cuadrado del mapa de sombras (2048x2048 píxeles).

  EditorViewportPass m_preShadowDebugPass; ///< Un pase extra nomás para que puedas ver las sombras en el editor.
  bool m_applyShadows = true;            ///< Interruptor global: true para encender sombras, false para apagarlas.

  CBPerFrame m_cbPerFrame{};             ///< Estructura con la info del frame actual (para mandar al buffer).
  CBPerObject m_cbPerObject{};           ///< Estructura con la info del objeto actual.
  CBPerMaterial m_cbPerMaterial{};       ///< Estructura con la info del material actual.

  std::vector<const RenderObject*> m_opaqueQueue;      ///< Tu lista de espera para dibujar cosas sólidas.
  std::vector<const RenderObject*> m_transparentQueue; ///< Tu lista de espera para dibujar cosas transparentes.
};