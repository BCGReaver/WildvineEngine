#pragma once
#include "Buffer.h"
#include "DepthStencilState.h"
#include "DepthStencilView.h"
#include "RasterizerState.h"
#include "Rendering/ISceneRenderer.h"
#include "Rendering/RenderScene.h"
#include "Rendering/RenderTypes.h"
#include "SamplerState.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "EngineUtilities/Utilities/EditorViewportPass.h"

class Device;
class DeviceContext;
class Camera;
class Material;

/**
 * @brief Clase principal para manejar el renderizado diferido.
 * * Esta clase hereda de ISceneRenderer y se encarga de separar el pase de geometría
 * del pase de iluminación para optimizar el rendimiento al tener muchas luces.
 */
class
  DeferredRenderer : public ISceneRenderer {
public:

  /**
   * @brief Inicializa el renderizador y crea todos los recursos necesarios.
   * @param device Referencia al dispositivo grafico para crear recursos.
   * @return HRESULT con el resultado de la operación (éxito o error).
   */
  HRESULT
    init(Device& device) override;

  /**
   * @brief Ajusta los buffers internos cuando cambia el tamaño de la ventana.
   * @param device Referencia al dispositivo grafico.
   * @param width Nuevo ancho de la ventana.
   * @param height Nuevo alto de la ventana.
   */
  void
    resize(Device& device, unsigned int width, unsigned int height) override;

  /**
   * @brief Dibuja toda la escena actual.
   * @param deviceContext Contexto del dispositivo para mandar los comandos de dibujo.
   * @param camera Cámara desde la cual se está viendo la escena.
   * @param scene Objeto de la escena que contiene toda la info a renderizar.
   * @param viewportPass Pase del viewport del editor para debuggear o dibujar UI.
   */
  void
    render(DeviceContext& deviceContext,
      const Camera& camera,
      RenderScene& scene,
      EditorViewportPass& viewportPass) override;

  /**
   * @brief Libera la memoria y destruye los recursos creados.
   */
  void
    destroy() override;

  /**
   * @brief Obtiene la textura del mapa de sombras.
   * @return Puntero al recurso (SRV) del mapa de sombras.
   */
  ID3D11ShaderResourceView*
    getShadowMapSRV() const override { return m_shadowDepthSRV.m_textureFromImg; }

  /**
   * @brief Obtiene la vista previa del debug de sombras.
   * @return Puntero al recurso (SRV) del debug de sombras.
   */
  ID3D11ShaderResourceView*
    getPreShadowSRV() const override { return m_preShadowDebugPass.getSRV(); }

  /**
   * @brief Obtiene la textura del G-Buffer para Color (Albedo) y Metálico.
   * @return Puntero al SRV de Albedo/Metallic.
   */
  ID3D11ShaderResourceView*
    getGBufferAlbedoMetallicSRV() const override { return m_gBufferAlbedoMetallicSRV.m_textureFromImg; }

  /**
   * @brief Obtiene la textura del G-Buffer para Normales y Rugosidad.
   * @return Puntero al SRV de Normal/Roughness.
   */
  ID3D11ShaderResourceView*
    getGBufferNormalRoughnessSRV() const override { return m_gBufferNormalRoughnessSRV.m_textureFromImg; }

  /**
   * @brief Obtiene la textura del G-Buffer para Posición en el mundo y Oclusión Ambiental (AO).
   * @return Puntero al SRV de World/AO.
   */
  ID3D11ShaderResourceView*
    getGBufferWorldAoSRV() const override { return m_gBufferWorldAoSRV.m_textureFromImg; }

  /**
   * @brief Obtiene la textura del G-Buffer para Emisividad y Alfa.
   * @return Puntero al SRV de Emissive/Alpha.
   */
  ID3D11ShaderResourceView*
    getGBufferEmissiveAlphaSRV() const override { return m_gBufferEmissiveAlphaSRV.m_textureFromImg; }

  /**
   * @brief Activa o desactiva la visualización del factor de sombras en debug.
   * @param enabled Booleano para prender (true) o apagar (false) el debug.
   */
  void
    setShaderFactorDebugEnabled(bool enabled) override { m_shadowFactorDebugEnabled = enabled; }

  /**
   * @brief Cambia el modo de vista de debug del render diferido.
   * @param mode Entero que representa el modo de debug (ej. ver normales, ver albedo, etc).
   */
  void
    setDeferredDebugViewMode(int mode) override { m_deferredDebugViewMode = mode; }

  /**
   * @brief Devuelve el nombre del renderizador para temas de debuggear.
   * @return Cadena de texto con el nombre "DeferredRenderer".
   */
  const char*
    getDebugName() const override { return "DeferredRenderer"; }

private:

  /**
   * @brief Separa los objetos en colas (opacos y transparentes) para renderizarlos en orden.
   * @param scene Escena con los objetos.
   * @param camera Cámara actual.
   */
  void
    buildQueues(RenderScene& scene, const Camera& camera);

  /**
   * @brief Actualiza los buffers que cambian cada frame.
   * @param camera Cámara actual.
   * @param scene Escena actual.
   * @param deviceContext Contexto del dispositivo.
   */
  void
    updatePerFrame(const Camera& camera, const RenderScene& scene, DeviceContext& deviceContext);

  /**
   * @brief Actualiza las matrices para el cálculo de las luces.
   * @param camera Cámara actual.
   * @param scene Escena actual.
   */
  void
    updateLightMatrices(const Camera& camera, const RenderScene& scene);

  /**
   * @brief Coordina todos los pases de renderizado hacia un target específico.
   * @param deviceContext Contexto del dispositivo.
   * @param scene Escena actual.
   * @param targetPass Pase de render destino.
   * @param applyShadows Si es true, aplica el pase de sombras.
   */
  void
    renderSceneToTarget(DeviceContext& deviceContext, RenderScene& scene, EditorViewportPass& targetPass, bool applyShadows);

  /**
   * @brief Enlaza los targets del G-Buffer a la tubería gráfica.
   * @param deviceContext Contexto del dispositivo.
   * @param depthStencilView Vista del buffer de profundidad.
   */
  void
    bindGBufferTargets(DeviceContext& deviceContext, ID3D11DepthStencilView* depthStencilView);

  /**
   * @brief Enlaza el render target final donde se va a combinar toda la luz.
   * @param deviceContext Contexto del dispositivo.
   * @param renderTargetView Vista del target final.
   * @param depthStencilView Vista del buffer de profundidad.
   */
  void
    bindFinalTarget(DeviceContext& deviceContext, ID3D11RenderTargetView* renderTargetView, ID3D11DepthStencilView* depthStencilView);

  /**
   * @brief Limpia las vistas de recursos del G-Buffer.
   * @param deviceContext Contexto del dispositivo.
   */
  void
    clearDeferredSRVs(DeviceContext& deviceContext);

  /**
   * @brief Ejecuta el pase de geometría llenando el G-Buffer con info pura de los modelos.
   * @param deviceContext Contexto del dispositivo.
   */
  void
    renderGeometryPass(DeviceContext& deviceContext);

  /**
   * @brief Renderiza un objeto individual en el pase de geometría.
   * @param deviceContext Contexto del dispositivo.
   * @param object Objeto a dibujar.
   */
  void
    renderGeometryObject(DeviceContext& deviceContext, const RenderObject& object);

  /**
   * @brief Ejecuta el pase de iluminación utilizando la info guardada en el G-Buffer.
   * @param deviceContext Contexto del dispositivo.
   */
  void
    renderLightingPass(DeviceContext& deviceContext);

  /**
   * @brief Dibuja el fondo o Skybox de la escena.
   * @param deviceContext Contexto del dispositivo.
   * @param scene Escena actual.
   */
  void
    renderSkyboxPass(DeviceContext& deviceContext, RenderScene& scene);

  /**
   * @brief Dibuja los objetos que tienen transparencia (se hace después de los opacos).
   * @param deviceContext Contexto del dispositivo.
   */
  void
    renderTransparentPass(DeviceContext& deviceContext);

  /**
   * @brief Renderiza un objeto de forma directa (Forward Rendering), usado para transparentes.
   * @param deviceContext Contexto del dispositivo.
   * @param object Objeto a dibujar.
   * @param passType Tipo de pase que se está ejecutando.
   */
  void
    renderForwardObject(DeviceContext& deviceContext, const RenderObject& object, RenderPassType passType);

  /**
   * @brief Ejecuta el pase para crear los mapas de sombras.
   * @param deviceContext Contexto del dispositivo.
   */
  void
    renderShadowPass(DeviceContext& deviceContext);

  /**
   * @brief Dibuja un objeto específico en el mapa de sombras.
   * @param deviceContext Contexto del dispositivo.
   * @param object Objeto a dibujar.
   */
  void
    renderShadowObject(DeviceContext& deviceContext, const RenderObject& object);

  /**
   * @brief Crea las texturas y recursos para las sombras.
   * @param device Dispositivo gráfico.
   * @return HRESULT estado de la creación.
   */
  HRESULT
    createShadowResources(Device& device);

  /**
   * @brief Crea las texturas base del G-Buffer.
   * @param device Dispositivo gráfico.
   * @param width Ancho base.
   * @param height Alto base.
   * @return HRESULT estado de la creación.
   */
  HRESULT
    createGBufferResources(Device& device, unsigned int width, unsigned int height);

  /**
   * @brief Función de apoyo para crear un target individual del G-Buffer.
   * @param device Dispositivo gráfico.
   * @param width Ancho.
   * @param height Alto.
   * @param format Formato DXGI de la textura.
   * @param texture Textura a crear.
   * @param srv Vista como recurso de shader.
   * @param rtv Vista como render target.
   * @return HRESULT estado de la creación.
   */
  HRESULT
    createGBufferTarget(Device& device,
      unsigned int width,
      unsigned int height,
      DXGI_FORMAT format,
      Texture& texture,
      Texture& srv,
      RenderTargetView& rtv);

  /**
   * @brief Crea los recursos necesarios para el cálculo de iluminación.
   * @param device Dispositivo gráfico.
   * @return HRESULT estado de la creación.
   */
  HRESULT
    createLightingResources(Device& device);

  /**
   * @brief Crea el quad (rectángulo a pantalla completa) para aplicar la luz en 2D.
   * @param device Dispositivo gráfico.
   * @return HRESULT estado de la creación.
   */
  HRESULT
    createFullScreenQuad(Device& device);

  /**
   * @brief Crea los diferentes estados de mezcla (Blend States) para opacos y transparentes.
   * @param device Dispositivo gráfico.
   * @return HRESULT estado de la creación.
   */
  HRESULT
    createBlendStates(Device& device);

  /**
   * @brief Determina qué estado de mezcla necesita un material en específico.
   * @param material Puntero al material a evaluar.
   * @return Puntero al estado de mezcla configurado.
   */
  ID3D11BlendState*
    resolveBlendState(const Material* material) const;


  // ---------------------------------------------------------
  // VARIABLES DE CLASE (Miembros)
  // ---------------------------------------------------------

  Buffer m_perFrameBuffer;               ///< Buffer de constantes actualizado por frame.
  Buffer m_perObjectBuffer;              ///< Buffer de constantes actualizado por objeto.
  Buffer m_perMaterialBuffer;            ///< Buffer de constantes actualizado por material.
  Buffer m_lightingDebugBuffer;          ///< Buffer para información de depuración de luces.
  Buffer m_fullscreenVertexBuffer;       ///< Vertices para el rectángulo a pantalla completa.
  Buffer m_fullscreenIndexBuffer;        ///< Índices para el rectángulo a pantalla completa.

  DepthStencilState m_transparentDepthStencil; ///< Estado de profundidad para cosas transparentes.
  DepthStencilState m_disabledDepthStencil;    ///< Estado con prueba de profundidad apagada.
  DepthStencilState m_shadowDepthStencil;      ///< Estado de profundidad para la creación de sombras.

  ID3D11BlendState* m_alphaBlendState = nullptr;         ///< Mezcla para alfa tradicional.
  ID3D11BlendState* m_opaqueBlendState = nullptr;        ///< Estado sin mezcla (sólido).
  ID3D11BlendState* m_additiveBlendState = nullptr;      ///< Mezcla aditiva (para luces/fuego).
  ID3D11BlendState* m_premultipliedBlendState = nullptr; ///< Mezcla para alfa pre-multiplicado.
  float m_blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };   ///< Factor constante de mezcla.

  Texture m_shadowDepthTexture;          ///< Textura donde se guarda la profundidad de la luz.
  Texture m_shadowDepthSRV;              ///< Vista de recurso de las sombras.
  DepthStencilView m_shadowDSV;          ///< Vista del depth stencil de las sombras.
  ShaderProgram m_shadowShader;          ///< Shader utilizado para generar sombras.
  RasterizerState m_shadowRasterizer;    ///< Estado del rasterizador para las sombras.
  unsigned int m_shadowMapSize = 2048;   ///< Resolución cuadrada del mapa de sombras.

  ShaderProgram m_gBufferShader;         ///< Shader para rellenar el G-Buffer.
  ShaderProgram m_deferredLightingShader;///< Shader para aplicar la luz diferida.
  SamplerState m_lightingSampler;        ///< Muestreador para leer las texturas en el pase de luz.
  RasterizerState m_fullscreenRasterizer;///< Rasterizador para dibujar a pantalla completa.

  // Targets del G-Buffer
  Texture m_gBufferAlbedoMetallicTexture;
  Texture m_gBufferAlbedoMetallicSRV;
  RenderTargetView m_gBufferAlbedoMetallicRTV;

  Texture m_gBufferNormalRoughnessTexture;
  Texture m_gBufferNormalRoughnessSRV;
  RenderTargetView m_gBufferNormalRoughnessRTV;

  Texture m_gBufferWorldAoTexture;
  Texture m_gBufferWorldAoSRV;
  RenderTargetView m_gBufferWorldAoRTV;

  Texture m_gBufferEmissiveAlphaTexture;
  Texture m_gBufferEmissiveAlphaSRV;
  RenderTargetView m_gBufferEmissiveAlphaRTV;

  EditorViewportPass m_preShadowDebugPass; ///< Pase temporal para debug de sombras en el editor.

  bool m_applyShadows = true;            ///< Bandera global para habilitar/deshabilitar sombras.
  unsigned int m_renderWidth = 1280;     ///< Ancho base de renderizado.
  unsigned int m_renderHeight = 720;     ///< Alto base de renderizado.

  CBPerFrame m_cbPerFrame{};             ///< Estructura de constantes por frame.
  CBPerObject m_cbPerObject{};           ///< Estructura de constantes por objeto.
  CBPerMaterial m_cbPerMaterial{};       ///< Estructura de constantes por material.

  /**
   * @brief Estructura para agrupar variables de depuración de luces.
   */
  struct DeferredLightingDebugData {
    int DebugViewMode = 0;               ///< Modo de vista activo.
    float ShadowStrength = 1.0f;         ///< Intensidad visual de la sombra.
    float pad0 = 0.0f;                   ///< Relleno para alineación de memoria (padding).
    float pad1 = 0.0f;                   ///< Relleno para alineación de memoria (padding).
  } m_lightingDebugData{};

  bool m_shadowFactorDebugEnabled = false; ///< Activa ver solo los valores de sombra.
  int m_deferredDebugViewMode = 0;         ///< Almacena el ID del modo de vista de depuración.

  std::vector<const RenderObject*> m_opaqueQueue;      ///< Cola de objetos opacos a renderizar.
  std::vector<const RenderObject*> m_transparentQueue; ///< Cola de objetos transparentes a renderizar.
};