#pragma once
#include "Prerequisites.h"
#include "Rendering/RenderTypes.h"

class Skybox;

/**
 * @class RenderScene
 * @brief Contenedor de todo lo que se va a renderizar en un frame.
 *
 * @details
 *  Este es básicamente el "snapshot" del mundo para el renderer.
 *
 *  Aquí NO hay lógica de juego, solo datos listos para dibujar.
 *
 *  Piensa en esto como:
 *   👉 "la lista final que el renderer necesita"
 *
 *  Normalmente se llena desde:
 *   - ECS
 *   - SceneGraph
 *   - Sistemas de luces
 *
 *  Y luego el renderer (ForwardRenderer) lo consume.
 */
class
	RenderScene {
public:

	/**
	 * @brief Limpia todos los datos del frame.
	 *
	 * @details
	 *  Esto se llama cada frame antes de volver a llenarlo.
	 *
	 *  Importante para evitar:
	 *   - Datos duplicados
	 *   - Render basura
	 */
	void
		clear() {

		opaqueObjects.clear();
		transparentObjects.clear();
		directionalLights.clear();
		skybox = nullptr;
	}

public:

	/**
	 * @brief Lista de objetos opacos.
	 *
	 * @details
	 *  Estos se renderizan primero porque:
	 *   - No usan blending
	 *   - Permiten optimizaciones como early-z
	 */
	std::vector<RenderObject> opaqueObjects;


	/**
	 * @brief Lista de objetos transparentes.
	 *
	 * @details
	 *  Estos se renderizan después porque:
	 *   - Necesitan blending
	 *   - Deben ordenarse por distancia (back-to-front)
	 */
	std::vector<RenderObject> transparentObjects;


	/**
	 * @brief Luces direccionales de la escena.
	 *
	 * @details
	 *  Ejemplo:
	 *   - Sol
	 *   - Luz global
	 *
	 *  Estas afectan a todos los objetos.
	 */
	std::vector<LightData> directionalLights;


	/**
	 * @brief Skybox actual de la escena.
	 *
	 * @details
	 *  Es el fondo del mundo:
	 *   - Cielo
	 *   - HDRI
	 *   - Ambiente
	 */
	Skybox* skybox = nullptr;
};