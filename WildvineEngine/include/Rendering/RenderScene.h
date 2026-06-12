#pragma once
#include "Prerequisites.h"
#include "Rendering/RenderTypes.h"

class Skybox;

/**
 * @brief Clase que guarda todo lo que se va a dibujar en un frame.
 * Es literalmente un contenedor. El motor recolecta todos los objetos visibles
 * de tu nivel y los acomoda aquí para pasárselos al renderizador en charola de plata.
 */
class
  RenderScene {
public:

  /**
   * @brief Limpia todas las listas de objetos y luces.
   * Se tiene que llamar al final o al inicio de cada frame para no dibujar
   * lo del frame pasado y evitar que la memoria explote.
   */
  void
    clear();

public:

  // ---------------------------------------------------------
  // VARIABLES PÚBLICAS (Listas de dibujo)
  // ---------------------------------------------------------

  /** * @brief Lista de todos los modelos sólidos y opacos (como paredes, pisos, personajes).
   * Estos se dibujan primero porque son los más fáciles y tapan lo que hay atrás.
   */
  std::vector<RenderObject> opaqueObjects;

  /** * @brief Lista de los objetos con transparencia (como ventanas o agua).
   * Estos se dibujan después de los opacos para que la mezcla de colores se vea bien.
   */
  std::vector<RenderObject> transparentObjects;

  /** * @brief Lista de las luces direccionales.
   * Básicamente las luces que simulan el sol o la luna, que pegan parejo en toda la escena.
   */
  std::vector<LightData> directionalLights;

  /** * @brief Puntero al Skybox actual.
   * El cubo gigante que envuelve tu nivel y tiene la textura del cielo.
   */
  Skybox* skybox = nullptr;
};