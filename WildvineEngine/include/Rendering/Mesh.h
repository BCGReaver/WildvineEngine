#pragma once
#include "Prerequisites.h"
#include "Buffer.h"

/**
 * @struct Submesh
 * @brief Representa una parte de un mesh con su propio material.
 *
 * @details
 *  Un mesh puede estar dividido en varias partes (submeshes),
 *  y cada una puede usar un material diferente.
 *
 *  Ejemplo:
 *   - Personaje:
 *      Submesh 0 → cuerpo
 *      Submesh 1 → armadura
 *      Submesh 2 → ojos
 *
 *  Cada submesh tiene:
 *   - Su propio vertex buffer
 *   - Su propio index buffer
 *   - Un material asignado (materialSlot)
 */
struct
	Submesh {

	/** @brief Buffer de vértices */
	Buffer vertexBuffer;

	/** @brief Buffer de índices */
	Buffer indexBuffer;

	/** @brief Número total de índices a dibujar */
	unsigned int indexCount = 0;

	/** @brief Índice inicial dentro del index buffer */
	unsigned int startIndex = 0;

	/** @brief Slot de material (para buscar en MaterialInstance o Material array) */
	unsigned int materialSlot = 0;
};


/**
 * @class Mesh
 * @brief Contenedor de geometría (uno o varios submeshes).
 *
 * @details
 *  Este compa representa la forma del objeto.
 *
 *  IMPORTANTE:
 *   - No tiene materiales
 *   - No tiene lógica
 *   - Solo datos de geometría
 *
 *  Es básicamente:
 *   "esto es lo que se va a dibujar"
 *
 *  Mientras que:
 *   - Material → cómo se ve
 *   - Transform → dónde está
 */
class
	Mesh {
public:

	/**
	 * @brief Obtiene los submeshes (editable).
	 *
	 * @details
	 *  Útil cuando quieres:
	 *   - Modificar buffers
	 *   - Asignar materiales por slot
	 */
	std::vector<Submesh>&
		getSubmeshes() { return m_submeshes; }


	/**
	 * @brief Obtiene los submeshes (solo lectura).
	 */
	const std::vector<Submesh>&
		getSubmeshes() const { return m_submeshes; }


	/**
	 * @brief Libera todos los buffers del mesh.
	 *
	 * @details
	 *  Aquí se hace cleanup completo:
	 *   - Vertex buffers
	 *   - Index buffers
	 *
	 *  Muy importante para evitar leaks en GPU.
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

	/**
	 * @brief Lista de submeshes.
	 *
	 * @details
	 *  Cada entrada representa una parte del modelo.
	 */
	std::vector<Submesh> m_submeshes;
};