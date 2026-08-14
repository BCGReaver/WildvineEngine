/**
 * @file Actor.cpp
 * @brief Implementa la logica de Actor dentro del subsistema ECS.
 * @ingroup ecs
 */
#include "ECS/Actor.h"
#include "MeshComponent.h"
#include "Device.h"
#include "DeviceContext.h"

Actor::Actor(Device& device) {
	// Setup Default Components
	EU::TSharedPointer<Transform> transform = EU::MakeShared<Transform>();
	addComponent(transform);
	EU::TSharedPointer<MeshComponent> meshComponent = EU::MakeShared<MeshComponent>();
	addComponent(meshComponent);

	HRESULT hr;
	std::string classNameType = "Actor -> " + m_name;
	hr = m_modelBuffer.init(device, sizeof(CBChangesEveryFrame));
	if (FAILED(hr)) {
		ERROR("Actor", classNameType.c_str(), "Failed to create new CBChangesEveryFrame");
	}

	// Awake
	awake();

	hr = m_sampler.init(device);
	if (FAILED(hr)) {
		ERROR("Actor", classNameType.c_str(), "Failed to create new SamplerState");
	}

	//hr = m_rasterizer.init(device);
	//if (FAILED(hr)) {
	//	ERROR("Actor", classNameType.c_str(), "Failed to create new Rasterizer");
	//}

	//hr = m_blendstate.init(device);
	//if (FAILED(hr)) {
	//	ERROR("Actor", classNameType.c_str(), "Failed to create new BlendState");
	//}

	//hr = m_shaderShadow.CreateShader(device, PIXEL_SHADER, "HybridEngine.fx");
	//
	//if (FAILED(hr)) {
	//	ERROR("Main", "InitDevice",
	//		("Failed to initialize Shadow Shader. HRESULT: " + std::to_string(hr)).c_str());
	//}
	//
	//hr = m_shaderBuffer.init(device, sizeof(CBChangesEveryFrame));
	//if (FAILED(hr)) {
	//	ERROR("Main", "InitDevice",
	//		("Failed to initialize Shadow Buffer. HRESULT: " + std::to_string(hr)).c_str());
	//
	//}
	//
	//hr = m_shadowBlendState.init(device);
	//if (FAILED(hr)) {
	//	ERROR("Main", "InitDevice",
	//		("Failed to initialize Shadow Blend State. HRESULT: " + std::to_string(hr)).c_str());
	//
	//}

	//hr = m_shadowDepthStencilState.init(device, true, false);
	//
	//if (FAILED(hr)) {
	//	ERROR("Main", "InitDevice",
	//		("Failed to initialize Depth Stencil State. HRESULT: " + std::to_string(hr)).c_str());
	//
	//}
	//
	//m_LightPos = XMFLOAT4(2.0f, 4.0f, -2.0f, 1.0f);
}

void
Actor::update(float deltaTime, DeviceContext& deviceContext) {
	// Update all components
	for (auto& component : m_components) {
		if (component) {
			component->update(deltaTime);
		}
	}

	// Update the model buffer
	auto transform = getComponent<Transform>();
	if (!transform) {
		ERROR("Actor", "update", "Transform component is missing.");
		return;
	}
	m_model.mWorld = XMMatrixTranspose(transform->matrix);
	m_model.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	// Update the constant buffer
	m_modelBuffer.update(deviceContext, nullptr, 0, nullptr, &m_model, 0, 0);
}

void
Actor::render(DeviceContext& deviceContext) {
	m_sampler.render(deviceContext, 0, 1);

	deviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Update buffer and render all components
	for (unsigned int i = 0; i < m_meshes.size(); i++)
	{
		m_vertexBuffers[i].render(deviceContext, 0, 1);
		m_indexBuffers[i].render(deviceContext, 0, 1, false, DXGI_FORMAT_R32_UINT);
		m_modelBuffer.render(deviceContext, 1, 1, true);

		// Limpieza por mesh (evita herencias)
		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		deviceContext.m_deviceContext->PSSetShaderResources(0, 1, nullSRV);

		// Bind correcto por mesh
		if (i < m_textures.size()) {
			for (int k = 0; k < m_textures.size(); k++) {
				m_textures[k].render(deviceContext, k, 1);   // albedo mesh i
			}
		}
		// else: se queda null
		deviceContext.DrawIndexed(m_meshes[i].m_numIndex, 0, 0);
	}
}

void
Actor::renderForSkybox(DeviceContext& deviceContext) {
	deviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Update buffer and render all components
	for (unsigned int i = 0; i < m_meshes.size(); i++) {
		m_vertexBuffers[i].render(deviceContext, 0, 1);
		m_indexBuffers[i].render(deviceContext, 0, 1, false, DXGI_FORMAT_R32_UINT);

		deviceContext.DrawIndexed(m_meshes[i].m_numIndex, 0, 0);
	}
}


void
Actor::destroy() {
	for (auto& vertexBuffer : m_vertexBuffers) {
		vertexBuffer.destroy();
	}

	for (auto& indexBuffer : m_indexBuffers) {
		indexBuffer.destroy();
	}

	for (auto& tex : m_textures) {
		tex.destroy();
	}
	m_modelBuffer.destroy();

	//m_rasterizer.destroy();
	//m_blendstate.destroy();
	m_sampler.destroy();

	m_vertexBuffers.clear();
	m_indexBuffers.clear();
	m_textures.clear();
	m_meshes.clear();
}

void
Actor::setMesh(Device& device, std::vector<MeshComponent> meshes) {
	for (auto& vertexBuffer : m_vertexBuffers) {
		vertexBuffer.destroy();
	}
	for (auto& indexBuffer : m_indexBuffers) {
		indexBuffer.destroy();
	}
	m_vertexBuffers.clear();
	m_indexBuffers.clear();
	m_meshes.clear();

	for (auto& mesh : meshes) {
		Buffer vertexBuffer;
		HRESULT hr = vertexBuffer.init(device, mesh, D3D11_BIND_VERTEX_BUFFER);
		if (FAILED(hr)) {
			ERROR("Actor", "setMesh", "Failed to create new vertexBuffer");
			continue;
		}

		Buffer indexBuffer;
		hr = indexBuffer.init(device, mesh, D3D11_BIND_INDEX_BUFFER);
		if (FAILED(hr)) {
			ERROR("Actor", "setMesh", "Failed to create new indexBuffer");
			vertexBuffer.destroy();
			continue;
		}

		// Mantener mesh/VB/IB alineados: solo se agrega el conjunto completo si ambos buffers existen.
		m_meshes.push_back(mesh);
		m_vertexBuffers.push_back(vertexBuffer);
		m_indexBuffers.push_back(indexBuffer);
	}
}


