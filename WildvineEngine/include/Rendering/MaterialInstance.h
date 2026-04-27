#pragma once
#include "Prerequisites.h"
#include "Rendering/RenderTypes.h"

class Material;
class DeviceContext;
class Texture;

/**
 * @class MaterialInstance
 * @brief Representa una instancia de un material con datos únicos.
 *
 * @details
 *  Aquí está la diferencia clave:
 *
 *   - Material → definición general (shader, estados, etc)
 *   - MaterialInstance → datos específicos de un objeto
 *
 *  Ejemplo:
 *   - Material = "Metal"
 *   - Instancia A = Metal rojo
 *   - Instancia B = Metal azul
 *
 *  Esto evita duplicar materiales y permite reutilización 🔥
 */
class
	MaterialInstance {
public:

	/**
	 * @brief Asigna el material base.
	 */
	void
		setMaterial(Material* material) { m_material = material; }


	/**
	 * @brief Setters de texturas PBR.
	 *
	 * @details
	 *  Estas son las clásicas:
	 *   - Albedo → color base
	 *   - Normal → detalle de superficie
	 *   - Metallic → qué tan metal es
	 *   - Roughness → qué tan rugoso
	 *   - AO → ambient occlusion
	 *   - Emissive → luz propia
	 */
	void setAlbedo(Texture* texture) { m_albedo = texture; }
	void setNormal(Texture* texture) { m_normal = texture; }
	void setMetallic(Texture* texture) { m_metallic = texture; }
	void setRoughness(Texture* texture) { m_roughness = texture; }
	void setAO(Texture* texture) { m_ao = texture; }
	void setEmissive(Texture* texture) { m_emissive = texture; }


	/**
	 * @brief Getters de material y texturas.
	 */
	Material* getMaterial() const { return m_material; }
	Texture* getAlbedo()   const { return m_albedo; }
	Texture* getNormal()   const { return m_normal; }
	Texture* getMetallic() const { return m_metallic; }
	Texture* getRoughness()const { return m_roughness; }
	Texture* getAO()       const { return m_ao; }
	Texture* getEmissive() const { return m_emissive; }


	/**
	 * @brief Parámetros extra del material.
	 *
	 * @details
	 *  Aquí puedes guardar cosas como:
	 *   - Color base (tint)
	 *   - Intensidad de emissive
	 *   - Roughness override
	 *
	 *  Esto se manda al shader vía constant buffer.
	 */
	MaterialParams&
		getParams() { return m_params; }

	const MaterialParams&
		getParams() const { return m_params; }


	/**
	 * @brief Bindea todas las texturas al pipeline.
	 *
	 * @details
	 *  Aquí le dices al GPU:
	 *   "Estas son las texturas que vas a usar en el shader"
	 *
	 *  Cada textura se asigna a un slot del Pixel Shader.
	 *
	 *  Ejemplo típico:
	 *   slot 0 → Albedo
	 *   slot 1 → Normal
	 *   slot 2 → Metallic
	 *   ...
	 */
	void
		bindTextures(DeviceContext& deviceContext) const {

		unsigned int slot = 0;

		if (m_albedo) {
			m_albedo->render(deviceContext, slot++, 1);
		}

		if (m_normal) {
			m_normal->render(deviceContext, slot++, 1);
		}

		if (m_metallic) {
			m_metallic->render(deviceContext, slot++, 1);
		}

		if (m_roughness) {
			m_roughness->render(deviceContext, slot++, 1);
		}

		if (m_ao) {
			m_ao->render(deviceContext, slot++, 1);
		}

		if (m_emissive) {
			m_emissive->render(deviceContext, slot++, 1);
		}
	}

private:

	/** @brief Material base compartido */
	Material* m_material = nullptr;

	/** @brief Texturas PBR */
	Texture* m_albedo = nullptr;
	Texture* m_normal = nullptr;
	Texture* m_metallic = nullptr;
	Texture* m_roughness = nullptr;
	Texture* m_ao = nullptr;
	Texture* m_emissive = nullptr;

	/**
	 * @brief Parámetros personalizados del material.
	 *
	 * @details
	 *  Se usa para enviar data al shader (CBPerMaterial).
	 */
	MaterialParams m_params;
};