#include "Renderer.h"
#include "Shader.h"

void Renderer::StaticAllocateBuffers(const std::shared_ptr<Scene>& scene) {
	for(const auto& obj : scene->GetObjects())
		obj->StaticAllocate();
}

void Renderer::RenderObject(const std::shared_ptr<GameObject>& object) {

	//send the model matrix to the shader
	object->GetShader()->SetUniform("world", object->GetGlobalReferenceFrame());

	auto& mesh = object->GetMesh();
	if(mesh == nullptr) {
		Util::Log("mesh is null");
		return;
	}

	mesh->Bind();
	//mesh->BindVBO();

	auto& shader = object->GetShader();
	auto& tex = object->GetTexture();

	if (tex != nullptr) {
		shader->SetUniform("tex", object->GetTexture()->GetTextureUnit());
		tex->Bind();
	}

	auto& material = object->GetMaterial();
	if (material != nullptr) {
		shader->SetUniform("materialAmbientIntensity", material->ambientIntensity);
		shader->SetUniform("materialSpecularIntensity", material->specularIntensity);
		shader->SetUniform("materialShininess", material->shininess);
	}

	//mesh->SetUpAttributeInterpretration();

	//if (mesh->IndexCount() != 0) {
		//mesh->BindIBO();
		glDrawElements(mesh->GetPrimitiveType(), (GLsizei) mesh->IndexElemCount(), GL_UNSIGNED_SHORT, (void*)0);
	//}
	//else {
		//glDrawArrays(mesh->GetPrimitiveType(), 0, (GLsizei) mesh->GetNumberOfVertices());
	//}

	// Recursively render the children
	auto& children = object->GetChildren();
	for (const auto& child : children)
		RenderObject(child);

	mesh->Unbind();
}

void Renderer::RenderScene(const std::shared_ptr<Scene>& scene, const std::shared_ptr<Shader>& shader, const std::shared_ptr<Camera>& cam) {

	if(shader->GetShaderProg() != 0) {

		//send the data for the global light source
		auto& globalLight = scene->GetGlobalLight();
		if (globalLight != nullptr) {
			shader->SetUniform("globalLightPosition", globalLight->position);
			shader->SetUniform("globalLightColor", globalLight->color);
			shader->SetUniform("globalLightIntensity", globalLight->intensity);
		}
		//send the data for the local light source
		auto& localLight = scene->GetLocalLight();
		if (localLight != nullptr) {
			shader->SetUniform("localLightPosition", localLight->position);
			shader->SetUniform("localLightColor", localLight->color);
			shader->SetUniform("localLightIntensity", localLight->intensity);
			shader->SetUniform("localLightAttenuationCoef", localLight->attenuationCoef);
		}

		//for each object in the scene, render it separately
		for(auto& object : scene->GetObjects())
			RenderObject(object);
	}
	else
		Util::Log("this renderer's shader was not linked");
}
