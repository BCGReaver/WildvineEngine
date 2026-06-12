#pragma once
#include "Prerequisites.h"
#include "Rendering/RenderTypes.h"

class ShaderProgram;
class RasterizerState;
class DepthStencilState;
class SamplerState;

/**
 * @brief Clase que guarda la configuración visual de un modelo.
 * Funciona como un paquete que junta el shader, las texturas y cómo se va a mezclar
 * o dibujar el objeto (si es opaco, transparente, etc.) para mandarlo a la gráfica.
 */
class
  Material {
public:

  /**
   * @brief Le asigna el programa de shaders a este material.
   * @param shader Puntero al shader que le va a dar color y forma.
   */
  void
    setShader(ShaderProgram* shader) { m_shader = shader; }

  /**
   * @brief Le dice al material cómo debe rellenar los polígonos (ej. sólido o en modo alambre).
   * @param state Puntero a la configuración del rasterizador.
   */
  void
    setRasterizerState(RasterizerState* state) { m_rasterizerState = state; }

  /**
   * @brief Configura cómo interactúa este material con la profundidad (qué se dibuja adelante o atrás).
   * @param state Puntero al estado de Depth/Stencil.
   */
  void
    setDepthStencilState(DepthStencilState* state) { m_depthStencilState = state; }

  /**
   * @brief Configura cómo se van a leer (muestrear) las texturas en este material.
   * @param state Puntero al estado del Sampler (ej. para que se vea pixelado o con un difuminado chido).
   */
  void
    setSamplerState(SamplerState* state) { m_samplerState = state; }

  /**
   * @brief Define la categoría general del material (opaco, transparente, etc.).
   * @param domain El tipo de dominio del material.
   */
  void
    setDomain(MaterialDomain domain) { m_domain = domain; }

  /**
   * @brief Configura cómo se van a mezclar los colores de este material con lo que hay detrás.
   * @param blendMode El modo de mezcla (ej. opaco, aditivo).
   */
  void
    setBlendMode(BlendMode blendMode) { m_blendMode = blendMode; }

  /**
   * @brief Te devuelve el shader que está usando el material actual.
   * @return Puntero al ShaderProgram.
   */
  ShaderProgram*
    getShader() const { return m_shader; }

  /**
   * @brief Te devuelve la configuración de cómo se rellenan los polígonos.
   * @return Puntero al RasterizerState.
   */
  RasterizerState*
    getRasterizerState() const { return m_rasterizerState; }

  /**
   * @brief Te devuelve cómo maneja la profundidad este material.
   * @return Puntero al DepthStencilState.
   */
  DepthStencilState*
    getDepthStencilState() const { return m_depthStencilState; }

  /**
   * @brief Te devuelve la configuración de lectura de texturas.
   * @return Puntero al SamplerState.
   */
  SamplerState*
    getSamplerState() const { return m_samplerState; }

  /**
   * @brief Te dice de qué tipo es el material (opaco, transparente, etc.).
   * @return El dominio actual del material.
   */
  MaterialDomain
    getDomain() const { return m_domain; }

  /**
   * @brief Te dice cómo se están mezclando los colores.
   * @return El modo de mezcla actual.
   */
  BlendMode
    getBlendMode() const { return m_blendMode; }

private:

  // ---------------------------------------------------------
  // VARIABLES DE CLASE (Miembros)
  // ---------------------------------------------------------

  ShaderProgram* m_shader = nullptr;                ///< El shader asignado para pintar este material.
  RasterizerState* m_rasterizerState = nullptr;     ///< Reglas de cómo pintar los triángulos (Rasterizer).
  DepthStencilState* m_depthStencilState = nullptr; ///< Reglas para ver si se oculta con otras cosas en frente.
  SamplerState* m_samplerState = nullptr;           ///< Reglas de cómo estirar o encoger las texturas.
  MaterialDomain m_domain = MaterialDomain::Opaque; ///< Tipo de material (por default opaco).
  BlendMode m_blendMode = BlendMode::Opaque;        ///< Tipo de mezcla (por default sin mezcla/opaco).
};