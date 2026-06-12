#pragma once
#include "Prerequisites.h"

class Device;
class DeviceContext;
class Camera;
class RenderScene;
class EditorViewportPass;

/**
 * @brief Enum para elegir el estilo de renderizado que vamos a usar.
 * Básicamente te deja cambiar entre el método clásico (Forward) y el optimizado para muchas luces (Deferred).
 */
enum class
  RenderType {

  Forward = 0, ///< Renderizado clásico, dibuja y calcula luz por cada objeto de una.

  Deferred = 1 ///< Renderizado diferido, guarda todo en texturas primero y calcula la luz al final.
};

/**
 * @brief Interfaz base para cualquier renderizador de la escena.
 * Es como un molde: cualquier clase que herede de esta (como ForwardRenderer o DeferredRenderer)
 * tiene que programar sí o sí las funciones principales de acá.
 */
class
  ISceneRenderer {
public:

  /**
   * @brief Destructor virtual por defecto.
   * Súper necesario en interfaces para que cuando borres un renderer, se llame al destructor correcto.
   */
  virtual
    ~ISceneRenderer() = default;

  /**
   * @brief Inicializa los recursos del renderizador. Obligatorio de implementar.
   * @param device Tu dispositivo gráfico para crear cosas en memoria.
   * @return HRESULT avisando si la armó o hubo error.
   */
  virtual HRESULT
    init(Device& device) = 0;

  /**
   * @brief Se llama cuando cambias el tamaño de la ventana para ajustar las texturas. Obligatorio.
   * @param device Dispositivo gráfico.
   * @param width Nuevo ancho.
   * @param height Nuevo alto.
   */
  virtual void
    resize(Device& device, unsigned int width, unsigned int height) = 0;

  /**
   * @brief Dibuja toda la escena. Es la función que hace la magia cada frame. Obligatorio.
   * @param deviceContext El contexto para mandar las órdenes a la gráfica.
   * @param camera La cámara actual.
   * @param scene La escena que vas a pintar.
   * @param viewport El pase del editor por si necesitas dibujar UI o cosas extras.
   */
  virtual void
    render(DeviceContext& deviceContext,
      const Camera& camera,
      RenderScene& scene,
      EditorViewportPass& viewport) = 0;

  /**
   * @brief Destruye todo y libera la RAM/VRAM. Obligatorio.
   */
  virtual void
    destroy() = 0;

  /**
   * @brief Obtiene la textura de las sombras.
   * Tiene un return default en nullptr para que no truene si el renderer actual no usa sombras.
   * @return Puntero al recurso de las sombras (SRV).
   */
  virtual ID3D11ShaderResourceView*
    getShadowMapSRV() const { return nullptr; }

  /**
   * @brief Obtiene la vista de debug de las sombras.
   * @return Puntero al recurso del debug (SRV).
   */
  virtual ID3D11ShaderResourceView*
    getPreShadowSRV() const { return nullptr; }

  /**
   * @brief Obtiene la textura de Color/Metal del G-Buffer. (Específico del Deferred).
   * @return Puntero al recurso SRV.
   */
  virtual ID3D11ShaderResourceView*
    getGBufferAlbedoMetallicSRV() const { return nullptr; }

  /**
   * @brief Obtiene la textura de Normales/Rugosidad del G-Buffer. (Específico del Deferred).
   * @return Puntero al recurso SRV.
   */
  virtual ID3D11ShaderResourceView*
    getGBufferNormalRoughnessSRV() const { return nullptr; }

  /**
   * @brief Obtiene la textura de Posición y Ambient Occlusion del G-Buffer. (Específico del Deferred).
   * @return Puntero al recurso SRV.
   */
  virtual ID3D11ShaderResourceView*
    getGBufferWorldAoSRV() const { return nullptr; }

  /**
   * @brief Obtiene la textura de Emisividad/Alfa del G-Buffer. (Específico del Deferred).
   * @return Puntero al recurso SRV.
   */
  virtual ID3D11ShaderResourceView*
    getGBufferEmissiveAlphaSRV() const { return nullptr; }

  /**
   * @brief Prende o apaga el modo debug visual para el factor de sombras.
   * @param enabled True para prender, false para apagar. Viene con código vacío por si el renderer no lo ocupa.
   */
  virtual void
    setShaderFactorDebugEnabled(bool enabled) { (void)enabled; }

  /**
   * @brief Cambia el modo de vista de depuración para ver texturas específicas del G-Buffer.
   * @param mode El ID numérico de la vista que quieres ver.
   */
  virtual void
    setDeferredDebugViewMode(int mode) { (void)mode; }

  /**
   * @brief Pide el nombre del renderizador. Obligatorio de implementar.
   * @return El nombre en texto plano (como "ForwardRenderer" o "DeferredRenderer").
   */
  virtual const char*
    getDebugName() const = 0;
};