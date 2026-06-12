#pragma once
#include "Prerequisites.h"

class Mesh;
class MaterialInstance;

/**
 * @brief Define el tipo de superficie que tiene un material.
 * Le dice al motor si el objeto bloquea la luz por completo, si tiene agujeros, o si se puede ver a través de él.
 */
enum class
  MaterialDomain {

  Opaque = 0, ///< Material sólido que tapa todo lo que tiene atrás (ej. una pared).

  Masked,     ///< Material que usa una textura de blanco y negro para recortar partes (ej. una reja o una hoja).

  Transparent ///< Material transparente que deja pasar la luz (ej. un cristal o agua).
};

/**
 * @brief Modos de mezcla para combinar los colores con el fondo.
 * Le dice a la gráfica cómo pintar los píxeles transparentes o con efectos sobre lo que ya estaba dibujado.
 */
enum class
  BlendMode {

  Opaque = 0,         ///< No mezcla nada, el píxel nuevo borra por completo al de atrás.

  Alpha,              ///< Mezcla transparente normalita basada en la opacidad (Lerp clásico).

  Additive,           ///< Suma los colores. Ideal para efectos que brillan como fuego, rayos o magia.

  PremultipliedAlpha  ///< Mezcla avanzada donde el color ya viene multiplicado por su transparencia para evitar bordes feos.
};

/**
 * @brief Los diferentes pasos (pases de render) por los que pasa la cámara para armar el cuadro final.
 */
enum class
  RenderPassType {

  Shadow = 0, ///< Paso donde se calcula la profundidad desde la luz para proyectar sombras.

  Opaque,     ///< Paso donde se dibujan todos los objetos sólidos.

  Skybox,     ///< Paso dedicado a pintar el fondo/cielo del mundo.

  Transparent,///< Paso final de dibujo donde se ponen los cristales y efectos encima de todo.

  Editor      ///< Paso extra para cosas del motor, como las líneas de las mallas o las flechas de mover objetos.
};

/**
 * @brief Tipos de luces que soporta el motor.
 */
enum class
  LightType {

  Directional = 0, ///< Como el sol: una luz infinita que viene de una sola dirección y pega en todo el mapa.

  Point,           ///< Como un foco pelón: una esfera de luz que brilla en todas direcciones pero tiene un límite de alcance.

  Spot             ///< Como una linterna: un cono de luz que apunta a un lugar específico.
};

/**
 * @brief El número máximo de luces que le podemos pasar al shader al mismo tiempo para que no explote la gráfica.
 */
constexpr int RMaxLights = 8;

/**
 * @brief Estructura que guarda toda la información de una luz.
 * Junta la posición, color, dirección y tipo de luz para mandarla directo al cálculo de iluminación.
 */
struct
  LightData {

  LightType type = LightType::Directional;       ///< El tipo de luz que va a ser.

  EU::Vector3 color = EU::Vector3(1.0f, 1.0f, 1.0f); ///< El color de la luz (por defecto blanca).

  float intensity = 1.0f;                         ///< Qué tan fuerte brilla la luz.

  EU::Vector3 direction = EU::Vector3(0.0f, -1.0f, 0.0f); ///< Hacia dónde apunta (importante para sol y linternas).

  float range = 0.0f;                             ///< Hasta dónde llega la luz antes de apagarse (para luces de punto/cono).

  EU::Vector3 position = EU::Vector3(0.0f, 0.0f, 0.0f); ///< Dónde está parada la luz en el mundo.

  float spotAngle = 0.0f;                         ///< El ángulo de apertura del cono (sólo si es tipo Spot).
};

/**
 * @brief Parámetros numéricos base para configurar los materiales PBR (Renderizado Físico).
 * Estos valores controlan cómo reacciona el material a la luz de una forma realista.
 */
struct
  MaterialParams {

  XMFLOAT4 baseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); ///< Color principal del material (RGBA).

  float metallic = 1.0f;          ///< Qué tan de metal es (0.0 = plástico/madera, 1.0 = metal puro).

  float roughness = 1.0f;         ///< Qué tan rugoso es (0.0 = espejo pulido, 1.0 = lija opaca).

  float ao = 1.0f;                ///< Factor de Oclusión Ambiental para oscurecer grietas.

  float normalScale = 1.0f;       ///< Qué tanto se van a notar los relieves del mapa de normales.

  float emissiveStrength = 1.0f;  ///< Multiplicador de brillo para las partes que emiten luz.

  float alphaCutoff = 0.5f;       ///< El límite para decidir si un píxel se borra o se queda (usado en materiales Masked).
};

