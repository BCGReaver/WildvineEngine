#pragma once
#include "Prerequisites.h"
#include "Buffer.h"

/**
 * @brief Estructura que representa una parte de un modelo 3D (Submesh).
 * Como un modelo 3D puede tener diferentes materiales (ej. llantas de goma y ventanas de cristal),
 * se divide en "Submeshes". Cada Submesh tiene su propia geometría y su ID de material.
 */
struct
  Submesh {

  Buffer vertexBuffer;           ///< El buffer donde se guardan los vértices (puntos 3D) de esta parte.
  Buffer indexBuffer;            ///< El buffer que dice cómo se conectan esos puntos para armar los triángulos.
  unsigned int indexCount = 0;   ///< La cantidad total de índices que tiene esta parte del modelo.
  unsigned int startIndex = 0;   ///< Desde dónde empezamos a leer los índices (por si están todos en un buffer gigante).
  unsigned int materialSlot = 0; ///< El ID del material que se le va a pegar a esta parte específica.
};

/**
 * @brief Clase que representa un modelo 3D completo (Mesh).
 * Actúa como un contenedor que agrupa todas las partes (Submeshes) de tu objeto
 * para que sea más fácil cargarlo y dibujarlo en el motor.
 */
class
  Mesh {
public:

  /**
   * @brief Te pasa la lista de submeshes para que puedas agregarle o modificar partes.
   * @return Referencia a la lista (vector) de Submeshes.
   */
  std::vector<Submesh>&
    getSubmeshes() { return m_submeshes; }

  /**
   * @brief Te pasa la lista de submeshes pero en modo solo lectura para que no la riegues cambiándole algo.
   * @return Referencia constante a la lista de Submeshes.
   */
  const std::vector<Submesh>&
    getSubmeshes() const { return m_submeshes; }

  /**
   * @brief Libera la memoria destruyendo los buffers de todos los submeshes.
   * Pasa un ciclo destruyendo la geometría para que no dejes fugas de memoria en la tarjeta gráfica.
   */
  void
    destroy() {
    for (Submesh& submesh : m_submeshes) {
      submesh.vertexBuffer.destroy();
      submesh.indexBuffer.destroy();
    }
    m_submeshes.clear();
  }

private:

  // ---------------------------------------------------------
  // VARIABLES DE CLASE (Miembros)
  // ---------------------------------------------------------

  std::vector<Submesh> m_submeshes; ///< Lista con todas las partes (submeshes) que arman el modelo 3D.
};