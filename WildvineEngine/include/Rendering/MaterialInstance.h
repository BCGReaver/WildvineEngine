#pragma once
#include "Prerequisites.h"
#include "Rendering/RenderTypes.h"

class Material;
class DeviceContext;
class Texture;

/**
 * @brief Clase para crear copias específicas (instancias) de un material base.
 * Nos sirve para que varios objetos usen el mismo shader (Material) pero cada uno
 * tenga texturas diferentes (ej. una pared de ladrillos y una de concreto).
 */
class
  MaterialInstance {
public:

  /**
   * @brief Vincula esta instancia a un material base (el que tiene los shaders).
   * @param material Puntero al material papá.
   */
  void
    setMaterial(Material* material) { m_material = material; }

  /**
   * @brief Le pone la textura de color base (Albedo) al objeto.
   * @param texture Puntero a la textura de color.
   */
  void
    setAlbedo(Texture* texture) { m_albedo = texture; }

  /**
   * @brief Le pone la textura de normales, que sirve para simular relieves e imperfecciones 3D sin gastar polígonos.
   * @param texture Puntero a la textura de normales.
   */
  void
    setNormal(Texture* texture) { m_normal = texture; }

  /**
   * @brief Le pone la textura que define qué partes del objeto son metálicas.
   * @param texture Puntero a la textura de metalicidad.
   */
  void
    setMetallic(Texture* texture) { m_metallic = texture; }

  /**
   * @brief Le pone la textura de rugosidad, para ver qué tan liso u opaco se ve al reflejar la luz.
   * @param texture Puntero a la textura de rugosidad.
   */
  void
    setRoughness(Texture* texture) { m_roughness = texture; }

  /**
   * @brief Le pone la textura de Oclusión Ambiental (AO) para sombrear las esquinas y zonas oscuras por defecto.
   * @param texture Puntero a la textura de AO.
   */
  void
    setAO(Texture* texture) { m_ao = texture; }

  /**
   * @brief Le pone la textura emisiva, por si el objeto tiene partes que brillan con luz propia (como luces neón).
   * @param texture Puntero a la textura emisiva.
   */
  void
    setEmissive(Texture* texture) { m_emissive = texture; }

  /**
   * @brief Te regresa el material base en el que se apoya esta instancia.
   * @return Puntero al Material original.
   */
  Material*
    getMaterial() const { return m_material; }

  /**
   * @brief Te da la textura de color de esta instancia.
   * @return Puntero a la textura Albedo.
   */
  Texture*
    getAlbedo() const { return m_albedo; }

  /**
   * @brief Te da la textura de mapa de normales.
   * @return Puntero a la textura de Normales.
   */
  Texture*
    getNormal() const { return m_normal; }

  /**
   * @brief Te da la textura que define el metal del objeto.
   * @return Puntero a la textura Metallic.
   */
  Texture*
    getMetallic() const { return m_metallic; }

  /**
   * @brief Te da la textura de qué tan pulido está el material.
   * @return Puntero a la textura Roughness.
   */
  Texture*
    getRoughness() const { return m_roughness; }

  /**
   * @brief Te da la textura de sombras estáticas (AO).
   * @return Puntero a la textura AO.
   */
  Texture*
    getAO() const { return m_ao; }

  /**
   * @brief Te da la textura de lo que brilla en la oscuridad.
   * @return Puntero a la textura Emissive.
   */
  Texture*
    getEmissive() const { return m_emissive; }

  /**
   * @brief Te da acceso a la estructura de parámetros numéricos del material para poder cambiarlos.
   * @return Referencia no-constante a MaterialParams.
   */
  MaterialParams&
    getParams() { return m_params; }

  /**
   * @brief Te deja leer los parámetros numéricos del material en modo de solo lectura.
   * @return Referencia constante a MaterialParams.
   */
  const MaterialParams&
    getParams() const { return m_params; }

  /**
   * @brief Activa todas las texturas de esta instancia en la tarjeta gráfica antes de empezar a dibujar.
   * @param deviceContext Contexto del dispositivo para enlazar los recursos.
   */
  void
    bindTextures(DeviceContext& deviceContext) const;

private:

  // ---------------------------------------------------------
  // VARIABLES DE CLASE (Miembros)
  // ---------------------------------------------------------

  Material* m_material = nullptr;  ///< El material base que tiene los shaders.

  Texture* m_albedo = nullptr;     ///< El mapa de color (Albedo).

  Texture* m_normal = nullptr;     ///< El mapa de relieves (Normal Map).

  Texture* m_metallic = nullptr;   ///< El mapa de qué tan metal es (Metallic).

  Texture* m_roughness = nullptr;  ///< El mapa de qué tan microsurco/rugoso es (Roughness).

  Texture* m_ao = nullptr;         ///< El mapa de sombras ambientales (Ambient Occlusion).

  Texture* m_emissive = nullptr;   ///< El mapa de brillo propio (Emissive).

  MaterialParams m_params;         ///< Estructura con datos extras (como multiplicadores de color o brillo).
};