/**
 * @brief Constant Buffer por Frame (CBPerFrame).
 * Datos que le mandas a los shaders una sola vez al inicio del frame, como la cámara o el sol principal.
 */
struct
  CBPerFrame {

  XMFLOAT4X4 View{};                    ///< Matriz de vista (la posición y rotación de la cámara).

  XMFLOAT4X4 Projection{};              ///< Matriz de proyección (el lente de la cámara, perspectiva).

  XMFLOAT4X4 LightViewProjection{};     ///< Matriz de la cámara de la luz (súper necesaria para las sombras).

  EU::Vector3 CameraPos{};              ///< Posición de la cámara en el espacio 3D.

  float pad0 = 0.0f;                    ///< Padding para alinear la memoria a 16 bytes (regla de DirectX).

  EU::Vector3 LightDir = EU::Vector3(0.0f, -1.0f, 0.0f); ///< Dirección del sol actual.

  float pad1 = 0.0f;                    ///< Relleno de memoria.

  EU::Vector3 LightColor = EU::Vector3(1.0f, 1.0f, 1.0f); ///< Color del sol actual.

  float ligthRange = 10.0f;             ///< Alcance de la luz.

  EU::Vector3 LightPosition = EU::Vector3(0.0f, 5.0f, 0.0f); ///< Posición de la luz.

  int LightCount = 0;                   ///< Cuántas luces activas hay en total este frame.

  XMFLOAT3 pad2 = XMFLOAT3(0.0f, 0.0f, 0.0f); ///< Relleno de memoria para completar el bloque de 16 bytes.
};

/**
 * @brief Constant Buffer por Objeto (CBPerObject).
 * Datos que cambian por cada modelo que vas a dibujar.
 */
struct
  CBPerObject {

  XMFLOAT4X4 World{}; ///< Matriz de mundo (posición, rotación y escala del objeto en el mapa).
};

/**
 * @brief Constant Buffer por Material (CBPerMaterial).
 * Pasa los datos numéricos del material al shader para que sepa cómo pintarlo.
 * Tiene un buen de paddings porque las estructuras en los shaders (HLSL) necesitan estar alineadas perfectas.
 */
struct
  CBPerMaterial {

  XMFLOAT4 BaseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); ///< Color base.

  float Metallic = 1.0f;          ///< Valor de metal.

  float Roughness = 1.0f;         ///< Valor de rugosidad.

  float AO = 1.0f;                ///< Valor de oclusión ambiental.

  float NormalScale = 1.0f;       ///< Escala de normales.

  float EmissiveStrength = 1.0f;  ///< Fuerza del brillo.

  float AlphaCutoff = 0.0f;       ///< Límite de recorte de alfa.

  float pad0 = 0.0f;              ///< Relleno de alineación 0.

  float pad1 = 0.0f;              ///< Relleno de alineación 1.

  float pad2 = 0.0f;              ///< Relleno de alineación 2.

  float pad3 = 0.0f;              ///< Relleno de alineación 3.

  float pad4 = 0.0f;              ///< Relleno de alineación 4.

  float pad5 = 0.0f;              ///< Relleno de alineación 5.
};

/**
 * @brief Representa una entidad o modelo listo para ser mandado a la cola de renderizado.
 * Junta la geometría (Mesh), los materiales que usa y su posición en el mundo para que el renderizador sepa qué hacer.
 */
struct
  RenderObject {

  Mesh* mesh = nullptr;                               ///< Puntero a la geometría/malla 3D.

  MaterialInstance* materialInstance = nullptr;       ///< Instancia de material principal del objeto.

  std::vector<MaterialInstance*> materialInstances;   ///< Lista de materiales por si el modelo tiene submeshes con diferentes texturas.

  XMMATRIX world = XMMatrixIdentity();                ///< La matriz de transformación del objeto en el mundo.

  bool castShadow = true;                             ///< Bandera: si es true, este objeto bloquea luz y proyecta sombra.

  bool transparent = false;                           ///< Bandera: nos dice rápido si el objeto tiene transparencias.

  float distanceToCamera = 0.0f;                      ///< Distancia a la cámara (clave para ordenar los transparentes de atrás hacia adelante).
};