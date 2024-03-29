#include "Renderer.h"
#include "Shader.h"

Renderer::Renderer(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Scene>& scene) :
	shader(shader),
	scene(scene),
	view(1.0f),
	proj(1.0f) {
	glGenVertexArrays(1, &vaoId);
}

void Renderer::StaticAllocateVertexBuffer(void) const {

	glBindVertexArray(vaoId); //bind it

	for(const auto& obj : scene->GetObjects())
		obj->StaticAllocateVertexBuffer();

	glBindVertexArray(0); //unbind it
}

void Renderer::RenderObject(GraphicsObject& object) {

	//send the model matrix to the shader
	shader->SendMat4Uniform("world", object.GetGlobalReferenceFrame());

	auto& buffer = object.GetVertexBuffer();
	buffer->Select();
	
	if (buffer->HasTexture()) {
		shader->SendIntUniform("texUnit", buffer->GetTextureUnit());
		buffer->GetTexture()->SelectToRender();
	}

	auto& material = object.GetMaterial();
	if (material != nullptr) {
		shader->SendFloatUniform("ambientIntensity", material->ambientIntensity);
		shader->SendFloatUniform("specularIntensity", material->specularIntensity);
		shader->SendFloatUniform("shininess", material->shininess);
	}

	buffer->SetUpAttributeInterpretration();
	glDrawArrays(buffer->GetPrimitiveType(), 0, buffer->GetNumberOfVertices());

	// Recursively render the children
	auto& children = object.GetChildren();
	for (auto& child : children)
		RenderObject(*child);
}

void Renderer::RenderScene(const std::shared_ptr<Camera>& cam) {

	if(shader->IsCreated()) {

		glUseProgram(shader->GetShaderProgram());
		glBindVertexArray(vaoId);

		//we must send the view matrix to the shader every frame
		shader->SendMat4Uniform("view", view);
		
		//send the data for the global light source
		auto& globalLight = scene->GetGlobalLight();
		shader->SendVec3Uniform("globalLightPosition", globalLight->position);
		shader->SendVec3Uniform("globalLightColor", globalLight->color);
		shader->SendFloatUniform("globalLightIntensity", globalLight->intensity);

		//send the data for the local light source
		auto& localLight = scene->GetLocalLight();
		shader->SendVec3Uniform("localLightPosition", localLight->position);
		shader->SendVec3Uniform("localLightColor", localLight->color);
		shader->SendFloatUniform("localLightIntensity", localLight->intensity);
		shader->SendFloatUniform("localLightAttenuationCoef", localLight->attenuationCoef);

		//send the camera position
		shader->SendVec3Uniform("viewPosition", cam->GetPosition());

		//for each object in the scene, render it separately
		for(auto& object : scene->GetObjects())
			RenderObject(*object);

		glDisableVertexAttribArray(0); //position
		glDisableVertexAttribArray(1); //color
		glUseProgram(0); //reset to zero
		glBindVertexArray(0); //unbind it
	}
}
