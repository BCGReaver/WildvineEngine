#pragma once
#include "Prerequisites.h"

class Mesh;
class MaterialInstance;

/**
 * @enum MaterialDomain
 * @brief Define en qué tipo de "mundo" vive el material.
 *
 * @details
 *  Básicamente esto le dice al renderer cómo tratar el material:
 *  - Opaque → sólido normal
 *  - Masked → con recortes tipo hojas o fences
 *  - Transparent → con transparencia real
 */
enum class
	MaterialDomain {
	Opaque = 0,
	Masked,
	Transparent
};

/**
 * @enum BlendMode
 * @brief Cómo se mezcla el material con lo que ya está en pantalla.
 *
 * @details
 *  Aquí decides cómo se combinan los colores:
 *  - Opaque → sin mezcla
 *  - Alpha → transparencia clásica
 *  - Additive → efectos tipo fuego/luz
 *  - PremultipliedAlpha → más correcto para pipelines avanzados
 */
enum class
	BlendMode {
	Opaque = 0,
	Alpha,
	Additive,
	PremultipliedAlpha
};

/**
 * @enum RenderPassType
 * @brief Define en qué fase del render se dibuja el objeto.
 *
 * @details
 *  Esto ayuda a organizar el pipeline:
 *  - Shadow → sombras
 *  - Opaque → objetos sólidos
 *  - Skybox → fondo
 *  - Transparent → objetos transparentes
 *  - Editor → gizmos / debug
 */
enum class
	RenderPassType {
	Shadow = 0,
	Opaque,
	Skybox,
	Transparent,
	Editor
};

/**
 * @enum LightType
 * @brief Tipos de luz soportados.
 *
 * @details
 *  Nada raro aquí:
 *  - Directional → tipo sol
 *  - Point → foco en todas direcciones
 *  - Spot → cono tipo linterna
 */
enum class
	LightType {
	Directional = 0,
	Point,
	Spot
};

/**
 * @struct LightData
 * @brief Datos básicos de una luz en la escena.
 *
 * @details
 *  Este struct es el que se manda al shader para iluminar.
 *  Tiene TODO lo necesario dependiendo del tipo de luz.
 */
struct
	LightData {
	LightType type = LightType::Directional;

	// Color de la luz
	EU::Vector3 color = EU::Vector3(1.0f, 1.0f, 1.0f);

	// Intensidad general
	float intensity = 1.0f;

	// Dirección (para directional y spot)
	EU::Vector3 direction = EU::Vector3(0.0f, -1.0f, 0.0f);

	// Rango (para point y spot)
	float range = 0.0f;

	// Posición (para point y spot)
	EU::Vector3 position = EU::Vector3(0.0f, 0.0f, 0.0f);

	// Ángulo del cono (spot)
	float spotAngle = 0.0f;
};

/**
 * @struct MaterialParams
 * @brief Parámetros físicos del material (PBR simplificado).
 *
 * @details
 *  Estos valores afectan cómo se ve el material:
 *  - baseColor → color base
 *  - metallic → qué tan metálico
 *  - roughness → qué tan rugoso
 *  - ao → ambient occlusion
 */
struct
	MaterialParams {
	XMFLOAT4 baseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	float metallic = 1.0f;
	float roughness = 1.0f;
	float ao = 1.0f;

	float normalScale = 1.0f;
	float emissiveStrength = 1.0f;

	float alphaCutoff = 0.5f;
};

/**
 * @struct CBPerFrame
 * @brief Constant buffer que cambia UNA VEZ por frame.
 *
 * @details
 *  Aquí metes info global:
 *  - Cámara
 *  - Proyección
 *  - Luz principal
 */
struct
	CBPerFrame {
	XMFLOAT4X4 View{};
	XMFLOAT4X4 Projection{};

	EU::Vector3 CameraPos{};
	float pad0 = 0.0f;

	EU::Vector3 LightDir = EU::Vector3(0.0f, -1.0f, 0.0f);
	float pad1 = 0.0f;

	EU::Vector3 LightColor = EU::Vector3(1.0f, 1.0f, 1.0f);
	float pad2 = 0.0f;
};

/**
 * @struct CBPerObject
 * @brief Constant buffer que cambia por objeto.
 *
 * @details
 *  Aquí solo mandas la matriz WORLD.
 */
struct
	CBPerObject {
	XMFLOAT4X4 World{};
};

/**
 * @struct CBPerMaterial
 * @brief Constant buffer para materiales.
 *
 * @details
 *  Esto se manda por material para que el shader sepa cómo renderizarlo.
 */
struct
	CBPerMaterial {
	XMFLOAT4 BaseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	float Metallic = 1.0f;
	float Roughness = 1.0f;
	float AO = 1.0f;

	float NormalScale = 1.0f;
	float EmissiveStrength = 1.0f;

	float AlphaCutoff = 0.0f;

	// Padding para alineación (DirectX es quisquilloso con esto)
	float pad0 = 0.0f;
	float pad1 = 0.0f;
	float pad2 = 0.0f;
	float pad3 = 0.0f;
	float pad4 = 0.0f;
	float pad5 = 0.0f;
};

/**
 * @struct RenderObject
 * @brief Representa un objeto listo para renderizar.
 *
 * @details
 *  Este es el "paquete final" que el renderer usa.
 *
 *  Contiene:
 *  - Mesh
 *  - Material(es)
 *  - Transform
 *  - Flags de render
 */
struct
	RenderObject {

	// Geometría
	Mesh* mesh = nullptr;

	// Material principal
	MaterialInstance* materialInstance = nullptr;

	// Por si el mesh tiene varios materiales (submeshes)
	std::vector<MaterialInstance*> materialInstances;

	// Transform en mundo
	XMMATRIX world = XMMatrixIdentity();

	// ¿Proyecta sombras?
	bool castShadow = true;

	// ¿Es transparente?
	bool transparent = false;

	// Distancia a cámara (para sorting de transparencias)
	float distanceToCamera = 0.0f;
};